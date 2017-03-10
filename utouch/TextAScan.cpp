/*
 * TextAScan.cpp
 *
 *  Created on: Mar 6, 2017
 *      Author: zaqc
 */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "TextAScan.h"
#include "GlUtil.h"
#include "TextFont.h"
//----------------------------------------------------------------------------

//============================================================================
//	IntMenuItem
//============================================================================

MenuItem::MenuItem(TextAScan *aTextAScan) {
	m_TextAScan = aTextAScan;
	m_Selected = false;
	m_IsChanged = false;
}
//----------------------------------------------------------------------------

MenuItem::~MenuItem() {
}
//----------------------------------------------------------------------------

void MenuItem::SetSelected(bool aSelected) {
	m_Selected = aSelected;
}
//----------------------------------------------------------------------------

int MenuItem::GetWidth(void) {
	return 0;
}
//----------------------------------------------------------------------------

void MenuItem::ProcessButton(unsigned int &aKey) {
}
//----------------------------------------------------------------------------

void MenuItem::Render(int aX, int aY) {
}
//----------------------------------------------------------------------------

bool MenuItem::IsChanged(void) {
	bool res = m_IsChanged;
	m_IsChanged = false;
	return res;
}
//----------------------------------------------------------------------------

//============================================================================
//	IntMenuItem
//============================================================================

SwitchMenuItem::SwitchMenuItem(TextAScan *aTextAScan, char *aCaption, char* aI1, char* aI2)
		: MenuItem(aTextAScan) {
	m_Caption = strdup(aCaption);
	m_I1 = strdup(aI1);
	m_I2 = strdup(aI2);

	m_ItemNumber = 0;
}
//----------------------------------------------------------------------------

SwitchMenuItem::~SwitchMenuItem() {
	free(m_I2);
	free(m_I1);
	free(m_Caption);
}
//----------------------------------------------------------------------------

int SwitchMenuItem::GetWidth(void) {
	char *buf = m_ItemNumber == 0 ? m_I1 : m_I2;
	return (font->GetStringWidth(m_Caption) + font->GetStringWidth(buf));
}
//----------------------------------------------------------------------------

int SwitchMenuItem::SelectedItem(void) {
	return m_ItemNumber;
}
//----------------------------------------------------------------------------

void SwitchMenuItem::ProcessButton(unsigned int &aKey) {
	if (aKey == JOY_UP || aKey == JOY_DN) {
		m_ItemNumber = 1 - m_ItemNumber;
		m_IsChanged = true;
	}
}
//----------------------------------------------------------------------------

void SwitchMenuItem::Render(int aX, int aY) {
	GLfloat sel_c[] = { /* texture coordinate */
	1.0f, 0.0f, 0.0, 1.0,/**/
	1.0f, 0.0f, 0.0, 1.0, /**/
	1.0f, 0.0f, 0.0, 1.0, /**/
	1.0f, 0.0f, 0.0, 1.0 };

	GLfloat c[] = { /* texture coordinate */
	0.0f, 0.0f, 0.0, 0.0,/**/
	0.0f, 0.0f, 0.0, 0.0, /**/
	0.0f, 0.0f, 0.0, 0.0, /**/
	0.0f, 0.0f, 0.0, 0.0 };

	int w = font->GetStringWidth(m_Caption);
	int h = font->GetStringHeight();
	m_TextAScan->FillRect(aX, aY, aX + w + 2, aY + h, m_Selected ? sel_c : c);
	font->RenderString(aX + 2, aY + 1, m_Caption);

	char *buf = m_ItemNumber == 0 ? m_I1 : m_I2;
	int x = aX + w + 2;
	w = font->GetStringWidth(buf);
	m_TextAScan->FillRect(x, aY, x + w + 2, aY + h, m_Selected ? sel_c : c);
	font->RenderString(x + 1, aY + 1, buf);
}
//----------------------------------------------------------------------------

//============================================================================
//	IntMenuItem
//============================================================================

