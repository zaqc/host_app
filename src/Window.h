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
#include <vector>
//----------------------------------------------------------------------------

class Control {
protected:
	bool m_Invalidate;
	bool m_Hide;
public:
	Control();
	virtual ~Control();

	void Hide() {
		m_Hide = true;
	}
	void Show() {
		m_Hide = false;
	}

	bool GetHideState(){
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

	virtual void Render(SDL_Renderer *aRnd) { // prepare Texture's if they needed to update for redrawing
	}

	virtual void Paint(void) { // paint controls stuff (lines, rects, txtures) but not critical by time
	}

	virtual bool ProcessEvent(SDL_Event aEvent) {
		switch (aEvent.type) {
		case SDL_MOUSEBUTTONDOWN:
			if (OnMouseDown(aEvent.button.button, aEvent.button.x,
					aEvent.button.y))
				return true;
			break;
		case SDL_MOUSEBUTTONUP:
			if (OnMouseUp(aEvent.button.button, aEvent.button.x,
					aEvent.button.y))
				return true;
			break;
		case SDL_MOUSEMOTION:
			if (OnMouseMove(aEvent.motion.x, aEvent.motion.y))
				return true;
			break;
		}

		return false;
	}
};
//----------------------------------------------------------------------------

class Window: public Control {
protected:
	SDL_Renderer *m_Rnd;
	int m_X;
	int m_Y;
	int m_W;
	int m_H;
	std::vector<Control*> m_Control;
public:
	Window(SDL_Renderer *aRnd, int aX, int aY, int aW, int aH);
	virtual ~Window();

	virtual void ProcessMessage(int aID);

	void AddControl(Control *aControl);

	virtual void Init(void) {
	}
	virtual void Done(void) {
	}

	virtual void UpdateControls(void) {
	}

	virtual void Paint(void);
	virtual bool ProcessEvent(SDL_Event aEvent);

	int Execute(void) {
		return 0;
	}
};
//----------------------------------------------------------------------------

#endif /* SRC_WINDOW_H_ */
