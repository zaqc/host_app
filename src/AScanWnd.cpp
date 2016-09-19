/*
 * AScanWnd.cpp
 *
 *  Created on: Sep 16, 2016
 *      Author: zaqc
 */

#include "AScanWnd.h"

#include "Label.h"
#include "TrackBar.h"
#include "Window.h"

#include <SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
//----------------------------------------------------------------------------

AScanWnd::AScanWnd(SDL_Renderer *aRnd, int aX, int aY, int aW, int aH) :
		Window(aRnd, aX, aY, aW, aH) {
	m_LAmpOne = NULL;
	m_TBAmpOne = NULL;
}

AScanWnd::~AScanWnd() {
}

void AScanWnd::Init(void) {
	m_LAmpOne = new Label(10, 10, 64, 24, "Amp1");
	AddControl(m_LAmpOne);
	m_TBAmpOne = new TrackBar(10, 32, 64, 400);
	AddControl(m_TBAmpOne);
}

void AScanWnd::Done(void) {
}

void AScanWnd::UpdateControls(void) {
	if (NULL != m_TBAmpOne && NULL != m_LAmpOne) {
		int val = m_TBAmpOne->GetValue();
		char str[128];
		sprintf(str, "%idB", val);
		m_LAmpOne->SetText(std::string(str));
	}
}

void AScanWnd::Paint(void) {
	SDL_SetRenderDrawColor(m_Rnd, 255, 255, 255, 255);
	SDL_Rect r = { m_X, m_Y, m_W, m_H };
	SDL_RenderFillRect(m_Rnd, &r);
	SDL_SetRenderDrawColor(m_Rnd, 0, 0, 0, 255);
	for (int i = 0; i < 7; i++) {
		int x = m_X + m_W / 7 * (i + 1);
		SDL_RenderDrawLine(m_Rnd, x, m_Y, x, m_Y + m_H);
	}

	for (int i = 0; i < 4; i++) {
		int y = m_Y + m_H / 4 * (i + 1);
		SDL_RenderDrawLine(m_Rnd, m_X, y, m_X + m_W, y);
	}

	roundedBoxRGBA(m_Rnd, 100, 100, 220, 140, 4, 255, 0, 0, 150);
	stringRGBA(m_Rnd, 110, 110, "asdasdasd", 255, 250, 20, 255);
}
