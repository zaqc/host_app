/*
 * Window.h
 *
 *  Created on: Sep 15, 2016
 *      Author: zaqc
 */

#ifndef SRC_WINDOW_H_
#define SRC_WINDOW_H_
//----------------------------------------------------------------------------

#include <SDL.h>
#include <SDL_ttf.h>
#include <vector>

#include "Desktop.h"
//----------------------------------------------------------------------------

class Control {
protected:
	bool m_Invalidate;
	bool m_Hide;
	SDL_Texture *m_ControlTexture;
	int m_ControlTextureWidth;
	int m_ControlTextureHeight;
	int m_X;
	int m_Y;
	int m_W;
	int m_H;
public:
	Control(int aX, int aY, int aW, int aH);
	virtual ~Control();

	virtual bool CanFocused(void) {
		return false;
	}

	void Invalidate() {
		m_Invalidate = true;
	}

	void Hide() {
		m_Hide = true;
	}
	void Show() {
		m_Hide = false;
	}

	bool GetHideState() {
		return m_Hide;
	}

	virtual bool OnMouseDown(uint8_t aButton, int32_t aX, int32_t aY) {
		return false;
	}

	virtual bool OnMouseUp(uint8_t aButton, int32_t aX, int32_t aY) {
		return false;
	}

	virtual bool OnMouseMove(int32_t aX, int32_t aY) {
		return false;
	}

	virtual bool OnKeyUp(SDL_Scancode aScanCode) {
		return false;
	}

	virtual bool OnKeyDown(SDL_Scancode aScanCode) {
		return false;
	}

	virtual void Render(SDL_Renderer *aRnd);
	virtual void Paint(SDL_Renderer *aRnd);

	virtual bool ProcessEvent(SDL_Event aEvent) {
		return false;
	}
};
//----------------------------------------------------------------------------

extern TTF_Font *g_ItemFont;
extern SDL_Color g_ItemColor;
extern SDL_Color g_ItemBackground;
//----------------------------------------------------------------------------

class Window: public Control {
protected:
	SDL_Renderer *m_Rnd;
	std::vector<Control*> m_Control;
	Control *m_ActiveControl;
public:
	Window(SDL_Renderer *aRnd, int aX, int aY, int aW, int aH);
	virtual ~Window();

	//void NextControl(void);

	virtual TTF_Font *GetItemFont(void) {
		return g_ItemFont;
	}
	virtual SDL_Color GetItemColor(void) {
		return g_ItemColor;
	}
	virtual SDL_Color GetItemBackground(void) {
		return g_ItemBackground;
	}

	virtual void ProcessMessage(int aID);

	void AddControl(Control *aControl);

	virtual void Init(void) {
	}
	virtual void Done(void) {
	}

	virtual void UpdateControls(void) {
	}

	virtual void PaintWindow(void);
	virtual bool ProcessEvent(SDL_Event aEvent);
};
//----------------------------------------------------------------------------

#endif /* SRC_WINDOW_H_ */
