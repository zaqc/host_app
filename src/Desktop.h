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

class Desktop {
protected:
	Window *m_ActiveWindow;
	SDL_Renderer *m_Rnd;
	SDL_Window *m_Wnd;

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
