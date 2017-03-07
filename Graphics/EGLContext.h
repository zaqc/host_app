#pragma once
#include <EGL\eglplatform.h>
#include <EGL\egl.h>
#include <GLES2\gl2.h>
#include "common_def.h"


namespace Graphics
{
	class Context
	{
	private:
		EGLDisplay m_Display;
		EGLConfig m_Config;
		EGLNativeWindowType m_hWnd;
		EGLSurface m_Surface;
		EGLContext m_Context;
		int m_Width;
		int m_Height;
		bool m_bErrorStatus; //true - no error, false - has error
	public:
		Context(EGLNativeWindowType ahWindow, EGLNativeDisplayType ahDisplay, int aWidth, int aHeight);
		~Context();
		bool HasError();
		void Render();
	};
}
