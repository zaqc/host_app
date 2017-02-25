/*
 * Label.h
 *
 *  Created on: Sep 13, 2016
 *      Author: zaqc
 */

#ifndef SRC_LABEL_H_
#define SRC_LABEL_H_

#include <SDL.h>
#include <SDL_ttf.h>

#include <string>

#include "Window.h"
//----------------------------------------------------------------------------

class Label: public Control {
protected:
	TTF_Font *m_Font;
	std::wstring m_Text;
public:
	Label(int aX, int aY, int aW, int aH, std::wstring aText);
	virtual ~Label();

	void SetText(std::wstring aStr);

	virtual bool ProcessEvent(SDL_Event aEvent);
	virtual void Render(SDL_Renderer *aRnd);
};

#endif /* SRC_LABEL_H_ */
