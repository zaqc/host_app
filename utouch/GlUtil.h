/*
 * GlUtil.h
 *
 *  Created on: Feb 23, 2017
 *      Author: zaqc
 */

#ifndef GLUTIL_H_
#define GLUTIL_H_

#include <GLES2/gl2.h>

GLuint loadShader(GLenum shaderType, const char* shaderSource);
GLuint createProgram(const char* vertexSource, const char * fragmentSource);

#endif /* GLUTIL_H_ */