IntMenuItem::IntMenuItem(TextAScan *aTextAScan, char *aCaption, int aVal, int aMin, int aMax)
		: MenuItem(aTextAScan) {
	m_Caption = new char[strlen(aCaption) + 1];
	memset(m_Caption, 0, strlen(aCaption) + 1);
	memcpy(m_Caption, aCaption, strlen(aCaption));
	m_Val = aVal;
	m_Min = aMin;
	m_Max = aMax;
	m_PrevKey = 0;
	m_HoldTickCount = 0;
}
//----------------------------------------------------------------------------

IntMenuItem::~IntMenuItem() {
	delete[] m_Caption;
}
//----------------------------------------------------------------------------

int IntMenuItem::GetValue(void) {
	return m_Val;
}
//----------------------------------------------------------------------------

void IntMenuItem::SetValue(int aVal, bool aIsChanged) {
	if (aVal < m_Min)
		m_Val = m_Min;
	else if (aVal > m_Max)
		aVal = m_Max;
	else
		m_Val = aVal;

	if (aIsChanged)
		m_IsChanged = true;
}
//----------------------------------------------------------------------------

int IntMenuItem::GetWidth(void) {
	char buf[32];
	memset(buf, 0, sizeof(buf));
	sprintf(buf, "%i", m_Val);

	return (font->GetStringWidth(m_Caption) + font->GetStringWidth(buf));
}
//----------------------------------------------------------------------------

void IntMenuItem::ProcessButton(unsigned int &aKey) {
	if (aKey != 0xFFFF0000) {
		if (aKey == 0xFFFFFFFF) {
			m_HoldTickCount++;
			if (m_HoldTickCount > 25) {
				if ((m_HoldTickCount < 100 && (m_HoldTickCount % 5) == 0)
						|| (m_HoldTickCount > 100 && (m_HoldTickCount % 2) == 0))
					aKey = m_PrevKey;
				else
					aKey = 0;
			}
		}
		else {
			m_PrevKey = aKey;
			m_HoldTickCount = 0;
		}
	}
	else {
		m_HoldTickCount = 0;
		m_PrevKey = 0;
	}

	if (aKey == JOY_UP)
		if (m_Val < m_Max) {
			m_Val++;
			m_IsChanged = true;
		}

	if (aKey == JOY_DN)
		if (m_Val > m_Min) {
			m_Val--;
			m_IsChanged = true;
		}
}
//----------------------------------------------------------------------------

void IntMenuItem::Render(int aX, int aY) {
	GLfloat sel_c[] = { /* texture coordinate */
	1.0f, 0.0f, 0.0, 1.0,/**/
	1.0f, 0.0f, 0.0, 1.0, /**/
	1.0f, 0.0f, 0.0, 1.0, /**/
	1.0f, 0.0f, 0.0, 1.0 };

	GLfloat c[] = { /* texture coordinate */
	0.0f, 0.0f, 0.0, 0.0,/**/
	0.0f, 0.0f, 0.0, 0.0, /**/
	0.0f, 0.0f, 0.0, 0.0, /**/
	0.0f, 0.0f, 0.0, 0.0 };

	int w = font->GetStringWidth(m_Caption);
	int h = font->GetStringHeight();
	m_TextAScan->FillRect(aX, aY, aX + w + 2, aY + h, m_Selected ? sel_c : c);
	font->RenderString(aX + 2, aY + 1, m_Caption);

	char buf[32];
	memset(buf, 0, sizeof(buf));
	sprintf(buf, "%i", m_Val);
	int x = aX + w + 2;
	w = font->GetStringWidth(buf);
	m_TextAScan->FillRect(x, aY, x + w + 2, aY + h, m_Selected ? sel_c : c);
	font->RenderString(x + 1, aY + 1, buf);
}
//----------------------------------------------------------------------------

//============================================================================
//	MenuAScan
//============================================================================

