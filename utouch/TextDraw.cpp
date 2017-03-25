/*
 * TextDraw.cpp
 *
 *  Created on: Mar 13, 2017
 *      Author: zaqc
 */

#include <stdlib.h>
#include <stdio.h>
#include <GLES2/gl2.h>

#include "TextDraw.h"
#include "GlUtil.h"
#include "TextFont.h"
#include "SmallFont.h"
//----------------------------------------------------------------------------

TextDraw *draw = NULL;
//----------------------------------------------------------------------------

TextDraw::TextDraw() {
	const char vs[] = "attribute vec4 VertexPos; \n"
			"uniform vec4 DrawColor; \n"
			"uniform vec2 ViewSize; \n"
			"varying vec4 vertex_color;\n"
			"float xx; \n"
			"float yy; \n"
			"void main() \n"
			"{ \n"
			"    xx = VertexPos.x / ViewSize.x * 2.0 - 1.0; \n"
			"    yy = 1.0 - VertexPos.y / ViewSize.y * 2.0; \n"
			"    gl_Position = vec4(xx, yy, 0.0, VertexPos.w); \n"
			"    vertex_color = DrawColor; \n" //vec4(1.0, 1.0, 1.0, 1.0); \n" //DrawColor; \n"
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

	m_R = m_G = m_B = m_A = 1.0f;

	m_paramVertexPos = glGetAttribLocation(m_Prog, "VertexPos");
	m_paramDrawColor = glGetUniformLocation(m_Prog, "DrawColor");
	m_paramViewSize = glGetUniformLocation(m_Prog, "ViewSize");

	m_ViewWidth = 800;
	m_ViewHeight = 480;

	m_LineCacheSize = 1024;
	m_LineVertex = new GLfloat[m_LineCacheSize * 6];
	m_LineColor = new GLfloat[m_LineCacheSize * 8];
	m_LineNdx = new GLushort[m_LineCacheSize * 2];
}
//----------------------------------------------------------------------------

TextDraw::~TextDraw() {
	delete[] m_LineNdx;
	delete[] m_LineColor;
	delete[] m_LineVertex;
}
//----------------------------------------------------------------------------

void TextDraw::SetViewSize(int aW, int aH) {
	m_ViewWidth = aW;
	m_ViewHeight = aH;
}
//----------------------------------------------------------------------------

void TextDraw::SetColor(GLfloat *aColor) {
	m_R = aColor[0];
	m_G = aColor[1];
	m_B = aColor[2];
	m_A = aColor[3];
}
//----------------------------------------------------------------------------

void TextDraw::SetColor(GLfloat aR, GLfloat aG, GLfloat aB, GLfloat aA) {
	m_R = aR;
	m_G = aG;
	m_B = aB;
	m_A = aA;
}
//----------------------------------------------------------------------------

void TextDraw::FillRect(int aX1, int aY1, int aX2, int aY2) {
	float x1 = aX1; //(float) aX1 / 400.0 - 1.0;
	float x2 = aX2; //(float) aX2 / 400.0 - 1.0;
	float y1 = aY1; //(float) aY1 / 240.0 - 1.0;
	float y2 = aY2; //(float) aY2 / 240.0 - 1.0;

	GLfloat v[] = { /* vertexes */
	x1, y1, 0.0f, /**/
	x1, y2, 0.0f, /**/
	x2, y1, 0.0f, /**/
	x2, y2, 0.0f };

	GLushort ndx[] = { 1, 0, 2, 1, 2, 3 };

	glUseProgram(m_Prog);

	glDisable(GL_DEPTH_TEST);
	glViewport(0, 0, m_ViewWidth, m_ViewHeight);

	glUniform4f(m_paramDrawColor, m_R, m_G, m_B, m_A);

	glVertexAttribPointer(m_paramVertexPos, 3, GL_FLOAT, GL_FALSE, 0, v);
	glEnableVertexAttribArray(m_paramVertexPos);

	glUniform2f(m_paramViewSize, m_ViewWidth, m_ViewHeight);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, ndx);
}
//----------------------------------------------------------------------------

