/*
 * TextScroller.h
 *
 *  Created on: Feb 23, 2017
 *      Author: zaqc
 */

#ifndef TEXTSCROLLER_H_
#define TEXTSCROLLER_H_
//----------------------------------------------------------------------------

#include <GLES2/gl2.h>
//----------------------------------------------------------------------------

class TextScroller {
protected:
	GLuint m_Prog;
	GLuint m_paramVertexPos;
	GLuint m_paramVerexTextCoord;
	GLuint m_paramTexture;
	GLuint m_paramShiftX;
	GLubyte *m_Data;
	GLuint m_BkText;
	GLuint m_BkFB;
	GLuint m_Text;
	GLuint m_FB;
public:
	TextScroller();
	virtual ~TextScroller();

	void InitProgram(void);
	void Init(int aW, int aH);
	void Scroll(int aDX);

	void DrawData(int aW, unsigned char *aBuf);
	void RenderFrame(unsigned char *aBuf);
};
//----------------------------------------------------------------------------

#endif /* TEXTSCROLLER_H_ */
