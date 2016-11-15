/*
 * AScanWnd.cpp
 *
 *  Created on: Sep 16, 2016
 *      Author: zaqc
 */

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <cwchar>
#include <string>
#include <strings.h>

#include <wchar.h>

#include <SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#include "AScanWnd.h"

#include "Label.h"
#include "TrackBar.h"
#include "Button.h"
#include "Window.h"

#include "hw/StreamLayer.h"

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

	SDL_Texture *save_txt = SDL_GetRenderTarget(aRnd);
	SDL_SetRenderTarget(aRnd, res_txt);
	//SDL_SetRenderDrawColor(aRnd, 0, 0, 0, 0);
	//SDL_RenderClear(aRnd);
	SDL_SetRenderDrawColor(aRnd, aBkColor.r, aBkColor.g, aBkColor.b,
			aBkColor.a);
	SDL_Rect r = { 0, 0, aW, aH };
	SDL_RenderFillRect(aRnd, &r);

	char *res = setlocale(LC_ALL, "ru_RU.utf8");
	if (NULL == res) {
		std::cout << "Can't set ru_RU.utf8 locale...." << std::endl
				<< "use sudo locale-gen ru_RU" << std::endl
				<< "use sudo locale-gen ru_RU.UTF-8" << std::endl
				<< "use sudo update-locale" << std::endl;
	}
	char buf[128];
	memset(buf, 0, 128);
	wcstombs(buf, aCaption.c_str(), 128);

	SDL_Surface *surf = TTF_RenderUTF8_Blended(aFont, buf, aColor);

	if (NULL != surf) {
		SDL_Texture *txt = SDL_CreateTextureFromSurface(aRnd, surf);

		SDL_FreeSurface(surf);

		if (NULL != txt) {

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

			SDL_Rect src_rect = (SDL_Rect ) {sx, sy, sw, sh};
			SDL_Rect dst_rect = (SDL_Rect ) {dx, dy, dw, dh};
			SDL_RenderCopy(aRnd, txt, &src_rect, &dst_rect);
		}
	}

	SDL_SetRenderTarget(aRnd, save_txt);

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
	m_ItemType = miSimple;
	m_ModalResult = false;
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
	m_ItemType = miSubMenu;
	m_ModalResult = false;
}

MenuItem::~MenuItem(void) {
}

void MenuItem::SetFocus(bool aFocus) {
	m_Focus = aFocus;
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

	SDL_Texture *save_txt = SDL_GetRenderTarget(aRnd);
	SDL_SetRenderTarget(aRnd, m_Txt);
	SDL_SetRenderDrawBlendMode(aRnd, SDL_BLENDMODE_BLEND);
	SDL_Color c = m_Menu->GetItemBackground();
	if (m_MouseOver) {
		c = (SDL_Color ) {0, 0, 255, 255};
	}
	if (m_Focus) {
		c = (SDL_Color ) {255, 0, 0, 255};
	}
	SDL_SetRenderDrawColor(aRnd, 0, 0, 0, 0);
	SDL_RenderClear(aRnd);
	SDL_SetRenderDrawColor(aRnd, c.r, c.g, c.b, c.a);
	SDL_Rect r = { 0, 0, aW, aH };
	SDL_RenderFillRect(aRnd, &r);

	TTF_Font *font = m_Menu->GetItemFont();
	SDL_Surface *surf = TTF_RenderText_Blended(font, m_Caption.c_str(),
			m_Menu->GetItemColor());

	if (NULL != surf) {
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

		SDL_Rect src_rect = (SDL_Rect ) {sx, sy, sw, sh};
		SDL_Rect dst_rect = (SDL_Rect ) {dx, dy, dw, dh};
		SDL_RenderCopy(aRnd, txt, &src_rect, &dst_rect);

		SDL_DestroyTexture(txt);
		SDL_FreeSurface(surf);
	}

	SDL_SetRenderDrawBlendMode(aRnd, SDL_BLENDMODE_NONE);
	SDL_SetRenderTarget(aRnd, save_txt);
}

