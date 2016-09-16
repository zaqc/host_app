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
};

#endif /* ASCANWND_H_ */
