/*
 * BScanWnd.cpp
 *
 *  Created on: Sep 28, 2016
 *      Author: zaqc
 */

#include <iostream>
#include <string.h>

#include <stdio.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

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

	unsigned char r = (int) (a1 + (float) (b1 - a1) / (float) aMax * (float) aVal) & 0xFF;
	unsigned char g = (int) (a2 + (float) (b2 - a2) / (float) aMax * (float) aVal) & 0xFF;
	unsigned char b = (int) (a3 + (float) (b3 - a3) / (float) aMax * (float) aVal) & 0xFF;
	unsigned char a = (int) (a4 + (float) (b4 - a4) / (float) aMax * (float) aVal) & 0xFF;

	SDL_Color ret = { r, g, b, a };
	return ret;
}

void InitCLUT(SDL_Color *aTab, SDL_Color aC1, SDL_Color aC2, SDL_Color aXX, SDL_Color aBG) {

	for (int index = 0; index < 4; index++) {
		for (int color = 0; color < 256; color += 16) {
			int ndx = (color << ((index & 1) << 3)) + (((index >> 1) & 1) << 16);
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
			aTab[i << 8 | n] = GetMdlColor(aTab[i << 8], aTab[i << 8 | i], n, i);
			aTab[n << 8 | i] = GetMdlColor(aTab[i], aTab[i << 8 | i], n, i);
		}
	}
}

