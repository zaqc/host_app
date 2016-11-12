/*
 * Transmitter.cpp
 *
 *  Created on: Nov 8, 2016
 *      Author: zaqc
 */

#include "Transmitter.h"
#include "StreamLayer.h"

#include <iostream>
#include <pthread.h>

#include <ftdi.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>

#include <unistd.h>

StreamLayer *stream_layer  = NULL;

void * transmitter_thread_proc(void *arg) {
	((Transmitter*) arg)->RecvThread();
	return NULL;
}

void * send_thread_proc(void *arg) {
	((Transmitter*) arg)->SendThread();
	return NULL;
}

Transmitter::Transmitter() {
	pthread_mutex_init(&m_Lock, NULL);
	m_Thread = 0;
	m_SL = NULL;
}

Transmitter::~Transmitter() {
	pthread_mutex_destroy(&m_Lock);
}

void Transmitter::Init(void) {
	m_SL = new StreamLayer();

	stream_layer = m_SL;

	m_Sock = socket(AF_INET, SOCK_DGRAM, 0);
	std::cout << m_Sock << std::endl;

	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(14232);
	addr.sin_addr.s_addr = inet_addr("192.168.1.22");
	unsigned char buf[1024];
	int res = sendto(m_Sock, buf, 1024, 0, (sockaddr*) &addr,
			sizeof(sockaddr_in));
	std::cout << res << std::endl;

	pthread_mutex_lock(&m_Lock);
	m_Running = false;
	if (0 == pthread_create(&m_Thread, NULL, &transmitter_thread_proc, this))
		m_Running = true;
	pthread_mutex_unlock(&m_Lock);
}

void Transmitter::Done(void) {
	pthread_mutex_lock(&m_Lock);
	m_Running = false;
	pthread_mutex_unlock(&m_Lock);
	pthread_join(m_Thread, NULL);

	shutdown(m_Sock, SHUT_RDWR);

	delete m_SL;
}

void Transmitter::RecvThread(void) {
	int nn = 0;
	while (true) {
		pthread_mutex_lock(&m_Lock);
		bool running = m_Running;
		pthread_mutex_unlock(&m_Lock);

		if (!running)
			break;

		usleep(1000000);
		unsigned int cmd;
		cmd = ds_addr(15, 13, 0) | (nn & 0x01) /*|  0x06;*/ | 0x10;
		nn = ~nn;
		m_SL->SendCommand(cmd, DS_SIDE_LEFT);

	}
}

void Transmitter::SendThread(void) {

}
