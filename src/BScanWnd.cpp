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
}
//----------------------------------------------------------------------------

BScanWnd::~BScanWnd() {
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

	for (int i = 0; i < 32; i++) {
		if (i < 28) {
			m_Channel[i].UseIt = true;
			m_Channel[i].DataIndex = i * 128;
			m_Channel[i].DataSize = 128;
			if (i < 14)
				m_Channel[i].Side = 1;
			else
				m_Channel[i].Side = 2;
		} else {
			m_Channel[i].UseIt = false;
		}
	}

	for (int i = 0; i < 4096; i++) {
		m_Index[i] = NULL;
	}
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

	for (int i = 0; i < 1024; i++)
		m_Tape.PP[i].Next = NULL;

	for (int i = 0; i < 4096; i++)
		m_Index[i] = NULL;

	m_Tape.PPCount = 0;
	if (fixed_space + min_stretch_space < m_H) {
		Track *t = m_Tape.Track;
		for (int track = 0; track < m_Tape.TrackCount; track++) {
			if (t->ShowIt) {
				if (t->AutoHeight) {
					t->RealHeight = (float) t->DefaultHeight
							/ (float) (stretch_space)
							* (float) (m_H - fixed_space);
					if (t->RealHeight < t->DefaultHeight) {
						for (int n = 0; n < t->DefaultHeight; n++) {
							int pos = (float) t->RealHeight
									/ (float) t->DefaultHeight * (float) n;
							for (int ch = 0; ch < 4; ch++) {
								if (NULL != t->Channel[ch]) {
									if (NULL
											== m_Index[t->Channel[ch]->DataIndex]) {
										m_Index[t->Channel[ch]->DataIndex] =
												&m_Tape.PP[pos + t->TrackTop];
									} else {
										Preparser *pp =
												m_Index[t->Channel[ch]->DataIndex];
										m_Index[t->Channel[ch]->DataIndex] =
												&m_Tape.PP[pos + t->TrackTop];
										m_Index[t->Channel[ch]->DataIndex]->Next =
												pp;
									}
								}
							}
						}
					}
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
	unsigned char *buf = GetData();
	Preparser **pp = m_Index;
	Channel *ch = m_Channel;
	for (int ch_num = 0; ch_num < 32; ch_num++) {
		if (ch->UseIt) {
			unsigned char *pbuf = buf + ch->DataIndex;
			for (int i = 0; i < ch->DataSize; i++) {
				int val = *pbuf;
				Preparser *pr = *pp;
				while (NULL != pr) {
					if (pr->Value1 < val) {
						pr->Value2 = pr->Value1;
						pr->Index2 = pr->Index1;
						pr->Value1 = val;
						pr->Index1 = ch->ColorIndex;
					} else {
						if (pr->Value2 < val) {
							pr->Value2 = val;
							pr->Index2 = ch->ColorIndex;
						}
					}
					pr = pr->Next;
				}
				pbuf++;
				(*pp)++;
			}
		}
		ch++;
	}
}
//----------------------------------------------------------------------------

void RealTapeScroller::ScrollAndDraw(void) {
}
//----------------------------------------------------------------------------
