/*
 * XWnd.h
 *
 *  Created on: Mar 10, 2017
 *      Author: melan
 */

#ifndef XWND_H_
#define XWND_H_

#include <X11/Xlib.h>
#include  <X11/Xatom.h>
#include  <X11/Xutil.h>

class XWnd
{
protected:
	Display *m_hDisplay;
	Window m_hDesktop;
	Window m_hWnd;
	XEvent m_hEvent;
public:
	XWnd();
	virtual ~XWnd();
	bool Create();
	bool GetEvent(XEvent* hEvent);
	Display* GetDislpay() {return m_hDisplay;}
	Window GetWindow() {return m_hWnd;}
};

#endif /* XWND_H_ */
