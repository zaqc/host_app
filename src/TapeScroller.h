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
public:
	TapeScroller();
	virtual ~TapeScroller();


};
//----------------------------------------------------------------------------

#endif /* TAPESCROLLER_H_ */
