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
			"uniform vec2 OffsetXY; \n"
			//"uniform vec4 FontColor; \n"
			"float x; \n"
			"float y; \n"
			"void main() \n"
			"{ \n"
			"    x = (OffsetXY.x + VertexPos.x) / 400.0 - 1.0; \n"
			"    y = 1.0 - (OffsetXY.y + VertexPos.y) / 240.0; \n"
			"    gl_Position = vec4(x, y, VertexPos.z, VertexPos.w); \n"
			"    TextCoord = TexturePos; \n"
			"} \n";

	const char fs[] = "precision mediump float;\n"
			"varying vec2 TextCoord;\n"
			"uniform vec4 FontColor; \n"
			"uniform sampler2D Texture; \n"
			"vec4 txtColor; \n"
			"void main() \n"
			"{ \n"
			"    txtColor = texture2D(Texture, TextCoord); \n"
			"    txtColor.r = FontColor.r; \n"
			"    txtColor.g = FontColor.g; \n"
			"    txtColor.b = FontColor.b; \n"
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
	m_paramOffsetXY = glGetUniformLocation(m_Prog, "OffsetXY");

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

	error = FT_Set_Char_Size(m_Face, 28 << 6, 28 << 6, 72, 72); /* set character size */
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

	m_R = m_G = m_B = 1.0f;
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

TTString TTFont::PrepTTString(std::wstring aStr) {
	TTString res;

	int w = GetStringWidth(aStr);
	int h = GetStringHeight(aStr);
	res.Init(w, h);

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
				int addr = slot->bitmap_left + i + x + ((float) h / 4.0 * 3.0 - (slot->bitmap_top - j)) * w;
				addr = addr > 0 ? addr : 0;
				addr = addr < w * h ? addr : w * h - 1;
				buf[addr] = v;
			}
		}

		x += slot->metrics.horiAdvance / 64;
	}

	glGenTextures(1, &res.m_Text);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, res.m_Text);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, w, h, 0, GL_ALPHA, GL_UNSIGNED_BYTE, buf);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	delete[] buf;

	return res;
}
//----------------------------------------------------------------------------

void TTFont::SetColor(GLfloat aR, GLfloat aG, GLfloat aB) {
	m_R = aR;
	m_G = aG;
	m_B = aB;
}
//----------------------------------------------------------------------------

void TTFont::Render(int aX, int aY, TTString *aTTS) {
	glUseProgram(m_Prog);

	glDisable(GL_DEPTH_TEST);
	glViewport(0, 0, 800, 480);

	glVertexAttribPointer(m_paramVertexPos, 3, GL_FLOAT, GL_FALSE, 0, aTTS->m_V);
	glEnableVertexAttribArray(m_paramVertexPos);

	glVertexAttribPointer(m_paramTexturePos, 2, GL_FLOAT, GL_FALSE, 0, aTTS->m_T);
	glEnableVertexAttribArray(m_paramTexturePos);

	glUniform2f(m_paramOffsetXY, aX, aY);
	glUniform4f(m_paramFontColor, m_R, m_G, m_B, 1.0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, aTTS->m_Text);

	glUniform1i(m_paramTexture, 1);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, aTTS->m_Ndx);
	glBindTexture(GL_TEXTURE_2D, 0);
}
//----------------------------------------------------------------------------

