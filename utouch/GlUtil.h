/*
 * GlUtil.h
 *
 *  Created on: Feb 23, 2017
 *      Author: zaqc
 */

#ifndef GLUTIL_H_
#define GLUTIL_H_

#include <GLES2/gl2.h>

#define	JOY_UP			0xFF7F0000
#define	JOY_DN			0xFFFD0000
#define	JOY_LEFT		0xFFF70000
#define	JOY_RIGHT		0xFFDF0000

#define BTN_HOME		0xFFFB0000

#define BTN_OK			0xFFBF0000
#define BTN_CANCEL		0xFFEF0000

GLuint loadShader(GLenum shaderType, const char* shaderSource);
GLuint createProgram(const char* vertexSource, const char * fragmentSource);

#endif /* GLUTIL_H_ */
