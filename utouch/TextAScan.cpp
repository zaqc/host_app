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

IntMenuItem::IntMenuItem(TextAScan *aTextAScan, char *aCaption, int aVal, int aMin, int aMax) {
	m_TextAScan = aTextAScan;

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
	GLfloat c[] = { /* texture coordinate */
	0.0f, 0.0f, 0.0, 1.0,/**/
	0.0f, 0.0f, 0.0, 1.0, /**/
	0.0f, 0.0f, 0.0, 1.0, /**/
	0.0f, 0.0f, 0.0, 1.0 };

	int w = font->GetStringWidth(m_Caption);
	int h = font->GetStringHeight();
	m_TextAScan->FillRect(aX, aY, aX + w + 2, aY + h, c);
	font->RenderString(aX + 2, aY + 1, m_Caption);

	char buf[32];
	memset(buf, 0, sizeof(buf));
	sprintf(buf, "%i", m_Val);
	int x = aX + w + 2;
	w = font->GetStringWidth(buf);
	m_TextAScan->FillRect(x, aY, x + w + 2, aY + h, c);
	font->RenderString(x + 1, aY + 1, buf);
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

	m_Amp1 = new IntMenuItem(this, (char*) "AMP1:", 0, 0, 96);
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

	m_Amp1->Render(10, 20);
}

void TextAScan::ProcessButton(unsigned int &aKey) {
	m_Amp1->ProcessButton(aKey);
}

