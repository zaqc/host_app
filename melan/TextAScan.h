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

class TextAScan {
protected:
	GLuint m_Prog;
	GLuint m_paramVertexPos;
	GLuint m_paramVertexColor;
public:
	TextAScan();
	virtual ~TextAScan();

	void DrawLine(int aX1, int aY1, int aX2, int aY2);
	void FillRect(int aX1, int aY1, int aX2, int aY2);

	void DrawBuf(int aX1, int aY1, int aX2, int aY2, unsigned char *aBuf, int aSize);
};
//----------------------------------------------------------------------------

#endif /* UTOUCH_TEXTASCAN_H_ */
