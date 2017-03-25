/*
 * GlUtil.cpp
 *
 *  Created on: Feb 23, 2017
 *      Author: zaqc
 */

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <cstdio>
#include <cstdlib>
#include <cmath>

#include <iostream>
#include <fstream>
#include <ctype.h>
//----------------------------------------------------------------------------

GLuint loadShader(GLenum shaderType, const char* shaderSource);
//----------------------------------------------------------------------------

int loadShaderFromFile(GLenum shaderType, const char* filename) {
	char *ShaderSource;
	std::ifstream file;
	file.open(filename, std::ios::in); // opens as ASCII!
	if (!file)
		return -1;

	file.seekg(0, file.end);
	uint64_t len = file.tellg();

	file.seekg(0, file.beg);

	if (len == 0)
		return -2;   // Error: Empty File

	ShaderSource = new char[len + 1];
	if (*ShaderSource == 0)
		return -3;   // can't reserve memory

	ShaderSource[len] = 0;

	unsigned int i = 0;
	while (file.good()) {
		ShaderSource[i] = file.get();       // get character from file.
		if (!file.eof())
			i++;
	}

	ShaderSource[i] = 0;  // 0-terminate it at the correct position

	GLuint sh = loadShader(shaderType, ShaderSource);

	file.close();
	delete[] ShaderSource;

	return sh; // No Error
}
//----------------------------------------------------------------------------

GLuint loadShader(GLenum shaderType, const char* shaderSource) {
	GLuint shader = glCreateShader(shaderType);
	if (shader != 0) {
		glShaderSource(shader, 1, &shaderSource, NULL);
		glCompileShader(shader);

		GLint compiled = 0;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);

		if (compiled != GL_TRUE) {
			GLint infoLen = 0;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);

			if (infoLen > 0) {
				char * logBuffer = (char*) malloc(infoLen);

				if (logBuffer != NULL) {
					glGetShaderInfoLog(shader, infoLen, NULL, logBuffer);
					printf("Could not Compile Shader %d:\n%s\n", shaderType, logBuffer);
					free(logBuffer);
					logBuffer = NULL;
				}

				glDeleteShader(shader);
				shader = 0;
			}
		}
	}

	return shader;
}
//----------------------------------------------------------------------------

GLuint loadProgram(const char* vs_name, const char * fs_name) {
	GLuint vertexShader = loadShaderFromFile(GL_VERTEX_SHADER, vs_name);
	if (vertexShader < 0) {
		printf("Could not load Vertex Shader... [%s]\n", vs_name);
		return 0;
	}

	GLuint fragmentShader = loadShaderFromFile(GL_FRAGMENT_SHADER, fs_name);
	if (fragmentShader < 0) {
		printf("Could not load Fragment Shader... [%s]\n", fs_name);
		return 0;
	}

	GLuint program = glCreateProgram();

	if (program != 0) {
		glAttachShader(program, vertexShader);
		glAttachShader(program, fragmentShader);
		glLinkProgram(program);
		GLint linkStatus = GL_FALSE;
		glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);

		if (linkStatus != GL_TRUE) {
			GLint bufLength = 0;

			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufLength);

			if (bufLength > 0) {
				char* logBuffer = (char*) malloc(bufLength);

				if (logBuffer != NULL) {
					glGetProgramInfoLog(program, bufLength, NULL, logBuffer);
					printf("Could not link program:\n%s\n", logBuffer);
					free(logBuffer);
					logBuffer = NULL;
				}
			}
			glDeleteProgram(program);
			program = 0;
		}
	}
	return program;
}
//----------------------------------------------------------------------------


GLuint createProgram(const char* vertexSource, const char * fragmentSource) {
	GLuint vertexShader = loadShader(GL_VERTEX_SHADER, vertexSource);
	if (vertexShader == 0) {
		return 0;
	}

	GLuint fragmentShader = loadShader(GL_FRAGMENT_SHADER, fragmentSource);
	if (fragmentShader == 0) {
		return 0;
	}

	GLuint program = glCreateProgram();

	if (program != 0) {
		glAttachShader(program, vertexShader);
		glAttachShader(program, fragmentShader);
		glLinkProgram(program);
		GLint linkStatus = GL_FALSE;
		glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);

		if (linkStatus != GL_TRUE) {
			GLint bufLength = 0;

			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufLength);

			if (bufLength > 0) {
				char* logBuffer = (char*) malloc(bufLength);

				if (logBuffer != NULL) {
					glGetProgramInfoLog(program, bufLength, NULL, logBuffer);
					printf("Could not link program:\n%s\n", logBuffer);
					free(logBuffer);
					logBuffer = NULL;
				}
			}
			glDeleteProgram(program);
			program = 0;
		}
	}
	return program;
}
//----------------------------------------------------------------------------
