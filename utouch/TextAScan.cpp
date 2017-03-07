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

extern TextFont *font;

void TextAScan::FillRect(int aX1, int aY1, int aX2, int aY2) {
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

	glVertexAttribPointer(m_paramVertexColor, 4, GL_FLOAT, GL_FALSE, 0, txc);
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

	if (font)
		font->RenderString(0, 10, 10, (unsigned char*) "A-Scan Render String...");

	delete[] ndx;
	delete[] c;
	delete[] v;
}

