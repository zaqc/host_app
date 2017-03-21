/*
 * TextDraw.h
 *
 *  Created on: Mar 13, 2017
 *      Author: zaqc
 */

#ifndef TEXTDRAW_H_
#define TEXTDRAW_H_
//----------------------------------------------------------------------------

#include <string>

#include <GLES2/gl2.h>
//----------------------------------------------------------------------------

class TextDraw {
protected:
	GLuint m_Prog;
	GLuint m_paramVertexPos;
	GLuint m_paramDrawColor;

	GLfloat m_R;
	GLfloat m_G;
	GLfloat m_B;
	GLfloat m_A;

	int m_LineCacheSize;
	GLfloat *m_LineVertex;
	GLfloat *m_LineColor;
	GLushort *m_LineNdx;
public:
	TextDraw();
	virtual ~TextDraw();

	void SetColor(GLfloat *aColor);
	void SetColor(GLfloat aR, GLfloat aG, GLfloat aB, GLfloat aA);

	void FillRect(int aX1, int aY1, int aX2, int aY2);
	void DrawLine(int aX1, int aY1, int aX2, int aY2);
	void DrawRect(int aX1, int aY1, int aX2, int aY2);

	void DrawGrid(int aX1, int aY1, int aX2, int aY2, bool aLogView, int aDelay = 0, int aAccum = 31);

	void BeginLines(void);
	void ShowLines(void);

	void FlushDraw(void);
};
//----------------------------------------------------------------------------

extern TextDraw *draw;
//----------------------------------------------------------------------------

#endif /* TEXTDRAW_H_ */
