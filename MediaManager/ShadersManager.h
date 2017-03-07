#ifndef SHADERSMANAGER_H_
#define SHADERSMANAGER_H_
#include <cstdio>
#include <stdlib.h>

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#include "LogHelper.h"

class ShadersManager {
public:
	static GLuint compileShader(GLenum shaderType, const char* pSource);
	static GLuint createProgram(const char* pVertexSource, const char* pFragmentSource);
};

#endif /* SHADERSMANAGER_H_ */
