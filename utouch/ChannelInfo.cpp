/*
 * ChannelInfo.cpp
 *
 *  Created on: Mar 20, 2017
 *      Author: zaqc
 */

#include "ChannelInfo.h"
#include "GlUtil.h"
//----------------------------------------------------------------------------

ChannelInfo::ChannelInfo() {
	m_Prog = loadProgram("./glsl/main_vs.fragx", "./glsl/main_fs.fragx");

	m_paramVertexPos = glGetAttribLocation(m_Prog, "VertexPos");
	m_paramTexture = glGetUniformLocation(m_Prog, "Texture");

	unsigned char *buf = new unsigned char[1024 * 32 * 4];
	for (int i = 0; i < 1024 * 32 * 4; i += 4) {
		buf[i] = i;
		buf[i + 1] = i;
		buf[i + 2] = i;
		buf[i + 3] = 255;
	}

	glGenTextures(1, &m_Text);
	glBindTexture(GL_TEXTURE_2D, m_Text);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1024, 32, 0, GL_RGBA, GL_UNSIGNED_BYTE, buf);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);	// unBind texture

	delete [] buf;

}
//----------------------------------------------------------------------------

ChannelInfo::~ChannelInfo() {
}
//----------------------------------------------------------------------------

void ChannelInfo::Render(void) {
	GLfloat v[] = { /* vertexes */
	-1.0, -1.0, 0.0f, /**/
	-1.0, 1.0, 0.0f, /**/
	1.0, -1.0, 0.0f, /**/
	1.0, 1.0, 0.0f };

	GLushort ndx[] = { 1, 0, 2, 1, 2, 3 };

	glUseProgram(m_Prog);

	glDisable(GL_DEPTH_TEST);
	glViewport(0, 0, 800, 480);
	glClearColor(0.0, 0.0, 0.75, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);

	glVertexAttribPointer(m_paramVertexPos, 3, GL_FLOAT, GL_FALSE, 0, v);
	glEnableVertexAttribArray(m_paramVertexPos);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_Text);

	glUniform1i(m_paramTexture, 1);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, ndx);

	glBindTexture(GL_TEXTURE_2D, 0);
}
//----------------------------------------------------------------------------
