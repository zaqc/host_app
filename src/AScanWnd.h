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

class MenuItem{
public:
};

class Menu: public Control {
protected:
	std::string m_Caprion;
	MenuItem *m_Parent;
	std::vector<MenuItem*> m_Items;
public:
	Menu(int aX, int aY, int aW, int aH, std::string aCaption, MenuItem *aParent);
	virtual ~Menu();
};

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

#endif /* ASCANWND_H_ */
