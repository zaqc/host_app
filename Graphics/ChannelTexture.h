#pragma once
#include "BaseGraphTypes.h"
#include <cstdint>

namespace Graphics
{
	class ChannelTexture : public Texture
	{
	public:
		int		m_DrawOffsetX;
		int		m_DrawOffsetY;
		int		m_ChannelNumber;
		std::uint32_t m_ChannelColor;

	protected:
		char*	m_pTempStorage;
		int		m_TempStorageSize;

	public:
		explicit ChannelTexture(int aChannelNumber);
		~ChannelTexture();
		void SetOffset(int aOffsetX, int aOffsetY);
		void AddOffset(int aOffsetX, int aOffsetY);
		void OffsetX(int aDXOffset, const char* pSrcBuffer);
		void OffsetY(int aDYOffset, const char* pSrcBuffer);
	};
}