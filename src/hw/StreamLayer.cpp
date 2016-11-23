/*
 * StreamLayer.cpp
 *
 *  Created on: May 20, 2016
 *      Author: zaqc
 */

#include "StreamLayer.h"

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <unistd.h>
#include <memory.h>
#include <sys/types.h>
#include <ftdi.h>
//----------------------------------------------------------------------------

void * stream_layer_recv_thread_proc(void *arg) {

	((StreamLayer*) arg)->RecvThreadProc();

	return NULL;
}
//----------------------------------------------------------------------------

StreamLayer::StreamLayer() {
	if ((m_FTDI = ftdi_new()) == 0) {
		std::cout << "ftdi_new failed" << std::endl;
		throw "ftdi_new failed";
	}

	ftdi_set_interface(m_FTDI, INTERFACE_A);
	int f = ftdi_usb_open(m_FTDI, 0xEDA3, 0x2179); // 0x0403, 0x6010); //
	if (f < 0 && f != -5) {
		std::cout << "unable to open ftdi device: " << f
				<< ftdi_get_error_string(m_FTDI) << std::endl;
		ftdi_free(m_FTDI);
		throw "ftdi_new failed";
	}
	std::cout << "ftdi open succeeded(channel 1): " << f << std::endl;

	int res = ftdi_set_bitmode(m_FTDI, 0xFF, BITMODE_SYNCFF);
	std::cout << "ftdi_set_bitmode res=" << res << std::endl;

	res = ftdi_read_data_set_chunksize(m_FTDI, 64 * 1024);
	std::cout << "ftdi_read_data_set_chunksize res=" << res << std::endl;

	res = ftdi_usb_purge_buffers(m_FTDI);
	std::cout << "ftdi_usb_purge_buffers res=%i\n" << res << std::endl;

	m_Buf = new unsigned char[4096];
	pthread_mutex_init(&m_Lock, NULL);

	pthread_mutex_lock(&m_Lock);
	m_Running = false;
	if (0
			== pthread_create(&m_RecvThread, NULL,
					&stream_layer_recv_thread_proc, this))
		m_Running = true;
	pthread_mutex_unlock(&m_Lock);
}
//----------------------------------------------------------------------------

StreamLayer::~StreamLayer() {
	ftdi_usb_close(m_FTDI);
	pthread_join(m_RecvThread, NULL);

	pthread_mutex_destroy(&m_Lock);
	delete[] m_Buf;

	ftdi_set_bitmode(m_FTDI, 0xFF, BITMODE_RESET);
	ftdi_free(m_FTDI);
}
//----------------------------------------------------------------------------

unsigned char StreamLayer::read_byte(void) {
	unsigned char b;
	read_buf(&b, 1);
	return b;
}
//----------------------------------------------------------------------------

unsigned int StreamLayer::read_uint32(void) {
	unsigned int v;
	read_buf((unsigned char*) &v, 4);
	return v;
}
//----------------------------------------------------------------------------

void StreamLayer::read_buf(unsigned char *buf, int size) {
	int s = 0;
	int bc;
	while (s < size) {
		bc = ftdi_read_data(m_FTDI, &(buf[s]), size - s);
		if (bc < 0) {
			pthread_mutex_lock(&m_Lock);
			m_Running = false;
			pthread_mutex_unlock(&m_Lock);
			break;
		}
		if (bc == 0)
			usleep(100);
		else
			s += bc;
	}
}
//----------------------------------------------------------------------------

unsigned int StreamLayer::calc_crc(unsigned char *buf, int size) {
	unsigned int crc = 0;
	for (int i = 0; i < size; i++) {
		crc += ((i % 2) == 0) ? ~((unsigned int) buf[i]) : buf[i];
	}

	return crc;
}
//----------------------------------------------------------------------------

void StreamLayer::DecodePacket(unsigned int aCmd, unsigned char *aBuf,
		int aSize) {

}
//----------------------------------------------------------------------------

int StreamLayer::GetLatestData(unsigned char *aBuf) {
	pthread_mutex_lock(&m_Lock);
	int ret = m_RealSize;
	if (m_RealSize > 0)
		memcpy(aBuf, m_Buf, m_RealSize);
	m_RealSize = -1;
	pthread_mutex_unlock(&m_Lock);

	return ret;
}
//----------------------------------------------------------------------------

