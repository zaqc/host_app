/*
 * BScanWnd.cpp
 *
 *  Created on: Sep 28, 2016
 *      Author: zaqc
 */

#include <iostream>

#include "BScanWnd.h"
#include "Window.h"
//----------------------------------------------------------------------------

SDL_Color CLUT[256 * 256 * 4];

SDL_Color GetMdlColor(SDL_Color aC1, SDL_Color aC2, int aVal, int aMax) {

	if (aVal > 255)
		aVal = 255;

	int a1 = aC1.r;
	int a2 = aC1.g;
	int a3 = aC1.b;
	int a4 = aC1.a;

	int b1 = aC2.r;
	int b2 = aC2.g;
	int b3 = aC2.b;
	int b4 = aC2.a;

	unsigned char r = (int) (a1
			+ (float) (b1 - a1) / (float) aMax * (float) aVal) & 0xFF;
	unsigned char g = (int) (a2
			+ (float) (b2 - a2) / (float) aMax * (float) aVal) & 0xFF;
	unsigned char b = (int) (a3
			+ (float) (b3 - a3) / (float) aMax * (float) aVal) & 0xFF;
	unsigned char a = (int) (a4
			+ (float) (b4 - a4) / (float) aMax * (float) aVal) & 0xFF;

	SDL_Color ret = { r, g, b, a };
	return ret;
}

void InitCLUT(SDL_Color *aTab, SDL_Color aC1, SDL_Color aC2, SDL_Color aXX,
		SDL_Color aBG) {

	for (int index = 0; index < 4; index++) {
		for (int color = 0; color < 256; color += 16) {
			int ndx = (color << ((index & 1) << 3))
					+ (((index >> 1) & 1) << 16);
			std::cout << ndx << " ";
		}
		std::cout << std::endl;
	}

	for (int j = 0; j < 256; j++) {
		for (int i = 0; i < 256; i++) {
			aTab[i << 8 | j] = (SDL_Color ) {0, 0, 0, 0};
		}
	}

	for (int i = 0; i < 256; i++) {
		aTab[i << 8 | i] = GetMdlColor(aBG, aXX, i, 255);
		aTab[i << 8] = GetMdlColor(aBG, aC1, i, 255);
		aTab[i] = GetMdlColor(aBG, aC2, i, 255);
		for (int n = 1; n < i; n++) {
			aTab[i << 8 | n] = GetMdlColor(aTab[i << 8], aTab[i << 8 | i], n,
					i);
			aTab[n << 8 | i] = GetMdlColor(aTab[i], aTab[i << 8 | i], n, i);
		}
	}
}

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

	m_RTS->Show(m_Rnd, 50, 10);
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

	SDL_Color ww = { 255, 255, 255, 255 }; // white
	SDL_Color c1 = { 255, 255, 0, 255 }; // yellow
	SDL_Color c2 = { 0, 255, 255, 255 }; // cyan
	SDL_Color c3 = { 255, 0, 255, 255 }; // fuchsia
	SDL_Color c4 = { 0, 255, 0, 255 }; // lime
	SDL_Color xx = { 255, 0, 0, 255 }; // red
	SDL_Color bg = { 0, 0, 0, 255 }; // black
	InitCLUT(CLUT, c1, ww, xx, bg);
	InitCLUT(CLUT + 256 * 256, c2, ww, xx, bg);
	InitCLUT(CLUT + 256 * 256 * 2, c3, ww, xx, bg);
	InitCLUT(CLUT + 256 * 256 * 3, c4, ww, xx, bg);

	CalcPreparserTable();
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

	int step = 4;

	if (NULL != m_Txt) {
		SDL_Rect r = { 0, 0, m_W, m_H };
		unsigned int *p;
		int pitch;
		if (0 == SDL_LockTexture(m_Txt, &r, (void**) &p, &pitch)) {
			unsigned char *pptr = (unsigned char*) p;
			for (int i = 0; i < m_H; i++) {
				for (int n = 0; n < m_W - step; n++)
					p[i * m_W + n] = p[i * m_W + n + step];
				//memcpy(&p[i * m_W], &p[i * m_W + 1], m_W * 4 -4);
				pptr += pitch;
			}
			for (int n = 0; n < step; n++) {
				unsigned char *data = GetData();
				pptr = (unsigned char*) p;
				for (int i = 0; i < m_H; i++) {
					memset(pptr + m_W * 4 - (step - n) * 4, data[i], 4);
					pptr += pitch;
				}
			}

//			for (int j = 0; j < 256; j++) {
//				for (int i = 0; i < 256 * 3; i++) {
//					p[i + (j + 2) * m_W] = *(unsigned int*) &CLUT[i << 8 | j];
//				}
//			}

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
