/*
 * This proprietary software may be used only as
 * authorised by a licensing agreement from ARM Limited
 * (C) COPYRIGHT 2013 ARM Limited
 * ALL RIGHTS RESERVED
 * The entire notice above must be reproduced on all authorised
 * copies and copies may only be made to the extent permitted
 * by a licensing agreement from ARM Limited.
 */

/* [includeTextureDefinition] */
#include "Texture.h"

#include <GLES2/gl2ext.h>
#include <cstdio>
#include <cstdlib>

#include "Util.h"

GLuint txt_loadShader(GLenum shaderType, const char* shaderSource) {
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

GLuint txt_createProgram(const char* vertexSource, const char * fragmentSource) {
	GLuint vertexShader = txt_loadShader(GL_VERTEX_SHADER, vertexSource);
	if (vertexShader == 0) {
		return 0;
	}

	GLuint fragmentShader = txt_loadShader(GL_FRAGMENT_SHADER, fragmentSource);
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

static const char txt_VS[] = "attribute vec4 vertexPosition;\n"
		"attribute vec2 vertexTextureCord;\n"
		"varying vec2 textureCord;\n"
//        "uniform mat4 projection;\n"
//        "uniform mat4 modelView;\n"
		"void main()\n"
		"{\n"
//        "    gl_Position = projection * modelView * vertexPosition; \n"
		"    gl_Position = vertexPosition; \n"
		"    textureCord = vertexTextureCord; \n"
		"}\n";

static const char txt_FS[] = "precision mediump float;\n"
		"uniform sampler2D texture;\n"
		"varying vec2 textureCord;\n"
		"vec2 cc;\n"
		"vec4 c; \n"
		"void main()\n"
		"{\n"
		"    cc = vec2(textureCord.x, textureCord.y);\n"
		"    c = texture2D(texture, cc); \n"
		"    if(c.x > c.y) \n"
		"      if(c.z > c.x) \n"
		"        gl_FragColor = vec4(c.x, c.y, 1.0, 1.0); \n"
		"      else \n"
		"        gl_FragColor = vec4(1.0, c.y, c.z, 1.0); \n"
		"    else \n"
		"      if(c.y > c.z) \n"
		"        gl_FragColor = vec4(c.x, 1.0, c.z, 1.0); \n"
		"      else \n"
		"        gl_FragColor = vec4(c.x, c.y, 1.0, 1.0); \n"
//		"    cc.x = cc.x / 2.0f; \n"
//		"    cc.x = (cc.x > 1.0f) ? 1.0f : cc.x; \n"
//		"    cc.y = cc.y / 2.0f; \n"
//		"    cc.y = (cc.y > 1.0f) ? 1.0f : cc.y; \n"
		"    gl_FragColor = texture2D(texture, cc);\n"
//		"    gl_FragColor = vec4(textureCord.x, textureCord.y, 1.0, 1.0);\n" //texture2D(texture, textureCord);\n"
		"}\n";

GLuint glTxtProg;
GLuint txt_vertexLocation;
GLuint txt_samplerLocation;
//GLuint projectionLocation;
//GLuint modelViewLocation;
GLuint txt_textureCordLocation;
//GLuint textureId;

void InitTexture(void) {
	glTxtProg = txt_createProgram(txt_VS, txt_FS);
	if (!glTxtProg) {
		printf("Could not create program");
		exit(-1);
	}

	txt_vertexLocation = glGetAttribLocation(glTxtProg, "vertexPosition");
	txt_textureCordLocation = glGetAttribLocation(glTxtProg, "vertexTextureCord");
//    projectionLocation = glGetUniformLocation(glProgram, "projection");
//    modelViewLocation = glGetUniformLocation(glProgram, "modelView");
	txt_samplerLocation = glGetUniformLocation(glTxtProg, "texture");
}

GLuint fb_txt = 0;
GLuint fb = 0;
void RenderToTxt(void) {
	//glCopyTexImage2D()
	//glCopyTexSubImage2D()
	if (fb_txt == 0) {
		glGenTextures(1, &fb_txt);
		glBindTexture(GL_TEXTURE_2D, fb_txt);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		GLubyte t[256 * 256 * 4];
		for (int i = 0; i < 256 * 256; i++) {
			t[i * 4] = i;
			t[i * 4 + 1] = i;
			t[i * 4 + 2] = i;
			t[i * 4 + 3] = 255;
		}
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, t);
		//glGenerateMipmap(GL_TEXTURE_2D);
		//glBindTexture(GL_TEXTURE_2D, 0);
	}

	if (fb == 0)
		glGenFramebuffers(1, &fb);

	glBindFramebuffer(GL_FRAMEBUFFER, fb);

	// attach the texture to FBO color attachment point
	glFramebufferTexture2D(GL_FRAMEBUFFER,	// 1. fbo target: GL_FRAMEBUFFER
			GL_COLOR_ATTACHMENT0, 			// 2. attachment point
			GL_TEXTURE_2D,					// 3. tex target: GL_TEXTURE_2D
			fb_txt,							// 4. tex ID
			0);								// 5. mipmap level: 0(base)

	//glViewport(0, 0, 128, 128);
	glClearColor(0.75, 0.75, 0.75, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glDisable(GL_DEPTH_TEST);
	glViewport(0, 0, 256, 256);

	GLfloat v[] = { /* vertexes */
	-1.0f, -1.0f, 0.0f, /**/
	-1.0f, 1.0f, 0.0f, /**/
	1.0f, -1.0f, 0.0f, /**/
	1.0f, 1.0f, 0.0f };
//	GLfloat v[] = { /* vertexes */
//	0.0f, 0.0f, 0.0f, /**/
//	0.0f, 1.0f, 0.0f, /**/
//	1.0f, 0.0f, 0.0f, /**/
//	1.0f, 1.0f, 0.0f };

	GLfloat txc[] = { /* texture coordinate */
	0.0f, 0.0f, /**/
	0.0f, 1.0f, /**/
	1.0f, 0.0f, /**/
	1.0f, 1.0f };

	GLushort ndx[] = { 1, 0, 2, 1, 2, 3 };

	glUseProgram(glTxtProg);

//	glBindTexture(GL_TEXTURE_2D, fb_txt);
//	glActiveTexture(GL_TEXTURE0);

	glVertexAttribPointer(txt_vertexLocation, 3, GL_FLOAT, GL_FALSE, 0, v);
	glEnableVertexAttribArray(txt_vertexLocation);

	/* [enableAttributes] */
	glVertexAttribPointer(txt_textureCordLocation, 2, GL_FLOAT, GL_FALSE, 0, txc);
	glEnableVertexAttribArray(txt_textureCordLocation);
	//glUniformMatrix4fv(projectionLocation, 1, GL_FALSE,projectionMatrix);
	//glUniformMatrix4fv(modelViewLocation, 1, GL_FALSE, modelViewMatrix);

	/* Set the sampler texture unit to 0. */
	glUniform1i(txt_samplerLocation, 2);
	/* [enableAttributes] */
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, ndx);

	glBindTexture(GL_TEXTURE_2D, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, fb_txt);

	//glBindTexture(GL_TEXTURE_2D, textureId);
	glGenerateMipmap(GL_TEXTURE_2D);
	//glBindTexture(GL_TEXTURE_2D, 0);
}

static int nn = 0;
GLuint txt_id = 0;
GLubyte *txt = NULL;
GLubyte *txt_2 = NULL;
GLuint txt_id_2 = 0;
void LoadTapeTxt(void) {
	if (!txt)
		txt = new GLubyte[800 * 480 * 4];

	if (!txt_2) {
		txt_2 = new GLubyte[256 * 256 * 4];
		unsigned char *dst = txt_2;
		for (int j = 0; j < 256; j++) {
			char *line = (char *) row_pointers[j];
			memcpy(dst, line, 1024);
			//memset(dst, 255, 1024);
			dst += 1024;
		}
	}

//	for (int i = 0; i < 100 * 100 * 4; i += 4) {
//		txt[i] = nn;
//		txt[i + 1] = nn;
//		txt[i + 2] = nn;
//		txt[i + 3] = 255;
//	}

	for (int j = 0; j < 128; j++) {
		for (int i = 0; i < 128; i++) {
			int n = (i + j * 128) * 4;
			int kk = 128;
			if (i > 32 && i < 96 && j > 32 && j < 96)
				kk = 64;
			txt[n] = kk;
			txt[n + 1] = kk;
			txt[n + 2] = kk;
			txt[n + 3] = 255;
		}
	}

	nn = 192;

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	if (txt_id == 0)
		glGenTextures(1, &txt_id);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, txt_id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 128, 128, 0, GL_RGBA, GL_UNSIGNED_BYTE, txt);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	if (txt_id_2 == 0)
		glGenTextures(1, &txt_id_2);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, txt_id_2);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, txt_2);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glGenerateMipmap(GL_TEXTURE_2D);

