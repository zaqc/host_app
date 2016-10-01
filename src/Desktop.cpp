/*
 * Desktop.cpp
 *
 *  Created on: Sep 21, 2016
 *      Author: zaqc
 */

#include <unistd.h>
#include <sys/time.h>
#include <iostream>

#include <SDL.h>
#include <SDL_ttf.h>

#include "Desktop.h"
#include "AScanWnd.h"
#include "BScanWnd.h"
//----------------------------------------------------------------------------

//============================================================================
//	InfoItem
//============================================================================
InfoItem::InfoItem(InfoLine *aInfoLine, std::wstring aText, int aW, int aH,
		InfoAlign aIA, int aBorderSize) {
	m_Txt = NULL;
	m_W = aW;
	m_H = aH;
	m_InfoAlign = aIA;
	m_Invalidate = true;
	m_Caption = aText;
	m_InfoLine = aInfoLine;
	m_BorderSize = 2;
	m_RealWidth = aBorderSize;
}
//----------------------------------------------------------------------------

InfoItem::~InfoItem() {
	if (NULL != m_Txt) {
		SDL_DestroyTexture(m_Txt);
		m_Txt = NULL;
	}
}
//----------------------------------------------------------------------------

void InfoItem::SetText(std::wstring aText) {
	m_Invalidate = true;
}
//----------------------------------------------------------------------------

void InfoItem::Render(SDL_Renderer *aRnd) {
	if (m_Invalidate) {
		if (NULL != m_Txt) {
			SDL_DestroyTexture(m_Txt);
			m_Txt = NULL;
		}
		setlocale(LC_ALL, "ru_RU.utf8");
		char buf[128];
		memset(buf, 0, 128);
		wcstombs(buf, m_Caption.c_str(), 128);
		SDL_Surface *surf = TTF_RenderUTF8_Blended(m_InfoLine->GetFont(), buf,
				m_InfoLine->GetTextColor());
		if (surf) {
			SDL_Texture *txt = SDL_CreateTextureFromSurface(aRnd, surf);
			if (txt) {
				if (m_InfoAlign == iaAutoWidth)
					m_RealWidth = surf->w + m_BorderSize * 2;
				else
					m_RealWidth = m_W;

				m_Txt = SDL_CreateTexture(aRnd, SDL_PIXELFORMAT_ABGR8888,
						SDL_TEXTUREACCESS_TARGET, m_RealWidth, m_H);
				SDL_SetRenderTarget(aRnd, m_Txt);
				SDL_Color c = m_InfoLine->GetBkColor();
				SDL_SetRenderDrawColor(aRnd, c.r, c.g, c.b, c.a);
				SDL_RenderClear(aRnd);
				int sx = 0;
				int sy = 0;
				int sw = surf->w;
				int sh = surf->h;
				int dx = m_BorderSize;
				int dy = 0;
				int dw = m_RealWidth - m_BorderSize * 2;
				int dh = m_H;
				if (sh > dh) {
					sy += (sh - dh) / 2;
					sh = dh;
				} else if (sh < dh) {
					dy += (dh - sh) / 2;
					dh = sh;
				}
				switch (m_InfoAlign) {
					case iaCenter:
						if (sw > dw) {
							sx += (sw - dw) / 2;
							sw = dw;
						} else if (sw < dw) {
							dx += (dw - sw) / 2;
							dw = sw;
						}
						break;

					case iaLeft:
						if (sw > dw)
							sw = dw;
						break;

					case iaRight:
						if (sw > dw)
							sw = dw;
						else {
							dx += dw - sw;
							dw = sw;
						}
						break;

					default:
						break;
				}
				SDL_Rect src_rect = { sx, sy, sw, sh };
				SDL_Rect dst_rect = { dx, dy, dw, dh };
				SDL_RenderCopy(aRnd, txt, &src_rect, &dst_rect);
				SDL_SetRenderTarget(aRnd, NULL);
			}
		}

		m_Invalidate = false;
	}
}
//----------------------------------------------------------------------------

void InfoItem::Paint(SDL_Renderer *aRnd, int aX, int aY) {
	Render(aRnd);
	if (NULL != m_Txt) {
		SDL_Rect src_rect = { 0, 0, m_RealWidth, m_H };
		SDL_Rect dst_rect = { aX, aY, m_RealWidth, m_H };
		SDL_RenderCopy(aRnd, m_Txt, &src_rect, &dst_rect);
	}
}
//----------------------------------------------------------------------------

//============================================================================
//	InfoLine
//============================================================================
InfoLine::InfoLine(int aX, int aY, int aW, int aH) {
	m_X = aX;
	m_Y = aY;
	m_W = aW;
	m_H = aH;
	m_Font = TTF_OpenFont("/usr/share/fonts/truetype/freefont/FreeSerif.ttf",
			20);
	m_TextColor = (SDL_Color ) { 0, 0, 255, 255 };
	m_BkColor = (SDL_Color ) { 255, 255, 0, 255 };
}
//----------------------------------------------------------------------------

