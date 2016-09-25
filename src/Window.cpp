/*
 * Window.cpp
 *
 *  Created on: Sep 15, 2016
 *      Author: zaqc
 */

#include "Window.h"

#include <iostream>
#include <SDL.h>
#include <vector>

#include "Button.h"
#include "Label.h"
#include "TrackBar.h"
//----------------------------------------------------------------------------

//============================================================================
//	Control
//============================================================================
Control::Control(int aX, int aY, int aW, int aH) {
	m_X = aX;
	m_Y = aY;
	m_W = aW;
	m_H = aH;
	m_Invalidate = true;
	m_Hide = false;
	m_ControlTexture = NULL;
}
//----------------------------------------------------------------------------

Control::~Control() {
	if (NULL != m_ControlTexture) {
		SDL_DestroyTexture(m_ControlTexture);
		m_ControlTexture = NULL;
	}
}
//----------------------------------------------------------------------------

void Control::Render(SDL_Renderer *aRnd) {
	SDL_SetRenderDrawColor(aRnd, 255, 255, 255, 255);
	SDL_RenderClear(aRnd);
}
//----------------------------------------------------------------------------

void Control::Paint(SDL_Renderer *aRnd) {
	if (0 != m_W && 0 != m_H) {
		if (NULL == m_ControlTexture) {
			m_ControlTexture = SDL_CreateTexture(aRnd, SDL_PIXELFORMAT_ABGR8888,
					SDL_TEXTUREACCESS_TARGET, m_W, m_H);
		}

		if (m_Invalidate) {
			SDL_Texture *save_txt = SDL_GetRenderTarget(aRnd);
			SDL_SetRenderTarget(aRnd, m_ControlTexture);
			Render(aRnd);
			SDL_SetRenderTarget(aRnd, save_txt);
			m_Invalidate = false;
		}

		SDL_Rect src_rect = { 0, 0, m_W, m_H };
		SDL_Rect dst_rect = { m_X, m_Y, m_W, m_H };
		SDL_RenderCopy(aRnd, m_ControlTexture, &src_rect, &dst_rect);
	}
}
//----------------------------------------------------------------------------

//============================================================================
//	Window
//============================================================================
Window::Window(SDL_Renderer *aRnd, int aX, int aY, int aW, int aH) :
		Control(aX, aY, aW, aH) {
	m_Rnd = aRnd;
}
//----------------------------------------------------------------------------

Window::~Window() {
	if (NULL != m_ControlTexture) {
		SDL_DestroyTexture(m_ControlTexture);
		m_ControlTexture = NULL;
	}
}
//----------------------------------------------------------------------------

void Window::ProcessMessage(int aID) {
}
//----------------------------------------------------------------------------

void Window::AddControl(Control *aControl) {
	m_Control.push_back(aControl);
}
//----------------------------------------------------------------------------

void Window::Paint(void) {
	if (NULL == m_ControlTexture) {
		m_ControlTexture = SDL_CreateTexture(m_Rnd, SDL_PIXELFORMAT_ABGR8888,
				SDL_TEXTUREACCESS_TARGET, m_W, m_H);
	}

	SDL_Texture *save_txt = SDL_GetRenderTarget(m_Rnd);
	SDL_SetRenderTarget(m_Rnd, m_ControlTexture);

	if (m_Invalidate) {
		SDL_SetRenderDrawColor(m_Rnd, 192, 192, 255, 255);
		SDL_Rect r = { 0, 0, m_W, m_H };
		SDL_RenderFillRect(m_Rnd, &r);
	}

	for (std::vector<Control*>::reverse_iterator i = m_Control.rbegin();
			i != m_Control.rend(); i++) {
		Control *cnt = *i;
		if (!cnt->GetHideState())
			cnt->Paint(m_Rnd);
	}

	SDL_SetRenderTarget(m_Rnd, save_txt);

	m_Invalidate = false;

	SDL_Rect src_rect = { 0, 0, m_W, m_H };
	SDL_Rect dst_rect = { m_X, m_Y, m_W, m_H };

	SDL_RenderCopy(m_Rnd, m_ControlTexture, &src_rect, &dst_rect);
}
//----------------------------------------------------------------------------

bool Window::ProcessEvent(SDL_Event aEvent) {
//	if (aEvent.type == SDL_KEYDOWN) {
//		std::cout << (int) aEvent.key.keysym.mod << std::endl;
//	}

	for (std::vector<Control*>::reverse_iterator i = m_Control.rbegin();
			i != m_Control.rend(); i++) {
		Control *cnt = *i;
		if (!cnt->GetHideState() && cnt->ProcessEvent(aEvent))
			return true;
	}

	if (Control::ProcessEvent(aEvent))
		return true;

	return false;
}
