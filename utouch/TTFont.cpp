/*
 * TTFont.cpp
 *
 *  Created on: Mar 20, 2017
 *      Author: zaqc
 */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

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

	error = FT_New_Face(m_Library, "/home/zaqc/work/1ftn/LiberationSerif-Bold.ttf", 0, &m_Face);/* create face object */
	//error = FT_New_Face(m_Library, "/usr/share/fonts/truetype/ubuntu-font-family/UbuntuMono-R.ttf", 0, &m_Face);
	//error = FT_New_Face(m_Library, "/usr/share/fonts/truetype/freefont/FreeMono.ttf", 0, &m_Face);
	if (error) {
		printf("Can't load /home/zaqc/work/1ftn/LiberationSerif-Bold.ttf...");
		exit(-1);
	}

	error = FT_Set_Char_Size(m_Face, 12 << 6, 16 << 6, 100, 100); /* set character size */
	if (error) {
		printf("Error on FT_Set_Char_Size...");
		exit(-1);
	}

	m_Slot = m_Face->glyph;

	FT_Matrix m;
	m.xx = 0x10000;	// this is 1.0
	m.xy = 0;
	m.yy = 0x10000;
	m.yx = 0;

	FT_Vector v;
	v.x = 10;
	v.y = 10;

	FT_Set_Transform(m_Face, &m, &v);
	FT_Load_Char(m_Face, 'D', FT_LOAD_RENDER);
	//m_Slot->bitmap.buffer;
	//m_Slot->advance.x

	unsigned char *buf = new unsigned char[1024 * 1024 * 4];
	memset(buf, 0, 1024 * 1024 * 4);
	for (unsigned int j = 0; j < m_Slot->bitmap.rows; j++) {
		for (unsigned int i = 0; i < m_Slot->bitmap.width; i++) {
			unsigned char v = m_Slot->bitmap.buffer[i + (m_Slot->bitmap.rows - j - 1) * m_Slot->bitmap.width];
			int addr = (i + 100) * 4 + (j + 50) * 4096;
			buf[addr] = 255;
			buf[addr + 1] = 255;
			buf[addr + 2] = 0;
			buf[addr + 3] = v;
		}
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

void TTFont::Render(void) {
	GLfloat v[] = { /* vertexes */
	-1.0f, -1.0f, 0.0f, /**/
	-1.0f, 1.0f, 0.0f, /**/
	1.0f, -1.0f, 0.0f, /**/
	1.0f, 1.0f, 0.0f };

	GLfloat t[] = { /* texture coordinate */
	0.0f, 0.0f, /**/
	0.0f, 1.0f, /**/
	1.0f, 0.0f, /**/
	1.0f, 1.0f };

	GLushort ndx[] = { 1, 0, 2, 1, 2, 3 };

	glUseProgram(m_Prog);

	glDisable(GL_DEPTH_TEST);
	glViewport(0, 0, 1024, 1024);

	glVertexAttribPointer(m_paramVertexPos, 3, GL_FLOAT, GL_FALSE, 0, v);
	glEnableVertexAttribArray(m_paramVertexPos);

	glVertexAttribPointer(m_paramTexturePos, 2, GL_FLOAT, GL_FALSE, 0, t);
	glEnableVertexAttribArray(m_paramTexturePos);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_Text);

	glUniform1i(m_paramTexture, 1);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, ndx);

	glBindTexture(GL_TEXTURE_2D, m_Text);
}
//----------------------------------------------------------------------------

