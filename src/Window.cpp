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
	m_Focused = false;
	m_Hide = false;
	m_ControlTexture = NULL;
	m_ControlTextureWidth = 0;
	m_ControlTextureHeight = 0;
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
		if (NULL == m_ControlTexture || m_ControlTextureWidth != m_W
				|| m_ControlTextureHeight != m_H) {
			if (NULL != m_ControlTexture) {
				SDL_DestroyTexture(m_ControlTexture);
				m_ControlTexture = NULL;
			}
			m_ControlTexture = SDL_CreateTexture(aRnd, SDL_PIXELFORMAT_ABGR8888,
					SDL_TEXTUREACCESS_TARGET, m_W, m_H);
			m_ControlTextureWidth = m_W;
			m_ControlTextureHeight = m_H;
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
	m_ActiveControl = NULL;
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

void Window::PaintWindow(void) {
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
	if (aEvent.type == SDL_KEYDOWN) {
		if (aEvent.key.keysym.scancode == SDL_SCANCODE_TAB) {
			std::vector<Control*>::iterator ii = m_Control.begin();
			std::vector<Control*>::reverse_iterator ri = m_Control.rbegin();
			bool ss = (aEvent.key.keysym.mod & KMOD_LSHIFT) == 0;

			int step = 0;
			while ((ss && ii != m_Control.end())
					|| (!ss && ri != m_Control.rend())) {
				Control *ctl = NULL;
				if (ss)
					ctl = *ii;
				else
					ctl = *ri;
				if (NULL == m_ActiveControl) {
					if (ctl->CanFocused()) {
						ctl->SetFocused(true);
						m_ActiveControl = ctl;
						return true;
					}
				} else {
					if (step == 0) {
						if (m_ActiveControl == ctl)
							step = 1;
					} else {
						if (ctl->CanFocused()) {
							m_ActiveControl->SetFocused(false);
							ctl->SetFocused(true);
							m_ActiveControl = ctl;
							return true;
						}
					}
				}
				if (ss)
					++ii;
				else
					++ri;
			}
			ii = m_Control.begin();
			ri = m_Control.rbegin();
			while ((ss && ii != m_Control.end())
					|| (!ss && ri != m_Control.rend())) {
				Control *ctl = NULL;
				if (ss)
					ctl = *ii;
				else
					ctl = *ri;
				if (ctl->CanFocused()) {
					if (NULL != m_ActiveControl)
						m_ActiveControl->SetFocused(false);
					m_ActiveControl = ctl;
					m_ActiveControl->SetFocused(true);
					return true;
				}
				if (ss)
					++ii;
				else
					++ri;
			}

//			if ((aEvent.key.keysym.mod & KMOD_LSHIFT) == 0) {
//				int step = 0;
//				for (std::vector<Control*>::iterator i = m_Control.begin();
//						i != m_Control.end(); i++) {
//					Control *ctl = *i;
//					if (NULL == m_ActiveControl) {
//						if (ctl->CanFocused()) {
//							ctl->SetFocused(true);
//							m_ActiveControl = ctl;
//							return true;
//						}
//					} else {
//						if (step == 0) {
//							if (m_ActiveControl == ctl)
//								step = 1;
//						} else {
//							if (ctl->CanFocused()) {
//								m_ActiveControl->SetFocused(false);
//								ctl->SetFocused(true);
//								m_ActiveControl = ctl;
//								return true;
//							}
//						}
//					}
//				}
//				for (std::vector<Control*>::iterator i = m_Control.begin();
//						i != m_Control.end(); i++) {
//					Control *ctl = *i;
//					if (ctl->CanFocused()) {
//						if (NULL != m_ActiveControl)
//							m_ActiveControl->SetFocused(false);
//						m_ActiveControl = ctl;
//						m_ActiveControl->SetFocused(true);
//						return true;
//					}
//				}
//			} else {
//				int step = 0;
//				for (std::vector<Control*>::reverse_iterator i =
//						m_Control.rbegin(); i != m_Control.rend(); i++) {
//					Control *ctl = *i;
//					if (NULL == m_ActiveControl) {
//						if (ctl->CanFocused()) {
//							ctl->SetFocused(true);
//							m_ActiveControl = ctl;
//							return true;
//						}
//					} else {
//						if (step == 0) {
//							if (m_ActiveControl == ctl)
//								step = 1;
//						} else {
//							if (ctl->CanFocused()) {
//								m_ActiveControl->SetFocused(false);
//								ctl->SetFocused(true);
//								m_ActiveControl = ctl;
//								return true;
//							}
//						}
//					}
//				}
//				for (std::vector<Control*>::reverse_iterator i =
//						m_Control.rbegin(); i != m_Control.rend(); i++) {
//					Control *ctl = *i;
//					if (ctl->CanFocused()) {
//						if (NULL != m_ActiveControl)
//							m_ActiveControl->SetFocused(false);
//						m_ActiveControl = ctl;
//						m_ActiveControl->SetFocused(true);
//						return true;
//					}
//				}
//			}
		}
	}

	for (std::vector<Control*>::reverse_iterator i = m_Control.rbegin();
			i != m_Control.rend(); i++) {
		Control *cnt = *i;
		if (!cnt->GetHideState()) {
			if (cnt->ProcessEvent(aEvent))
				return true;

			switch (aEvent.type) {
			case SDL_KEYDOWN:
				if (NULL != m_ActiveControl
						&& m_ActiveControl->OnKeyDown(
								aEvent.key.keysym.scancode))
					return true;
				break;
			case SDL_KEYUP:
				if (NULL != m_ActiveControl
						&& m_ActiveControl->OnKeyUp(aEvent.key.keysym.scancode))
					return true;
				break;
			case SDL_MOUSEBUTTONDOWN: {
				int x = aEvent.button.x - m_X;
				int y = aEvent.button.y - m_Y;
				if (cnt->OnMouseDown(aEvent.button.button, x, y))
					return true;
				break;
			}
			case SDL_MOUSEBUTTONUP: {
				int x = aEvent.button.x - m_X;
				int y = aEvent.button.y - m_Y;
				if (cnt->OnMouseUp(aEvent.button.button, x, y))
					return true;
				break;
			}
			case SDL_MOUSEMOTION: {
				int x = aEvent.motion.x - m_X;
				int y = aEvent.motion.y - m_Y;
				if (cnt->OnMouseMove(x, y))
					return true;
				break;
			}
			}
		}
	}

	switch (aEvent.type) {
	case SDL_MOUSEBUTTONDOWN: {
		int x = aEvent.button.x - m_X;
		int y = aEvent.button.y - m_Y;
		if (OnMouseDown(aEvent.button.button, x, y))
			return true;
		break;
	}
	case SDL_MOUSEBUTTONUP: {
		int x = aEvent.button.x - m_X;
		int y = aEvent.button.y - m_Y;
		if (OnMouseUp(aEvent.button.button, x, y))
			return true;
		break;
	}
	case SDL_MOUSEMOTION: {
		int x = aEvent.motion.x - m_X;
		int y = aEvent.motion.y - m_Y;
		if (OnMouseMove(x, y))
			return true;
		break;
	}
	}

	return false;
}
