#include "ChannelTexture.h"
#include "LogHelper.h"
#include "HelperFunctions.h"

using namespace Graphics;

ChannelTexture::ChannelTexture(int aChannelNumber)
	:Texture()
	, m_DrawOffsetX(0)
	, m_DrawOffsetY(0)
	, m_pTempStorage(nullptr)
	, m_TempStorageSize(0)
	, m_ChannelNumber(aChannelNumber)
	, m_ChannelColor(0)
{
	m_WrapSMode = GL_CLAMP_TO_EDGE;
	m_WrapTMode = GL_REPEAT;
}

ChannelTexture::~ChannelTexture()
{
	SAFE_ARRAY_DELETE(m_pTempStorage);
}

void ChannelTexture::SetOffset(int aOffsetX, int aOffsetY)
{
	m_DrawOffsetX = aOffsetX;
	m_DrawOffsetY = aOffsetY;
	while (aOffsetX > m_Width)
		aOffsetX -= m_Width;
	while (aOffsetX < -m_Width)
		aOffsetX += m_Width;
}

void ChannelTexture::AddOffset(int aOffsetX, int aOffsetY)
{
	m_DrawOffsetX += aOffsetX;
	m_DrawOffsetY += aOffsetY;
	while (m_DrawOffsetX > m_Width)
		m_DrawOffsetX -= m_Width;
	while (m_DrawOffsetX < -m_Width)
		m_DrawOffsetX += m_Width;
}

void ChannelTexture::OffsetX(int aDXOffset, const char* pSrcBuffer)
{
	//glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	while (aDXOffset > m_Width)
	{
		aDXOffset -= m_Width;
		pSrcBuffer += m_Width * m_Height * 4;
	}
	int offset = m_DrawOffsetX;
	if (offset < 0) //переведем отрицательный сдвиг текстуры при рендере, в ’-координату первой видимой на экране точки
		offset += m_Width;
	offset = aDXOffset > 0 ? offset : offset + aDXOffset;
	if (offset < 0)
		offset += m_Width;

	int rest = 0;
	int width = ABS(aDXOffset);

	if (offset + width >= m_Width)
	{
		rest = offset + width - m_Width;
		width -= rest;
	}
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_ID);
	//если копируемый блок целиком помещаетс€ на текстуру, копируем за один раз
	if (0 == rest)
	{
		glTexSubImage2D(GL_TEXTURE_2D, 0, offset, 0, width, m_Height, m_Format, GL_UNSIGNED_BYTE, pSrcBuffer);
		checkGlError("glTexSubImage2D");
	}
	else //если копируемый блок не помещаетс€ целиком на текстуру, копируем за 2 подхода, сначала первую часть буфера копируем 
		 //во временный буфер равный ширине текстуры от offset до m_Width и переносим его на текстуру с позиции offset, затем оставшуюс€ часть буфера копируем
		 //во временный буфер равный width - (m_Width-offset) и переносим его на текстуру с позиции 0. 
		 //“аким образом реализуетс€ √ќ–»«ќЌ“јЋ№Ќјя циклическа€ текстура
	{
		if (m_TempStorageSize < std::max(rest, width) * m_Height * m_PixelSize)
		{
			SAFE_ARRAY_DELETE(m_pTempStorage);
			m_TempStorageSize = std::max(rest, width) * m_Height * m_PixelSize;
			m_pTempStorage = new char[m_TempStorageSize];
			LOGE("Allocate %d bytes for texture (ID=%d) temporary storage", m_TempStorageSize, m_ID);
		}
		char* p = m_pTempStorage;
		const char* p1 = pSrcBuffer;
		int strideDst = ABS(aDXOffset) * m_PixelSize;
		int strideSrc = width * m_PixelSize;
		for (int i = 0; i < m_Height; i++)
		{
			memcpy(p, p1, strideSrc);
			p1 += strideDst;
			p += strideSrc;
		}
		glTexSubImage2D(GL_TEXTURE_2D, 0, offset, 0, width, m_Height, m_Format, GL_UNSIGNED_BYTE, m_pTempStorage);
		checkGlError("glTexSubImage2D");
		p = m_pTempStorage;
		p1 = pSrcBuffer;
		int strideSrcRest = rest * m_PixelSize;
		for (int i = 0; i < m_Height; i++)
		{
			memcpy(p, p1 + strideSrc, strideSrcRest);
			p1 += strideDst;
			p += strideSrcRest;
		}
		p = m_pTempStorage;
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, rest, m_Height, m_Format, GL_UNSIGNED_BYTE, m_pTempStorage);
		checkGlError("rest glTexSubImage2D");
	}
	m_DrawOffsetX += aDXOffset;
	while (m_DrawOffsetX >= m_Width)
		m_DrawOffsetX -= m_Width;
	while (m_DrawOffsetX <= -m_Width)
		m_DrawOffsetX += m_Width;
}

void ChannelTexture::OffsetY(int aDYOffset, const char* pSrcBuffer)
{
	//glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	while (aDYOffset > m_Height)
	{
		aDYOffset -= m_Height;
		pSrcBuffer += m_Width * m_Height * m_PixelSize;
	}
	int offset = m_DrawOffsetY;
	if (offset < 0) //переведем отрицательный сдвиг текстуры при рендере, в Y-координату первой видимой на экране линии
		offset += m_Height;
	offset = aDYOffset > 0 ? offset : offset + aDYOffset;
	if (offset < 0)
		offset += m_Height;

	int rest = 0;
	int height = ABS(aDYOffset);

	if (offset + height >= m_Height)
	{
		rest = offset + height - m_Height;
		height -= rest;
	}
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_ID);
	//если копируемый блок целиком помещаетс€ на текстуру, копируем за один раз
	if (0 == rest)
	{
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, offset, m_Width, height, m_Format, GL_UNSIGNED_BYTE, pSrcBuffer);
		checkGlError("glTexSubImage2D");
	}
	else //если копируемый блок не помещаетс€ целиком на текстуру, копируем за 2 подхода, сначала первую часть буфера 
		 //переносим на текстуру с позиции offset, затем оставшуюс€ часть буфера переносим на текстуру с позиции 0. 
		 //“аким образом реализуетс€ ¬≈–“» јЋ№Ќјя циклическа€ текстура
	{
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, offset, m_Width, height, m_Format, GL_UNSIGNED_BYTE, pSrcBuffer);
		checkGlError("glTexSubImage2D");

		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_Width, rest, m_Format, GL_UNSIGNED_BYTE, pSrcBuffer + height * m_Width * m_PixelSize);
		checkGlError("rest glTexSubImage2D");
	}
	m_DrawOffsetY += aDYOffset;
	while (m_DrawOffsetY >= m_Height)
		m_DrawOffsetY -= m_Height;
	while (m_DrawOffsetY <= -m_Height)
		m_DrawOffsetY += m_Height;
}
