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

//============================================================================
//	SDL Util
//============================================================================
SDL_Texture *CreateText(SDL_Renderer *aRnd, TTF_Font *aFont,
		std::string aCaption, int aW, int aH, SDL_Color aColor,
		SDL_Color aBkColor) {

	SDL_Texture *res_txt = SDL_CreateTexture(aRnd, SDL_PIXELFORMAT_ABGR8888,
			SDL_TEXTUREACCESS_TARGET, aW, aH);

	SDL_SetRenderTarget(aRnd, res_txt);
	SDL_SetRenderDrawColor(aRnd, 0, 0, 0, 0);
	SDL_RenderClear(aRnd);
	SDL_SetRenderDrawColor(aRnd, aBkColor.r, aBkColor.g, aBkColor.b,
			aBkColor.a);
	SDL_Rect r = { 2, 2, aW - 4, aH - 4 };
	SDL_RenderFillRect(aRnd, &r);

	SDL_Surface *surf = TTF_RenderText_Blended(aFont, aCaption.c_str(), aColor);

	SDL_Texture *txt = SDL_CreateTextureFromSurface(aRnd, surf);

	int sx = 0;
	int sy = 0;
	int sw = surf->w;
	int sh = surf->h;
	int dx = 3;
	int dy = 3;
	int dw = aW - 6;
	int dh = aH - 6;
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

	SDL_Rect src_rect = (SDL_Rect ) { sx, sy, sw, sh };
	SDL_Rect dst_rect = (SDL_Rect ) { dx, dy, dw, dh };
	SDL_RenderCopy(aRnd, txt, &src_rect, &dst_rect);

	SDL_SetRenderTarget(aRnd, NULL);

	return res_txt;
}
//----------------------------------------------------------------------------

//============================================================================
//	MenuItem
//============================================================================
MenuItem::MenuItem(Menu *aMenu, std::string aCaption, int aID) {
	m_Menu = aMenu;
	m_Caption = aCaption;
	m_ID = aID;
	m_SubMenu = NULL;
	m_Txt = NULL;
	m_Focus = false;
	m_Invalidate = false;
	m_W = m_H = 0;
}

MenuItem::MenuItem(Menu *aMenu, std::string aCaption, Menu *aSubMenu) {
	m_Menu = aMenu;
	m_Caption = aCaption;
	m_ID = -1;
	m_SubMenu = aSubMenu;
	m_Txt = NULL;
	m_Focus = false;
	m_Invalidate = false;
	m_W = m_H = 0;
}

MenuItem::~MenuItem(void) {

}

void MenuItem::SetFocus(bool aFocus) {

}

void MenuItem::Render(SDL_Renderer *aRnd, int aW, int aH) {
	if (m_Txt != NULL)
		SDL_DestroyTexture(m_Txt);
	m_Txt = SDL_CreateTexture(aRnd, SDL_PIXELFORMAT_ABGR8888,
			SDL_TEXTUREACCESS_TARGET, aW, aH);
	m_W = aW;
	m_H = aH;

	SDL_SetRenderTarget(aRnd, m_Txt);
	SDL_Color c = m_Menu->GetItemBackground();
	SDL_SetRenderDrawColor(aRnd, 0, 0, 0, 0);
	SDL_RenderClear(aRnd);
	SDL_SetRenderDrawColor(aRnd, c.r, c.g, c.b, c.a);
	SDL_Rect r = { 2, 2, aW - 4, aH - 4 };
	SDL_RenderFillRect(aRnd, &r);

	TTF_Font *font = m_Menu->GetItemFont();
	SDL_Surface *surf = TTF_RenderText_Blended(font, m_Caption.c_str(),
			m_Menu->GetItemColor());

	SDL_Texture *txt = SDL_CreateTextureFromSurface(aRnd, surf);

	int sx = 0;
	int sy = 0;
	int sw = surf->w;
	int sh = surf->h;
	int dx = 3;
	int dy = 3;
	int dw = m_W - 6;
	int dh = m_H - 6;
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

	SDL_Rect src_rect = (SDL_Rect ) { sx, sy, sw, sh };
	SDL_Rect dst_rect = (SDL_Rect ) { dx, dy, dw, dh };
	SDL_RenderCopy(aRnd, txt, &src_rect, &dst_rect);

	SDL_SetRenderTarget(aRnd, NULL);
}

void MenuItem::Paint(SDL_Renderer *aRnd, int aX, int aY) {

}

//============================================================================
//
//============================================================================
Menu::Menu(int aX, int aY, int aW, int aH, std::string aCaption,
		Menu *aParent) {
	m_X = aX;
	m_Y = aY;
	m_W = aW;
	m_H = aH;

	m_Caprion = aCaption;
	m_Parent = aParent;

	m_CaptionTxt = NULL;

	if (NULL == aParent) {
		m_ItemFont = TTF_OpenFont(
				"/usr/share/fonts/truetype/freefont/FreeSerif.ttf", 24);
		m_ItemColor = (SDL_Color ) { 255, 255, 0, 255 };
		m_ItemBackground = (SDL_Color ) { 32, 32, 32, 192 };
	} else {
		m_ItemFont = NULL;
	}
}

Menu::~Menu() {
	if (NULL != m_ItemFont) {
		TTF_CloseFont(m_ItemFont);
		m_ItemFont = NULL;
	}
}

TTF_Font *Menu::GetItemFont(void) {
	if (NULL != m_Parent)
		return m_Parent->GetItemFont();
	return m_ItemFont;
}

SDL_Color Menu::GetItemColor(void) {
	if (NULL != m_Parent)
		return m_Parent->GetItemColor();
	return m_ItemColor;
}

SDL_Color Menu::GetItemBackground(void) {
	if (NULL != m_Parent)
		return m_Parent->GetItemBackground();
	return m_ItemBackground;
}

void Menu::AddMenuItem(std::string aCaption, int aID) {
	MenuItem *mi = new MenuItem(this, aCaption, aID);
	m_Items.push_back(mi);
}

void Menu::AddMenuItem(std::string aCaption, Menu *aSubMenu) {
	MenuItem *mi = new MenuItem(this, aCaption, aSubMenu);
	m_Items.push_back(mi);
}

void Menu::Render(SDL_Renderer *aRnd) {

	if (NULL == m_CaptionTxt) {
		m_CaptionTxt = CreateText(aRnd, GetItemFont(), m_Caprion, m_W, 32,
				GetItemColor(), GetItemBackground());
	}

	SDL_Rect r = { m_X, m_Y, m_W, m_H };
	SDL_SetRenderDrawBlendMode(aRnd, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(aRnd, 32, 32, 32, 192);
	SDL_RenderFillRect(aRnd, &r);
	SDL_SetRenderDrawBlendMode(aRnd, SDL_BLENDMODE_NONE);

	SDL_Rect dst_rect = { m_X, m_Y, m_W, 32 };
	SDL_RenderCopy(aRnd, m_CaptionTxt, NULL, &dst_rect);

}

bool Menu::ProcessEvent(SDL_Event aEvent) {

	return false;
}

//============================================================================
//
//============================================================================
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

	Menu *mm = new Menu(100, 100, 200, 340, "Main Menu", NULL);
	AddControl(mm);
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

	roundedBoxRGBA(m_Rnd, 100, 40, 220, 80, 4, 255, 0, 0, 150);
	stringRGBA(m_Rnd, 110, 50, "asdasdasd", 255, 250, 20, 255);
}