MenuAScan::MenuAScan(TextAScan * aTextAScan) {
	m_TextAScan = aTextAScan;
	m_Side = new SwitchMenuItem(aTextAScan, (char*) "DS Side:", (char*) "Left", (char*) "Right");
	m_ChNumber = new IntMenuItem(aTextAScan, (char*) "Ch Num:", 1, 1, 14);

	m_AScanType = new SwitchMenuItem(aTextAScan, (char*) "View Type:", (char*) "Log", (char*) "VGA");
	m_Delay = new IntMenuItem(aTextAScan, (char*) "Delay:", 0, 0, 100);
	m_ADCAccum = new IntMenuItem(aTextAScan, (char*) "Accum:", 31, 0, 128);

	m_Amp1 = new IntMenuItem(aTextAScan, (char*) "AMP1:", 0, 0, 83);
	m_Amp2 = new IntMenuItem(aTextAScan, (char*) "AMP2:", 0, 0, 83);
	m_VRC = new IntMenuItem(aTextAScan, (char*) "VRC:", 0, 0, 50);

	m_Level = new IntMenuItem(aTextAScan, (char*) "Level:", 0, -127, 128);

	m_Selected = m_AScanType;
	m_Selected->SetSelected(true);

	m_AmpDelta = 0;

	m_ShowAllChannel = false;

	UpdateControl(true);
	FillVRC();
}
//----------------------------------------------------------------------------

MenuAScan::~MenuAScan() {
	delete m_Level;
	delete m_VRC;
	delete m_Amp2;
	delete m_Amp1;
	delete m_ADCAccum;
	delete m_Delay;
	delete m_AScanType;
	delete m_ChNumber;
	delete m_Side;
}
//----------------------------------------------------------------------------

int MenuAScan::GetSide(void) {
	return m_Side->SelectedItem();
}
//----------------------------------------------------------------------------

int MenuAScan::GetChNumber(void) {
	return m_ChNumber->GetValue();
}
//----------------------------------------------------------------------------

bool MenuAScan::GetShowAllChannel(void) {
	return m_ShowAllChannel;
}
//----------------------------------------------------------------------------

bool MenuAScan::IsLogView(void) {
	return m_AScanType->SelectedItem() == 0;
}
//----------------------------------------------------------------------------

void MenuAScan::FillVRC(void) {
	float amp1 = m_Amp1->GetValue() / 83.0; // VRC hight from "0" to this value
	float amp2 = m_Amp2->GetValue() / 83.0; // time of this point = VRC + 100 uSec
	float delay = (float) m_Delay->GetValue() * 40.0; // nSec
	float vrc = (float) m_VRC->GetValue() * 1000.0; // nSec (time for grow VRC to AMP1 value)
	float tick_step = (float) (m_ADCAccum->GetValue() + 1) * 40.0;
	float x = delay;

	int vrc_tick = (int) ((vrc - delay) / tick_step);
	for (int i = 0; i < 128; i++) {
		if (i < vrc_tick) {
			float val = 1.0 - (amp1 / vrc) * (x + (float) i * tick_step);
			m_VRCTab[i] = 1.0 / pow(10.0, val * 83.0 / 20.0);
		}
		else {
			float val = 1.0
					- (amp1 + ((amp2 - amp1) / (vrc + 100.0 * 1000.0)) * (x + (float) (i - vrc_tick) * tick_step));
			if (val < 0.0)
				val = 0.0;
			m_VRCTab[i] = 1.0 / pow(10.0, val * 83.0 / 20.0);
		}
	}

	for (int i = 0; i < 256; i++) {
		float dB = (float) i / 255.0 * 83.0;
		m_LogTab[i] = pow(10.0, dB / 20.0);
	}
}
//----------------------------------------------------------------------------

void MenuAScan::CalcVGA(float *aVal, unsigned char *aBuf, int aSize) {
	for (int i = 0; i < 128; i++) {
		aVal[i] = m_LogTab[aBuf[i]] * m_VRCTab[i];
	}
}
//----------------------------------------------------------------------------

void MenuAScan::UpdateControl(bool aLogType) {
	m_Items.clear();
	m_Items.push_back(m_Side);
	m_Items.push_back(m_ChNumber);
	m_Items.push_back(m_AScanType);
	if (aLogType) {
		m_Items.push_back(m_Level);
		m_Items.push_back(m_Delay);
		m_Items.push_back(m_ADCAccum);
	}
	else {
		m_Items.push_back(m_Amp1);
		m_Items.push_back(m_Amp2);
		m_Items.push_back(m_VRC);
	}
}
//----------------------------------------------------------------------------

