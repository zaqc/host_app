/*
 * ChannelTag.cpp
 *
 *  Created on: Mar 22, 2017
 *      Author: zaqc
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <GLES2/gl2.h>

#include "ChannelTag.h"
#include "TextDraw.h"
#include "GlUtil.h"
//----------------------------------------------------------------------------

ChannelTag::ChannelTag() {
	 const char vs[] = "attribute vec4 VertexPos; \n"
	 "attribute vec2 TexturePos; \n"
	 "varying vec2 TextCoord; \n"
	 "uniform vec2 OffsetXY; \n"
	 "uniform vec4 FontColor; \n"
	 "uniform float Angle; \n"
	 "float x; \n"
	 "float y; \n"
	 "void main() \n"
	 "{ \n"
	 "    mat3 r = mat3( \n"
	 "        vec3(cos(Angle), sin(Angle), 0.0), \n"
	 "        vec3(-sin(Angle), cos(Angle), 0.0), \n"
	 "        vec3(0.0, 0.0, 1.0 ) \n"
	 "    ); \n"
	 "    vec3 v = vec3(0.5, 0.5, 0.0); \n"
	 "    mat4 mv = mat4( \n"
	 "        vec4(1.0, 0.0, 0.0, 0.0), \n"
	 "        vec4(0.0, 1.0, 0.0, 0.0), \n"
	 "        vec4(0.0, 0.0, 1.0, 0.0), \n"
	 "        vec4(0.0, 0.0, 0.0, 1.0)  \n"
	 "    ); \n"
	 "    x = (OffsetXY.x + VertexPos.x); \n"
	 "    y = (OffsetXY.y + VertexPos.y); \n"
	 "    vec4 xy = vec4(x, y, VertexPos.z, VertexPos.w); \n"
	 "    gl_Position = vec4(vec3(xy.x, xy.y, xy.z) * r, xy.w); \n"
	 "    TextCoord = TexturePos; \n"
	 "} \n";

	const char fs[] = "precision mediump float;\n"
			"#define resolution vec2(800.0, 480.0) \n"
			"#define Thickness 0.003 \n"
			"float drawLine(vec2 p1, vec2 p2) { \n"
			"    vec2 uv = vec2(gl_FragCoord.x * 2.0, gl_FragCoord.y * 2.0) / resolution.xy; \n"
			"    float a = abs(distance(p1, uv)); \n"
			"    float b = abs(distance(p2, uv)); \n"
			"    float c = abs(distance(p1, p2)); \n"
			"    if ( a >= c || b >=  c ) return 0.0; \n"
			"    float p = (a + b + c) * 0.5; \n"
			"    float h = 2.0 / c * sqrt( p * ( p - a) * ( p - b) * ( p - c)); \n"
			"    return mix(1.0, 0.0, smoothstep(0.5 * Thickness, 1.5 * Thickness, h)); \n"
			"} \n"
			"varying vec4 frag_color; \n"
			"void main() \n"
			"{ \n"
			"    gl_FragColor = vec4( \n"
			"    max( \n"
			"        max( \n"
			"            drawLine(vec2(0.1, 0.1), vec2(0.1, 0.9)), \n"
			"            drawLine(vec2(0.1, 0.9), vec2(0.7, 0.5))), \n"
			"            drawLine(vec2(0.1, 0.1), vec2(0.7, 0.5)))); \n"
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

	m_paramAngle = glGetUniformLocation(m_Prog, "Angle");

	m_Text = 0;
}
//----------------------------------------------------------------------------

ChannelTag::~ChannelTag() {
}
//----------------------------------------------------------------------------

void DrawTag(int aX1, int aY1, int aX2, int aY2, int aCh) {
	draw->SetColor(1.0, 1.0, 1.0, 1.0);
	draw->FillRect(aX1, aY1, aX2, aY2);
	draw->SetColor(0.0, 0.0, 0.0, 1.0);
	draw->DrawRect(aX1, aY1, aX2, aY2);
}
//----------------------------------------------------------------------------

void ChannelTag::InitTexture(void) {
	glGenTextures(1, &m_Text);
	glBindTexture(GL_TEXTURE_2D, m_Text);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 512, 32, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	GLuint fb;
	glGenFramebuffers(1, &fb);
	glBindFramebuffer(GL_FRAMEBUFFER, fb);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_Text, 0);

	glViewport(0, 0, 512, 32);
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	const char vs[] = "attribute vec4 VertexPos; \n"
			"attribute vec2 TexturePos; \n"
			"uniform float Angle; \n"
			"varying vec2 TextCoord; \n"
			"void main() \n"
			"{ \n"
			"    mat4 rotate = mat4( \n"
			"        vec4(cos(Angle), sin(Angle), 0.0, 0.0), \n"
			"        vec4(-sin(Angle), cos(Angle), 0.0, 0.0), \n"
			"        vec4(0.0, 0.0, 1.0, 0.0), \n"
			"        vec4(0.0, 0.0, 0.0, 1.0) \n"
			"    ); \n"
			"    mat4 shift = mat4( \n"
			"        vec4(1.0, 0.0, 0.0, 0.0), \n"
			"        vec4(0.0, 1.0, 0.0, 0.0), \n"
			"        vec4(0.0, 0.0, 1.0, 0.0), \n"
			"        vec4(0.0, 0.0, 0.0, 1.0) \n"
			"    ); \n"
			"    vec4 scale = vec4(1.0, 1.0, 1.0, 1.0); \n"
			"    gl_Position = VertexPos * shift * scale * rotate; \n"
			"    TextCoord = TexturePos; \n"
			"} \n";

	const char fs[] = "precision mediump float;\n"
			"varying vec2 TextCoord;\n"
			"uniform sampler2D Texture; \n"
			"vec4 txtColor; \n"
			"void main() \n"
			"{ \n"
//			"    txtColor = texture2D(Texture, TextCoord); \n"
			"    txtColor = vec4(1.0, 1.0, 1.0, 1.0); \n"
			"    gl_FragColor = txtColor; \n"
			"} \n";

	GLuint prog = createProgram(vs, fs);
	if (!prog) {
		printf("Can't load program for TextScroller...\n");
		exit(-1);
	}
	GLuint param_vertex_pos = glGetAttribLocation(prog, "VertexPos");
	GLuint param_angle = glGetUniformLocation(prog, "Angle");

//	m_paramTexturePos = glGetAttribLocation(m_Prog, "TexturePos");
//	m_paramTexture = glGetUniformLocation(m_Prog, "Texture");
//	m_paramFontColor = glGetUniformLocation(m_Prog, "FontColor");
//	m_paramOffsetXY = glGetUniformLocation(m_Prog, "OffsetXY");
//	m_paramAngle = glGetUniformLocation(m_Prog, "Angle");

	GLfloat v[] = { /**/
	-2.0 / 3.0, 1.0 / 3.0, 0.0, /**/
	0.0, 1.0, 0.0, /**/
	2.0 / 3.0, 1.0 / 3.0, 0.0, /**/
	1.0 / 3.9, 1.0 / 3.0, 0.0, /**/
	1.0 / 3.9, -1.0, 0.0, /**/
	-1.0 / 3.0, -1.0, 0.0, /**/
	-1.0 / 3.0, 1.0 / 3.0, 0.0 /**/
	};

	GLushort ndx[] = { 0, 1, 2, 3, 4, 5, 5, 6, 3 };

	glUseProgram(prog);

	glDisable(GL_DEPTH_TEST);

	glVertexAttribPointer(param_vertex_pos, 3, GL_FLOAT, GL_FALSE, 0, v);
	glEnableVertexAttribArray(param_vertex_pos);

	for (int i = 0; i < 8; i++) {
		glViewport(i * 32, 0, 32, 32);

		glUniform1f(param_angle, 3.1415 / 4.0 * (float) i);

		glDrawElements(GL_TRIANGLES, 9, GL_UNSIGNED_SHORT, ndx);
	}