void TextDraw::DrawLine(int aX1, int aY1, int aX2, int aY2) {
	float x1 = aX1; //(float) aX1 / 400.0 - 1.0;
	float x2 = aX2; //(float) aX2 / 400.0 - 1.0;
	float y1 = aY1; //(float) aY1 / 240.0 - 1.0;
	float y2 = aY2; //(float) aY2 / 240.0 - 1.0;

	GLfloat v[] = { /* vertexes */
	x1, y1, 0.0f, /**/
	x2, y2, 0.0f };

	GLushort ndx[] = { 0, 1 };

	glUseProgram(m_Prog);

	glDisable(GL_DEPTH_TEST);
	glViewport(0, 0, m_ViewWidth, m_ViewHeight);

	glUniform4f(m_paramDrawColor, m_R, m_G, m_B, m_A);

	glVertexAttribPointer(m_paramVertexPos, 3, GL_FLOAT, GL_FALSE, 0, v);
	glEnableVertexAttribArray(m_paramVertexPos);

	glDrawElements(GL_LINES, 2, GL_UNSIGNED_SHORT, ndx);
}
//----------------------------------------------------------------------------

void TextDraw::DrawRect(int aX1, int aY1, int aX2, int aY2) {
	float x1 = aX1; //(float) aX1 / 400.0 - 1.0;
	float x2 = aX2; //(float) aX2 / 400.0 - 1.0;
	float y1 = aY1; //(float) aY1 / 240.0 - 1.0;
	float y2 = aY2; //(float) aY2 / 240.0 - 1.0;

	GLfloat v[] = { /* vertexes */
	x1, y1, 0.0f, /**/
	x1, y2, 0.0f, /**/
	x2, y1, 0.0f, /**/
	x2, y2, 0.0f };

	GLushort ndx[] = { 0, 2, 2, 3, 3, 1, 1, 0 };

	glUseProgram(m_Prog);

	glDisable(GL_DEPTH_TEST);
	glViewport(0, 0, m_ViewWidth, m_ViewHeight);

	glUniform4f(m_paramDrawColor, m_R, m_G, m_B, m_A);

	glVertexAttribPointer(m_paramVertexPos, 3, GL_FLOAT, GL_FALSE, 0, v);
	glEnableVertexAttribArray(m_paramVertexPos);

	glDrawElements(GL_LINES, 8, GL_UNSIGNED_SHORT, ndx);
}
//----------------------------------------------------------------------------

void TextDraw::DrawArrow(int aX1, int aY1, int aX2, int aY2) {
//	float x1 = aX1; //(float) aX1 / 400.0 - 1.0;
//	float x2 = aX2; //(float) aX2 / 400.0 - 1.0;
//	float y1 = aY1; //(float) aY1 / 240.0 - 1.0;
//	float y2 = aY2; //(float) aY2 / 240.0 - 1.0;

	GLfloat v[] = { /**/
	-2.0 / 3.0, 1.0 / 3.0, 0.0, /**/
	0.0, 1.0, 0.0, /**/
	2.0 / 3.0, 1.0 / 3.0, 0.0, /**/
	1.0 / 3.0, 1.0 / 3.0, 0.0, /**/
	1.0 / 3.0, -1.0, 0.0, /**/
	-1.0 / 3.0, -1.0, 0.0, /**/
	-1.0 / 3.0, 1.0 / 3.0, 0.0 /**/
	};

	GLushort ndx[] = { 0, 1, 2, 3, 4, 5, 5, 6, 3 };

	glUseProgram(m_Prog);

	glDisable(GL_DEPTH_TEST);
	glViewport(0, 0, m_ViewWidth, m_ViewHeight);

	glUniform4f(m_paramDrawColor, m_R, m_G, m_B, m_A);

	glVertexAttribPointer(m_paramVertexPos, 3, GL_FLOAT, GL_FALSE, 0, v);
	glEnableVertexAttribArray(m_paramVertexPos);

	glDrawElements(GL_LINES, 8, GL_UNSIGNED_SHORT, ndx);
}
//----------------------------------------------------------------------------

