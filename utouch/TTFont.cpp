/*
 * TTFont.cpp
 *
 *  Created on: Mar 20, 2017
 *      Author: zaqc
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wchar.h>
#include <wctype.h>

#include <ft2build.h>

#include "TTFont.h"
#include "GlUtil.h"
//----------------------------------------------------------------------------

TTFont::TTFont() {
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
			"    txtColor.r = 1.0; \n"
			"    txtColor.g = 1.0; \n"
			"    txtColor.b = 0.0; \n"
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

	FT_Error error;
	error = FT_Init_FreeType(&m_Library); /* initialize library */
	if (error) {
		printf("Error on FT_Init_FreeType...");
		exit(-1);
	}

	//error = FT_New_Face(m_Library, "/home/zaqc/work/1ftn/LiberationSerif-Bold.ttf", 0, &m_Face);/* create face object */
	//error = FT_New_Face(m_Library, "/usr/share/fonts/truetype/ubuntu-font-family/UbuntuMono-R.ttf", 0, &m_Face);
	//error = FT_New_Face(m_Library, "/usr/share/fonts/truetype/freefont/FreeMono.ttf", 0, &m_Face);
	//error = FT_New_Face(m_Library, "/usr/share/fonts/opentype/freefont/FreeMonoBold.otf", 0, &m_Face);
	error = FT_New_Face(m_Library, "/usr/share/fonts/opentype/freefont/FreeSerifBold.otf", 0, &m_Face);
	if (error) {
		printf("Can't load /home/zaqc/work/1ftn/LiberationSerif-Bold.ttf...");
		exit(-1);
	}

	error = FT_Set_Char_Size(m_Face, 20 << 6, 20 << 6, 72, 72); /* set character size */
	if (error) {
		printf("Error on FT_Set_Char_Size...");
		exit(-1);
	}

	FT_GlyphSlot slot = m_Face->glyph;

	FT_Matrix m;
	m.xx = 0x10000;	// this is 1.0
	m.xy = 0;
	m.yy = 0x10000;
	m.yx = 0;

	FT_Vector v;
	v.x = 0;
	v.y = 0;

	wchar_t str[32];
	swprintf(str, 32, L"↖70° ↑70° ↗70° Вжух...");

	unsigned char *buf = new unsigned char[1024 * 1024 * 4];
	memset(buf, 0, 1024 * 1024 * 4);

	int x = 100;
	for (unsigned int n = 0; n < std::wstring(str).length(); n++) {
		wchar_t ch = str[n];

		FT_Set_Transform(m_Face, &m, &v);
		//FT_Load_Char(m_Face, 0x2196, FT_LOAD_RENDER);
		FT_Load_Char(m_Face, ch, FT_LOAD_RENDER);
		//FT_Load_Char(m_Face, 0x2297, FT_LOAD_RENDER);
		//m_Slot->bitmap.buffer;
		//m_Slot->advance.x

		for (unsigned int j = 0; j < slot->bitmap.rows; j++) {
			for (unsigned int i = 0; i < slot->bitmap.width; i++) {
				unsigned char v = slot->bitmap.buffer[i + j * slot->bitmap.width];
				int addr = (i + x + slot->bitmap_left) * 4 + (100 - j + slot->bitmap_top) * 4096;
				unsigned char r, g, b;
				r = 255;
				g = 255;
				b = 0;
				if (n > 4) {
					r = 0;
				}
				if (n > 9) {
					b = 255;
				}
				buf[addr] = r;
				buf[addr + 1] = g;
				buf[addr + 2] = b;
				buf[addr + 3] = v;
			}
		}
		x += /*m_Slot->bitmap.width + m_Slot->bitmap_left +*/slot->metrics.horiAdvance / 64;
	}

	glGenTextures(1, &m_Text);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_Text);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1024, 1024, 0, GL_RGBA, GL_UNSIGNED_BYTE, buf);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	delete[] buf;
}
//----------------------------------------------------------------------------

TTFont::~TTFont() {
	FT_Done_Face(m_Face);
	FT_Done_FreeType(m_Library);
}
//----------------------------------------------------------------------------

