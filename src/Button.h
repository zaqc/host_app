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

class Button {
	int m_X;
	int m_Y;
	int m_W;
	int m_H;
	TTF_Font *m_Font;
	std::string m_Text;
	bool m_Down;
public:
	Button(int aX, int aY, int aW, int aH, std::string aText);
	virtual ~Button();

//	void Init(SDL_Renderer *aRnd);
//	void Done(void);

	void Render(SDL_Renderer *aRnd);
	bool ProcessEvent(SDL_Event aEvent);
};

#endif /* SRC_BUTTON_H_ */