void TextDraw::DrawGrid(int aX1, int aY1, int aX2, int aY2, bool aLogView, int aDelay, int aAccum) {
	int lm = 36;
	int bm = 16;
	int tm = 4;
	int rm = 4;
	draw->SetColor(0.0f, 0.0f, 0.25f, 1.0f);
	draw->FillRect(aX1, aY1, aX2, aY2);
	draw->SetColor(1.0f, 1.0f, 1.0f, 1.0f);
	draw->DrawRect(aX1, aY1, aX2, aY2);
	draw->DrawRect(aX1 + lm, aY1 + tm, aX2 - rm, aY2 - bm);

	float delay = (float) (aDelay * 40.0 * (aAccum + 1)) / 1000.0;	// uSec
	float accum = 40.0 * (float) (aAccum + 1) / 1000.0; // uSec
	int tick_count = 128;

	float full_time = (float) tick_count * accum;

	draw->SetColor(1.0f, 1.0f, 1.0f, .25f);
	float rw = aX2 - aX1 - lm - rm;
	float x = aX1 + lm;
	for (int i = 0; i <= 10; i++) {
		char str[16];
		if (i == 10) {
			sprintf(str, "uSec");
			int str_w = small_font->GetStringWidth(str);
			small_font->RenderString(aX2 - str_w - rm - 2, aY2 - 12, str);
		}
		else {
			float t = delay + full_time / 10.0 * (float) i;
			sprintf(str, "%.2f", t);
			int str_w = small_font->GetStringWidth(str);
			small_font->RenderString(x - ((i == 0) ? 0 : str_w / 2), aY2 - 12, str);
		}
		x = x + rw / 10.0;
		draw->DrawLine(x, aY1 + tm, x, aY2 - bm);
	}

	if (aLogView) {
		float rh = aY2 - aY1 - tm - bm;
		float y = aY1 + tm;
		for (int i = 0; i <= 10; i++) {
			if (i == 0) {
				char str[16];
				sprintf(str, "dB");
				int str_w = small_font->GetStringWidth(str);
				small_font->RenderString(aX1 + lm - str_w - 2, aY1 + tm, str);
			}
			else {
				y = y + rh / 10.0;
				draw->DrawLine(aX1 + lm, y, aX2 - rm, y);
				float t = 84.0 / 10.0 * (float) (10 - i);
				char str[16];
				sprintf(str, "%.2f", t);
				int str_w = small_font->GetStringWidth(str);
				float yy = y - ((i == 10) ? small_font->GetStringHeight() : small_font->GetStringHeight() / 2);
				small_font->RenderString(aX1 + lm - str_w - 2, yy, str);
			}
		}
	}
	else {
		float rh = aY2 - aY1 - tm - bm;
		float y = aY1 + tm;
		float prev_y = y;
		int dB = 6;
		for (int i = 0; i <= 10; i++) {
			if (prev_y + 8 < y || i == 10 || i == 0) {
				char str[16];
				if (i == 10)
					sprintf(str, "-72.0");
				else if (i == 0)
					sprintf(str, "dB");
				else
					sprintf(str, "-%.1f", (float) dB);
				int str_w = small_font->GetStringWidth(str);
				float yy = y
						- ((i == 0) ?
								0 : ((i == 10) ? small_font->GetStringHeight() : small_font->GetStringHeight() / 2));
				small_font->RenderString(aX1 + lm - str_w - 2, yy, str);
			}
			prev_y = y;
			dB += 6;

			rh = rh / 2;
			y = y + rh;
			draw->DrawLine(aX1 + lm, y, aX2 - rm, y);
		}
	}

	small_font->FlushText();
}
//----------------------------------------------------------------------------
