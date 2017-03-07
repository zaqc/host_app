#include "EGLContext.h"
#include <GLES2\gl2.h>
using namespace Graphics;

Context::Context(EGLNativeWindowType ahWindow, EGLNativeDisplayType ahDisplay, int aWidth, int aHeight)
	: m_hWnd(ahWindow)
	, m_bErrorStatus(true)
	, m_Width(aWidth)
	, m_Height(aHeight)

{
	
	m_Display = eglGetDisplay(ahDisplay);
	if(m_Display == EGL_NO_DISPLAY)
	{
		m_bErrorStatus = false;
		return;
	}
	GLint majorVersion, minorVersion;
	if (!eglInitialize(m_Display, &majorVersion, &minorVersion))
	{
		m_bErrorStatus = false;
		return;
	}
	EGLint attributes[] = {             // Request OpenGL ES 2.0 configs                 
		EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,              
		EGL_SURFACE_TYPE, EGL_WINDOW_BIT,            
		EGL_RED_SIZE, 8,                 
		EGL_GREEN_SIZE, 8,                 
		EGL_BLUE_SIZE, 8,                 
		EGL_NONE         
	};  
	EGLint num_configs;  
	eglChooseConfig(m_Display, attributes, &m_Config, 1, &num_configs); 
	m_Surface = eglCreateWindowSurface(m_Display, m_Config, m_hWnd, NULL);
	EGLint context_attributes[] = { EGL_CONTEXT_CLIENT_VERSION, 2,  // Select an OpenGL ES 2.0 context          
		EGL_NONE      
	};  
	m_Context = eglCreateContext(m_Display, m_Config, EGL_NO_CONTEXT, context_attributes);
	eglMakeCurrent(m_Display, m_Surface, m_Surface, m_Context);
	eglSwapInterval(m_Display, 1);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);
	glViewport(0, 0, m_Width, m_Height);
}
Context::~Context()
{
	eglDestroyContext(m_Display, m_Context);
	eglDestroySurface(m_Display, m_Surface);
	eglTerminate(m_Display);
}
bool Context::HasError()
{
	return !m_bErrorStatus;
}
void Context::Render()
{
	if (!m_bErrorStatus)
		return;
	eglSwapBuffers(m_Display, m_Surface);
}