//============================================================================
// Name        : utouch.cpp
// Author      : zaqc
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C, Ansi-style
//============================================================================

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <memory.h>

#include <GLES2/gl2.h>
#include <EGL/eglplatform.h>
#include <EGL/egl.h>

#include <X11/Xlib.h>
#include  <X11/Xatom.h>
#include  <X11/Xutil.h>

#include "Util.h"
#include "GlUtil.h"
#include "TextScroller.h"
#include "DScopeStream.h"
#include "TextAScan.h"
#include "TextFont.h"
#include "SmallFont.h"
#include "TextDraw.h"
#include "TTFont.h"
#include "ChannelTag.h"
#include "ChannelInfo.h"
//----------------------------------------------------------------------------

//bool setupGraphics(int w, int h);
//void renderFrame();

int main(void) {
	puts("!!!Hello World!!!");

	//read_png_file("/home/zaqc/work/png/cat_eat.png");
	read_png_file((char *) "cat_eat.png");

	DScopeStream *dss = new DScopeStream();

	Display *x_disp;
	x_disp = XOpenDisplay(NULL);

	Window x_desktop;
	x_desktop = DefaultRootWindow(x_disp);

	XWindowAttributes attr;
	XGetWindowAttributes(x_disp, x_desktop, &attr);

	Window x_wnd = XCreateSimpleWindow(x_disp, x_desktop, 0, 0, 800, 480, 8, 0, 255);
	//eglInitialize()

	XSetWindowAttributes xattr;
	xattr.override_redirect = False;
	XChangeWindowAttributes(x_disp, x_wnd, CWOverrideRedirect, &xattr);

	XSelectInput(x_disp, x_wnd, ExposureMask |
	KeyPressMask | KeyReleaseMask |
	ButtonPressMask | ButtonReleaseMask | PointerMotionMask);

	//	Atom atom = XInternAtom(x_disp, "_NET_WM_STATE_FULLSCREEN", true);
	//	XChangeProperty(x_disp, x_wnd, XInternAtom(x_disp, "_NET_WM_STATE", true), XA_ATOM, 32, PropModeReplace, (unsigned char *) &atom, 1);

	Atom window_type = XInternAtom(x_disp, "_NET_WM_WINDOW_TYPE", False);
	long value = XInternAtom(x_disp, "_NET_WM_WINDOW_TYPE_DOCK", False);
	XChangeProperty(x_disp, x_wnd, window_type, XA_ATOM, 32, PropModeReplace, (unsigned char *) &value, 1);

	XMapWindow(x_disp, x_wnd);
	XMoveWindow(x_disp, x_wnd, (attr.width - 800) / 2, (attr.height - 480) / 2);

	Atom WM_DELETE_WINDOW = XInternAtom(x_disp, "WM_DELETE_WINDOW", False);
	XSetWMProtocols(x_disp, x_wnd, &WM_DELETE_WINDOW, 1);

	EGLint majorVersion;
	EGLint minorVersion;
	EGLDisplay __egl_display;
	__egl_display = (EGLDisplay) eglGetDisplay(x_disp);

	// most egl you can sends NULLS for maj/min but not RPi
	if (!eglInitialize(__egl_display, &majorVersion, &minorVersion)) {
		printf("Unable to initialize EGL\n");
		return 1;
	}

	EGLint _attr[] = {	// some attributes to set up our egl-interface
			EGL_BUFFER_SIZE, 16,
			EGL_DEPTH_SIZE, 16,
			EGL_RENDERABLE_TYPE,
			EGL_OPENGL_ES2_BIT,
			EGL_NONE };
	EGLConfig ecfg;
	EGLint num_config;
	if (!eglChooseConfig(__egl_display, _attr, &ecfg, 1, &num_config)) {
		//cerr << "Failed to choose config (eglError: " << eglGetError() << ")" << endl;
		printf("failed to choose config eglerror:%i\n", eglGetError());	// todo change error number to text error
		return 1;
	}

	EGLSurface surf = eglCreateWindowSurface(__egl_display, ecfg, x_wnd, NULL);

//	const EGLint srfPbufferAttr[] = {
//	EGL_WIDTH, 800,
//	EGL_HEIGHT, 480,
//	EGL_COLORSPACE, GL_RGBA,
//	EGL_TEXTURE_FORMAT, EGL_TEXTURE_RGBA,
//	EGL_TEXTURE_TARGET, EGL_TEXTURE_2D,
//	EGL_LARGEST_PBUFFER, EGL_TRUE,
//	EGL_NONE };
//	EGLSurface surf;// = eglCreatePbufferSurface( __egl_display, ecfg, srfPbufferAttr);
//	surf = eglGetCurrentSurface(EGL_DRAW);

	EGLint ctxattr[] = {
	EGL_CONTEXT_CLIENT_VERSION, 2,
	EGL_NONE };

	EGLContext __egl_context = eglCreateContext(__egl_display, ecfg,
	EGL_NO_CONTEXT, ctxattr);
	if (__egl_context == EGL_NO_CONTEXT) {
		//cerr << "Unable to create EGL context (eglError: " << eglGetError() << ")" << endl;
		printf("unable to create EGL context eglerror:%i\n", eglGetError());
		return 1;
	}

	eglMakeCurrent(__egl_display, surf, surf, __egl_context);

	//setupGraphics(800, 480);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	//glDisable(GL_DEPTH_TEST);

	//glEnable(GL_DEPTH_TEST);

	font = new TextFont();
	small_font = new SmallFont();
	draw = new TextDraw();

	TextScroller *tscroll = new TextScroller();
	tscroll->InitProgram();
	tscroll->Init();

	timeval ts, ts_prev;
	gettimeofday(&ts, 0);
	int fc = 0;
	ts_prev = ts;

	eglSwapInterval(__egl_display, 1);

	TextAScan *a_scan = new TextAScan();

	bool show_a_scan = false;

	TTFont *ttfont = new TTFont();

	TTString l70 = ttfont->PrepTTString(L"↖70°");
	TTString up70 = ttfont->PrepTTString(L"↑70°");
	TTString r70 = ttfont->PrepTTString(L"↗70°");

	TTString d70_45 = ttfont->PrepTTString(L"70° 45°");

	ChannelTag *ch_tag = new ChannelTag();
	ch_tag->InitTexture();

	ChannelInfo *ci = new ChannelInfo();

	XEvent x_event;
	while (true) {
		if (XPending(x_disp)) {
			XNextEvent(x_disp, &x_event);

			if (x_event.type == ClientMessage) {
				printf("Client Message\n");
				break;
			}
			if (x_event.type == ButtonPress) {
				printf("Trying to close full screen mode\n");
				//				Atom atom = XInternAtom(x_disp, "_NET_WM_DESKTOP", true);
				//				XChangeProperty(x_disp, x_desktop, XInternAtom(x_disp, "_NET_WM_DESKTOP", false),
				//				XA_ATOM, 32, PropModeReplace, (unsigned char *) &atom, 1);
				break;
			}

		}

//		if (dss->GetFrameCount()) {
//			DataFrame *df;
//			dss->GetFrame(df);
//			tscroll->RenderFrame(df->m_RData);
//		}
//		else
//			usleep(10000);

		//renderFrame();

		if (dss) {
			unsigned int key = dss->GetKey();
			if (key == BTN_HOME) {
				show_a_scan = !show_a_scan;
				key = 0;
			}
			else if (key == BTN_CANCEL) {
				dss->PrintInfo();
			}

			if (show_a_scan) {
				a_scan->ProcessButton(key);

				a_scan->FillRect(0, 0, 800, 480);
				a_scan->DrawBuf(dss, 30, 10, 770, 440);
			}
			else {
				tscroll->RenderFrame(dss);
			}
		}
		else {
			a_scan->FillRect(0, 0, 800, 480);
			for (int j = 0; j < 30; j++)
				for (int i = 0; i < 4; i++)
					font->RenderString(i * 200, j * 16, (char*) "String render slow slow...");
			font->RenderString(10, 50, (char*) "this is string for flush out text...");

			font->FlushText();
		}

		//draw->DrawGrid(10, 10, 790, 450, false);

		//tscroll->RenderFrame(dss);

		//renderFrame();

		/*
		 draw->SetColor(0.0, 1.0, 0.0, 1.0);
		 draw->FillRect(0, 0, 129 + 32, 480);
		 draw->FillRect(799 - 129 - 32, 0, 799, 480);
		 draw->SetColor(0.0, 0.0, 0.25, 1.0);
		 for(int i = 0; i < 9; i++) {
		 int y1 = (int)((float)i * (480.0 / 9.0));
		 int y2 = (int)((float)(i + 1) * (480.0 / 9.0) - 1.0);
		 draw->FillRect(32, y1, 32 + 128, y2);
		 draw->FillRect(799 - 128 - 32, y1, 799 - 32, y2);
		 //draw->FillRect(129, y1, 129 + 128, y2);
		 //draw->FillRect(799 - 128 - 129, y1, 799 - 129, y2);
		 }
		 */

//		draw->SetColor(0.0, 0.0, .25, .75);
//		draw->FillRect(30, 10, 34 + up70.m_W, 14 + up70.m_H);
		for (int i = 0; i < 8; i++) {
			if (i == 2 || i == 0)
				draw->SetColor(1.0, 0.0, 0.0, 1.0);
			else
				draw->SetColor(.25, .25, .25, .75);
			draw->FillRect(4, i * 480 / 8, 144, (i + 1) * 480 / 8 - 4);
			draw->SetColor(1.0, 1.0, 1.0, .75);
			draw->DrawRect(4, i * 480 / 8, 144, (i + 1) * 480 / 8 - 4);
			if (i == 0) {
				ttfont->SetColor(0.0, 1.0, 0.0);
				ttfont->Render(4 + 70 - up70.m_W / 2, 4 + i * 480 / 8, &up70);
			}
			else if (i == 1 || i == 2) {
				ttfont->SetColor(1.0, 1.0, 0.0);
				ttfont->Render(6, 4 + i * 480 / 8, &l70);
				ttfont->SetColor(0.0, 1.0, 1.0);
				ttfont->Render(142 - r70.m_W, 4 + i * 480 / 8, &r70);
			}
			else if (i == 3) {
				ttfont->SetColor(0.0, 1.0, 0.0);
				ttfont->Render(6, 4 + i * 480 / 8, &l70);
				ttfont->SetColor(1.0, 0.0, 1.0);
				ttfont->Render(142 - r70.m_W, 4 + i * 480 / 8, &r70);
			}
		}

		for (int i = 0; i < 13; i++) {
			draw->SetColor(.25, .25, .25, .75);
			draw->FillRect(700, i * 480 / 13, 798, (i + 1) * 480 / 13 - 4);
			draw->SetColor(1.0, 1.0, 1.0, .75);
			draw->DrawRect(700, i * 480 / 13, 798, (i + 1) * 480 / 13 - 4);
			if (i == 0) {
				ttfont->SetColor(0.0, 1.0, 0.0);
				ttfont->Render(706, 4 + i * 480 / 13, &up70);
			}
			else if (i == 1) {
				ttfont->SetColor(0.0, 1.0, 1.0);
				ttfont->Render(706, 4 + i * 480 / 13, &l70);
			}
			else if (i == 2) {
				ttfont->SetColor(1.0, 1.0, 0.0);
				ttfont->Render(706, 4 + i * 480 / 13, &r70);
			}
			else if (i == 3) {
				ttfont->SetColor(1.0, 1.0, 1.0);
				ttfont->Render(706, 4 + i * 480 / 13, &d70_45);
			}

			char str[8];
			sprintf(str, "%i", i + 1);
			int sw = small_font->GetStringWidth(str);
			small_font->RenderString(796 - sw, i * 480 / 13 + 2, str);
		}
		small_font->FlushText();

		ch_tag->Render();

		ci->Render();

		eglSwapBuffers(__egl_display, surf);

		fc++;
		gettimeofday(&ts, 0);
		float delta = (ts.tv_sec * 1000000 + ts.tv_usec) - (ts_prev.tv_sec * 1000000 + ts_prev.tv_usec);

		if (delta >= 1000000.0f) {
			printf("FPS=%.4f \n", (float) fc * 1000000.0 / delta);

			fc = 0;
			ts_prev = ts;
		}
	}

	delete ci;

	delete ch_tag;

	delete ttfont;

	delete a_scan;

	printf("try to delete TextScroller...\n");
	delete tscroll;

	delete draw;
	delete small_font;
	delete font;

	usleep(100000);

	eglDestroyContext(__egl_display, __egl_context);
	eglDestroySurface(__egl_display, surf);
	eglTerminate(__egl_display);

	XDestroyWindow(x_disp, x_wnd);

	printf("try to delete DScopeStream...\n");
	if (dss)
		delete dss;

	usleep(100000);

	printf("see you soon :-)\n");

	return EXIT_SUCCESS;
}
