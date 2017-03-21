/*
 * TTFont.h
 *
 *  Created on: Mar 20, 2017
 *      Author: zaqc
 */

#ifndef TTFONT_H_
#define TTFONT_H_
//----------------------------------------------------------------------------

#include <string>

#include <GLES2/gl2.h>
#include <ft2build.h>
#include FT_FREETYPE_H

struct TTString {
	std::string m_Str;
	GLuint m_Text;
	int m_W;
	int m_H;
	GLfloat m_V[12];
	GLfloat m_T[8];
	GLushort m_Ndx[6];

	void Init(int aW, int aH) {
		m_W = aW;
		m_H = aH;
		float x1 = 0;
		float y1 = 0;
		float x2 = aW;
		float y2 = aH;
		GLfloat v[] = { /* vertexes */
		x1, y1, 0.0f, /**/
		x1, y2, 0.0f, /**/
		x2, y1, 0.0f, /**/
		x2, y2, 0.0f };
		memcpy(m_V, v, sizeof(GLfloat) * 12);

		GLfloat t[] = { /* texture coordinate */
		0.0f, 0.0f, /**/
		0.0f, 1.0f, /**/
		1.0f, 0.0f, /**/
		1.0f, 1.0f };
		memcpy(m_T, t, sizeof(GLfloat) * 8);

		GLushort ndx[] = { 1, 0, 2, 1, 2, 3 };
		memcpy(m_Ndx, ndx, sizeof(GLushort) * 6);
	}
};
//----------------------------------------------------------------------------

class TTFont {
protected:
	GLuint m_Prog;
	GLuint m_paramVertexPos;
	GLuint m_paramTexturePos;
	GLuint m_paramFontColor;
	GLuint m_paramTexture;
	GLuint m_paramOffsetXY;

	GLuint m_Text;

	FT_Library m_Library;
	FT_Face m_Face;

	TTString m_TTS;
	GLfloat m_R;
	GLfloat m_G;
	GLfloat m_B;
public:
	TTFont();
	virtual ~TTFont();

	int GetStringWidth(std::wstring aStr);
	int GetStringHeight(std::wstring aStr);

	TTString PrepTTString(std::wstring aStr);

	void SetColor(GLfloat aR, GLfloat aG, GLfloat aB);
	void Render(int aX, int aY, TTString *aTTS);
};
//----------------------------------------------------------------------------

#endif /* TTFONT_H_ */
