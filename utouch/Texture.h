/*
 * This proprietary software may be used only as
 * authorised by a licensing agreement from ARM Limited
 * (C) COPYRIGHT 2013 ARM Limited
 * ALL RIGHTS RESERVED
 * The entire notice above must be reproduced on all authorised
 * copies and copies may only be made to the extent permitted
 * by a licensing agreement from ARM Limited.
 */

#ifndef TEXTURE_H
#define TEXTURE_H

#include <GLES2/gl2.h>

/**
 * \brief Loads a simple 3 x 3 static texture into OpenGL ES.
 * \return Returns the handle to the texture object.
 */
GLuint loadSimpleTexture();
void LoadTapeTxt(void);

void InitTexture(void);
void RenderToTxt(void);

#endif