void MenuAScan::ProcessButton(unsigned int &aKey) {
	if (aKey == JOY_LEFT) {
		std::vector<MenuItem*>::reverse_iterator i = m_Items.rbegin();
		while (i != m_Items.rend()) {
			if (*i == m_Selected) {
				i++;
				break;
			}
			i++;
		}
		if (i == m_Items.rend())
			i = m_Items.rbegin();
		m_Selected->SetSelected(false);
		m_Selected = *i;
		m_Selected->SetSelected(true);
	}
	else if (aKey == JOY_RIGHT) {
		std::vector<MenuItem*>::iterator i = m_Items.begin();
		while (i != m_Items.end()) {
			if (*i == m_Selected) {
				i++;
				break;
			}
			i++;
		}
		if (i == m_Items.end())
			i = m_Items.begin();
		m_Selected->SetSelected(false);
		m_Selected = *i;
		m_Selected->SetSelected(true);
	}
	else if (aKey == BTN_CANCEL) {
		m_ShowAllChannel = !m_ShowAllChannel;
	}
	else {
		m_Selected->ProcessButton(aKey);
		HandleEvent();
	}
}
//----------------------------------------------------------------------------

void MenuAScan::HandleEvent(void) {
	int ch_num = m_ChNumber->GetValue() - 1;

	DPart *dpart = (m_Side->SelectedItem() == 0) ? dscope->LFish : dscope->RFish;

	if (m_AScanType->IsChanged())
		UpdateControl(m_AScanType->SelectedItem() == 0);

	if (m_Delay->IsChanged()) {
		int acum = m_ADCAccum->GetValue() + 1;
		dpart->Channel[ch_num]->SetDelay(m_Delay->GetValue() * acum);
		//dscope->LFish->Channel[0]->SetADCAccum(m_Delay->GetValue());
	}

	if (m_ADCAccum->IsChanged()) {
		dpart->Channel[ch_num]->SetADCAccum(m_ADCAccum->GetValue());
	}


	if (m_Level->IsChanged()) {
		int val = m_Level->GetValue() + 127;
		dpart->Channel[ch_num]->SetADCOffset(val);
	}

	if (m_VRC->IsChanged()) {
		FillVRC();
	}

	if (m_Amp1->IsChanged()) {
		m_Amp2->SetValue(m_Amp1->GetValue() + m_AmpDelta);
		FillVRC();
	}

	if (m_Amp2->IsChanged()) {
		m_AmpDelta = m_Amp2->GetValue() - m_Amp1->GetValue();
		if (m_AmpDelta < 0) {
			m_AmpDelta = 0;
			m_Amp2->SetValue(m_Amp1->GetValue());
		}
		FillVRC();
	}
}
//----------------------------------------------------------------------------

void MenuAScan::Render(int aX, int aY) {
	int x = aX;
	for (std::vector<MenuItem*>::iterator i = m_Items.begin(); i != m_Items.end(); i++) {
		(*i)->Render(x, aY);
		x += (*i)->GetWidth() + 16;
	}
}
//----------------------------------------------------------------------------

//============================================================================
//	TextAScan
//============================================================================

TextAScan::TextAScan() {
	const char vs[] = "attribute vec4 VertexPos; \n"
			"attribute vec4 VertexColor; \n"
			"varying vec4 vertex_color;\n"
			"void main() \n"
			"{ \n"
			"    gl_Position = VertexPos; \n"
			"    vertex_color = VertexColor; \n"
			"} \n";

	const char fs[] = "precision mediump float;\n"
			"varying vec4 vertex_color;\n"
			"void main() \n"
			"{ \n"
			"    gl_FragColor = vertex_color; \n"
			"} \n";

	m_Prog = createProgram(vs, fs);
	if (!m_Prog) {
		printf("Can't load programm for TextScroller...\n");
		exit(-1);
	}

	m_paramVertexPos = glGetAttribLocation(m_Prog, "VertexPos");
	m_paramVertexColor = glGetAttribLocation(m_Prog, "VertexColor");

	m_Menu = new MenuAScan(this);

	m_PtCount = 256;
	m_V = new GLfloat[m_PtCount * 3];
	m_C = new GLfloat[m_PtCount * 4];
	m_Ndx = new GLushort[m_PtCount];
}
//----------------------------------------------------------------------------

