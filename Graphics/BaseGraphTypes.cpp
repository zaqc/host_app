#include "BaseGraphTypes.h"
#include "LogHelper.h"

//#include "HelperFunctions.h"

using namespace Graphics;
using namespace Media;

Texture::Texture()
	: m_ID(INVALID_HANDLE)
	, m_Width(0)
	, m_Height(0)
	, m_WrapSMode(GL_CLAMP_TO_EDGE)
	, m_WrapTMode(GL_CLAMP_TO_EDGE)
	, m_Format(GL_RGBA)
	, m_PixelSize(4)

{
}
Texture::~Texture()
{
	Delete();
}
bool Texture::Create(int aWidth, int aHeight, GLint aFormat)
{
	m_Width = aWidth;
	m_Height = aHeight;

	glGenTextures(1, &m_ID);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_ID);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_WrapSMode);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_WrapTMode);

	glTexImage2D(GL_TEXTURE_2D, 0, aFormat, aWidth, aHeight, 0, aFormat, GL_UNSIGNED_BYTE, nullptr);
	checkGlError("glTexImage2D");
	switch (aFormat)
	{
	case GL_RGBA: m_PixelSize = 4; break;
	case GL_LUMINANCE: m_PixelSize = 1; break;
	default: m_PixelSize = 4;
	}
	m_Format = aFormat;
	return true;
}
bool Texture::Create(int aWidth, int aHeight, uint32_t aFillColor, GLint aFormat)
{
	m_Width = aWidth;
	m_Height = aHeight;

	glGenTextures(1, &m_ID);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_ID);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_WrapSMode);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_WrapTMode);
	switch (aFormat)
	{
	case GL_RGBA: m_PixelSize = 4; break;
	case GL_LUMINANCE: m_PixelSize = 1; break;
	default: m_PixelSize = 4;
	}

	char* pData = new char[aWidth * aHeight * m_PixelSize];
	if (GL_RGBA == aFormat)
	{
		char* ptr = pData;
		for (int y = 0; y < aHeight; y++)
			for (int x = 0; x < aWidth; x++)
			{
				if (GL_RGBA == aFormat)
					*((uint32_t*)ptr) = aFillColor; //COLOR_ARGB(255, 255, x, y);
				ptr += m_PixelSize;
			}
	}
	else if (GL_LUMINANCE == aFormat)
		memset(pData, static_cast<char>(aFillColor), aWidth * aHeight);

	glTexImage2D(GL_TEXTURE_2D, 0, aFormat, aWidth, aHeight, 0, aFormat, GL_UNSIGNED_BYTE, pData);
	delete[] pData;
	checkGlError("glTexImage2D");
	m_Format = aFormat;
	return true;
}
bool Texture::Create(const Image* aImage)
{
	m_Format = GL_RGBA;
	m_Width = aImage->width;
	m_Height = aImage->height;

	glGenTextures(1, &m_ID);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_ID);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_WrapSMode);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_WrapTMode);

	glTexImage2D(GL_TEXTURE_2D, 0, m_Format, aImage->width, aImage->height, 0, m_Format, GL_UNSIGNED_BYTE, aImage->m_pPixels);
	m_PixelSize = 4;
	checkGlError("glTexImage2D");
	return true;
}

void Texture::Delete()
{
	glDeleteTextures(1, &m_ID);
}
