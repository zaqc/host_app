/*
 * Window.cpp
 *
 *  Created on: Sep 15, 2016
 *      Author: zaqc
 */

#include <Window.h>

#include <SDL.h>
#include <vector>
//----------------------------------------------------------------------------

//============================================================================
//	Control
//============================================================================
Control::Control() {
	m_Invalidate = true;
}
//----------------------------------------------------------------------------

Control::~Control() {
}
//----------------------------------------------------------------------------

//============================================================================
//	Window
//============================================================================
Window::Window(SDL_Renderer *aRnd, int aX, int aY, int aW, int aH) {
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

void Window::AddControl(Control *aControl) {
	m_Control.push_back(aControl);
}
//----------------------------------------------------------------------------

void Window::Init(void) {
}
//----------------------------------------------------------------------------

void Window::Done(void) {

}
//----------------------------------------------------------------------------

void Window::Paint(void) {
	SDL_SetRenderDrawColor(m_Rnd, 192, 192, 255, 255);
	SDL_Rect r = { m_X, m_Y, m_W, m_H };
	SDL_RenderFillRect(m_Rnd, &r);

	for (std::vector<Control*>::reverse_iterator i = m_Control.rbegin();
			i != m_Control.rend(); i++) {
		Control *cnt = *i;
		cnt->Render(m_Rnd);
	}
}
//----------------------------------------------------------------------------

int Window::Execute(void) {
	while (true) {
		SDL_Event e;
		SDL_PollEvent(&e);
		if (e.type == SDL_QUIT)
			return -1;

		for (std::vector<Control*>::reverse_iterator i = m_Control.rbegin();
				i != m_Control.rend(); i++) {
			Control *cnt = *i;
			if (cnt->ProcessEvent(e))
				break;
		}
	}

	return 0;
}
//----------------------------------------------------------------------------
