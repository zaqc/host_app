#pragma once
#ifndef GusPacketH
#define GusPacketH

//----------------------------------------------------------------------------
// #include "SpeedSmoother.h"
// #include <deque>
// #include "GusProcess.h"
// #include "BaseSysEx.h"
// #include "SysPacket.h"

#include <cstdint>

#define	US_TICK_COUNT		128
#define US_CHANNEL_COUNT	18

//----------------------------------------------------------------------------
#pragma pack(push, 1)
struct GusPacket
{
	int32_t Size;
	uint32_t ChMask;
	uint32_t MasterFrame;
	int32_t MGData[20];
	uint8_t MGCount; //количество отсчетов магнитки

	struct USStruct
	{
		uint32_t TuneParam[2];
		uint8_t Data[US_TICK_COUNT];

		USStruct()
			: TuneParam()
			, Data()
		{};
	};

	USStruct USData[ US_CHANNEL_COUNT ];
	int32_t TTL;
public:
	GusPacket()
		: Size(0)
		, ChMask(0)
		, MasterFrame(0)
		, MGData()
		, MGCount(0)
		, USData()
		, TTL(0xDEADBEAF)
	{};
	// inline int GetPacketType(void) { return PACKET_TYPE_GUS; }

	// void FromWSABUF(WSABUF *aBuf)
	// {
		// Size = aBuf->len;
		// unsigned char *ptr = (unsigned char *)aBuf->buf;
		// ChMask = *(unsigned int*)ptr; ptr += sizeof(unsigned int);
		// MGCount = static_cast<char>(ChMask >> 24); //прочитать количество отсчетов для каждого магнитного канала
		// if(0 == MGCount) //если 0, значит берем значение по умолчанию 5 мм
			// MGCount = 5;

		// MasterFrame = *(unsigned int*)ptr; ptr += sizeof(unsigned int);
		// int MGDataSize = sizeof(MGData) / sizeof(MGData[0]) / 2;
		// memcpy(MGData, ptr, sizeof(int) * (MGCount > MGDataSize ? MGDataSize : MGCount) * 2);
		// ptr += sizeof(int) * MGCount * 2;
// //		for(int mch=0; mch<10; mch++)
// //			MGData[mch] = MGData[mch] & 0x00FFFFFF;
		// unsigned int mask = 0x00000001;
		// USStruct *us_ptr = USData;
		// memset(us_ptr, 0, sizeof(USData));
		// //ptr += 4; //for fixing floating amp and vrc in buggy def
		// for(int ch = 0; ch < US_CHANNEL_COUNT; ch++)
		// {
			// if(mask & ChMask)
			// {
				// memcpy(us_ptr, ptr, sizeof(USStruct));
				// ptr += sizeof(USStruct);
				// us_ptr++;
			// }
			// mask <<= 1;
		// }
	// }
};

struct GusTuneParam
{
	unsigned char AmpOne;		// 0-255
	unsigned char AmpTwo;
	unsigned char VRCLen;		// 0-50 (для РС 0-25)
	unsigned char IsRS;			// RS
	unsigned char PulseTime;	// 8.3(3) нс
	unsigned short Delay;		// 0.08(3) мкс

public:
	void FromData(unsigned char *aBuf)
	{
		AmpOne = *aBuf++;
		AmpTwo = *aBuf++;
		VRCLen = *aBuf & 0x7F;
		IsRS = *aBuf++ >> 7;
		PulseTime = *aBuf++;
		Delay = *(short *)aBuf;
	}
};
#pragma pack(pop)

#endif
