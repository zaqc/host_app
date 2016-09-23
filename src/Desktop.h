/*
 * Desktop.h
 *
 *  Created on: Sep 21, 2016
 *      Author: zaqc
 */

#ifndef DESKTOP_H_
#define DESKTOP_H_

#include "Window.h"

#include <queue>
#include <string>
#include <vector>
//----------------------------------------------------------------------------

enum InfoAlign {
	iaNone, iaCenter, iaAutoWidth, iaLeft, iaRight
};
//----------------------------------------------------------------------------

class InfoLine;

class InfoItem {
protected:
	std::wstring m_Caption;
	InfoAlign m_InfoAlign;
	SDL_Texture *m_Txt;
	int m_RealWidth;
	int m_W;
	int m_H;
	bool m_Invalidate;
	InfoLine *m_InfoLine;
	int m_BorderSize;
public:
	InfoItem(InfoLine *aInfoLine, std::wstring aText, int aW, int aH,
			InfoAlign aIA, int aBorderSize = 2);
	virtual ~InfoItem();

	void SetText(std::wstring aText);
	void Render(SDL_Renderer *aRnd);
	void Paint(SDL_Renderer *aRnd, int aX, int aY);

	int GetWidth(void) {
		return m_RealWidth;
	}
};
//----------------------------------------------------------------------------

class InfoLine {
protected:
	std::vector<InfoItem*> m_Items;
	SDL_Color m_TextColor;
	SDL_Color m_BkColor;
	TTF_Font *m_Font;
	int m_X;
	int m_Y;
	int m_W;
	int m_H;
public:
	InfoLine(int aX, int aY, int aW, int aH);
	virtual ~InfoLine();

	SDL_Color GetBkColor(void) {
		return m_BkColor;
	}
	SDL_Color GetTextColor(void) {
		return m_TextColor;
	}
	TTF_Font *GetFont(void) {
		return m_Font;
	}

	void AddItem(InfoItem *aInfoItem);
	InfoItem *AddItem(std::wstring aText, InfoAlign aIA, int aWidth = 32);

	void Paint(SDL_Renderer *aRnd);
};
//----------------------------------------------------------------------------

class Desktop {
protected:
	Window *m_ActiveWindow;
	SDL_Renderer *m_Rnd;
	SDL_Window *m_Wnd;

	InfoLine *m_IL;

	std::queue<int> m_Messages;
public:
	Desktop();
	virtual ~Desktop();

	void PushMessage(int aID);

	void SetActiveWindow(Window *aWindow);
	void ShowModal(Window *aWindow);

	void Init(void);
	void Run(void);
	void Done(void);
};

#endif /* DESKTOP_H_ */