//	glActiveTexture(GL_TEXTURE1);
//	glCopyTexSubImage2D(txt_2, 0, 0, 0, 0, 0, 64, 64);

	//glActiveTexture(GL_TEXTURE2);
	//delete[] txt;
}

GLuint loadSimpleTexture() {
	/* Texture Object Handle. */
	GLuint textureId;

	/* 3 x 3 Image,  R G B A Channels RAW Format. */
//    GLubyte pixels[9 * 4] =
//    {
//            18,  140, 171, 132, /* Some Colour Bottom Left. */
//            143, 143, 143, 132, /* Some Colour Bottom Middle. */
//            255, 255, 255, 132, /* Some Colour Bottom Right. */
//            255, 255, 0,   132, /* Yellow Middle Left. */
//            0,   255, 255, 132, /* Some Colour Middle. */
//            255, 0,   255, 132, /* Some Colour Middle Right. */
//            255, 0,   0,   132, /* Red Top Left. */
//            0,   255, 0,   132, /* Green Top Middle. */
//            0,   0,   255, 132, /* Blue Top Right. */
//    };
	GLubyte pixels[9 * 4] = { 0, 255, 0, 32, /* Some Colour Bottom Left. */
	0, 255, 0, 32, /* Some Colour Bottom Middle. */
	0, 255, 0, 32, /* Some Colour Bottom Right. */
	0, 255, 0, 32, /* Yellow Middle Left. */
	128, 128, 0, 100, /* Some Colour Middle. */
	0, 255, 0, 32, /* Some Colour Middle Right. */
	0, 255, 0, 32, /* Red Top Left. */
	0, 255, 0, 32, /* Green Top Middle. */
	0, 255, 0, 32, /* Blue Top Right. */
	};

	/* [includeTextureDefinition] */

	/* [placeTextureInMemory] */
	/* Use tightly packed data. */
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	/* Generate a texture object. */
	glGenTextures(1, &textureId);

	/* Activate a texture. */
	glActiveTexture(GL_TEXTURE0);

	/* Bind the texture object. */
	glBindTexture(GL_TEXTURE_2D, textureId);

	/* Load the texture. */
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 3, 3, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

	/* Set the filtering mode. */
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	return textureId;
}
/* [placeTextureInMemory] */
