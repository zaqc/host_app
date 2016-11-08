/*
 * Transmitter.cpp
 *
 *  Created on: Nov 8, 2016
 *      Author: zaqc
 */

#include "Transmitter.h"

#include <iostream>
#include <pthread.h>

#include <ftdi.h>
#include <sys/socket.h>

void * recv_thread_proc(void *arg) {
	((Transmitter*) arg)->RecvThread();
	return NULL;
}

void * send_thread_proc(void *arg) {
	((Transmitter*) arg)->SendThread();
	return NULL;
}

Transmitter::Transmitter() {
	pthread_mutex_init(&m_Lock, NULL);
	pthread_mutex_lock(&m_Lock);
	m_Running = false;
	if (0 == pthread_create(&m_RecvThread, NULL, &recv_thread_proc, this))
		m_Running = true;
	pthread_mutex_unlock(&m_Lock);

	int sock = socket(AF_INET, SOCK_DGRAM, 0);
	std::cout << sock << std::endl;
	shutdown(sock, SHUT_RDWR);
}

Transmitter::~Transmitter() {
}

void Transmitter::Init(void) {

}

void Transmitter::Done(void) {
	pthread_mutex_lock(&m_Lock);
	m_Running = false;
	pthread_mutex_unlock(&m_Lock);
}

void Transmitter::RecvThread(void) {
	while (true) {
		pthread_mutex_lock(&m_Lock);
		bool running = m_Running;
		pthread_mutex_unlock(&m_Lock);

		if (!running)
			break;
	}
}

void Transmitter::SendThread(void) {

}