InfoLine::~InfoLine() {
	if (NULL != m_Font) {
		TTF_CloseFont(m_Font);
		m_Font = NULL;
	}
}
//----------------------------------------------------------------------------

void InfoLine::AddItem(InfoItem *aInfoItem) {
	m_Items.push_back(aInfoItem);
}
//----------------------------------------------------------------------------

InfoItem *InfoLine::AddItem(std::wstring aText, InfoAlign aIA, int aWidth) {
	InfoItem *ii = new InfoItem(this, aText, aWidth, m_H, aIA);
	m_Items.push_back(ii);
	return ii;
}
//----------------------------------------------------------------------------

void InfoLine::Paint(SDL_Renderer *aRnd) {

	SDL_SetRenderDrawColor(aRnd, 32, 32, 32, 255);
	SDL_Rect r = { m_X, m_Y, m_W, m_H };
	SDL_RenderFillRect(aRnd, &r);

	int x = m_X;
	for (std::vector<InfoItem*>::iterator i = m_Items.begin();
			i != m_Items.end(); i++) {
		InfoItem *ii = *i;
		ii->Render(aRnd);
		int w = ii->GetWidth();
		if (x + w < m_W) {
			ii->Paint(aRnd, x, m_Y);
			x += w + 2;
		} else
			break;
	}
}
//----------------------------------------------------------------------------

//============================================================================
//	Desktop
//============================================================================
Desktop::Desktop() {
	m_ActiveWindow = NULL;
	m_Wnd = NULL;
	m_Rnd = NULL;

	m_IL = NULL;
}
//----------------------------------------------------------------------------

Desktop::~Desktop() {
}
//----------------------------------------------------------------------------

void Desktop::Init(void) {
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
	TTF_Init();
	SDL_CreateWindowAndRenderer(800, 480, SDL_WINDOW_BORDERLESS, &m_Wnd,
			&m_Rnd);
	SDL_GL_SetSwapInterval(1);

	m_IL = new InfoLine(0, 0, 800, 30);
	m_IL->AddItem(L"P75", iaAutoWidth);
	m_IL->AddItem(L"Moscow-Petushky", iaAutoWidth);
	m_IL->AddItem(L"Way 1", iaAutoWidth);
	m_IL->AddItem(L"KM:123", iaAutoWidth);
	m_IL->AddItem(L"PK:7", iaAutoWidth);
	m_IL->AddItem(L"M:67.5", iaAutoWidth);

	m_IL->AddItem(L"->+", iaRight, 80);
	m_IL->AddItem(L"abc", iaCenter, 80);

	m_ActiveWindow = new BScanWnd(m_Rnd, 0, 30, 800, 450);
	m_ActiveWindow->Init();
}
//----------------------------------------------------------------------------

void Desktop::PushMessage(int aID) {
	m_Messages.push(aID);
}
//----------------------------------------------------------------------------

void Desktop::SetActiveWindow(Window *aWindow) {
	m_ActiveWindow = aWindow;
}
//----------------------------------------------------------------------------

void Desktop::ShowModal(Window *aWindow) {
	while (true) {
		SDL_Event e;
		while (SDL_PollEvent(&e)) {
			if (NULL != m_ActiveWindow)
				m_ActiveWindow->PaintWindow();

			aWindow->ProcessEvent(e);
			aWindow->PaintWindow();

			SDL_RenderPresent(m_Rnd);
		}
	}
}
//----------------------------------------------------------------------------

void Desktop::Run(void) {
	timeval ts, ts_prev;
	gettimeofday(&ts, 0);
	int fc = 0;
	ts_prev = ts;

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

		if (NULL != m_IL) {
			m_IL->Paint(m_Rnd);
		}

		if (NULL != m_ActiveWindow) {
			m_ActiveWindow->UpdateControls();
			m_ActiveWindow->PaintWindow();
		}

		SDL_RenderPresent(m_Rnd);

		gettimeofday(&ts, 0);
		float delta = (ts.tv_sec * 1000000 + ts.tv_usec)
				- (ts_prev.tv_sec * 1000000 + ts_prev.tv_usec);

		if (delta >= 1000000.0f) {
			std::cout << (float) fc * 1000000.0 / delta << std::endl;

			fc = 0;
			ts_prev = ts;
		}
		fc++;
	}
}
//----------------------------------------------------------------------------

void Desktop::Done(void) {
	SDL_DestroyRenderer(m_Rnd);
	SDL_DestroyWindow(m_Wnd);

	TTF_Quit();

	SDL_Quit();
}
//----------------------------------------------------------------------------
