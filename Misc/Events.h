/*
 * WIN32 Events for POSIX
 * Author: Mahmoud Al-Qudsi <mqudsi@neosmart.net>
 * Copyright (C) 2011 - 2015 by NeoSmart Technologies
 * This code is released under the terms of the MIT License
*/

#pragma once


#ifndef WAIT_TIMEOUT
#include <errno.h>
#define WAIT_TIMEOUT							ETIMEDOUT
#define STATUS_WAIT_0                           0x00000000L
#define WAIT_OBJECT_0							((STATUS_WAIT_0 ) + 0 )
#define INFINITE 0xFFFFFFFF

#endif

#include <stdint.h>

namespace Events
{
	//Type declarations
	struct event_t_;
	typedef event_t_ * HANDLE;
	
	//WIN32-style functions
	HANDLE CreateEvent(bool manualReset = false, bool initialState = false);
	int DestroyEvent(HANDLE hEvent);
	int WaitForSingleObject(HANDLE hEvent, uint32_t milliseconds = INFINITE);
	int SetEvent(HANDLE hEvent);
	int ResetEvent(HANDLE hEvent);
	int WaitForMultipleObjects(int count, HANDLE *events, bool waitAll, uint32_t milliseconds, int &index);
	//POSIX-style functions
	//TBD
}
