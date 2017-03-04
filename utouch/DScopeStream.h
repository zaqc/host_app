/*
 * DScopeStream.h
 *
 *  Created on: Feb 9, 2017
 *      Author: zaqc
 */

#ifndef SRC_DSCOPESTREAM_H_
#define SRC_DSCOPESTREAM_H_
//----------------------------------------------------------------------------

#include <ftdi.h>
#include <pthread.h>
#include <memory.h>
//----------------------------------------------------------------------------

#define	DSCOPE_PID	0xEDA3
#define	DSCOPE_VID	0x2179
//----------------------------------------------------------------------------

/**
 * structure for hold data frame from DScope (USB stream decode to it)
 */
struct DataFrame {
	//! hardware counter (how many "Sync" request processed) in Left side of DScope
	unsigned int m_LPktCounter;
	//! hardware counter (how many "Sync" request processed) in Right side of DScope
	unsigned int m_RPktCounter;
	//! channel present in data for Left Side
//	unsigned int m_LChMask;
//	//! channel present in data for Right Side
//	unsigned int m_RChMask;
//	//! each channel data letgth for Left Side
	int m_LChLen[16];
	//! each channel data letgth for Right Side
	int m_RChLen[16];
	//! buffer contains data from left side of DScope (in accordance with the m_LChMask & m_LChLen)
	unsigned char *m_LData;
	//! buffer contains data from right side of DScope (in accordance with the m_RChMask & m_RChLen)
	unsigned char *m_RData;
	//! length in byte of data in m_LData buffer
	int m_LDataLen;
	//! length in byte of data in m_RData buffer
	int m_RDataLen;

	void Create(void) {
		m_LData = new unsigned char[4096];
		m_RData = new unsigned char[4096];
	}

	void Destroy(void) {
		delete[] m_RData;
		delete[] m_LData;
	}

	void Clean(void) {
		m_LPktCounter = 0;
		m_RPktCounter = 0;
//		m_LChMask = 0;
//		m_RChMask = 0;
		memset(m_LData, 0, 4096);
		memset(m_RData, 0, 4096);
		for (int i = 0; i < 16; i++) {
			m_LChLen[i] = 0;
			m_RChLen[i] = 0;
		}
		m_LDataLen = 0;
		m_RDataLen = 0;
	}
};
//----------------------------------------------------------------------------

/**
 * Queue class for hold latest data from DScope
 */
class DataFrameQueue {
protected:
	DataFrame **m_Q;
	DataFrame *m_ExtFrame;
	DataFrame *m_NewFrame;
	//! size of created queue
	int m_QSize;
	//! frame count in queue
	int m_QLen;
	//! drop oldest frame if queue is full
	bool m_OverPush;

	bool m_ReadyToPush;
	int m_GetPtr;
	int m_PutPtr;
public:
	/**
	 * create queue for store data frame
	 * @param aQSize maximum sizr of queue
	 * @param aOverPush drop oldest frame if queue is full
	 */
	DataFrameQueue(int aQSize, bool aOverPush);
	virtual ~DataFrameQueue();

	/**
	 * get DataFrame* from front of queue
	 * @return DataFrame* from front of queue
	 */
	DataFrame *PullFront(void);
	/**
	 * Get DataFrame* from back of the queue for fill it with new data from DScope.
	 * Queue is not update length until call Push(void) method
	 * @return DataFrame* from back of queue
	 */
	DataFrame *GetBack(void);
	/**
	 * push data in to queue
	 */
	void Push(void);
	/**
	 * test is queue full
	 * @return true if queue is full
	 */
	bool IsFull(void) {
		return m_QLen == m_QSize;
	}
	/**
	 * test is queue empty
	 * @return true if queue is empty
	 */
	bool IsEmpty(void) {
		return m_QLen == 0;
	}

	/**
	 * queue length
	 * @return Length of queue (it's not size of queue (len < size))
	 */
	int GetLen(void) {
		return m_QLen;
	}
};
//----------------------------------------------------------------------------

enum StreamState {
	ssStateNone, ssWaitPreamble, ssGetPreamble, ssGetDataBlock, ssError
};
//----------------------------------------------------------------------------

/**
 * class for accept data from DScope and send control command to it
 * DScope contains two part left and right
 */
class DScopeStream {
protected:
	ftdi_context *m_FTDI;
	pthread_t m_Thread;
	pthread_cond_t m_DataReady;
	bool m_DataAccepted; // data accepted in GetFrame method
	pthread_mutex_t m_FrameLock;
	bool m_ThreadRunning;
	pthread_mutex_t m_ReadLock;

	DataFrame *m_Frame;

	DataFrameQueue *m_Q;

	StreamState m_StreamState;
	int m_PreambleCount;

	unsigned char *m_OutData;
	bool m_LeftEndMarker;
	bool m_RightEndMarker;

	unsigned int m_ErrorCount;
	unsigned int m_RecvFrameCount;

	int m_DecoLen;
	int m_AlignShift;

	void UpdateFrame(bool aPush);
	/**
	 * Decode received data (aligned by 5 bytes)
	 * @param aBuf input data from USB
	 * @param aSize size of buffer
	 * @return value != 0 if error occurred
	 */
	int DecodeBuffer(unsigned char *aBuf, int aSize);
	void DecodeStream(void);
	int RecvBuf(unsigned char *aBuf, int aSize, bool aWait);
public:
	DScopeStream();
	virtual ~DScopeStream();

	/**
	 * method which called from recv_thread function
	 */
	void RecvThread(void);

	/**
	 * Get latest data frame received from both part of DScope
	 * @param aDataFrame return data frame if data present (caller thread is paused if data are not ready yet)
	 */
	void GetFrame(DataFrame* &aDataFrame);

	/**
	 * Get frame count in Queue
	 * @return count of frames that are stored in queue
	 */
	int GetFrameCount(void);
};
//----------------------------------------------------------------------------

#endif /* SRC_DSCOPESTREAM_H_ */