void MenuItem::Paint(SDL_Renderer *aRnd, int aX, int aY) {
	SDL_Rect dr = { aX, aY, m_W, m_H };
	SDL_RenderCopy(aRnd, m_Txt, NULL, &dr);
}

bool MenuItem::ProcessEvent(SDL_Event aEvent) {
	if (aEvent.type == SDL_KEYDOWN)
		if (aEvent.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
			m_ModalResult = true;
	return true;
}

//============================================================================
//	IntMenuItem
//============================================================================
IntMenuItem::IntMenuItem(Menu *aMenu, std::string aCaption, int aVal, int aMin,
		int aMax) :
		MenuItem(aMenu, aCaption, 0) {
	m_Val = aVal;
	m_Min = aMin;
	m_Max = aMax;
	m_ValTxt = NULL;
}

IntMenuItem::~IntMenuItem() {
}

void IntMenuItem::Render(SDL_Renderer *aRnd, int aW, int aH) {
	MenuItem::Render(aRnd, aW, aH);
}

void IntMenuItem::Paint(SDL_Renderer *aRnd, int aX, int aY) {
	MenuItem::Paint(aRnd, aX, aY);
}

bool IntMenuItem::OnKeyDown(SDL_Scancode aScanCode) {
	if (aScanCode == SDL_SCANCODE_ESCAPE)
		m_ModalResult = true;

	return true;
}

bool IntMenuItem::ProcessEvent(SDL_Event aEvent) {
	return false;
}

//============================================================================
//	Menu
//============================================================================
Menu::Menu(int aX, int aY, int aW, int aH, std::wstring aCaption, Menu *aParent) :
		Control(aX, aY, aW, aH) {
	m_Caprion = aCaption;
	m_Parent = aParent;

	m_CaptionTxt = NULL;

	if (NULL == aParent) {
		m_ItemFont = TTF_OpenFont(
				"/usr/share/fonts/truetype/freefont/FreeSerif.ttf", 24);
		m_ItemColor = (SDL_Color ) {255, 255, 0, 255};
		m_ItemBackground = (SDL_Color ) {32, 32, 32, 192};

		m_CaptionFont = TTF_OpenFont(
				"/usr/share/fonts/truetype/freefont/FreeSerif.ttf", 18);
	} else {
		m_ItemFont = NULL;
	}

	m_CaptionHeight = 32;
	m_ItemHeight = 42;
	m_BorderSize = 4;

	m_FocusedItem = NULL;
	m_ItemCaptureEvent = false;
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
	SDL_Rect r = { m_BorderSize, m_BorderSize + m_CaptionHeight
			+ item_num * m_ItemHeight, m_W - m_BorderSize * 2, m_ItemHeight };
	return r;
}

void Menu::CalcMenuRect(void) {
	int menu_height = 0;
	menu_height += m_BorderSize * 2;
	menu_height += m_CaptionHeight;
	menu_height += m_Items.size() * m_ItemHeight;
	m_MenuRect = (SDL_Rect ) {0, 0, m_W, menu_height};
}

void Menu::AddMenuItem(std::string aCaption, int aID) {
	MenuItem *mi = new MenuItem(this, aCaption, aID);
	m_Items.push_back(mi);
	CalcMenuRect();
	m_H = m_MenuRect.h;
	m_W = m_MenuRect.w;
	m_Invalidate = true;
	CheckFocusItem();
}

void Menu::AddMenuItem(std::string aCaption, Menu *aSubMenu) {
	MenuItem *mi = new MenuItem(this, aCaption, aSubMenu);
	m_Items.push_back(mi);
	CalcMenuRect();
	m_H = m_MenuRect.h;
	m_W = m_MenuRect.w;
	m_Invalidate = true;
	CheckFocusItem();
}

void Menu::AddMenuItem(MenuItem *aMI) {
	m_Items.push_back(aMI);
	CalcMenuRect();
	m_H = m_MenuRect.h;
	m_W = m_MenuRect.w;
	m_Invalidate = true;
	CheckFocusItem();
}

void Menu::Render(SDL_Renderer *aRnd) {
	if (NULL == m_CaptionTxt) {
		SDL_Color c = { 255, 255, 255, 255 };
		SDL_Color bk = { 0, 0, 128, 192 };
		m_CaptionTxt = CreateText(aRnd, GetItemFont(), m_Caprion,
				m_W - m_BorderSize * 2, m_CaptionHeight, c, bk);
	}

	SDL_Rect r = m_MenuRect; //{ m_X, m_Y, m_W, m_H };
	SDL_SetRenderDrawBlendMode(aRnd, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(aRnd, 32, 32, 32, 192);
	SDL_RenderFillRect(aRnd, &r);
	SDL_SetRenderDrawColor(aRnd, 255, 255, 255, 255);
	SDL_RenderDrawRect(aRnd, &r);

	SDL_Rect dst_rect = { m_BorderSize, m_BorderSize, m_W - m_BorderSize * 2,
			m_CaptionHeight };
	SDL_RenderCopy(aRnd, m_CaptionTxt, NULL, &dst_rect);

	int inum = 0;
	for (std::vector<MenuItem*>::iterator i = m_Items.begin();
			i != m_Items.end(); i++) {
		MenuItem *mi = *i;
		mi->Render(aRnd, m_W - m_BorderSize * 2, m_ItemHeight - 1);
		int y = m_BorderSize + m_CaptionHeight + inum * m_ItemHeight;
		mi->Paint(aRnd, m_BorderSize, y + 1);
		inum++;
	}

	SDL_SetRenderDrawBlendMode(aRnd, SDL_BLENDMODE_NONE);
}

bool Menu::OnKeyDown(SDL_Scancode aScanCode) {
	MenuItem *mi = m_FocusedItem;
	bool res = false;
	if (m_ItemCaptureEvent) {
		if (miSubMenu == m_FocusedItem->m_ItemType
				&& NULL != m_FocusedItem->m_SubMenu) {
			m_FocusedItem->m_SubMenu->OnKeyDown(aScanCode);
		} else
			m_FocusedItem->OnKeyDown(aScanCode);

		if (m_FocusedItem->GetModalResult())
			m_ItemCaptureEvent = false;

		res = true;
	} else {
		switch (aScanCode) {

		case SDL_SCANCODE_DOWN: {
			std::vector<MenuItem*>::iterator i = m_Items.begin();
			while (*i != m_FocusedItem)
				if (i != m_Items.end())
					i++;
				else
					break;

			if (i != m_Items.end() && ++i != m_Items.end())
				m_FocusedItem = *i;

			res = true;
			break;
		}

		case SDL_SCANCODE_UP: {
			std::vector<MenuItem*>::reverse_iterator i = m_Items.rbegin();
			while (*i != m_FocusedItem)
				if (i != m_Items.rend())
					i++;
				else
					break;

			if (i != m_Items.rend() && ++i != m_Items.rend())
				m_FocusedItem = *i;

			res = true;
			break;
		}

		case SDL_SCANCODE_RETURN:
			if (m_FocusedItem->CanCaptureFocus()) {
				m_ItemCaptureEvent = true;
			}
			break;

		case SDL_SCANCODE_ESCAPE:
			if (NULL != m_Parent)
				m_Parent->CloseModal();
			break;

		default:
			break;
		}

		if (mi != m_FocusedItem) {
			if (mi)
				mi->SetFocus(false);
			if (m_FocusedItem)
				m_FocusedItem->SetFocus(true);
			m_Invalidate = true;
		}
	}
	return res;
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

void Menu::CheckFocusItem(void) {
	if (m_FocusedItem == NULL) {
		if (m_Items.size() != 0) {
			m_FocusedItem = m_Items[0];
			m_FocusedItem->SetFocus(true);
			m_Invalidate = true;
		}
	}
}

void Menu::Paint(SDL_Renderer *aRnd) {
	Control::Paint(aRnd);

	if (m_ItemCaptureEvent && NULL != m_FocusedItem
			&& m_FocusedItem->m_ItemType == miSubMenu
			&& NULL != m_FocusedItem->m_SubMenu)
		m_FocusedItem->m_SubMenu->Paint(aRnd);
}

//============================================================================
//	FormItem
//============================================================================
FormItem::FormItem(Window *aWnd, int aX, int aY, int aW, int aH,
		std::wstring aCaption, std::wstring aUnits, int aValue, int aMin,
		int aMax) :
		Control(aX, aY, aW, aH) {
	m_Caption = aCaption;
	m_Units = aUnits;
	m_Value = 0;
	m_Min = 0;
	m_Max = 96;
}
//----------------------------------------------------------------------------

FormItem::~FormItem() {

}
//----------------------------------------------------------------------------

bool FormItem::OnKeyDown(SDL_Scancode aScanCode) {
	return false;
}
//----------------------------------------------------------------------------

void FormItem::Render(SDL_Renderer *aRnd) {
	SDL_SetRenderDrawColor(aRnd, 0, 0, 0, 255);
	SDL_Rect r = { 0, 0, m_W, m_H };
	SDL_RenderFillRect(aRnd, &r);

	SDL_SetRenderDrawColor(aRnd, 255, 255, 255, 255);
	SDL_RenderDrawRect(aRnd, &r);

	wchar_t str[128];
	std::swprintf(str, 128, L"%ls%i%ls", m_Caption.c_str(), m_Value,
			m_Units.c_str());

	SDL_Color bk = g_ItemBackground;
	if (GetFocused()) {
		bk = (SDL_Color) {192, 192, 192, 255};
	}
	SDL_Texture *txt = CreateText(aRnd, g_ItemFont, std::wstring(str), m_W, m_H,
			g_ItemColor, bk);
	SDL_Rect dst_rect = { 0, 0, m_W, m_H };
	SDL_RenderCopy(aRnd, txt, &r, &dst_rect);

	SDL_SetRenderDrawColor(aRnd, 255, 255, 255, 255);
}
//----------------------------------------------------------------------------

//============================================================================
//	AScanWnd
//============================================================================
AScanView::AScanView(Window *aWnd, int aX, int aY, int aW, int aH) :
		Control(aX, aY, aW, aH) {
	m_Wnd = aWnd;

	for (int i = 0; i < 1024; i++) {
		m_Data[i] = 0;
	}
}

AScanView::~AScanView() {
}

extern StreamLayer *stream_layer;

void AScanView::Render(SDL_Renderer *aRnd) {
	SDL_SetRenderDrawColor(aRnd, 0, 0, 0, 255);
	SDL_Rect r = { 0, 0, m_W, m_H };
	SDL_RenderFillRect(aRnd, &r);

	SDL_SetRenderDrawColor(aRnd, 255, 255, 255, 255);
	SDL_RenderDrawRect(aRnd, &r);

	SDL_SetRenderDrawColor(aRnd, 64, 64, 64, 255);
	int tick_count = 10;
	for (int i = 1; i < tick_count; i++) {
		int x = (float) m_W / (float) tick_count * (float) i;
		SDL_RenderDrawLine(aRnd, x, 1, x, m_H - 1);
	}
	for (int i = 1; i < tick_count; i++) {
		int y = (float) m_H / (float) tick_count * (float) i;
		SDL_RenderDrawLine(aRnd, 1, y, m_W - 1, y);
	}

	SDL_SetRenderDrawColor(aRnd, 255, 255, 255, 255);

	unsigned char buf[4096];
	stream_layer->GetLatestData(buf);
	int prev_val = m_H;
	int prev_x = 0;
	for (int i = 0; i < 128; i++) {
		int x = (int) ((float) i / 128.0f * (float) m_W);
		int val = m_H - (int) ((float) buf[i] / 255.0f * (float) m_H);
		SDL_RenderDrawLine(aRnd, prev_x, prev_val, x, val);
		prev_val = val;
		prev_x = x;
	}
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
	m_LAmpOne = new Label(10, 35, 64, 24, L"Amp1");
	AddControl(m_LAmpOne);
	m_TBAmpOne = new TrackBar(10, 60, 64, 340);
	m_TBAmpOne->SetValue(105, 0, 255);
	AddControl(m_TBAmpOne);

	m_Button = new Button(200, 5, 120, 40, "MainMenu");
	AddControl(m_Button);

	m_BtnQuit = new Button(330, 5, 120, 40, "Quit");
	AddControl(m_BtnQuit);

	m_AScanView = NULL;
	m_AScanView = new AScanView(this, 100, 50, 650, 350);
	AddControl(m_AScanView);

	int fn = 6;
	for (int i = 0; i < fn; i++) {
		FormItem *fi = new FormItem(this, 1 + i * 800 / 6, 410, 800 / 6 - 2, 40,
				L"A1:", L"dB", 0, 0, 96);
		AddControl(fi);
	}

//	m_MainMenu = new Menu(100, 50, 420, 380, L"Главное меню", NULL);
//
//	Menu *sub_menu = new Menu(110, 110, 420, 340, L"SubMenu", m_MainMenu);
//	sub_menu->AddMenuItem("Item One", 11);
//	sub_menu->AddMenuItem("Item Two", 12);
//	sub_menu->AddMenuItem("Item Three", 13);
//
//	m_MainMenu->AddMenuItem("", sub_menu);
//	m_MainMenu->AddMenuItem("BScan tape", 1);
//	IntMenuItem *imi = new IntMenuItem(m_MainMenu, "IncDec Item", 0, 0, 100);
//	m_MainMenu->AddMenuItem(imi);
//	m_MainMenu->AddMenuItem("WayMeter tape", 2);
//	m_MainMenu->AddMenuItem("AScan TuneMaster", 3);
//	m_MainMenu->AddMenuItem("Calibrate WayMeter", 4);
//	m_MainMenu->AddMenuItem("Configure", 5);
//	m_MainMenu->AddMenuItem("Hide menu", 6);
//	AddControl(m_MainMenu);
}

void AScanWnd::Done(void) {
}

void AScanWnd::UpdateControls(void) {
	if (NULL != m_AScanView) {
		m_AScanView->Invalidate();
	}

	if (NULL != m_TBAmpOne && NULL != m_LAmpOne) {
		int val = m_TBAmpOne->GetValue();

		unsigned int cmd = ds_addr(0, 7, 0) | val;
		stream_layer->SendCommand(cmd, DS_SIDE_LEFT);

		wchar_t str[128];
		swprintf(str, 128, L"%idB", val);
		m_LAmpOne->SetText(std::wstring(str));
	}

	if (NULL != m_Button) {
		if (m_Button->OnClick()) {
			if (NULL != m_MainMenu) {
				if (m_MainMenu->GetHideState()) {
					m_MainMenu->Show();
				} else {
					m_MainMenu->Hide();
				}
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

void AScanWnd::PaintWindow(void) {
	/*
	 SDL_SetRenderDrawColor(m_Rnd, 255, 255, 255, 255);
	 SDL_Rect r = { m_X, m_Y, m_W, m_H };
	 SDL_RenderFillRect(m_Rnd, &r);
	 SDL_SetRenderDrawColor(m_Rnd, 0, 0, 0, 0);
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
	 */
	Window::PaintWindow();
}
