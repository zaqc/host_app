/*
 * Button.h
 *
 *  Created on: Sep 14, 2016
 *      Author: zaqc
 */

#ifndef SRC_BUTTON_H_
#define SRC_BUTTON_H_

#include <SDL.h>
#include <SDL_ttf.h>
#include <string>

#include "Window.h"
//----------------------------------------------------------------------------

class Button: public Control {
	TTF_Font *m_Font;
	std::string m_Text;
	bool m_Down;
	bool m_OnClick;
public:
	Button(int aX, int aY, int aW, int aH, std::string aText);
	virtual ~Button();

//	void Init(void);
//	void Done(void);

	virtual bool CanFocused(void) {
		return true;
	}

	bool OnClick(void);

	virtual bool OnMouseDown(uint8_t aButton, int32_t aX, int32_t aY);
	virtual bool OnMouseUp(uint8_t aButton, int32_t aX, int32_t aY);
	virtual bool OnMouseMove(int32_t aX, int32_t aY);

	virtual void Render(SDL_Renderer *aRnd);
	virtual bool ProcessEvent(SDL_Event aEvent);
};

#endif /* SRC_BUTTON_H_ */
