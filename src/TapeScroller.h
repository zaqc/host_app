/*
 * TapeScroller.h
 *
 *  Created on: Oct 25, 2016
 *      Author: zaqc
 */

#ifndef TAPESCROLLER_H_
#define TAPESCROLLER_H_

#include <SDL.h>
//----------------------------------------------------------------------------

class TapeScroller {
protected:
	int m_Position;
	SDL_Texture *m_T1;
	SDL_Texture *m_T2;
	int m_W;
	int m_H;
public:
	TapeScroller(SDL_Renderer *aRnd, int aW, int aH);
	virtual ~TapeScroller();

	virtual void PaintTape(SDL_Texture *aT, int aX, int aW);

	void ScrollTo(int aDX);

	void Flip(SDL_Renderer *aRnd);
};
//----------------------------------------------------------------------------

#endif /* TAPESCROLLER_H_ */
