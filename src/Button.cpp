/*
 * Button.cpp
 *
 *  Created on: Sep 14, 2016
 *      Author: zaqc
 */

#include <Button.h>

#include <SDL.h>
#include <SDL_ttf.h>
#include <string>
#include <iostream>

Button::Button(int aX, int aY, int aW, int aH, std::string aText) :
		Control() {
	m_X = aX;
	m_Y = aY;
	m_W = aW;
	m_H = aH;
	m_Text = aText;
	m_Font = TTF_OpenFont("/usr/share/fonts/truetype/freefont/FreeSerif.ttf",
			24);
	std::cout << "Can't open /usr/share/fonts/truetype/freefont/FreeSerif.ttf"
			<< std::endl;
	m_Down = false;
	m_OnClick = false;
}

Button::~Button() {
	TTF_CloseFont(m_Font);
}

bool Button::OnClick(void) {
	if (m_OnClick) {
		m_OnClick = false;
		return true;
	}

	return false;
}

void Button::Render(SDL_Renderer *aRnd) {

	if (m_Down) {
		SDL_SetRenderDrawColor(aRnd, 225, 225, 255, 255);
	} else {
		SDL_SetRenderDrawColor(aRnd, 192, 192, 192, 255);
	}
	SDL_Rect r = { m_X, m_Y, m_W, m_H };
	SDL_RenderFillRect(aRnd, &r);

	SDL_Color c = { 0, 0, 0, 0 };
	SDL_Surface *surf = TTF_RenderText_Blended(m_Font, m_Text.c_str(), c);
	SDL_Texture *txt = SDL_CreateTextureFromSurface(aRnd, surf);
	int sx = 0;
	int sy = 0;
	int sw = surf->w;
	int sh = surf->h;
	int dx = m_X;
	int dy = m_Y;
	int dw = m_W;
	int dh = m_H;
	if (sh > dh) {
		sy += (sh - dh) / 2;
		sh = dh;
	} else if (sh < dh) {
		dy += (dh - sh) / 2;
		dh = sh;
	}

	if (sw > dw) {
		sx += (sw - dw) / 2;
		sw = dw;
	} else if (sw < dw) {
		dx += (dw - sw) / 2;
		dw = sw;
	}

	SDL_Rect src_rect = (SDL_Rect ) {sx, sy, sw, sh};
	SDL_Rect dst_rect = (SDL_Rect ) {dx, dy, dw, dh};
	SDL_RenderCopy(aRnd, txt, &src_rect, &dst_rect);

	SDL_DestroyTexture(txt);
	SDL_FreeSurface(surf);
}

bool Button::ProcessEvent(SDL_Event aEvent) {
	switch (aEvent.type) {
	case SDL_MOUSEBUTTONDOWN: {
		int x = aEvent.button.x;
		int y = aEvent.button.y;
		if (x >= m_X && x < m_X + m_W && y >= m_Y && y < m_Y + m_H) {
			m_Down = true;
			return true;
		}
		break;
	}

	case SDL_MOUSEMOTION: {
		break;
	}

	case SDL_MOUSEBUTTONUP: {
		if (m_Down) {
			m_Down = false;
			int x = aEvent.button.x;
			int y = aEvent.button.y;
			if (x >= m_X && x < m_X + m_W && y >= m_Y && y < m_Y + m_H) {
				m_OnClick = true;
				return true;
			}
		}
		break;
	}

	}
	return false;
}
