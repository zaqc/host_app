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
public:
	Control();
	virtual ~Control();

	virtual void Render(SDL_Renderer *aRnd) {
	}
	virtual bool ProcessEvent(SDL_Event aEvent) {
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

	virtual void Init(void);
	virtual void Done(void);

	virtual void UpdateControls(void);

	virtual void Paint(void);
	int Execute(void);
};
//----------------------------------------------------------------------------

#endif /* SRC_WINDOW_H_ */
