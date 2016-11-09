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
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>

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
}

Transmitter::~Transmitter() {
	pthread_mutex_destroy(&m_Lock);
}

void Transmitter::Init(void) {

	ftdi_context *ftdi;
	ftdi = ftdi_new();
	if (!ftdi)
		std::cout << "Can't initialize libftdi1..." << std::endl;

	ftdi_version_info ver;
	ver = ftdi_get_library_version();
	std::cout << ver.version_str << std::endl;

	int res = ftdi_usb_open(ftdi, 0xeda3, 0x2179);
	std::cout << "usb_open res=" << res << std::endl;
	if(res < 0)
		std::cout << ftdi_get_error_string(ftdi) << std::endl;

	int sock = socket(AF_INET, SOCK_DGRAM, 0);
	std::cout << sock << std::endl;


	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(14232);
	addr.sin_addr.s_addr = inet_addr("192.168.1.22");
	unsigned char buf[1024];
	res = sendto(sock, buf, 1024, 0, (sockaddr*) &addr,
			sizeof(sockaddr_in));
	std::cout << res << std::endl;
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
