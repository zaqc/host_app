/*
 * TextFont.cpp
 *
 *  Created on: Mar 7, 2017
 *      Author: zaqc
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <GLES2/gl2.h>

#include "TextFont.h"
#include "GlUtil.h"

#include "font.h"
//----------------------------------------------------------------------------

TextFont *font = NULL;
//----------------------------------------------------------------------------

TextFont::TextFont() {
	const char vs[] = "attribute vec4 VertexPos; \n"
			"attribute vec2 TexturePos; \n"
			"varying vec2 TextCoord; \n"
			"uniform vec4 FontColor; \n"
			"void main() \n"
			"{ \n"
			"    gl_Position = VertexPos; \n"
			"    TextCoord = TexturePos; \n"
			"} \n";

	const char fs[] = "precision mediump float;\n"
			"varying vec2 TextCoord;\n"
			"uniform sampler2D Texture; \n"
			"vec4 txtColor; \n"
			"void main() \n"
			"{ \n"
			"    txtColor = texture2D(Texture, TextCoord); \n"
			"    gl_FragColor = txtColor; \n"
			"} \n";

	m_Prog = createProgram(vs, fs);
	if (!m_Prog) {
		printf("Can't load program for TextScroller...\n");
		exit(-1);
	}
	m_paramVertexPos = glGetAttribLocation(m_Prog, "VertexPos");
	m_paramTexturePos = glGetAttribLocation(m_Prog, "TexturePos");
	m_paramTexture = glGetUniformLocation(m_Prog, "Texture");
	m_paramFontColor = glGetUniformLocation(m_Prog, "FontColor");

	m_Data = new GLubyte[1024 * 16 * 4];
	for (int i = 0; i < 1024 * 16; i++) {
		m_Data[i * 4] = 0;
		m_Data[i * 4 + 1] = 0;
		m_Data[i * 4 + 2] = 0;
		m_Data[i * 4 + 3] = 0;
	}
	unsigned char *font_ptr = &font_8x13[0][0];
	for (unsigned int n = 0; n < sizeof(font_8x13) / 13; n++) {
		for (int j = 0; j < 13; j++) {
			int offset = (n * 9 * 4) + j * 4096;
			unsigned char v = *font_ptr;
			for (int i = 0; i < 8; i++) {
				m_Data[offset + i * 4] = (v & 0x80) ? 255 : 0;
				m_Data[offset + i * 4 + 1] = (v & 0x80) ? 255 : 0;
				m_Data[offset + i * 4 + 2] = (v & 0x80) ? 255 : 0;
				m_Data[offset + i * 4 + 3] = (v & 0x80) ? 255 : 0;
				v <<= 1;
			}
			font_ptr++;
		}
	}

	glGenTextures(1, &m_Text);
	glBindTexture(GL_TEXTURE_2D, m_Text);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1024, 16, 0, GL_RGBA,
	GL_UNSIGNED_BYTE, m_Data);	// GL_ALPHA
	glGenerateMipmap(GL_TEXTURE_2D);

	glGenFramebuffers(1, &m_FB);
	glBindFramebuffer(GL_FRAMEBUFFER, m_FB);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
			m_Text, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);	// unBind texture
	glBindTexture(GL_TEXTURE_2D, 0);	// unBind texture

	glActiveTexture(GL_TEXTURE2);
	glGenTextures(1, &m_BkText);
	glBindTexture(GL_TEXTURE_2D, m_BkText);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1024, 16, 0, GL_RGBA,
	GL_UNSIGNED_BYTE, NULL);	// GL_ALPHA
	glBindTexture(GL_TEXTURE_2D, 0);

	m_V = new GLfloat[12 * 10000];
	m_T = new GLfloat[8 * 10000];
	m_Ndx = new GLushort[6 * 10000];
	m_Index = 0;
}
//----------------------------------------------------------------------------

TextFont::~TextFont() {
	delete[] m_Ndx;
	delete[] m_T;
	delete[] m_V;

	glDeleteTextures(1, &m_BkText);

	glDeleteFramebuffers(1, &m_FB);
	glDeleteTextures(1, &m_Text);
	delete[] m_Data;
}
//----------------------------------------------------------------------------

int TextFont::GetStringWidth(char *aStr) {
	return strlen(aStr) * 9;
}
//----------------------------------------------------------------------------

int TextFont::GetStringHeight(void) {
	return 16;
}
//----------------------------------------------------------------------------

void TextFont::RenderString(int aX, int aY, char *aStr, bool aFlush) {
	int len = strlen((char *) aStr);

	if (m_Index + len >= 1000) {
		glViewport(0, 0, 800, 480);
		glDisable(GL_DEPTH_TEST);

		glUseProgram(m_Prog);

		glVertexAttribPointer(m_paramVertexPos, 3, GL_FLOAT, GL_FALSE, 0, m_V);
		glEnableVertexAttribArray(m_paramVertexPos);

		glVertexAttribPointer(m_paramTexturePos, 2, GL_FLOAT, GL_FALSE, 0, m_T);
		glEnableVertexAttribArray(m_paramTexturePos);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, m_Text);
		glUniform1i(m_paramTexture, 1);

		//glDrawElements(GL_TRIANGLES, 6 * m_Index, GL_UNSIGNED_SHORT, m_Ndx);

		glBindTexture(GL_TEXTURE_2D, 0);
		m_Index = 0;
	}

	for (int i = 0; i < len; i++) {
		int ch = (unsigned char) *aStr - 32;
		float x1 = 2.0 / 800.0 * (float) (aX + i * 9) - 1.0;
		float x2 = 2.0 / 800.0 * (float) (aX + (i + 1) * 9) - 1.0;

		float y1 = 2.0 / 480.0 * (float)aY - 1.0;
		float y2 = 2.0 / 480.0 * (float)(aY + 16) - 1.0;

		GLfloat _v[] = { /* vertexes */
		x1, y1, 0.0f, /**/
		x1, y2, 0.0f, /**/
		x2, y1, 0.0f, /**/
		x2, y2, 0.0f };
		memcpy(&m_V[m_Index * 12], _v, 12 * sizeof(GLfloat));

		float tx1 = 1.0 / 1024.0 * (float) ch * 9.0;
		float tx2 = 1.0 / 1024.0 * (float) (ch + 1) * 9.0;
		GLfloat _txc[] = { /* texture coordinate */
		tx1, 0.0f, /**/
		tx1, 1.0f, /**/
		tx2, 0.0f, /**/
		tx2, 1.0f };
		memcpy(&m_T[m_Index * 8], _txc, 8 * sizeof(GLfloat));

		GLushort n = i * 4;
		GLushort _ndx[] = { (GLushort) (n + 1), (GLushort) (n + 0),
				(GLushort) (n + 2), (GLushort) (n + 1), (GLushort) (n + 2),
				(GLushort) (n + 3) };
		memcpy(&m_Ndx[m_Index * 6], _ndx, 6 * sizeof(GLushort));

		m_Index++;
		aStr++;
	}

	if (aFlush) {
		glViewport(0, 0, 800, 480);
		glDisable(GL_DEPTH_TEST);

		glUseProgram(m_Prog);

		glVertexAttribPointer(m_paramVertexPos, 3, GL_FLOAT, GL_FALSE, 0, m_V);
		glEnableVertexAttribArray(m_paramVertexPos);

		glVertexAttribPointer(m_paramTexturePos, 2, GL_FLOAT, GL_FALSE, 0, m_T);
		glEnableVertexAttribArray(m_paramTexturePos);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, m_Text);
		glUniform1i(m_paramTexture, 1);

		glDrawElements(GL_TRIANGLES, 6 * m_Index, GL_UNSIGNED_SHORT, m_Ndx);

		glBindTexture(GL_TEXTURE_2D, 0);
		m_Index = 0;
	}
}
//----------------------------------------------------------------------------
