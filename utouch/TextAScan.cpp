/*
 * TextAScan.cpp
 *
 *  Created on: Mar 6, 2017
 *      Author: zaqc
 */

#include <stdlib.h>
#include <stdio.h>

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
		if (m_Val < m_Max)
			m_Val++;

	if (aKey == JOY_DN)
		if (m_Val > m_Min)
			m_Val--;
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
	m_AScanType = new SwitchMenuItem(aTextAScan, (char*) "View Type:", (char*) "Log", (char*) "VGA");

	m_Amp1 = new IntMenuItem(aTextAScan, (char*) "AMP1:", 0, 0, 96);
	m_Amp2 = new IntMenuItem(aTextAScan, (char*) "AMP2:", 0, 0, 96);
	m_VRC = new IntMenuItem(aTextAScan, (char*) "VRC:", 0, 0, 180);

	m_Level = new IntMenuItem(aTextAScan, (char*) "Level:", 0, -127, 128);

	m_Selected = m_AScanType;
	m_Selected->SetSelected(true);

	UpdateControl(true);
}
//----------------------------------------------------------------------------

MenuAScan::~MenuAScan() {
	delete m_VRC;
	delete m_Amp2;
	delete m_Amp1;
	delete m_AScanType;
}
//----------------------------------------------------------------------------

void MenuAScan::UpdateControl(bool aLogType) {
	if (aLogType) {
		m_Items.clear();
		m_Items.push_back(m_AScanType);
		m_Items.push_back(m_Level);
	}
	else {
		m_Items.clear();
		m_Items.push_back(m_AScanType);
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
	else {
		m_Selected->ProcessButton(aKey);
		if (m_AScanType->IsChanged())
			UpdateControl(m_AScanType->SelectedItem() == 0);
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
}
//----------------------------------------------------------------------------

TextAScan::~TextAScan() {
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

void TextAScan::DrawBuf(int aX1, int aY1, int aX2, int aY2, unsigned char *aBuf, int aSize) {
	GLfloat *v = new GLfloat[(aSize + 1) * 3];
	GLfloat *c = new GLfloat[(aSize + 1) * 4];
	GLushort *ndx = new GLushort[aSize + 1];

	float x1 = (float) aX1 / 400.0 - 1.0;
	float x2 = (float) aX2 / 400.0 - 1.0;
	float y1 = (float) aY1 / 240.0 - 1.0;
	float y2 = (float) aY2 / 240.0 - 1.0;

	for (int i = 0; i <= aSize; i++) {
		if (i == 0) {
			v[i * 3] = x1;
			v[i * 3 + 1] = y1;
		}
		else if (i == aSize - 1) {
			v[i * 3] = x2;
			v[i * 3 + 1] = y1;
		}
		else {
			v[i * 3] = x1 + (float) i / (float) aSize * (x2 - x1);
			v[i * 3 + 1] = y1 + (float) aBuf[i] / 255.0 * (y2 - y1);
		}
		v[i * 3 + 2] = 0.0f;

		c[i * 4] = 1.0f;
		c[i * 4 + 1] = 1.0f;
		c[i * 4 + 2] = 1.0f;
		c[i * 4 + 3] = 1.0f; //(i >= aSize - 1) || (i < 1) ? 0.0f : 1.0f;

		ndx[i] = i;
	}

	glUseProgram(m_Prog);

	glDisable(GL_DEPTH_TEST);
	glViewport(0, 0, 800, 480);

	glVertexAttribPointer(m_paramVertexPos, 3, GL_FLOAT, GL_FALSE, 0, v);
	glEnableVertexAttribArray(m_paramVertexPos);

	glVertexAttribPointer(m_paramVertexColor, 4, GL_FLOAT, GL_FALSE, 0, c);
	glEnableVertexAttribArray(m_paramVertexColor);

	glDrawElements(GL_LINE_LOOP, aSize, GL_UNSIGNED_SHORT, ndx);

	font->RenderString(10, 10, (char*) "A-Scan Render String...");

	delete[] ndx;
	delete[] c;
	delete[] v;

	m_Menu->Render(10, 20);
}

void TextAScan::ProcessButton(unsigned int &aKey) {
	m_Menu->ProcessButton(aKey);
}

