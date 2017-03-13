/*
 * SmallFont.h
 *
 *  Created on: Mar 13, 2017
 *      Author: zaqc
 */

#ifndef SMALLFONT_H_
#define SMALLFONT_H_
//----------------------------------------------------------------------------

#include <GLES2/gl2.h>
//----------------------------------------------------------------------------

class SmallFont {
protected:
	GLuint m_Prog;
	GLuint m_paramVertexPos;
	GLuint m_paramTexturePos;
	GLuint m_paramFontColor;
	GLuint m_paramTexture;
	GLuint m_FB;
	GLuint m_Text;
	GLuint m_BkText;
	GLubyte *m_Data;

	GLfloat *m_V;
	GLfloat *m_T;
	GLushort *m_Ndx;
	int m_Index;

public:
	SmallFont();
	virtual ~SmallFont();

	int GetStringWidth(char *aStr);
	int GetStringHeight(void);
	void RenderString(int aX, int aY, char *aStr);
	void FlushText(void);
};
//----------------------------------------------------------------------------

extern SmallFont *small_font;
//----------------------------------------------------------------------------

#endif /* SMALLFONT_H_ */
