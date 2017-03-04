/*
 * TextScroller.cpp
 *
 *  Created on: Feb 23, 2017
 *      Author: zaqc
 */

#include "TextScroller.h"
#include "GlUtil.h"

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <GLES2/gl2.h>

#include "DScopeStream.h"
//----------------------------------------------------------------------------

//============================================================================
//	TextScroller
//============================================================================

double t_log[256];

TextScroller::TextScroller() {
	m_paramVertexPos = 0;
	m_paramVerexTextCoord = 0;
	m_paramTexture = 0;
	m_paramShiftX = 0;
	m_Prog = 0;
	m_Text = 0;
	m_FB = 0;
	m_BkText = 0;
	m_BkFB = 0;
	m_Data = NULL;

	for (int i = 0; i < 256; i++) {
		t_log[i] = pow(20, (double) i / 20.0);
	}
}
//----------------------------------------------------------------------------

TextScroller::~TextScroller() {
}
//----------------------------------------------------------------------------

void TextScroller::InitProgram(void) {
	const char vs[] = "attribute vec4 VertexPos; \n"
			"uniform float ShiftX; \n"
			"attribute vec2 VertexTextCoord; \n"
			"varying vec2 TextCoord;\n"
			"float _x; \n"
			"void main() \n"
			"{ \n"
			"    gl_Position = VertexPos; \n"
			"    _x = VertexTextCoord.x + ShiftX; \n"
			"    TextCoord = vec2(_x, VertexTextCoord.y); \n"
			"} \n";

	const char fs[] = "precision mediump float;\n"
			"varying vec2 TextCoord;\n"
			"uniform sampler2D Texture; \n"
			"vec4 txtColor; \n"
			"void main() \n"
			"{ \n"
			"    txtColor = texture2D(Texture, TextCoord); \n"
			"    gl_FragColor = txtColor; \n"
//			"vec4(txtColor.x, txtColor.y, txtColor.z, 0.5f); \n"
			"} \n";

	m_Prog = createProgram(vs, fs);
	if (!m_Prog) {
		printf("Can't load programm for TextScroller...\n");
		exit(-1);
	}
	m_paramVertexPos = glGetAttribLocation(m_Prog, "VertexPos");
	m_paramVerexTextCoord = glGetAttribLocation(m_Prog, "VertexTextCoord");
	m_paramTexture = glGetUniformLocation(m_Prog, "Texture");
	m_paramShiftX = glGetUniformLocation(m_Prog, "ShiftX");
}
//----------------------------------------------------------------------------

void TextScroller::Init(int aW, int aH) {

	int ms = aH;
	if (aW > aH)
		ms = aW;
	int s = 2;
	while (s < ms)
		s *= 2;

	s = 1024;

	//m_Data = new GLubyte[s * s * 4];

	glUseProgram(m_Prog);

	unsigned char *buf = new unsigned char[1024 * 1024 * 4];

	for (int i = 0; i < 1024 * 1024 * 4; i += 4) {
		buf[i] = i;
		buf[i + 1] = i;
		buf[i + 2] = i;
		buf[i + 3] = 255;
	}

	glGenTextures(1, &m_BkText);
	glBindTexture(GL_TEXTURE_2D, m_BkText);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1024, 512, 0, GL_RGBA, GL_UNSIGNED_BYTE, buf);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);	// unBind texture

	glGenFramebuffers(1, &m_BkFB);
	glBindFramebuffer(GL_FRAMEBUFFER, m_BkFB);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_BkText, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);	// unBind texture

	glGenTextures(1, &m_Text);
	glBindTexture(GL_TEXTURE_2D, m_Text);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1024, 512, 0, GL_RGBA, GL_UNSIGNED_BYTE, buf);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenFramebuffers(1, &m_FB);
	glBindFramebuffer(GL_FRAMEBUFFER, m_FB);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_Text, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);	// unBind texture

	delete[] buf;
}
//----------------------------------------------------------------------------

