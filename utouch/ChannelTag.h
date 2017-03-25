/*
 * ChannelTag.h
 *
 *  Created on: Mar 22, 2017
 *      Author: zaqc
 */

#ifndef CHANNELTAG_H_
#define CHANNELTAG_H_
//----------------------------------------------------------------------------

#include <GLES2/gl2.h>
//----------------------------------------------------------------------------

// DrawLine http://stackoverflow.com/questions/15276454/is-it-possible-to-draw-line-thickness-in-a-fragment-shader

class TagItem {
protected:
	GLfloat *m_T;
	GLfloat *m_V;
	GLushort *m_Ndx;
public:
	TagItem(GLfloat *aV, GLfloat *aT, GLushort *aNdx);
	virtual ~TagItem();

	void Texture(int aX1, int aY1, int aX2, int aY2);
	void Rect(int aX1, int aY1, int aX2, int aY2);
	void Color(float aR, float aG, float aB, float aA);
};
//----------------------------------------------------------------------------

class ChannelTag {
	GLuint m_Prog;
	GLuint m_paramVertexPos;
	GLuint m_paramTexturePos;
	GLuint m_paramFontColor;
	GLuint m_paramTexture;
	GLuint m_paramOffsetXY;

	GLuint m_paramAngle;

	GLuint m_Text;	// 512x512 for all possible tags
public:
	ChannelTag();
	virtual ~ChannelTag();

	void InitTexture(void);

	void Render(void);
};
//----------------------------------------------------------------------------

#endif /* CHANNELTAG_H_ */
