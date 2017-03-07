/*
 * TextAScan.h
 *
 *  Created on: Mar 6, 2017
 *      Author: zaqc
 */

#ifndef UTOUCH_TEXTASCAN_H_
#define UTOUCH_TEXTASCAN_H_
//----------------------------------------------------------------------------

#include <GLES2/gl2.h>
#include "DScopeStream.h"
//----------------------------------------------------------------------------

class TextAScan;
//----------------------------------------------------------------------------

class IntMenuItem {
protected:
	TextAScan *m_TextAScan;
	char *m_Caption;
	int m_Val;
	int m_Min;
	int m_Max;
	unsigned int m_PrevKey;
	int m_HoldTickCount;
public:
	IntMenuItem(TextAScan *aTextAScan, char *aCaption, int aVal, int aMin, int aMax);
	virtual ~IntMenuItem();

	int GetWidth(void);

	void ProcessButton(unsigned int &aKey);
	void Render(int aX, int aY);
};
//----------------------------------------------------------------------------

class MenuAScan {
protected:
	TextAScan *m_TextAScan;
	bool m_Log;	// or VGA
	int m_LogLevel;
	int m_Amp2;
	int m_Vrc;
public:
	MenuAScan(TextAScan *aTextAScan);
	virtual ~MenuAScan();

	void ProcessButton(unsigned int &aKey);
	void Render(int aX, int aY);
};
//----------------------------------------------------------------------------

class TextAScan {
protected:
	GLuint m_Prog;
	GLuint m_paramVertexPos;
	GLuint m_paramVertexColor;

	IntMenuItem *m_Amp1;
public:
	TextAScan();
	virtual ~TextAScan();

	void DrawLine(int aX1, int aY1, int aX2, int aY2);
	void FillRect(int aX1, int aY1, int aX2, int aY2, GLfloat *aColor = NULL);

	void ProcessButton(unsigned int &aKey);
	void DrawBuf(int aX1, int aY1, int aX2, int aY2, unsigned char *aBuf, int aSize);
};
//----------------------------------------------------------------------------

#endif /* UTOUCH_TEXTASCAN_H_ */
