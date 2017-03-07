/*
 * TextFont.h
 *
 *  Created on: Mar 7, 2017
 *      Author: zaqc
 */

#ifndef TEXTFONT_H_
#define TEXTFONT_H_

#include <GLES2/gl2.h>

class TextFont {
protected:
	GLuint m_Prog;
	GLuint m_paramVertexPos;
	GLuint m_paramVertexTextCoord;
	GLuint m_paramFontColor;
	GLuint m_paramTexture;
	GLuint m_FB;
	GLuint m_Text;
	GLuint m_BkText;
	GLubyte *m_Data;
public:
	TextFont();
	virtual ~TextFont();

	int GetStringWidth(char *aStr);
	int GetStringHeight(void);
	void RenderString(int aX, int aY, char *aStr);
};

extern TextFont *font;

#endif /* TEXTFONT_H_ */