//============================================================================
//	TapeConfig
//============================================================================
<<<<<<< HEAD
void print_node(xmlNode *aNode) {
=======
TapeConfig::TapeConfig() {
	m_CurrentTrack = NULL;
	m_Doc = xmlReadFile("default_tape_config.xml", NULL, 0);
	if (NULL != m_Doc) {
		m_RootNode = xmlDocGetRootElement(m_Doc);
	}
}

TapeConfig::~TapeConfig() {
	if (NULL != m_Doc)
		xmlFreeDoc(m_Doc);

	xmlCleanupParser();
}

int StrToInt(xmlChar *str) {
	return 1;
}

Channel *TapeConfig::ParseChannel(xmlAttr *aAttr) {
	Channel *ch = new Channel();
	for (xmlAttr *attr = aAttr; NULL != attr; attr = attr->next) {
		if (0 == xmlStrcmp(attr->name, (xmlChar*) "number")) {
			std::cout << "\t number" << std::endl;
			if (NULL != attr->children && NULL != attr->children->content)
				ch->Number = StrToInt(attr->children->content);
		} else if (0 == xmlStrcmp(attr->name, (xmlChar*) "data_offset")) {
			std::cout << "\t data_offset" << std::endl;
			if (NULL != attr->children && NULL != attr->children->content)
				ch->DataIndex = StrToInt(attr->children->content);
		} else if (0 == xmlStrcmp(attr->name, (xmlChar*) "data_size")) {
			std::cout << "\t data_size" << std::endl;
			if (NULL != attr->children && NULL != attr->children->content)
				ch->DataIndex = StrToInt(attr->children->content);
		} else if (0 == xmlStrcmp(attr->name, (xmlChar*) "direction")) {
			std::cout << "\t direction" << std::endl;
			if (NULL != attr->children && NULL != attr->children->content)
				ch->DataIndex = StrToInt(attr->children->content);
		} else if (0 == xmlStrcmp(attr->name, (xmlChar*) "angle")) {
			std::cout << "\t angle" << std::endl;
			if (NULL != attr->children && NULL != attr->children->content)
				ch->DataIndex = StrToInt(attr->children->content);
		} else if (0 == xmlStrcmp(attr->name, (xmlChar*) "rotate")) {
			std::cout << "\t rotate" << std::endl;
			if (NULL != attr->children && NULL != attr->children->content)
				ch->DataIndex = StrToInt(attr->children->content);
		} else if (0 == xmlStrcmp(attr->name, (xmlChar*) "work_edge")) {
			std::cout << "\t work_edge" << std::endl;
			if (NULL != attr->children && NULL != attr->children->content)
				ch->DataIndex = StrToInt(attr->children->content);
		}
	}
	return ch;
}

Track *TapeConfig::ParseTrack(xmlAttr *aAttr) {
	Track *tr = new Track();
	return tr;
}

void TapeConfig::ProcessNode(xmlNode *aNode, NodeLevel aNodeLevel) {
>>>>>>> abf8105a8e0df2d6c4b82d56f4b5690566140cf0
	xmlNode *p = aNode;
	while (NULL != p) {
		if (p->type == XML_ELEMENT_NODE) {
			switch (aNodeLevel) {
			case nlNone:
				if (0 == xmlStrcmp(p->name, (xmlChar*) "root"))
					ProcessNode(p->children, nlRoot);
				break;

			case nlRoot:
				if (0 == xmlStrcmp(p->name, (xmlChar*) "channel"))
					ProcessNode(p->children, nlChannel);
				else if (0 == xmlStrcmp(p->name, (xmlChar*) "tape"))
					ProcessNode(p->children, nlTape);
				break;

			case nlChannel:
				if (0 == xmlStrcmp(p->name, (xmlChar*) "item")) {
					std::cout << "Process Channel Item" << std::endl;
					if (NULL != p->properties) {
						Channel *ch = ParseChannel(p->properties);
						m_Channel.push_back(ch);
					}
				}
				break;

			case nlTape:
				if (0 == xmlStrcmp(p->name, (xmlChar*) "track")) {
					std::cout << "Process Track Of Tape Item" << std::endl;
					m_CurrentTrack = ParseTrack(p->properties);
					ProcessNode(p->children, nlChannelOfTrack);
				}
				break;

			case nlChannelOfTrack:
				if (0 == xmlStrcmp(p->name, (xmlChar*) "channel")) {
					std::cout << "Process Channel Of Track Item" << std::endl;
				}
				break;

			default:
				break;
			}
		}

		p = p->next;
	}
}

void TapeConfig::LoadConfig(void) {
	ProcessNode(m_RootNode, nlNone);
}

//============================================================================
//	XML Routine
//============================================================================
void print_node(xmlNode *aNode) {
	xmlNode *p = aNode;
	while (NULL != p) {
		if (p->type == XML_ELEMENT_NODE) {
			std::cout << "ELEMENT name: " << p->name << " type:" << p->type << std::endl;

			xmlAttr *attr = p->properties;
			while (NULL != attr) {
<<<<<<< HEAD
				std::cout << "ATTR name:" << attr->name << " type" << attr->type
						<< std::endl;
=======
				std::cout << "ATTR name:" << attr->name << " type" << attr->type << std::endl;

>>>>>>> abf8105a8e0df2d6c4b82d56f4b5690566140cf0
				if (attr->children && attr->children->content)
					std::cout << attr->children->content << std::endl;
				attr = attr->next;
			}

			if (0 == xmlStrcmp(p->name, (xmlChar*) "root"))
				print_node(p->children);
			else if (0 == xmlStrcmp(p->name, (xmlChar*) "tape")) {
				std::cout << "---=== TAPE ===---" << std::endl;
				print_node(p->children);
			} else if (0 == xmlStrcmp(p->name, (xmlChar*) "track")) {
				std::cout << "-= Track =-" << std::endl;
				print_node(p->children);
			} else {
				std::cout << "p->name = " << p->name << std::endl;
			}
		}

		p = p->next;
	}
<<<<<<< HEAD
}

//============================================================================
//	TapeConfig
//============================================================================
TapeConfig::TapeConfig() {
	m_Doc = xmlReadFile("default_tape_config.xml", NULL, 0);
	if (NULL != m_Doc)
		m_RootNode = xmlDocGetRootElement(m_Doc);
}
//----------------------------------------------------------------------------

TapeConfig::~TapeConfig() {
	if (NULL != m_Doc)
		xmlFreeDoc(m_Doc);

	xmlCleanupParser();
=======
>>>>>>> abf8105a8e0df2d6c4b82d56f4b5690566140cf0
}
//----------------------------------------------------------------------------

int TapeConfig::GetChannelCount(void) {
	return 0;
}

Channel TapeConfig::GetChannel(int aIndex) {
	Channel ch;
	return ch;
}

int TapeConfig::GetTrackCount(void) {
	return 0;
}

Track TapeConfig::GetTrack(int aIndex) {
	Track t;
	return t;
}

void TapeConfig::ParseChannelItem(xmlNode *aNode) {

}

void TapeConfig::DecodeChannelParam(xmlAttr *aParam) {

}

void TapeConfig::ParseTrackItem(xmlNode *aNode) {

}

void TapeConfig::DecodeTrackParam(xmlAttr *aParam) {

}

void TapeConfig::LoadConfig() {
	if (NULL != m_RootNode) {

	}
}
//----------------------------------------------------------------------------

//============================================================================
//	BScanWnd
//============================================================================
BScanWnd::BScanWnd(SDL_Renderer *aRnd, int aX, int aY, int aW, int aH) :
		Window(aRnd, aX, aY, aW, aH) {
	m_RTS = NULL;

	TapeConfig *tc = new TapeConfig();
	tc->LoadConfig();
	delete tc;

//	xmlDoc *doc;
//	xmlNode *root;
//	doc = xmlReadFile("default_tape_config.xml", NULL, 0);
//	if (NULL != doc) {
//		root = xmlDocGetRootElement(doc);
//		xmlNode *p = root;
//		print_node(p);
//
//		xmlFreeDoc(doc);
//	}
//
//	xmlCleanupParser();
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

	for (std::vector<LineGenStruct>::iterator i = m_Gen.begin(); i != m_Gen.end(); i++) {
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

	for (int i = 0; i < MAX_CHANNEL_COUNT; i++) {
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

	for (int i = 0; i < MAX_TRACK_COUNT; i++) {
		if (i < 6) {
			m_Track[i].ShowIt = true;
			m_Track[i].AutoHeight = true;
			m_Track[i].DefaultHeight = 128;
			m_Track[i].MinTrackHeight = 16;
			m_Track[i].Side = i < 3 ? 1 : 2;
			m_Track[i].TrackTop = i * 10;
			for (int ch = 0; ch < 4; ch++) {
				m_Track[i].Channel.push_back(&m_Channel[i * 4 + ch]);
				m_Channel[i * 4 + ch].ColorIndex = ch;
			}
		} else {
			m_Track[i].ShowIt = false;
		}
	}

	for (int i = 0; i < MAX_DATA_SIZE; i++) {
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
	SDL_Color bg = { 0, 0, 128, 255 }; // black
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

	// calc default tape (full) height
	Track *t = m_Track;
	for (int track = 0; track < MAX_TRACK_COUNT; track++) {
		if (t->ShowIt) {
			if (t->AutoHeight)
				stretch_space += t->DefaultHeight + 1; //TODO: add track bevel (1 line)
			else {
				fixed_space += t->DefaultHeight + 1;
				min_stretch_space += t->MinTrackHeight;
			}
		}
		t++;
	}

	// clear Preparser table & Index table

	for (int i = 0; i < MAX_TAPE_HEIGHT; i++) {
		m_SO[i].UseIt = false;
	}

	for (int i = 0; i < MAX_DATA_SIZE; i++) {
		m_Index[i] = NULL;
	}

	// align track on tape with real height
	if (fixed_space + min_stretch_space < m_H) {
		Track *t = m_Track;
		int track_top = 0;
		ScreenOut *pso = m_SO;
		for (int track = 0; track < MAX_TRACK_COUNT; track++) {
			if (t->ShowIt) {
				if (t->AutoHeight) //TODO: add more code for fixed size track & track greate then channel DefaultHeight (where pso->H > 1)
					t->RealHeight = (float) t->DefaultHeight / (float) (stretch_space) * (float) (m_H - fixed_space);
				else
					t->RealHeight = t->DefaultHeight;

				for (int i = 0; i < t->RealHeight; i++) {
					pso->FirstUse = true;
					pso->UseIt = true;
					pso->H = 1;
					pso->Y = track_top + i;
					pso->Index = 0;
					pso->Value1 = 0;
					pso->Value2 = 0;
					pso++;
				}

				t->TrackTop = track_top;
				track_top += t->RealHeight + 1;
			}
			t++;
		}
	}

	// fill m_Index table with pointers to Preparser (on screen data)
	Channel *channel = m_Channel;
	for (int ch = 0; ch < MAX_CHANNEL_COUNT; ch++) {
		if (channel->UseIt) {
			Track *t = m_Track;
			for (int track = 0; track < MAX_TRACK_COUNT; track++) {
				if (t->ShowIt) {
					for (std::vector<Channel*>::iterator ich = t->Channel.begin(); ich != t->Channel.end(); ich++) {
						if (*ich == channel) {
							for (int n = 0; n < channel->DataSize; n++) {
								int n_top = t->TrackTop + (float) n / (float) channel->DataSize * (float) t->RealHeight;

								for (int i = 0; i < MAX_TAPE_HEIGHT; i++) {
									if (m_SO[i].UseIt && m_SO[i].Y == n_top) {
										m_Index[channel->DataIndex + n] = &m_SO[i];
										break;
									}
								}
							}
						}
					}
				}
				t++;
			}
		}
		channel++;
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
		m_Txt = SDL_CreateTexture(aRnd, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, m_W, m_H);
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
//				unsigned char *data = GetData();
//				pptr = (unsigned char*) p;
//				for (int i = 0; i < m_H; i++) {
//					memset(pptr + m_W * 4 - (step - n) * 4, data[i], 4);
//					pptr += pitch;
//				}
				PrepareDrawBuffer();

				pptr = (unsigned char*) p;
				ScreenOut *so = m_SO;
				while (so->UseIt) {
					if (so->Y >= 0 && so->Y < m_H) {
						if (so->Index >= 0) {
							SDL_Color c = CLUT[(so->Value1 << 8) | (so->Value2) | (so->Index << 16)];
							*(SDL_Color*) (pptr + so->Y * pitch + m_W * 4 - (step - n) * 4) = c;
						} else {
<<<<<<< HEAD
							*(SDL_Color*) (pptr + so->Y * pitch + m_W * 4
									- (step - n) * 4) = (SDL_Color ) {0, 0, 0,
								255};
=======
							*(SDL_Color*) (pptr + so->Y * pitch + m_W * 4 - (step - n) * 4) = (SDL_Color ) { 0, 0, 0,
											255 };
>>>>>>> abf8105a8e0df2d6c4b82d56f4b5690566140cf0
						}
					}
					so++;
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
// color_index = CLUT[(i1 << 16) + c1 + c2 << 8]
void RealTapeScroller::PrepareDrawBuffer(void) {
	for (int i = 0; i < MAX_TAPE_HEIGHT; i++) {
		m_SO[i].Value1 = 0;
		m_SO[i].Value2 = 0;
		m_SO[i].Index = -1;
	}

	int m_Zoom = 8; //TODO:	export it to header
	for (int zoom = 0; zoom < m_Zoom; zoom++) {
		unsigned char *buf = GetData();
		ScreenOut **pp = m_Index;
		Channel *ch = m_Channel;
		for (int ch_num = 0; ch_num < MAX_CHANNEL_COUNT; ch_num++) {
			if (ch->UseIt) {
				unsigned char *pbuf = buf + ch->DataIndex;
				for (int i = 0; i < ch->DataSize; i++) {
					int val = *pbuf;
					ScreenOut *p = m_Index[ch->DataIndex + i];
					if (NULL != p) {
						if (ch->ColorIndex != p->Index) {
							if (p->Value1 <= val) {
								p->Value2 = p->Value1;
								p->Value1 = val;
								p->Index = ch->ColorIndex;
							} else {
								if (p->Value2 < val)
									p->Value2 = val;
							}
						} else {
							if (p->Value1 < val)
								p->Value1 = val;
						}
					}
					pbuf++;
					pp++;
				}
			}
			ch++;
		}
	}
}
//----------------------------------------------------------------------------

void RealTapeScroller::ScrollAndDraw(void) {
}
//----------------------------------------------------------------------------
