/*
 * TapeScroller.cpp
 *
 *  Created on: Oct 25, 2016
 *      Author: zaqc
 */
#include <SDL.h>

#include "TapeScroller.h"
//----------------------------------------------------------------------------

TapeScroller::TapeScroller(SDL_Renderer *aRnd, int aW, int aH) {
	m_T1 = m_T2 = NULL;

	m_Position = 0;

	m_W = aW;
	m_H = aH;

	m_T1 = SDL_CreateTexture(aRnd, SDL_PIXELFORMAT_ABGR8888,
			SDL_TEXTUREACCESS_STREAMING, aW, aH);

	m_T2 = SDL_CreateTexture(aRnd, SDL_PIXELFORMAT_ABGR8888,
			SDL_TEXTUREACCESS_STREAMING, aW, aH);
}
//----------------------------------------------------------------------------

TapeScroller::~TapeScroller() {
	if (NULL != m_T2)
		SDL_DestroyTexture(m_T2);

	if (NULL != m_T1)
		SDL_DestroyTexture(m_T1);
}
//----------------------------------------------------------------------------

void TapeScroller::PaintTape(SDL_Texture *aTxt, int aX, int aW) {

}
//----------------------------------------------------------------------------

void TapeScroller::ScrollTo(int aDX) { // scroll size in Screen Point (considering Zoom)
	if (aDX > 0) {
		if (aDX < m_W) {
			int tx = m_Position % m_W;
			if (aDX < m_W - tx) {
				PaintTape(m_T2, m_Position % m_W, aDX);
			} else {
				int w = m_W - tx;
				PaintTape(m_T2, tx, w);
				PaintTape(m_T1, 0, aDX - w);

				SDL_Texture *tmp = m_T1;
				m_T1 = m_T2;
				m_T2 = tmp;
			}
			m_Position += aDX;
		} else if (aDX < m_W * 2) {
			int tx = m_Position % m_W;
			int w = m_W - tx;
			PaintTape(m_T1, tx, w);
			PaintTape(m_T1, 0, aDX - w);

			SDL_Texture *tmp = m_T1;
			m_T1 = m_T2;
			m_T2 = tmp;
		}
	} else if (aDX < 0) {

	}
}
//----------------------------------------------------------------------------

void TapeScroller::Flip(SDL_Renderer *aRnd) {
	if (m_Position < m_W) {
	} else {
	}
}
//----------------------------------------------------------------------------