TextAScan::~TextAScan() {
	delete[] m_Ndx;
	delete[] m_C;
	delete[] m_V;
}
//----------------------------------------------------------------------------

void TextAScan::DrawLine(int aX1, int aY1, int aX2, int aY2) {
	float x1 = (float) aX1 / 400.0 - 1.0;
	float x2 = (float) aX2 / 400.0 - 1.0;
	float y1 = (float) aY1 / 240.0 - 1.0;
	float y2 = (float) aY2 / 240.0 - 1.0;

	GLfloat v[] = { /* vertexes */
	x1, y1, 0.0f, /**/
	x2, y2, 0.0f };

	GLfloat txc[] = { /* texture coordinate */
	0.0f, 0.0f, 0.0, 1.0,/**/
	0.0f, 0.0f, 0.0, 1.0 };

	GLushort ndx[] = { 0, 1 };

	glUseProgram(m_Prog);

	glDisable(GL_DEPTH_TEST);
	glViewport(0, 0, 800, 480);

	glVertexAttribPointer(m_paramVertexPos, 3, GL_FLOAT, GL_FALSE, 0, v);
	glEnableVertexAttribArray(m_paramVertexPos);

	glVertexAttribPointer(m_paramVertexColor, 4, GL_FLOAT, GL_FALSE, 0, txc);
	glEnableVertexAttribArray(m_paramVertexColor);

	glDrawElements(GL_LINES, 2, GL_UNSIGNED_SHORT, ndx);
}
//----------------------------------------------------------------------------

void TextAScan::FillRect(int aX1, int aY1, int aX2, int aY2, GLfloat *aColor) {
	float x1 = (float) aX1 / 400.0 - 1.0;
	float x2 = (float) aX2 / 400.0 - 1.0;
	float y1 = (float) aY1 / 240.0 - 1.0;
	float y2 = (float) aY2 / 240.0 - 1.0;

	GLfloat v[] = { /* vertexes */
	x1, y1, 0.0f, /**/
	x1, y2, 0.0f, /**/
	x2, y1, 0.0f, /**/
	x2, y2, 0.0f };

//	GLfloat v[] = { /* vertexes */
//	-1.0f, -1.0f, 0.0f, /**/
//	-1.0f, 1.0f, 0.0f, /**/
//	1.0f, -1.0f, 0.0f, /**/
//	1.0f, 1.0f, 0.0f };

	GLfloat txc[] = { /* texture coordinate */
	0.0f, 0.0f, 1.0, 1.0,/**/
	0.0f, 1.0f, 1.0, 1.0, /**/
	1.0f, 0.0f, 1.0, 1.0, /**/
	1.0f, 1.0f, 1.0, 1.0 };

	GLushort ndx[] = { 1, 0, 2, 1, 2, 3 };

	glUseProgram(m_Prog);

	glDisable(GL_DEPTH_TEST);
	glViewport(0, 0, 800, 480);

	glVertexAttribPointer(m_paramVertexPos, 3, GL_FLOAT, GL_FALSE, 0, v);
	glEnableVertexAttribArray(m_paramVertexPos);

	glVertexAttribPointer(m_paramVertexColor, 4, GL_FLOAT, GL_FALSE, 0, aColor ? aColor : txc);
	glEnableVertexAttribArray(m_paramVertexColor);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, ndx);
}
//----------------------------------------------------------------------------