int TTFont::GetStringWidth(std::wstring aStr) {
	int len = aStr.length();

	FT_Matrix m;
	m.xx = 0x10000;	// this is 1.0
	m.xy = 0;
	m.yy = 0x10000;
	m.yx = 0;

	FT_Vector v;
	v.x = 0;
	v.y = 0;

	int x = 0;
	for (int n = 0; n < len; n++) {
		wchar_t ch = aStr[n];

		FT_Set_Transform(m_Face, &m, &v);

		FT_Load_Char(m_Face, ch, FT_LOAD_RENDER);

		x += m_Face->glyph->metrics.horiAdvance / 64;
	}
	return x;
}
//----------------------------------------------------------------------------

int TTFont::GetStringHeight(std::wstring aStr) {
	int len = aStr.length();

	FT_Matrix m;
	m.xx = 0x10000;	// this is 1.0
	m.xy = 0;
	m.yy = 0x10000;
	m.yx = 0;

	FT_Vector v;
	v.x = 0;
	v.y = 0;

	int max_h = 0;
	for (int n = 0; n < len; n++) {
		wchar_t ch = aStr[n];

		FT_Set_Transform(m_Face, &m, &v);

		FT_Load_Char(m_Face, ch, FT_LOAD_RENDER);

		int h = m_Face->glyph->metrics.vertAdvance / 64;
		if (h > max_h)
			max_h = h;
	}
	return max_h;
}
//----------------------------------------------------------------------------

void TTFont::RenderString(std::wstring aStr) {
	int w = GetStringWidth(aStr);
	int h = GetStringHeight(aStr);

	w = ((w + 3) / 4) * 4;

	FT_Matrix m;
	m.xx = 0x10000;	// this is 1.0
	m.xy = 0;
	m.yy = 0x10000;
	m.yx = 0;

	FT_Vector v;
	v.x = 0;
	v.y = 0;

	unsigned char *buf = new unsigned char[w * h];
	memset(buf, 0, w * h);
	int len = aStr.length();
	int x = 0;
	for (int n = 0; n < len; n++) {
		wchar_t ch = aStr[n];

		FT_Set_Transform(m_Face, &m, &v);

		FT_Load_Char(m_Face, ch, FT_LOAD_RENDER);

		FT_GlyphSlot slot = m_Face->glyph;

		int ch_w = slot->bitmap.width;
		int ch_h = slot->bitmap.rows;

		for (int j = 0; j < ch_h; j++) {
			for (int i = 0; i < ch_w; i++) {
				unsigned char v = slot->bitmap.buffer[i + j * slot->bitmap.pitch];
				int addr = slot->bitmap_left + i + x + (h / 4 - (j - slot->bitmap_top) - 1) * w;
				addr = addr > 0 ? addr : 0;
				addr = addr < w * h ? addr : w * h - 1;
				buf[addr] = v;
			}
		}

		x += slot->metrics.horiAdvance / 64;
	}

	GLuint txt;
	glGenTextures(1, &txt);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, txt);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, w, h, 0, GL_ALPHA, GL_UNSIGNED_BYTE, buf);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	m_Text = txt;

	delete[] buf;

	m_TTS.m_W = w;
	m_TTS.m_H = h;
	m_TTS.m_Text = txt;
}
//----------------------------------------------------------------------------

void TTFont::Render(void) {
	float x1 = -.25f;
	float y1 = -.25f;
	float x2 = (float)m_TTS.m_W / 400.0 - .25f;
	float y2 = (float)m_TTS.m_H / 240.0 - .25f;
	GLfloat v[] = { /* vertexes */
	x1, y1, 0.0f, /**/
	x1, y2, 0.0f, /**/
	x2, y1, 0.0f, /**/
	x2, y2, 0.0f };

	GLfloat t[] = { /* texture coordinate */
	0.0f, 0.0f, /**/
	0.0f, 1.0f, /**/
	1.0f, 0.0f, /**/
	1.0f, 1.0f };

	GLushort ndx[] = { 1, 0, 2, 1, 2, 3 };

	glUseProgram(m_Prog);

	glDisable(GL_DEPTH_TEST);
	glViewport(0, 0, 800, 480);

	glVertexAttribPointer(m_paramVertexPos, 3, GL_FLOAT, GL_FALSE, 0, v);
	glEnableVertexAttribArray(m_paramVertexPos);

	glVertexAttribPointer(m_paramTexturePos, 2, GL_FLOAT, GL_FALSE, 0, t);
	glEnableVertexAttribArray(m_paramTexturePos);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_Text);

	glUniform1i(m_paramTexture, 1);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, ndx);

	glBindTexture(GL_TEXTURE_2D, 0);
}
//----------------------------------------------------------------------------

