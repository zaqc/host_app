#pragma once
#ifndef COMMON_DEF_H
#define COMMON_DEF_H

#if (((defined WIN32) || (defined _WIN32) || (defined _WIN64)) && !defined __ANDROID_API__ && !defined __linux__)
	#define WINDOWS_OS
#else
	#define LINUX_OS
#endif

#define INVALID_HANDLE 0xFFFFFFFF



#define ABS(a) ((a) < 0 ? -(a) : (a))
#define MIN(a,b) ((a) <= (b) ? (a) : (b))
#define MAX(a,b) ((a) >= (b) ? (a) : (b))

#include <cstring>
#include <cstdint>
#include <cassert>

#ifdef LINUX_OS
#ifdef __cplusplus
#undef __cplusplus
#define  __cplusplus 201304L
#endif
typedef struct _GUID
{
	uint32_t Data1;
	uint16_t Data2;
	uint16_t Data3;
	uint8_t Data4[8];

	bool operator== (const _GUID& aGuid) const
	{
		for(size_t i{0}; i < 8; ++i)
		{
			if(aGuid.Data4[i] != Data4[i])
				return false;
		}

		return (aGuid.Data1 == Data1 && aGuid.Data2 == Data2 && aGuid.Data3 == Data3);
	}
} GUID;

extern GUID GUID_NULL;

typedef int SOCKET;
//typedef void HANDLE;

#define strcpy_s(a, b, c) strcpy(a, c)
#define strncpy_s(a, b, c, d) strncpy(a, c, d)
#define wcscpy_s(a, b, c) wcscpy(a, c)
#define sprintf_s(a, b, c, ...) sprintf(a, c, __VA_ARGS__)
#define swprintf_s(a, b, c, ...) swprintf(a, b, c, __VA_ARGS__)
#define sscanf_s(a, b, ...) sscanf(a, b, __VA_ARGS__)
#endif

#endif // !COMMON_DEF_H
