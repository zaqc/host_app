/*
 * AScanWnd.cpp
 *
 *  Created on: Sep 16, 2016
 *      Author: zaqc
 */

#include "AScanWnd.h"

#include "Label.h"
#include "TrackBar.h"
#include "Button.h"
#include "Window.h"

#include <SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
//----------------------------------------------------------------------------

//============================================================================
//	SDL Util
//============================================================================
SDL_Texture *CreateText(SDL_Renderer *aRnd, TTF_Font *aFont,
		std::wstring aCaption, int aW, int aH, SDL_Color aColor,
		SDL_Color aBkColor) {

	SDL_Texture *res_txt = SDL_CreateTexture(aRnd, SDL_PIXELFORMAT_ABGR8888,
			SDL_TEXTUREACCESS_TARGET, aW, aH);

	SDL_SetTextureBlendMode(res_txt, SDL_BLENDMODE_BLEND);

	SDL_SetRenderTarget(aRnd, res_txt);
	//SDL_SetRenderDrawColor(aRnd, 0, 0, 0, 0);
	//SDL_RenderClear(aRnd);
	SDL_SetRenderDrawColor(aRnd, aBkColor.r, aBkColor.g, aBkColor.b,
			aBkColor.a);
	SDL_Rect r = { 0, 0, aW, aH };
	SDL_RenderFillRect(aRnd, &r);

	setlocale(LC_ALL, "ru_RU.utf8");
	char buf[128];
	memset(buf, 0, 128);
	wcstombs(buf, aCaption.c_str(), 128);

	SDL_Surface *surf = TTF_RenderUTF8_Blended(aFont, buf, aColor);

	SDL_Texture *txt = SDL_CreateTextureFromSurface(aRnd, surf);

	int sx = 0;
	int sy = 0;
	int sw = surf->w;
	int sh = surf->h;
	int dx = 0;
	int dy = 0;
	int dw = aW;
	int dh = aH;
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
	m_MouseOver = false;
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
	m_MouseOver = false;
	m_Invalidate = false;
	m_W = m_H = 0;
}

MenuItem::~MenuItem(void) {

}

void MenuItem::SetFocus(bool aFocus) {

}

void MenuItem::SetMouseOver(bool aMouseOver) {
	m_MouseOver = aMouseOver;
}

void MenuItem::Render(SDL_Renderer *aRnd, int aW, int aH) {
	if (m_Txt != NULL)
		SDL_DestroyTexture(m_Txt);
	m_Txt = SDL_CreateTexture(aRnd, SDL_PIXELFORMAT_ABGR8888,
			SDL_TEXTUREACCESS_TARGET, aW, aH);
	m_W = aW;
	m_H = aH;

	SDL_SetTextureBlendMode(m_Txt, SDL_BLENDMODE_BLEND);

	SDL_SetRenderTarget(aRnd, m_Txt);
	SDL_SetRenderDrawBlendMode(aRnd, SDL_BLENDMODE_BLEND);
	SDL_Color c = m_Menu->GetItemBackground();
	if (m_MouseOver) {
		c = (SDL_Color ) { 0, 0, 255, 255 };
	}
	SDL_SetRenderDrawColor(aRnd, 0, 0, 0, 0);
	SDL_RenderClear(aRnd);
	SDL_SetRenderDrawColor(aRnd, c.r, c.g, c.b, c.a);
	SDL_Rect r = { 0, 0, aW, aH };
	SDL_RenderFillRect(aRnd, &r);

	TTF_Font *font = m_Menu->GetItemFont();
	SDL_Surface *surf = TTF_RenderText_Blended(font, m_Caption.c_str(),
			m_Menu->GetItemColor());

	SDL_Texture *txt = SDL_CreateTextureFromSurface(aRnd, surf);

	int sx = 0;
	int sy = 0;
	int sw = surf->w;
	int sh = surf->h;
	int dx = 0;
	int dy = 0;
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

	SDL_Rect src_rect = (SDL_Rect ) { sx, sy, sw, sh };
	SDL_Rect dst_rect = (SDL_Rect ) { dx, dy, dw, dh };
	SDL_RenderCopy(aRnd, txt, &src_rect, &dst_rect);

	SDL_SetRenderDrawBlendMode(aRnd, SDL_BLENDMODE_NONE);
	SDL_SetRenderTarget(aRnd, NULL);

	SDL_DestroyTexture(txt);
	SDL_FreeSurface(surf);
}

