/*
 * DScopeStream.cpp
 *
 *  Created on: Feb 9, 2017
 *      Author: zaqc
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <pthread.h>

#include <iostream>

#include "DScopeStream.h"
//----------------------------------------------------------------------------

//============================================================================
//	DataFrameQueue
//============================================================================

#define	DATA_MAX_SIZE		4096
//----------------------------------------------------------------------------

DataFrameQueue::DataFrameQueue(int aQSize, bool aOverPush) {
	m_QSize = aQSize;
	m_OverPush = aOverPush;

	m_GetPtr = 0;
	m_PutPtr = 0;

	m_ExtFrame = new DataFrame();
	m_ExtFrame->Create();
	m_ExtFrame->Clean();

	m_NewFrame = new DataFrame();
	m_NewFrame->Create();
	m_NewFrame->Clean();

	m_Q = new DataFrame*[m_QSize];
	for (int i = 0; i < m_QSize; i++) {
		m_Q[i] = new DataFrame();
		m_Q[i]->Create();
		m_Q[i]->Clean();
	}
	m_QLen = 0;
	m_ReadyToPush = false;
}
//----------------------------------------------------------------------------

DataFrameQueue::~DataFrameQueue() {
	for (int i = 0; i < m_QSize; i++) {
		m_Q[i]->Destroy();
		delete m_Q[i];
	}
	delete[] m_Q;

	m_NewFrame->Destroy();
	delete m_NewFrame;

	m_ExtFrame->Destroy();
	delete m_ExtFrame;
}
//----------------------------------------------------------------------------

DataFrame *DataFrameQueue::PullFront(void) {
	if (0 != m_QLen) {
		DataFrame *res = m_Q[m_GetPtr];
		m_Q[m_GetPtr] = m_ExtFrame;
		m_ExtFrame = res;
		m_GetPtr = (m_GetPtr + 1) % m_QSize;
		m_QLen--;
		return res;
	}
	return NULL;
}
//----------------------------------------------------------------------------

DataFrame *DataFrameQueue::GetBack(void) {
	return m_NewFrame;
}
//----------------------------------------------------------------------------

void DataFrameQueue::Push(void) {
	if (m_QLen < m_QSize || m_OverPush) {
		if (m_QLen == m_QSize) {
			m_GetPtr = (m_GetPtr + 1) % m_QSize;
			m_QLen--;
		}
		DataFrame *tmp = m_Q[m_PutPtr];
		m_Q[m_PutPtr] = m_NewFrame;
		m_NewFrame = tmp;
		m_PutPtr = (m_PutPtr + 1) % m_QSize;
		m_QLen++;
	}
}
//----------------------------------------------------------------------------

//============================================================================
//	DScopeStream
//============================================================================

#define REV_BYTE_ORDER(a)	(((unsigned int)(a & 0xFF) << 24) | \
		((unsigned int)(a & 0xFF00) << 8) | ((unsigned int)(a & 0xFF0000) >> 8) | \
		((unsigned int)(a & 0xFF000000) >> 24))
//----------------------------------------------------------------------------

void * recv_thread(void *context) {
	((DScopeStream*) context)->RecvThread();
	return NULL;
}
//----------------------------------------------------------------------------

DScopeStream::DScopeStream() {
	m_StreamState = ssNone;
	m_PreambleCount = 0;

	m_ErrorCount = 0;

	m_Frame = NULL;

	m_LeftEndMarker = false;
	m_RightEndMarker = false;
	m_OutData = NULL;

	m_FTDI = NULL;
	if ((m_FTDI = ftdi_new()) == 0) {
		std::cout << "ftdi_new failed" << std::endl;
		throw "ftdi_new failed";
	}

	ftdi_set_interface(m_FTDI, INTERFACE_A);
	int f = ftdi_usb_open(m_FTDI, DSCOPE_PID, DSCOPE_VID);
	if (f < 0 && f != -5) {
		std::cout << "unable to open ftdi device: " << f << " " << ftdi_get_error_string(m_FTDI) << std::endl;
		ftdi_free(m_FTDI);
		throw "ftdi_new failed";
	}
	std::cout << "ftdi open succeeded(channel 1): " << f << std::endl;

	int res = ftdi_set_bitmode(m_FTDI, 0xFF, BITMODE_SYNCFF);
	std::cout << "ftdi_set_bitmode res=" << res << std::endl;

	res = ftdi_read_data_set_chunksize(m_FTDI, 64 * 1024);
	std::cout << "ftdi_read_data_set_chunksize res=" << res << std::endl;

	res = ftdi_usb_purge_buffers(m_FTDI);
	std::cout << "ftdi_usb_purge_buffers res=" << res << std::endl;

	unsigned char buf[64];
	buf[0] = 0x55;
	buf[1] = 0x55;
	buf[2] = 0x55;
	buf[3] = 0xd5;

	buf[4] = 0xFF;
	buf[5] = 0x00;
	buf[6] = 0x12;
	buf[7] = 0xDE;

	buf[8] = 0x00;
	buf[9] = 0x00;
	buf[10] = 0x00;
	buf[11] = 0x01;

	buf[12] = 0x00;
	buf[13] = 0x00;
	buf[14] = 0x00;
	buf[15] = 0x00;

	for (int i = 0; i < 10; i++) {
		ftdi_write_data(m_FTDI, buf, 16);
		usleep(1000 * 1000);
	}

	m_Q = new DataFrameQueue(16, true);

	m_DataAccepted = false;
	pthread_mutex_init(&m_FrameLock, NULL);
	pthread_cond_init(&m_DataReady, NULL);
	pthread_create(&m_Thread, NULL, &recv_thread, this);
}
//----------------------------------------------------------------------------

DScopeStream::~DScopeStream() {

	delete m_Q;

	if (NULL != m_FTDI) {
		ftdi_usb_close(m_FTDI);
		pthread_join(m_Thread, NULL);
		ftdi_free(m_FTDI);
		m_FTDI = NULL;
	}
}
//----------------------------------------------------------------------------

int DScopeStream::RecvBuf(unsigned char *aBuf, int aSize) {
	int s = 0;
	int bc;
	while (s < aSize) {
		bc = ftdi_read_data(m_FTDI, &aBuf[s], aSize - s);

		if (bc < 0)
			return -1;

		if (bc == 0) {
			//usleep(1);
		}
		else
			s += bc;
	}

	return s;
}
//----------------------------------------------------------------------------

void DScopeStream::UpdateFrame(bool aPush) {
	pthread_mutex_lock(&m_FrameLock);
	if (aPush) {
		m_Q->Push();
		if (m_DataAccepted) {
			m_DataAccepted = false;
			pthread_cond_signal(&m_DataReady);
		}
	}
	m_Frame = m_Q->GetBack();
	pthread_mutex_unlock(&m_FrameLock);
	m_Frame->Clean();
	m_LeftEndMarker = false;
	m_RightEndMarker = false;
}
//----------------------------------------------------------------------------

int DScopeStream::DecodeBuffer(unsigned char *aBuf, int aSize) {
	if (NULL == m_Frame)
		UpdateFrame(false);

	unsigned char *buf = aBuf;
	int len = aSize / 5;
	for (int i = 0; i < len; i++) {
		unsigned char ch = *buf;
		buf++;
		unsigned int w = *(unsigned int *) buf;
		buf += 4;
		if ((ch & 0xF0) == 0xA0) {
			//if (!m_LeftEndMarker) {
			if ((ch & 0x0F) == 0x0F) {
				m_Frame->m_LPktCounter = REV_BYTE_ORDER(w);
				m_LeftEndMarker = true;
			}
			else {
				m_Frame->m_LChLen[ch & 0x0F]++;
				if (m_Frame->m_LDataLen < 4096) {
					*(unsigned int *) &m_Frame->m_LData[m_Frame->m_LDataLen] = REV_BYTE_ORDER(w);
					m_Frame->m_LDataLen += 4;
				}
				//buf += 5;
			}
		}
		//else
		//	UpdateFrame(true);

		//}
		else if ((ch & 0xF0) == 0x50) {
			//if (!m_RightEndMarker) {
			if ((ch & 0x0F) == 0x0F) {
				m_Frame->m_RPktCounter = REV_BYTE_ORDER(w);
				m_RightEndMarker = true;
			}
			else {
				m_Frame->m_RChLen[ch & 0x0F]++;
				if (m_Frame->m_RDataLen < 4096) {
					*(unsigned int *) &m_Frame->m_RData[m_Frame->m_RDataLen] = REV_BYTE_ORDER(w);
					m_Frame->m_RDataLen += 4;
				}
				//buf += 5;
			}
		}
		//else
		//	UpdateFrame(true);
		//}
		else {
			m_ErrorCount++;
			m_Frame->Clean();
			m_LeftEndMarker = false;
			m_RightEndMarker = false;
			return -1;
		}

		if (m_LeftEndMarker && m_RightEndMarker) {
			UpdateFrame(true);
		}
	}

	return 0;
}
//----------------------------------------------------------------------------

int block_count = 0;
int error_count = 0;

void DScopeStream::DecodeStream(void) {
	unsigned char ch[1024];
	switch (m_StreamState) {
		case ssNone:
			m_StreamState = ssWaitPreamble;
			break;

		case ssWaitPreamble: {
			if (RecvBuf(ch, 1) == 1) {
				if (ch[0] == 0x55) {
					m_PreambleCount++;
				}
				else {
					if (m_PreambleCount == 4 && ch[0] == 0xD5)
						m_StreamState = ssGetDataBlock;
					m_PreambleCount = 0;
				}
			}
			else
				m_StreamState = ssError;

			break;
		}

		case ssGetPreamble: {
			if (RecvBuf(ch, 5) == 5) {
				m_StreamState = ssGetDataBlock;
				for (int i = 0; i < 5; i++) {
					if ((i != 4 && ch[i] != 0x55) || (i == 4 && ch[i] != 0xD5)) {
						m_StreamState = ssWaitPreamble;
						break;
					}
				}
			}
			else
				m_StreamState = ssError;

			break;
		}

		case ssGetDataBlock: {
			if (RecvBuf(ch, 1000) == 1000) {
				if (0 == DecodeBuffer(ch, 1000)) {
					m_StreamState = ssGetPreamble;
					block_count++;
				}
				else {
					m_StreamState = ssWaitPreamble;
					error_count++;
				}
			}
			else
				m_StreamState = ssError;

			break;
		}

		case ssError: {
			m_ErrorCount++;
			break;
		}
	}
}
//----------------------------------------------------------------------------

void DScopeStream::RecvThread(void) {
	printf("Start receive thread...\n");
	//int n = 0;
	while (true) {
		DecodeStream();
		if (m_StreamState == ssError) {
			printf("USB Stream Error...\n");
			printf("block count=%i\n", block_count);
			printf("error count=%i\n", error_count);
			printf("m_ErrorCount=%i\n", m_ErrorCount);
			break;
		}
//		if(n++ > 10000000) {
//			printf("error count=%i\n", m_ErrorCount);
//			n = 0;
//		}
	}
}
//----------------------------------------------------------------------------

void DScopeStream::GetFrame(DataFrame* &aDataFrame) {
	pthread_mutex_lock(&m_FrameLock);
	if (m_Q->IsEmpty()) {
		int err = 0;
		m_DataAccepted = true;
		while (m_DataAccepted) {
			err = pthread_cond_wait(&m_DataReady, &m_FrameLock);
			if (err)
				break;
		}
	}
	aDataFrame = m_Q->PullFront();
	pthread_mutex_unlock(&m_FrameLock);
}
//----------------------------------------------------------------------------