//	GLushort l_ndx[] = { 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 0 };
//	glDrawElements(GL_LINES, 14, GL_UNSIGNED_SHORT, l_ndx);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glDeleteFramebuffers(1, &fb);
}

float __angle = 0.0f;
void ChannelTag::Render(void) {
	glUseProgram(m_Prog);

	glDisable(GL_DEPTH_TEST);
	//glViewport(400 - 256, 240 - 16, 512, 32);
	glViewport(0, 0, 800, 480);

	float x1 = -1.0;
	float y1 = -1.0;
	float x2 = 1.0;
	float y2 = 1.0;

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

	glVertexAttribPointer(m_paramVertexPos, 3, GL_FLOAT, GL_FALSE, 0, v);
	glEnableVertexAttribArray(m_paramVertexPos);

	glVertexAttribPointer(m_paramTexturePos, 2, GL_FLOAT, GL_FALSE, 0, t);
	glEnableVertexAttribArray(m_paramTexturePos);

	glUniform2f(m_paramOffsetXY, 0, 0);
	glUniform4f(m_paramFontColor, 1.0, 1.0, 1.0, 1.0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_Text);

	glUniform1i(m_paramTexture, 1);

	glUniform1f(m_paramAngle, __angle);
	//__angle += 0.01;

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, ndx);
	glBindTexture(GL_TEXTURE_2D, 0);

}

