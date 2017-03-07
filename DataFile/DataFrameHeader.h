#pragma once
#ifndef DataframeheaderH
#define DataframeheaderH

#include <cstdint>

namespace DataFile
{

#pragma pack(push, 1)
struct BaseFrameHeader
{
	uint32_t Masterframe;
	uint16_t Speed;
	uint16_t Buttons;
	int32_t Size;

	BaseFrameHeader()
		: Masterframe(0)
		, Speed(0)
		, Buttons(0)
		, Size(0)
	{};
};
#pragma pack(pop)
};
#endif