void MenuItem::Paint(SDL_Renderer *aRnd, int aX, int aY) {
	SDL_Rect dr = { aX, aY, m_W, m_H };
	SDL_RenderCopy(aRnd, m_Txt, NULL, &dr);
}

//============================================================================
//	Menu
//============================================================================
Menu::Menu(int aX, int aY, int aW, int aH, std::wstring aCaption, Menu *aParent) :
		Control() {
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

		m_CaptionFont = TTF_OpenFont(
				"/usr/share/fonts/truetype/freefont/FreeSerif.ttf", 18);
	} else {
		m_ItemFont = NULL;
	}

	m_CaptionHeight = 32;
	m_ItemHeight = 42;
	m_BorderSize = 4;
}

Menu::~Menu() {
	if (NULL != m_CaptionFont) {
		TTF_CloseFont(m_CaptionFont);
		m_CaptionFont = NULL;
	}
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

SDL_Rect Menu::CalcItemRect(MenuItem *aMI) {
	int item_num = 0;
	for (std::vector<MenuItem*>::iterator i = m_Items.begin();
			i != m_Items.end(); i++) {
		if (*i == aMI)
			break;
		item_num++;
	}
	SDL_Rect r = { m_X + m_BorderSize, m_Y + m_BorderSize + m_CaptionHeight
			+ item_num * m_ItemHeight, m_W - m_BorderSize * 2, m_ItemHeight };
	return r;
}

void Menu::CalcMenuRect(void) {
	int menu_height = 0;
	menu_height += m_BorderSize * 2;
	menu_height += m_CaptionHeight;
	menu_height += m_Items.size() * m_ItemHeight;
	m_MenuRect = (SDL_Rect ) { m_X, m_Y, m_W, menu_height };
}

void Menu::AddMenuItem(std::string aCaption, int aID) {
	MenuItem *mi = new MenuItem(this, aCaption, aID);
	m_Items.push_back(mi);
	CalcMenuRect();
}

void Menu::AddMenuItem(std::string aCaption, Menu *aSubMenu) {
	MenuItem *mi = new MenuItem(this, aCaption, aSubMenu);
	m_Items.push_back(mi);
	CalcMenuRect();
}

void Menu::Render(SDL_Renderer *aRnd) {

	if (NULL == m_CaptionTxt) {
		SDL_Color c = { 255, 255, 255, 255 };
		SDL_Color bk = { 0, 0, 64, 192 };
		m_CaptionTxt = CreateText(aRnd, GetItemFont(), m_Caprion,
				m_W - m_BorderSize * 2, m_CaptionHeight, c, bk);
	}

	SDL_Rect r = m_MenuRect; //{ m_X, m_Y, m_W, m_H };
	SDL_SetRenderDrawBlendMode(aRnd, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(aRnd, 32, 32, 32, 192);
	SDL_RenderFillRect(aRnd, &r);
	SDL_SetRenderDrawColor(aRnd, 0, 0, 0, 255);
	SDL_RenderDrawRect(aRnd, &r);

	SDL_Rect dst_rect = { m_X + m_BorderSize, m_Y + m_BorderSize, m_W
			- m_BorderSize * 2, m_CaptionHeight };
	SDL_RenderCopy(aRnd, m_CaptionTxt, NULL, &dst_rect);

	int inum = 0;
	for (std::vector<MenuItem*>::iterator i = m_Items.begin();
			i != m_Items.end(); i++) {
		MenuItem *mi = *i;
		mi->Render(aRnd, m_W - m_BorderSize * 2, m_ItemHeight - 1);
		int y = m_Y + m_BorderSize + m_CaptionHeight + inum * m_ItemHeight;
		mi->Paint(aRnd, m_X + m_BorderSize, y + 1);
		inum++;
	}

	SDL_SetRenderDrawBlendMode(aRnd, SDL_BLENDMODE_NONE);
}

bool Menu::ProcessEvent(SDL_Event aEvent) {

	if (aEvent.type == SDL_MOUSEMOTION) {
		int x = aEvent.motion.x;
		int y = aEvent.motion.y;
		int item_num = 0;
		for (std::vector<MenuItem*>::iterator i = m_Items.begin();
				i != m_Items.end(); i++) {
			MenuItem *mi = *i;
			int ix = m_X + m_BorderSize;
			int iy = m_Y + m_BorderSize + m_CaptionHeight
					+ item_num * m_ItemHeight;
			int iw = m_W - m_BorderSize * 2;
			int ih = m_ItemHeight;
			if (x >= ix && x < ix + iw && y >= iy && y < iy + ih) {
				mi->SetMouseOver(true);
			} else {
				mi->SetMouseOver(false);
			}
			item_num++;
		}
	}
	return false;
}

int Menu::Execute(void) {
	return 0;
}

void Menu::Paint(void) {
	Control::Paint();
}

//============================================================================
//	AScanWnd
//============================================================================
AScanWnd::AScanWnd(SDL_Renderer *aRnd, int aX, int aY, int aW, int aH) :
		Window(aRnd, aX, aY, aW, aH) {
	m_LAmpOne = NULL;
	m_TBAmpOne = NULL;
	m_Button = NULL;
	m_MainMenu = NULL;
	m_BtnQuit = NULL;
}

AScanWnd::~AScanWnd() {
}

void AScanWnd::Init(void) {
	m_LAmpOne = new Label(10, 10, 64, 24, "Amp1");
	AddControl(m_LAmpOne);
	m_TBAmpOne = new TrackBar(10, 32, 64, 400);
	AddControl(m_TBAmpOne);

	m_Button = new Button(200, 10, 120, 40, "MainMenu");
	AddControl(m_Button);

	m_BtnQuit = new Button(200, 60, 120, 40, "Quit");
	AddControl(m_BtnQuit);

	m_MainMenu = new Menu(100, 100, 420, 340, L"Главное меню", NULL);
	m_MainMenu->AddMenuItem("BScan tape", 1);
	m_MainMenu->AddMenuItem("WayMeter tape", 2);
	m_MainMenu->AddMenuItem("AScan TuneMaster", 3);
	m_MainMenu->AddMenuItem("Calibrate WayMeter", 4);
	m_MainMenu->AddMenuItem("Configure", 5);
	m_MainMenu->AddMenuItem("Hide menu", 6);
	AddControl(m_MainMenu);
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

	if (NULL != m_Button) {
		if (m_Button->OnClick()) {
			if (m_MainMenu->GetHideState()) {
				m_MainMenu->Show();
			} else {
				m_MainMenu->Hide();
			}
		}
	}

	if (NULL != m_BtnQuit) {
		if (m_BtnQuit->OnClick()) {
			SDL_Event e;
			e.type = SDL_QUIT;
			SDL_PushEvent(&e);
		}
	}
}

void AScanWnd::Paint(void) {
	SDL_SetRenderDrawColor(m_Rnd, 255, 255, 255, 255);
	SDL_Rect r = { m_X, m_Y, m_W, m_H };
	SDL_RenderFillRect(m_Rnd, &r);
	SDL_SetRenderDrawColor(m_Rnd, 0, 0, 0, 255);
	SDL_RenderDrawRect(m_Rnd, &r);
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

	Window::Paint();
}
