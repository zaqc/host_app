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
	m_RTS = NULL;
}
//----------------------------------------------------------------------------

BScanWnd::~BScanWnd() {
}
//----------------------------------------------------------------------------

void BScanWnd::Init(void) {
	m_RTS = new RealTapeScroller(700, 450);
}
//----------------------------------------------------------------------------

void BScanWnd::Done(void) {
	delete m_RTS;
}
//----------------------------------------------------------------------------

void BScanWnd::PaintWindow(void) {
	SDL_SetRenderDrawColor(m_Rnd, 192, 192, 192, 255);
	SDL_RenderClear(m_Rnd);

	m_RTS->Show(m_Rnd, 10, 10);
}
//----------------------------------------------------------------------------

//============================================================================
//	DataGenerator
//============================================================================
DataGenerator::DataGenerator() {
	memset(m_OutData, 0, sizeof(m_OutData));
	for (int i = 0; i < 28; i++) {
		LineGenStruct lgs;
		if (i & 1) {
			lgs.CurX = i * 128;
			lgs.DX = 1;
			lgs.Valid = true;
			lgs.YCount = 0;
			lgs.YMaxCount = 128;
			m_Gen.push_back(lgs);
		} else {
			lgs.CurX = i * 128 + 127;
			lgs.DX = -1;
			lgs.Valid = true;
			lgs.YCount = -140 - i * 50;
			lgs.YMaxCount = 128;
			m_Gen.push_back(lgs);
		}
	}
}

DataGenerator::~DataGenerator() {

}

unsigned char *DataGenerator::GetData(void) {
	memset(m_OutData, 0, sizeof(m_OutData));

	if (m_Gen.size() < MAX_GEN_LINE && ((rand() % 10) == 5)) {
		LineGenStruct lgs;
		lgs.CurX = rand() % 4096;
		lgs.DX = (float) ((rand() % 100) - 50) / 100.0f;
		lgs.Valid = true;
		lgs.YCount = 0;
		lgs.YMaxCount = rand() % 1024;
		m_Gen.push_back(lgs);
	}

	for (std::vector<LineGenStruct>::iterator i = m_Gen.begin();
			i != m_Gen.end(); i++) {
		if (i->CurX >= 0 && i->CurX < 4096 && i->YCount < i->YMaxCount) {
			if (i->YCount >= 0) {
				m_OutData[(int) i->CurX] = rand() % 256;
				i->CurX += i->DX;
			}
			i->YCount++;
		} else
			i->Valid = false;
	}

	std::vector<LineGenStruct>::iterator i = m_Gen.begin();
	while (i != m_Gen.end()) {
		if (!i->Valid)
			i = m_Gen.erase(i);
		else
			i++;
	}

	return m_OutData;
}

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
			m_Channel[i].ColorIndex = i % 4;
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

	for (int i = 0; i < 10; i++) {
		if (i < 6) {
			m_Tape.Track[i].ShowIt = true;
			m_Tape.Track[i].AutoHeight = true;
			m_Tape.Track[i].DefaultHeight = 128;
			m_Tape.Track[i].MinTrackHeight = 16;
			m_Tape.Track[i].Side = i < 3 ? 1 : 2;
			m_Tape.Track[i].TrackTop = i * 10;
			for (int ch = 0; ch < 4; ch++)
				m_Tape.Track[i].Channel[ch] = &m_Channel[i * 4 + ch];
		} else {
			m_Tape.Track[i].ShowIt = false;
		}
	}

	for (int i = 0; i < 4096; i++) {
		m_Index[i] = NULL;
	}

	m_DataGenerator = new DataGenerator();
	m_Txt = NULL;
}
//----------------------------------------------------------------------------

RealTapeScroller::~RealTapeScroller() {
	if (m_Txt != NULL) {
		SDL_DestroyTexture(m_Txt);
		m_Txt = NULL;
	}
	delete m_DataGenerator;

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
	return m_DataGenerator->GetData();
}
//----------------------------------------------------------------------------

void RealTapeScroller::Show(SDL_Renderer *aRnd, int aX, int aY) {
	if (NULL == m_Txt) {
		m_Txt = SDL_CreateTexture(aRnd, SDL_PIXELFORMAT_ABGR8888,
				SDL_TEXTUREACCESS_STREAMING, m_W, m_H);
		SDL_Rect r = { 0, 0, m_W, m_H };
		unsigned int *p;
		int pitch;
		if (0 == SDL_LockTexture(m_Txt, &r, (void**) &p, &pitch)) {
			unsigned char *pptr = (unsigned char*) p;
			for (int i = 0; i < m_H; i++) {
				memset(pptr, 255, m_W * 4);
				pptr += pitch;
			}
			SDL_UnlockTexture(m_Txt);
		}
	}

	if (NULL != m_Txt) {
		SDL_Rect r = { 0, 0, m_W, m_H };
		unsigned int *p;
		int pitch;
		if (0 == SDL_LockTexture(m_Txt, &r, (void**) &p, &pitch)) {
			unsigned char *pptr = (unsigned char*) p;
			unsigned char *data = GetData();
			for (int i = 0; i < m_H; i++) {
				for (int n = 0; n < m_W - 1; n++)
					p[i * m_W + n] = p[i * m_W + n + 1];
				//memcpy(&p[i * m_W], &p[i * m_W + 1], m_W * 4 -4);
				memset(pptr + m_W * 4 - 4, data[i], 4);
				pptr += pitch;
			}
			SDL_UnlockTexture(m_Txt);
		}
		SDL_Rect dr = { aX, aY, m_W, m_H };
		SDL_RenderCopy(aRnd, m_Txt, &r, &dr);
	}
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
