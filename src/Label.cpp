/*
 * Label.cpp
 *
 *  Created on: Sep 13, 2016
 *      Author: zaqc
 */

#include "Label.h"

#include <stdlib.h>
#include <iostream>
#include <SDL_ttf.h>

Label::Label(int aX, int aY, int aW, int aH, std::wstring aText) :
		Control(aX, aY, aW, aH) {
	m_Text = aText;

	m_Font = TTF_OpenFont("/usr/share/fonts/truetype/freefont/FreeSerif.ttf",
			24);
	std::cout << "Can't open /usr/share/fonts/truetype/freefont/FreeSerif.ttf"
			<< std::endl;
}

Label::~Label() {
	TTF_CloseFont(m_Font);
}

void Label::SetText(std::wstring aStr) {
	m_Text = aStr;
	m_Invalidate = true;
}

bool Label::ProcessEvent(SDL_Event aEvent) {
	return false;
}

void Label::Render(SDL_Renderer *aRnd) {
	SDL_Color fc = { 192, 168, 1, 255 };

	char buf[128];
	memset(buf, 0, 128);
	wcstombs(buf, m_Text.c_str(), 128);

	SDL_Surface *surf = TTF_RenderUTF8_Blended(m_Font, buf, fc);
	SDL_Texture *txt = SDL_CreateTextureFromSurface(aRnd, surf);
	int sx = 0;
	int sy = 0;
	int sw = surf->w;
	int sh = surf->h;
	int dx = 0; //m_X;
	int dy = 0; //m_Y;
	int dw = m_W;
	int dh = m_H;

	SDL_SetRenderDrawColor(aRnd, 0, 0, 32, 255);
	//SDL_Rect r = { m_X, m_Y, m_W, m_H };
	SDL_Rect r = { 0, 0, m_W, m_H };
	SDL_RenderFillRect(aRnd, &r);

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
