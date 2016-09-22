/*
 * Desktop.cpp
 *
 *  Created on: Sep 21, 2016
 *      Author: zaqc
 */

#include <SDL.h>
#include <SDL_ttf.h>

#include "Desktop.h"
#include "AScanWnd.h"

Desktop::Desktop() {
	m_ActiveWindow = NULL;
	m_Wnd = NULL;
	m_Rnd = NULL;
}

Desktop::~Desktop() {
}

void Desktop::Init(void) {
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
	TTF_Init();
	SDL_CreateWindowAndRenderer(800, 480, SDL_WINDOW_BORDERLESS, &m_Wnd,
			&m_Rnd);
	SDL_GL_SetSwapInterval(1);

	m_ActiveWindow = new AScanWnd(m_Rnd, 0, 0, 800, 480);
	m_ActiveWindow->Init();
}

void Desktop::PushMessage(int aID) {
	m_Messages.push(aID);
}

void Desktop::SetActiveWindow(Window *aWindow) {
	m_ActiveWindow = aWindow;
}

void Desktop::ShowModal(Window *aWindow) {
	while (true) {
		SDL_Event e;
		while (SDL_PollEvent(&e)) {
			if (NULL != m_ActiveWindow)
				m_ActiveWindow->Paint();

			aWindow->ProcessEvent(e);
			aWindow->Paint();

			SDL_RenderPresent(m_Rnd);
		}
	}
}

void Desktop::Run(void) {
	while (true) {
		SDL_Event e;
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT)
				return;

			if (NULL != m_ActiveWindow) {
				m_ActiveWindow->ProcessEvent(e);
				while (0 != m_Messages.size()) {
					int id = m_Messages.front();
					m_Messages.pop();
					m_ActiveWindow->ProcessMessage(id);
				}
			}
		}
		if (NULL != m_ActiveWindow) {
			m_ActiveWindow->UpdateControls();
			m_ActiveWindow->Paint();
		}

		SDL_RenderPresent(m_Rnd);
	}
}

void Desktop::Done(void) {
	SDL_DestroyRenderer(m_Rnd);
	SDL_DestroyWindow(m_Wnd);

	TTF_Quit();

	SDL_Quit();
}

