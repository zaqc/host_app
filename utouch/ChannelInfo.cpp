/*
 * ChannelInfo.cpp
 *
 *  Created on: Mar 20, 2017
 *      Author: zaqc
 */

#include <memory.h>

#include "ChannelInfo.h"
#include "GlUtil.h"
//----------------------------------------------------------------------------

TapeSchema::TapeSchema() {
	m_PreCalc = NULL;

	for (int i = 0; i < 14; i++) {

	}
}

TapeSchema::~TapeSchema() {
	if (NULL != m_PreCalc) {
		delete[] m_PreCalc;
		m_PreCalc = NULL;
	}
}
//----------------------------------------------------------------------------
// michelleho

void TapeSchema::SimpleSchema(void) {

	int side_height = 240;	// in pixels

	ChannelParam *cp = new ChannelParam();
	cp->m_ChNum = 0;
	cp->m_Side = 0;
	cp->m_TickTime = 32.0f * 40.0f / 1000.0f;
	cp->m_FromTime = -32.0f * 40.0f * 5.0f / 1000.0f;
	m_Channel.push_back(cp);

	TrackParam *tp = new TrackParam();
	tp->m_Channel.push_back(cp);
	tp->m_FromTime = 0.0f;
	tp->m_TimeScale = 180.0f; // mks
	tp->m_DefaultHeight = 100.0; // %
	tp->m_Fixed = false;
	tp->m_FixedHeight = 128;
	tp->m_MinHeight = 10;
	tp->m_UseIt = true;
	m_Track.push_back(tp);

	float full_scale = 0.0f;
	int track_count = 0;
	for (std::vector<TrackParam*>::iterator i = m_Track.begin(); i != m_Track.end(); i++) {
		full_scale += (*i)->m_DefaultHeight;
		track_count++;
	}

	float tr_div = 1.0;

	if (track_count > 1)
		full_scale += tr_div * (track_count - 1);

	// vertex [0..128*28] y [0.0..480.0]
	// texture [0..128*28]

	for (std::vector<ChannelParam*>::iterator ch = m_Channel.begin(); ch != m_Channel.end(); ch++) {
		float top = 0.0f;
		for (std::vector<TrackParam*>::iterator tr = m_Track.begin(); tr != m_Track.end(); tr++) {
			for (std::vector<ChannelParam*>::iterator tr_ch = (*tr)->m_Channel.begin(); tr_ch != (*tr)->m_Channel.end();
					tr_ch++) {
				if (*tr_ch == *ch) {
					for (int i = 0; i < (*ch)->m_DataSize; i++) {
						float ch_time = (*ch)->m_FromTime + (float) i * (*ch)->m_TickTime;

						float tr_time_from = (*tr)->m_FromTime;
						float tr_time_to = tr_time_from + (*tr)->m_TimeScale;

						if (ch_time >= tr_time_from && ch_time < tr_time_to) {
							float tr_pos = (ch_time - tr_time_from) / (*tr)->m_TimeScale * (float)(*tr)->m_RealHeight;
							tr_pos += (*tr)->m_YPos;
						}
					}
				}
			}

//			float th = (float) side_height * (*i)->m_DefaultHeight / full_scale;
//			(*i)->m_RealHeight = (int) th;
//			(*i)->m_YPos = (int) top;
//			top += th;
//
//			int ith = (int) th;
//
//			for (int t = 0; t < (int) th; t++) {
//				float tt = (*i)->m_FromTime + (*i)->m_TimeScale / (float) (ith - 1);
//				for (std::vector<ChannelParam*>::iterator ch = (*i)->m_Channel.begin(); ch != (*i)->m_Channel.end();
//						ch++) {
//					float to_time = (*ch)->m_FromTime + (*ch)->m_DataSize * (*ch)->m_TickTime;
//					if (tt >= (*ch)->m_FromTime && tt < to_time) {
//						int tick = (tt - (*ch)->m_FromTime) / (*ch)->m_TickTime;
//					}
//				}
//			}
		}
	}
}
//----------------------------------------------------------------------------

void TapeSchema::PrepareSchema(void) {
}
//----------------------------------------------------------------------------

#include "./img/sonic_45_70.c"
#include "./img/sonic_70.c"
#include "./img/sonic_58.c"
#include "./img/sonic_0.c"

ChannelInfo::ChannelInfo() {
	m_Prog = loadProgram("./glsl/main.glslv", "./glsl/main.glslf");

	m_paramVertexPos = glGetAttribLocation(m_Prog, "VertexPos");
	m_paramTexture = glGetUniformLocation(m_Prog, "Texture");
	m_paramGlobalTime = glGetUniformLocation(m_Prog, "GlobalTime");

	m_paramLeftLevelMask = glGetUniformLocation(m_Prog, "LeftLevelMask");

	unsigned char *buf = new unsigned char[96 * 96 * 4 * 4];

	for (int n = 0; n < 4; n++) {
		unsigned char *in_buf = NULL;
		in_buf = (unsigned char *) ((n == 0) ? GIMP_IMAGE_pixel_data : (n == 1) ? GIMP_IMAGE_70_pixel_data :
									(n == 2) ? GIMP_IMAGE_58_pixel_data : GIMP_IMAGE_0_pixel_data);
		if (NULL != in_buf) {
			for (int i = 0; i < 96; i++) {
				memcpy(&buf[n * 96 * 4 + i * 4 * 4 * 96], in_buf, 96 * 4);
				in_buf += 96 * 4;
			}
		}
	}

	glGenTextures(1, &m_Text);
	glBindTexture(GL_TEXTURE_2D, m_Text);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 96 * 4, 96, 0, GL_RGBA, GL_UNSIGNED_BYTE, buf);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);	// unBind texture

	delete[] buf;
}
//----------------------------------------------------------------------------

ChannelInfo::~ChannelInfo() {
}
//----------------------------------------------------------------------------

float global_time = 0.0;

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
	glClearColor(0.0, 0.0, 0.25, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);

	glVertexAttribPointer(m_paramVertexPos, 3, GL_FLOAT, GL_FALSE, 0, v);
	glEnableVertexAttribArray(m_paramVertexPos);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_Text);

	glUniform1i(m_paramTexture, 1);

	glUniform1f(m_paramGlobalTime, global_time);
	global_time += 0.05;

	glUniform1f(m_paramLeftLevelMask, 1.0);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, ndx);

	glBindTexture(GL_TEXTURE_2D, 0);
}
//----------------------------------------------------------------------------
