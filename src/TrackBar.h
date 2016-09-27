/*
 * TrackBar.h
 *
 *  Created on: Sep 12, 2016
 *      Author: zaqc
 */

#ifndef SRC_TRACKBAR_H_
#define SRC_TRACKBAR_H_

#include <SDL.h>

#include "Window.h"
//----------------------------------------------------------------------------

class TrackBar: public Control {
protected:
	int m_Pos;
	int m_Min;
	int m_Max;
	int m_ThumbWidth;
	int m_ThumbHeight;
	int m_TrackSize;
	int m_Border;
	int m_MouseDownX;
	int m_MouseDownY;
	bool m_MouseDown;
	float m_PosDelta;
	bool m_Revert;
public:
	TrackBar(int aX, int aY, int aW, int aH);
	virtual ~TrackBar();

	int GetValue(void) {
		if (m_MouseDown)
			return m_PosDelta;
		return m_Pos;
	}

	virtual bool CanFocused(void) {
		return true;
	}

	virtual bool OnMouseDown(uint8_t aButton, int32_t aX, int32_t aY);
	virtual bool OnMouseUp(uint8_t aButton, int32_t aX, int32_t aY);
	virtual bool OnMouseMove(int32_t aX, int32_t aY);

	virtual bool OnKeyDown(SDL_Scancode aScanCode);

	SDL_Rect CalcThumbRect(void);

	void PaintBackground(SDL_Renderer *aRnd, int aX, int aY, int aW, int aH);
	void PaintTrack(SDL_Renderer *aRnd, int aX, int aY, int aW, int aH);
	void PaintThumb(SDL_Renderer *aRnd, int aX, int aY, int aW, int aH);

	bool PtInThumb(int aX, int aY);

	virtual void Render(SDL_Renderer *aRnd);
};
//----------------------------------------------------------------------------

#endif /* SRC_TRACKBAR_H_ */