void TextAScan::CalcAScan(int aX1, int aY1, int aX2, int aY2, unsigned char *aBuf, int aSize) {
	float x1 = (float) aX1 / 400.0 - 1.0;
	float x2 = (float) aX2 / 400.0 - 1.0;
	float y1 = (float) aY1 / 240.0 - 1.0;
	float y2 = (float) aY2 / 240.0 - 1.0;

	float val[256];
	if (m_Menu->IsLogView()) {
		for (int i = 0; i < aSize; i++) {
			val[i] = aBuf[i];
		}
	}
	else {
		m_Menu->CalcVGA(val, aBuf, aSize);
	}

	for (int i = 0; i <= aSize; i++) {
		if (i == 0) {
			m_V[i * 3] = x1;
			m_V[i * 3 + 1] = y1; // + (float) aBuf[i] / 255.0 * (y2 - y1);
		}
		else if (i == aSize) {
			m_V[i * 3] = x2;
			m_V[i * 3 + 1] = y1;
		}
		else {
			m_V[i * 3] = x1 + (float) (i - 1) / (float) (aSize - 2) * (float) (x2 - x1);
			float v = y1 + (float) val[i - 1] / 255.0 * (y2 - y1);
			if (v < y1)
				v = y1;
			if (v > y2)
				v = y2;
			m_V[i * 3 + 1] = v;
		}
		m_V[i * 3 + 2] = 0.0f;

		m_C[i * 4] = 1.0f;
		m_C[i * 4 + 1] = 1.0f;
		m_C[i * 4 + 2] = 1.0f;
		m_C[i * 4 + 3] = 1.0f; //(i >= aSize - 1) || (i < 1) ? 0.0f : 1.0f;

		m_Ndx[i] = i;
	}
}
//----------------------------------------------------------------------------

void TextAScan::DrawBuf(DScopeStream *aDSS, int aX1, int aY1, int aX2, int aY2) {
	int size = 128;
	int side = m_Menu->GetSide();
	int ch = m_Menu->GetChNumber() - 1;

	DataFrame *fr = aDSS->GetRealtime();

	glUseProgram(m_Prog);

	glDisable(GL_DEPTH_TEST);
	glViewport(0, 0, 800, 480);

	if (m_Menu->GetShowAllChannel()) {
		for (int j = 0; j < 4; j++) {
			for (int i = 0; i < 7; i++) {
				ch = (j % 2) * 7 + i;
				side = j / 2;
				float x1 = 800.0 / 7.0 * (float) i;
				float x2 = 800.0 / 7.0 * (float) (i + 1);
				float y1 = 40 + 440.0 / 4.0 * (float) j;
				float y2 = 40 + 440.0 / 4.0 * (float) (j + 1);
				unsigned char *b = (side == 0) ? &fr->m_LData[128 * ch] : &fr->m_RData[128 * ch];
				CalcAScan(x1, y1, x2, y2, b, size);

				glVertexAttribPointer(m_paramVertexPos, 3, GL_FLOAT, GL_FALSE, 0, m_V);
				glEnableVertexAttribArray(m_paramVertexPos);

				glVertexAttribPointer(m_paramVertexColor, 4, GL_FLOAT, GL_FALSE, 0, m_C);
				glEnableVertexAttribArray(m_paramVertexColor);

				glDrawElements(GL_LINE_LOOP, size + 1, GL_UNSIGNED_SHORT, m_Ndx);
			}
		}
	}
	else {
		unsigned char *b = (side == 0) ? &fr->m_LData[128 * ch] : &fr->m_RData[128 * ch];
		CalcAScan(aX1, aY1, aX2, aY2, b, size);

		glVertexAttribPointer(m_paramVertexPos, 3, GL_FLOAT, GL_FALSE, 0, m_V);
		glEnableVertexAttribArray(m_paramVertexPos);

		glVertexAttribPointer(m_paramVertexColor, 4, GL_FLOAT, GL_FALSE, 0, m_C);
		glEnableVertexAttribArray(m_paramVertexColor);

		glDrawElements(GL_LINE_LOOP, size + 1, GL_UNSIGNED_SHORT, m_Ndx);
	}

	//font->RenderString(2, 2, (char*) "A-Scan Render String...");

	//m_Menu->Render(10, 20);
}
//----------------------------------------------------------------------------

void TextAScan::ProcessButton(unsigned int &aKey) {
	m_Menu->ProcessButton(aKey);
}
//----------------------------------------------------------------------------
