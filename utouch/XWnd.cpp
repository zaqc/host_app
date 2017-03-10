/*
 * XWnd.cpp
 *
 *  Created on: Mar 10, 2017
 *      Author: denis
 */

#include "XWnd.h"

XWnd::XWnd() {
	// TODO Auto-generated constructor stub

}

XWnd::~XWnd()
{
	XDestroyWindow(m_hDisplay, m_hWnd);
}

bool XWnd::Create()
{
	m_hDisplay = XOpenDisplay(NULL);

	m_hDesktop = DefaultRootWindow(m_hDisplay);

	XWindowAttributes attr;
	XGetWindowAttributes(m_hDisplay, m_hDesktop, &attr);

	m_hWnd = XCreateSimpleWindow(m_hDisplay, m_hDesktop, 0, 0, 800, 480, 8, 0, 255);
	XSetWindowAttributes xattr;
	xattr.override_redirect = False;
	XChangeWindowAttributes(m_hDisplay, m_hWnd, CWOverrideRedirect, &xattr);

	XSelectInput(m_hDisplay, m_hWnd, ExposureMask |
	KeyPressMask | KeyReleaseMask |
	ButtonPressMask | ButtonReleaseMask | PointerMotionMask);

	Atom window_type = XInternAtom(m_hDisplay, "_NET_WM_WINDOW_TYPE", False);
	long value = XInternAtom(m_hDisplay, "_NET_WM_WINDOW_TYPE_DOCK", False);
	XChangeProperty(m_hDisplay, m_hWnd, window_type, XA_ATOM, 32, PropModeReplace, (unsigned char *) &value, 1);

	XMapWindow(m_hDisplay, m_hWnd);
	XMoveWindow(m_hDisplay, m_hWnd, (attr.width - 800) / 2, (attr.height - 480) / 2);

	Atom WM_DELETE_WINDOW = XInternAtom(m_hDisplay, "WM_DELETE_WINDOW", False);
	XSetWMProtocols(m_hDisplay, m_hWnd, &WM_DELETE_WINDOW, 1);
}
bool XWnd::GetEvent(XEvent* hEvent)
{
	if (XPending(m_hDisplay))
	{
		XNextEvent(m_hDisplay, &m_hEvent);
		return true;
	}
	return false;
}