void StreamLayer::RecvThreadProc(void) {
	int state = SS_WAIT_PREAMBLE;
	int preable_count = 0;
	int real_size = 0;
	int error_count = 0;
	unsigned char buf[4096];
	unsigned char *ptr = buf;

	while (true) {
		unsigned char b;
		unsigned int w;

		pthread_mutex_lock(&m_Lock);
		bool running = m_Running;
		pthread_mutex_unlock(&m_Lock);
		if (!running)
			break;

		switch (state) {
		case SS_WAIT_PREAMBLE:
			b = read_byte();
			if (b == PREAMBLE_BYTE) {
				if (preable_count < 15)
					preable_count++;
				else {
					state = SS_WAIT_SFD1;
					preable_count = 0;
				}

			} else if (preable_count >= 4 && b == SFD_BYTE1) {
				state = SS_WAIT_SFD2;
				preable_count = 0;
			} else
				preable_count = 0;
			break;

		case SS_WAIT_SFD1:
			b = read_byte();
			if (b == SFD_BYTE1)
				state = SS_WAIT_SFD2;
			else
				state = SS_ERROR;
			break;

		case SS_WAIT_SFD2:
			b = read_byte();
			if (b == SFD_BYTE2)
				state = SS_WAIT_SFD3;
			else
				state = SS_ERROR;
			break;

		case SS_WAIT_SFD3:
			b = read_byte();
			if (b == SFD_BYTE3)
				state = SS_GET_ID;
			else
				state = SS_ERROR;
			break;

		case SS_GET_ID:
			b = read_byte();
			if (b == SFD_BYTE4)
				state = SS_GET_DATA_64;
			else
				state = SS_ERROR;
			break;

		case SS_GET_DATA_64:
			read_buf(ptr, 65 * 4);
			ptr += 65 * 4;
			state = SS_GET_BLOCK_SIZE;
			break;

		case SS_GET_BLOCK_SIZE:
			w = read_uint32();
			if ((w & 0xFF) == END_FRAME_MARKER) {
				real_size += (w & 0x0000FF00) >> 8;

				pthread_mutex_lock(&m_Lock);
				memcpy(m_Buf, buf, real_size * 4);
				m_RealSize = real_size * 4;
				pthread_mutex_unlock(&m_Lock);

				real_size = 0;
				ptr = buf;
				state = SS_GET_CHANNEL_INFO;
			} else {
				if ((w & 0xFF) == END_BLOCK_MARKER) {
					real_size += (w & 0xFF000000) >> 24;
					state = SS_GET_DATA_64;
				} else
					state = SS_ERROR;
			}
			break;

		case SS_GET_CHANNEL_INFO:
			read_buf(buf, 16 * 2);
			state = SS_GET_PR_ALL;
			break;

		case SS_GET_PR_ALL:
			read_buf(buf, 16);
			state = SS_GET_MAGIC_WORD;
			for (int i = 0; i < 4; i++) {
				if (((unsigned int *) buf)[i] != PREAMBLE_ALL)
					state = SS_ERROR;
			}
			break;

		case SS_GET_MAGIC_WORD:
			w = read_uint32();
			if (w == SFD_MW)
				state = SS_GET_DATA_64;
			else
				state = SS_ERROR;
			break;

		case SS_ERROR:
			preable_count = 0;
			error_count++;
			std::cout << error_count << " ";
			if (0 == (error_count % 32))
				std::cout << std::endl;
			state = SS_WAIT_PREAMBLE;
			break;
		}
	}
}
//----------------------------------------------------------------------------

void StreamLayer::send_buf(unsigned char *buf, int size) {
	int s = 0;
	int bc;
	while (s < size) {
		bc = ftdi_write_data(m_FTDI, &(buf[s]), size - s);
		if (bc < 0)
			break;
		if (bc == 0)
			usleep(100);
		else
			s += bc;
	}
}
//----------------------------------------------------------------------------

void StreamLayer::SendCommand(unsigned int aCmd, unsigned char aID) {
	unsigned char hdr[14];
	for (int i = 0; i < 8; i++)
		hdr[i] = PREAMBLE_BYTE;
	hdr[8] = 0xFF;
	hdr[9] = aID;
	hdr[10] = (aCmd >> 24) & 0xFF;
	hdr[11] = (aCmd >> 18) & 0xFF;
	hdr[12] = (aCmd >> 8) & 0xFF;
	hdr[13] = aCmd & 0xFF;
	send_buf(hdr, 14);
}
//----------------------------------------------------------------------------
