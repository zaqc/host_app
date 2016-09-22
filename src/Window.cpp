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
Control::Control() {
	m_Invalidate = true;
	m_Hide = false;
}
//----------------------------------------------------------------------------

Control::~Control() {
}
//----------------------------------------------------------------------------

//============================================================================
//	Window
//============================================================================
Window::Window(SDL_Renderer *aRnd, int aX, int aY, int aW, int aH) :
		Control() {
	m_Rnd = aRnd;
	m_X = aX;
	m_Y = aY;
	m_W = aW;
	m_H = aH;
}
//----------------------------------------------------------------------------

Window::~Window() {
	// TODO Auto-generated destructor stub
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
	SDL_SetRenderDrawColor(m_Rnd, 192, 192, 255, 255);
	SDL_Rect r = { m_X, m_Y, m_W, m_H };
	SDL_RenderFillRect(m_Rnd, &r);

	for (std::vector<Control*>::reverse_iterator i = m_Control.rbegin();
			i != m_Control.rend(); i++) {
		Control *cnt = *i;
		if (!cnt->GetHideState())
			cnt->Render(m_Rnd);
	}
}
//----------------------------------------------------------------------------

bool Window::ProcessEvent(SDL_Event aEvent) {
	if (aEvent.type == SDL_KEYDOWN) {
		std::cout << (int) aEvent.key.keysym.mod << std::endl;
	}

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
