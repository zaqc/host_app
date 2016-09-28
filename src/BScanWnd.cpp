/*
 * BScanWnd.cpp
 *
 *  Created on: Sep 28, 2016
 *      Author: zaqc
 */

#include "BScanWnd.h"
#include "Window.h"
//----------------------------------------------------------------------------

//============================================================================
//	BScanWnd
//============================================================================
BScanWnd::BScanWnd(SDL_Renderer *aRnd, int aX, int aY, int aW, int aH) :
		Window(aRnd, aX, aY, aW, aH) {
	// TODO Auto-generated constructor stub

}
//----------------------------------------------------------------------------

BScanWnd::~BScanWnd() {
	// TODO Auto-generated destructor stub
}
//----------------------------------------------------------------------------

//============================================================================
//	RealTapeScroller
//============================================================================
RealTapeScroller::RealTapeScroller(int aW, int aH) {
	m_W = aW;
	m_H = aH;
	m_FB = NULL;
	m_DB = NULL;

	m_FB = new unsigned int[aW * aH];
	m_DB = new unsigned int[aW * aH];
}
//----------------------------------------------------------------------------

RealTapeScroller::~RealTapeScroller() {
	delete[] m_DB;
	delete[] m_FB;
}
//----------------------------------------------------------------------------

void RealTapeScroller::InitTapeConfig(void) {

}
//----------------------------------------------------------------------------

void RealTapeScroller::CalcPreparserTable(void) {
	int fixed_space = 0;
	int stretch_space = 0;
	int min_stretch_space = 0;
	Track *t = m_Tape.Track;
	for (int track = 0; track < m_Tape.TrackCount; track++) {
		if (t->ShowIt) {
			if (t->AutoHeight)
				stretch_space += t->DefaultHeight;
			else {
				fixed_space += t->DefaultHeight;
				min_stretch_space += t->MinTrackHeight;
			}
		}
		t++;
	}

	if (fixed_space + min_stretch_space < m_H) {
		Track *t = m_Tape.Track;
		for (int track = 0; track < m_Tape.TrackCount; track++) {
			if (t->ShowIt) {
				if (t->AutoHeight) {
					t->RealHeight = (float) t->DefaultHeight
							/ (float) (stretch_space)
							* (float) (m_H - fixed_space);
				}
			}
		}
	}
}
//----------------------------------------------------------------------------

int RealTapeScroller::GetDataLength(void) {

	return 0;
}
//----------------------------------------------------------------------------

unsigned char * RealTapeScroller::GetData(void) {
	return NULL;
}
//----------------------------------------------------------------------------

void RealTapeScroller::Show(SDL_Renderer *aRnd, int aX, int aY) {
}
//----------------------------------------------------------------------------

void RealTapeScroller::PrepareDrawBuffer(void) {
}
//----------------------------------------------------------------------------

void RealTapeScroller::ScrollAndDraw(void) {
}
//----------------------------------------------------------------------------
