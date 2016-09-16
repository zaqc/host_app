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

class Window {
protected:
	std::vector<Control*> m_Control;
public:
	Window(int aX, int aY, int aW, int aH);
	virtual ~Window();

	void Paint(SDL_Renderer *aRnd);

	int Execute(SDL_Renderer *aRnd);
};
//----------------------------------------------------------------------------

#endif /* SRC_WINDOW_H_ */
