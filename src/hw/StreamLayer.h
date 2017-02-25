/*
 * StreamLayer.h
 *
 *  Created on: May 20, 2016
 *      Author: zaqc
 */

#ifndef STREAMLAYER_H_
#define STREAMLAYER_H_

#include <ftdi.h>
#include <pthread.h>

#define	DS_SIDE_LEFT	0xA0
#define	DS_SIDE_RIGHT	0x0A
#define	DS_COMMAND		0xAA

#define	SS_WAIT_PREAMBLE	1
#define	SS_WAIT_SFD1		2
#define	SS_WAIT_SFD2		3
#define	SS_WAIT_SFD3		4
#define	SS_GET_ID			5
#define SS_GET_DATA_64		6
#define SS_GET_BLOCK_SIZE	7
#define SS_GET_CHANNEL_INFO	8
#define SS_GET_PR_ALL		9
#define SS_GET_MAGIC_WORD	10

#define	SS_ERROR			100

#define	PREAMBLE_BYTE		0x55
#define	SFD_BYTE1			0xFF
#define	SFD_BYTE2			0x00
#define	SFD_BYTE3			0xAA
#define	SFD_BYTE4			0x00

#define	END_BLOCK_MARKER	0xED
#define	END_FRAME_MARKER	0x77

#define	PREAMBLE_ALL		0x55555555
#define	SFD_MW				0x00AA00FF
//----------------------------------------------------------------------------

#define ds_addr(ch, param, addr) \
	((((unsigned int)ch << 28) & 0xF0000000) | \
	 (((unsigned int)param << 24) & 0x0F000000) | \
	 (((unsigned int)addr << 16) & 0x00FF0000))
//----------------------------------------------------------------------------

class StreamLayer {
protected:
	ftdi_context *m_FTDI;
	pthread_t m_RecvThread;

	unsigned char *m_Buf;
	int m_RealSize;
	bool m_Running;
	pthread_mutex_t m_Lock;

public:
	StreamLayer();
	virtual ~StreamLayer();

	void OpenStream();
	void CloseStream();

	unsigned char read_byte(void);
	unsigned int read_uint32(void);
	unsigned int calc_crc(unsigned char *buf, int size);
	void read_buf(unsigned char *buf, int size);

	void RecvThreadProc(void);

	int GetLatestData(unsigned char *aBuf);

	virtual void DecodePacket(unsigned int aCmd, unsigned char *aBuf, int aSize);

	void send_buf(unsigned char *buf, int size);

	void SendCommand(unsigned int aCmd, unsigned char aID);
};

#endif /* STREAMLAYER_H_ */
