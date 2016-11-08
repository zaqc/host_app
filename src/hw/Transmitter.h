/*
 * Transmitter.h
 *
 *  Created on: Nov 8, 2016
 *      Author: zaqc
 */

#ifndef TRANSMITTER_H_
#define TRANSMITTER_H_

#include <pthread.h>

class Transmitter {
protected:
	pthread_t m_RecvThread;
	pthread_mutex_t m_Lock;
	bool m_Running;
public:
	Transmitter();
	virtual ~Transmitter();

	void Init(void);
	void Done(void);

	void RecvThread(void);
	void SendThread(void);
};

#endif /* TRANSMITTER_H_ */
