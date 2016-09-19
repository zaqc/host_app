/*
 * AScanWnd.h
 *
 *  Created on: Sep 16, 2016
 *      Author: zaqc
 */

#ifndef ASCANWND_H_
#define ASCANWND_H_

#include "Window.h"

#include "Label.h"
#include "TrackBar.h"
//----------------------------------------------------------------------------

class Menu;
//----------------------------------------------------------------------------

SDL_Texture *CreateText(SDL_Renderer *aRnd, TTF_Font *aFont, std::string aCaption, int aW, int aH, SDL_Color aColor, SDL_Color aBkColor);
//----------------------------------------------------------------------------

class MenuItem {
protected:
	Menu *m_Menu;
	std::string m_Caption;
	int m_ID;
	Menu* m_SubMenu;
	SDL_Texture *m_Txt;
	bool m_Focus;
	bool m_Invalidate;
	int m_H;
	int m_W;
public:
	MenuItem(Menu *aMenu, std::string aCaption, int aID);
	MenuItem(Menu *aMenu, std::string aCaption, Menu *aSubMenu);
	virtual ~MenuItem(void);

	void SetFocus(bool aFocus);

	void Render(SDL_Renderer *aRnd, int aW, int aH);
	void Paint(SDL_Renderer *aRnd, int aX, int aY);
};
//----------------------------------------------------------------------------

class Menu: public Control {
protected:
	int m_X;
	int m_Y;
	int m_W;
	int m_H;
	std::string m_Caprion;
	Menu *m_Parent;
	std::vector<MenuItem*> m_Items;

	SDL_Color m_ItemColor;
	SDL_Color m_ItemBackground;
	TTF_Font *m_ItemFont;

	SDL_Texture *m_CaptionTxt;
public:
	Menu(int aX, int aY, int aW, int aH, std::string aCaption,
			Menu *aParent);
	virtual ~Menu();

	TTF_Font *GetItemFont(void);
	SDL_Color GetItemColor(void);
	SDL_Color GetItemBackground(void);

	void AddMenuItem(std::string aCaption, int aID);
	void AddMenuItem(std::string aCaption, Menu *aSubMenu);

	virtual void Render(SDL_Renderer *aRnd);
	virtual bool ProcessEvent(SDL_Event aEvent);
};
//----------------------------------------------------------------------------

class AScanWnd: public Window {
protected:
	Label *m_LAmpOne;
	TrackBar *m_TBAmpOne;
public:
	AScanWnd(SDL_Renderer *aRnd, int aX, int aY, int aW, int aH);
	virtual ~AScanWnd();

	virtual void Init(void);
	virtual void Done(void);

	virtual void UpdateControls(void);

	virtual void Paint(void);
};
//----------------------------------------------------------------------------

#endif /* ASCANWND_H_ */