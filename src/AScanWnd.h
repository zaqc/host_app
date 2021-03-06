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
#include "Button.h"
//----------------------------------------------------------------------------

#define	KEY_UP		1073741906
#define	KEY_DN		1073741905
#define	KEY_LEFT	1073741904
#define	KEY_RIGHT	1073741903
#define	KEY_ESC		27
#define	KEY_ENTER	13
#define	KEY_TAB		9
#define	KEY_PLUS	1073741911
#define	KEY_MINUS	1073741910
//----------------------------------------------------------------------------

class Menu;
//----------------------------------------------------------------------------

SDL_Texture *CreateText(SDL_Renderer *aRnd, TTF_Font *aFont,
		std::wstring aCaption, int aW, int aH, SDL_Color aColor,
		SDL_Color aBkColor);
//----------------------------------------------------------------------------

enum TextAlign {
	taNone, taLeft, taCenter, taRight
};

enum MenuItemType {
	miSimple, miSubMenu, miIntVal, miString, miCheck
};
//----------------------------------------------------------------------------

class MenuItem {
protected:
	Menu *m_Menu;
	std::string m_Caption;
	int m_ID;
	SDL_Texture *m_Txt;
	bool m_Focus;
	bool m_MouseOver;
	bool m_Invalidate;
	int m_H;
	int m_W;
	bool m_ModalResult;
public:
	Menu* m_SubMenu;
	MenuItemType m_ItemType;

	MenuItem(Menu *aMenu, std::string aCaption, int aID);
	MenuItem(Menu *aMenu, std::string aCaption, Menu *aSubMenu);
	virtual ~MenuItem(void);

	void SetItemType(MenuItemType *aMIT);

	void SetFocus(bool aFocus);
	void SetMouseOver(bool aMouseOver);

	virtual bool CanCaptureFocus(void) {
		if (m_ItemType == miSubMenu)
			return true;
		return false;
	}

	void SetModalResult(void) {
		m_ModalResult = true;
	}

	virtual bool GetModalResult(void) {
		return m_ModalResult;
	}

	virtual bool OnKeyDown(SDL_Scancode aScanCode) {
		return false;
	}

	virtual void Render(SDL_Renderer *aRnd, int aW, int aH);
	virtual void Paint(SDL_Renderer *aRnd, int aX, int aY);

	virtual bool ProcessEvent(SDL_Event aEvent);
};
//----------------------------------------------------------------------------

/*
 * ===========================================================================
 * Value is right aligned on Manu Item
 * Item Caption is left aligned on space excepted rect needed for show Value
 * ===========================================================================
 */
class IntMenuItem: public MenuItem {
protected:
	int m_Min;
	int m_Max;
	int m_Val;
	SDL_Texture *m_ValTxt;
public:
	IntMenuItem(Menu *aMenu, std::string aCaption, int aVal, int aMin,
			int aMax);
	virtual ~IntMenuItem();

	virtual bool CanCaptureFocus(void) {
		m_ModalResult = false;
		return true;
	}

	virtual void Render(SDL_Renderer *aRnd, int aW, int aH);
	virtual void Paint(SDL_Renderer *aRnd, int aX, int aY);

	virtual bool OnKeyDown(SDL_Scancode aScanCode);

	virtual bool ProcessEvent(SDL_Event aEvent);
};
//----------------------------------------------------------------------------

class Menu: public Control {
protected:
	std::wstring m_Caprion;
	Menu *m_Parent;
	std::vector<MenuItem*> m_Items;

	MenuItem *m_FocusedItem;
	bool m_ItemCaptureEvent;

	SDL_Color m_ItemColor;
	SDL_Color m_ItemBackground;
	TTF_Font *m_ItemFont;

	TTF_Font *m_CaptionFont;

	SDL_Texture *m_CaptionTxt;

	int m_CaptionHeight;
	int m_ItemHeight;
	int m_BorderSize;
	SDL_Rect m_MenuRect;
public:
	Menu(int aX, int aY, int aW, int aH, std::wstring aCaption, Menu *aParent);
	virtual ~Menu();

	void CloseModal() {
		m_ItemCaptureEvent = false;
	}

	virtual bool CanFocused(void) {
		return true;
	}

	TTF_Font *GetItemFont(void);
	SDL_Color GetItemColor(void);
	SDL_Color GetItemBackground(void);

	SDL_Rect CalcItemRect(MenuItem *aMI);

	void CalcMenuRect(void);

	void AddMenuItem(std::string aCaption, int aID);
	void AddMenuItem(std::string aCaption, Menu *aSubMenu);
	void AddMenuItem(MenuItem *aMI);

	void CheckFocusItem(void);
	virtual void Paint(SDL_Renderer *aRnd);

	virtual bool OnKeyDown(SDL_Scancode aScanCode);

	virtual void Render(SDL_Renderer *aRnd);
	virtual bool ProcessEvent(SDL_Event aEvent);
};
//----------------------------------------------------------------------------

class FormItem: public Control {
protected:
	int m_Min;
	int m_Max;
	int m_Value;
	std::wstring m_Caption;
	std::wstring m_Units;
public:
	FormItem(Window *aWnd, int aX, int aY, int aW, int aH,
			std::wstring aCaption, std::wstring aUnits, int aValue, int aMin,
			int aMax);
	virtual ~FormItem();

	virtual bool CanFocused(void) {
		return true;
	}

	virtual bool OnKeyDown(SDL_Scancode aScanCode);

	virtual void Render(SDL_Renderer *aRnd);
};
//----------------------------------------------------------------------------

class AScanView: public Control {
protected:
	Window *m_Wnd;
	int m_Data[1024];
public:
	AScanView(Window *aWnd, int aX, int aY, int aW, int aH);
	virtual ~AScanView();

	virtual void Render(SDL_Renderer *aRnd);
};
//----------------------------------------------------------------------------

class AScanWnd: public Window {
protected:
	Label *m_LAmpOne;
	TrackBar *m_TBAmpOne;
	Button *m_Button;
	Button *m_BtnQuit;
	Menu *m_MainMenu;
	AScanView *m_AScanView;
public:
	AScanWnd(SDL_Renderer *aRnd, int aX, int aY, int aW, int aH);
	virtual ~AScanWnd();

	virtual void Init(void);
	virtual void Done(void);

	virtual void UpdateControls(void);

	virtual void PaintWindow(void);
};
//----------------------------------------------------------------------------

#endif /* ASCANWND_H_ */