int aa = 0;
void TextScroller::Scroll(int aDX) {
//	GLfloat v[] = { /* vertexes */
//	-1.0f, -1.0f, 0.0f, /**/
//	-1.0f, 1.0f, 0.0f, /**/
//	1.0f, -1.0f, 0.0f, /**/
//	1.0f, 1.0f, 0.0f };
	GLfloat v[] = { /* vertexes */
	-1.0f, -1.0f, 0.0f, /**/
	-1.0f, 1.0f, 0.0f, /**/
	1.0f, -1.0f, 0.0f, /**/
	1.0f, 1.0f, 0.0f };

//	GLfloat txc[] = { /* texture coordinate */
//	0.0f, 0.0f, /**/
//	0.0f, 1.0f, /**/
//	1.0f, 0.0f, /**/
//	1.0f, 1.0f };
	GLfloat txc[] = { /* texture coordinate */
	0.0f, 0.0f, /**/
	0.0f, 1.0f, /**/
	1.0f, 0.0f, /**/
	1.0f, 1.0f };

	GLushort ndx[] = { 1, 0, 2, 1, 2, 3 };

	glUseProgram(m_Prog);

	glBindFramebuffer(GL_FRAMEBUFFER, m_BkFB);

	glDisable(GL_DEPTH_TEST);
	glViewport(0, 0, 1024, 512);

	//glClearColor(1.0f, .5f, 0.0f, 1.0f);
	//glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_Text);

	glUniform1f(m_paramShiftX, 0.0f);

	glVertexAttribPointer(m_paramVertexPos, 3, GL_FLOAT, GL_FALSE, 0, v);
	glEnableVertexAttribArray(m_paramVertexPos);

	glVertexAttribPointer(m_paramVerexTextCoord, 2, GL_FLOAT, GL_FALSE, 0, txc);
	glEnableVertexAttribArray(m_paramVerexTextCoord);

	glUniform1i(m_paramTexture, 1);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, ndx);

	//glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//glBindBuffer()
	//glCopyTexImage2D(GL_)

//	GLuint tmp;
//	tmp = m_BkText;
//	m_BkText = m_Text;
//	m_Text = tmp;
//
//	tmp = m_BkFB;
//	m_BkFB = m_FB;
//	m_FB = tmp;
}
//----------------------------------------------------------------------------

void TextScroller::DrawData(int aW, DScopeStream *aDSS) {

}
//----------------------------------------------------------------------------

GLubyte *_buf = NULL;
int nnnn = 0;
void TextScroller::DrawData(int aW, unsigned char *aBuf) {
	if (!_buf)
		_buf = new GLubyte[1024 * 512 * 4];

	for (int j = 0; j < 480; j++) {
		int n = j * aW * 4;
		for (int i = 0; i < aW; i++) {
			unsigned char v = *aBuf; //t_log[*aBuf] < 255 ? (unsigned char) t_log[*aBuf] : 255;
			_buf[n++] = v;
			_buf[n++] = v;
			_buf[n++] = v;
			_buf[n++] = 255;
			aBuf++;
		}
	}

	GLfloat v[] = { /* vertexes */
	-1.0f, -1.0f, 0.0f, /**/
	-1.0f, 1.0f, 0.0f, /**/
	1.0f, -1.0f, 0.0f, /**/
	1.0f, 1.0f, 0.0f };

	GLfloat txc[] = { /* texture coordinate */
	0.0f, 0.0f, /**/
	0.0f, 1.0f, /**/
	1.0f, 0.0f, /**/
	1.0f, 1.0f };

	GLushort ndx[] = { 1, 0, 2, 1, 2, 3 };

	glUseProgram(m_Prog);

	glDisable(GL_DEPTH_TEST);
	glViewport(0, 0, aW, 480);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, m_BkText);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, aW, 480, 0, GL_RGBA, GL_UNSIGNED_BYTE, _buf);

	glBindFramebuffer(GL_FRAMEBUFFER, m_BkFB);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_BkText, 0);

	glUniform1f(m_paramShiftX, 0.0f);

	glVertexAttribPointer(m_paramVertexPos, 3, GL_FLOAT, GL_FALSE, 0, v);
	glEnableVertexAttribArray(m_paramVertexPos);

	glVertexAttribPointer(m_paramVerexTextCoord, 2, GL_FLOAT, GL_FALSE, 0, txc);
	glEnableVertexAttribArray(m_paramVerexTextCoord);

	glUniform1i(m_paramTexture, 2);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, ndx);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_Text);
	glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 800 - aW, 0, 0, 0, aW, 480);
	glBindTexture(GL_TEXTURE_2D, 0);
}
//----------------------------------------------------------------------------

