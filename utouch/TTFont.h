/*
 * TTFont.h
 *
 *  Created on: Mar 20, 2017
 *      Author: zaqc
 */

#ifndef TTFONT_H_
#define TTFONT_H_
//----------------------------------------------------------------------------

#include <GLES2/gl2.h>
#include <ft2build.h>
#include FT_FREETYPE_H


class TTFont {
protected:
	GLuint m_Prog;
	GLuint m_paramVertexPos;
	GLuint m_paramTexturePos;
	GLuint m_paramFontColor;
	GLuint m_paramTexture;

	GLuint m_Text;

	FT_Library m_Library;
	FT_Face m_Face;
	FT_GlyphSlot  m_Slot;

public:
	TTFont();
	virtual ~TTFont();

	void Render(void);
};
//----------------------------------------------------------------------------

#endif /* TTFONT_H_ */
