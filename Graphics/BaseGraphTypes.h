#pragma once

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include "common_def.h"
#include "Image.h"

#define COLOR_ARGB(a,r,g,b) ((uint32_t)((((a)&0xff)<<24)|(((b)&0xff)<<16)|(((g)&0xff)<<8)|((r)&0xff)))
#define COLOR_RGBA(a,r,g,b) ((uint32_t)((((a)&0xff)<<24)|(((r)&0xff)<<16)|(((g)&0xff)<<8)|((b)&0xff)))

namespace Graphics
{
	struct Rect
	{
		int Left;
		int Top;
		int Right;
		int Bottom;
		int Height() const { return ABS(Bottom - Top); }
		int Width() const { return ABS(Right - Left); }
		Rect()
		{}
		Rect(int aLeft, int aTop, int aRight, int aBottom)
			: Left(aLeft)
			, Top(aTop)
			, Right(aRight)
			, Bottom(aBottom)
		{}
		bool Contains(int X, int Y)
		{
			return X >= Left && X <= Right && Y >= Top && Y <= Bottom;
		}
	};

	class Texture
	{
	public:
		GLuint		m_ID;
		int			m_Width;
		int			m_Height;
	protected:
		GLint		m_WrapSMode;
		GLint		m_WrapTMode;
		GLint		m_Format;
		int			m_PixelSize;
	public:
		Texture();
		~Texture();
		bool Create(int aWidth, int aHeight, GLint aFormat = GL_RGBA);
		bool Create(int aWidth, int aHeight, uint32_t aFillColor, GLint aFormat = GL_RGBA);
		bool Create(const Media::Image* aImage);
		void Delete();
		int GetPixelSize() { return m_PixelSize; }
	};

}

