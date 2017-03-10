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
			"varying vec2 TextCoord; \n"
			"uniform vec4 FontColor; \n"
			"void main() \n"
			"{ \n"
			"    gl_Position = VertexPos; \n"
			"    TextCoord = vec2((VertexPos.x + 1.0) / 2.0, (VertexPos.y + 1.0) / 2.0); \n"
			"} \n";

	const char fs[] = "precision mediump float;\n"
			"varying vec2 TextCoord;\n"
			"uniform sampler2D Texture; \n"
			"vec4 txtColor; \n"
			"void main() \n"
			"{ \n"
//			"    txtColor = vec4(TextCoord.x, TextCoord.y, 1.0, 1.0); \n"
			"    txtColor = texture2D(Texture, TextCoord); \n"
//			"    txtColor.y = 0.0; \n"
//			"    txtColor.z = 0.0; \n"
			"    gl_FragColor = txtColor; \n"
			"} \n";

	m_Prog = createProgram(vs, fs);
	if (!m_Prog) {
		printf("Can't load programm for TextScroller...\n");
		exit(-1);
	}
	m_paramVertexPos = glGetAttribLocation(m_Prog, "VertexPos");
	m_paramVertexTextCoord = 0; //glGetAttribLocation(m_Prog, "VertexTextCoord");
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
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1024, 16, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_Data);	// GL_ALPHA
	glGenerateMipmap(GL_TEXTURE_2D);

	glGenFramebuffers(1, &m_FB);
	glBindFramebuffer(GL_FRAMEBUFFER, m_FB);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_Text, 0);

//	glViewport(0, 0, 1024, 16);
//	glDisable(GL_DEPTH_TEST);
//	glClearColor(1.0, 1.0, 0.0, 1.0);
//	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);	// unBind texture
	glBindTexture(GL_TEXTURE_2D, 0);	// unBind texture
}
//----------------------------------------------------------------------------

TextFont::~TextFont() {
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

void TextFont::RenderString(int aX, int aY, char *aStr) {
//unsigned char glyph[] = { 0x00, 0x20, 0x78, 0xA8, 0xA0, 0x60, 0x30, 0x28, 0xA8, 0xF0, 0x20, 0x00 };

	glBindFramebuffer(GL_FRAMEBUFFER, m_FB);
//	glViewport(0, 0, 1024, 16);
//	glDisable(GL_DEPTH_TEST);
//	glClearColor(1.0, 1.0, 0.0, 1.0);
//	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	int len = strlen((char *) aStr);

	glActiveTexture(GL_TEXTURE2);
	glGenTextures(1, &m_BkText);
	glBindTexture(GL_TEXTURE_2D, m_BkText);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1024, 16, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);	// GL_ALPHA
	glGenerateMipmap(GL_TEXTURE_2D);

	int x = 0;
	for (int i = 0; i < len; i++) {
		glCopyTexSubImage2D(GL_TEXTURE_2D, 0, x, 0, 9 * ((unsigned char) *aStr - 32), 0, 9, 16);
		x += 9;
		aStr++;
	}

	float x2 = 2.0 / 1024.0 * (float) (len * 9) - 1.0;

	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	GLfloat v[] = { /* vertexes */
	-1.0f, -1.0f, 0.0f, /**/
	-1.0f, 1.0f, 0.0f, /**/
	x2, -1.0f, 0.0f, /**/
	x2, 1.0f, 0.0f };

//	GLfloat txc[] = { /* texture coordinate */
//	0.0f, 0.0f, /**/
//	0.0f, 1.0f, /**/
//	1.0f, 0.0f, /**/
//	1.0f, 1.0f };

	GLushort ndx[] = { 1, 0, 2, 1, 2, 3 };

	glViewport(aX, aY, 1024, 16);
	glDisable(GL_DEPTH_TEST);

	glUseProgram(m_Prog);

	glVertexAttribPointer(m_paramVertexPos, 3, GL_FLOAT, GL_FALSE, 0, v);
	glEnableVertexAttribArray(m_paramVertexPos);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_BkText);
	glUniform1i(m_paramTexture, 1);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, ndx);

	glBindTexture(GL_TEXTURE_2D, 0);

	glDeleteTextures(1, &m_BkText);
}
//----------------------------------------------------------------------------