int nnn = 0;

void TextScroller::RenderFrame(unsigned char *aBuf) {
	Scroll(10);

	glUseProgram(m_Prog);

	glDisable(GL_DEPTH_TEST);
	glViewport(0, 0, 1024, 512);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	GLfloat v[] = { /* vertexes */
	-1.0f, -1.0f, 0.0f, /**/
	-1.0f, 1.0f, 0.0f, /**/
	1.0f, -1.0f, 0.0f, /**/
	1.0f, 1.0f, 0.0f };

	GLfloat txc[] = { /* texture coordinate */
	0.0f, 0.0f, /**/
	0.0f, 1.0f, /**/
	1.0f, 0.0f, /**/
	1.0f, 1.0f };

	GLushort ndx[] = { 1, 0, 2, 1, 2, 3 };

	glUniform1f(m_paramShiftX, 0.0f);

	glVertexAttribPointer(m_paramVertexPos, 3, GL_FLOAT, GL_FALSE, 0, v);
	glEnableVertexAttribArray(m_paramVertexPos);

	glVertexAttribPointer(m_paramVerexTextCoord, 2, GL_FLOAT, GL_FALSE, 0, txc);
	glEnableVertexAttribArray(m_paramVerexTextCoord);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, m_Text);
	glUniform1i(m_paramTexture, 2);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, ndx);

	//if (nnn++ < 100) {
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_Text);
	int ss = 1;
	glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, ss, 0, 800 - ss, 480);
	glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 800 - ss, 0, ss, 0, ss, 480);
	//glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	DrawData(ss, aBuf);
	//}
	//nnn = 0;
}
//----------------------------------------------------------------------------

void TextScroller::RenderFrame(DScopeStream *aDSS) {
	glUseProgram(m_Prog);

	glDisable(GL_DEPTH_TEST);
	glViewport(0, 0, 1024, 512);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	GLfloat v[] = { /* vertexes */
	-1.0f, -1.0f, 0.0f, /**/
	-1.0f, 1.0f, 0.0f, /**/
	1.0f, -1.0f, 0.0f, /**/
	1.0f, 1.0f, 0.0f };

	GLfloat txc[] = { /* texture coordinate */
	0.0f, 0.0f, /**/
	0.0f, 1.0f, /**/
	1.0f, 0.0f, /**/
	1.0f, 1.0f };

	GLushort ndx[] = { 1, 0, 2, 1, 2, 3 };

	glUniform1f(m_paramShiftX, 0.0f);

	glVertexAttribPointer(m_paramVertexPos, 3, GL_FLOAT, GL_FALSE, 0, v);
	glEnableVertexAttribArray(m_paramVertexPos);

	glVertexAttribPointer(m_paramVerexTextCoord, 2, GL_FLOAT, GL_FALSE, 0, txc);
	glEnableVertexAttribArray(m_paramVerexTextCoord);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, m_Text);
	glUniform1i(m_paramTexture, 2);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, ndx);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_Text);
	int ss = aDSS->GetFrameCount();
	glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, ss, 0, 800 - ss, 480);
	glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 800 - ss, 0, ss, 0, ss, 480);
	glBindTexture(GL_TEXTURE_2D, 0);

	DrawData(ss, aDSS);
}
//----------------------------------------------------------------------------

