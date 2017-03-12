/*
 * This proprietary software may be used only as
 * authorised by a licensing agreement from ARM Limited
 * (C) COPYRIGHT 2013 ARM Limited
 * ALL RIGHTS RESERVED
 * The entire notice above must be reproduced on all authorised
 * copies and copies may only be made to the extent permitted
 * by a licensing agreement from ARM Limited.
 */

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#include <cstdio>
#include <cstdlib>
#include <cmath>

#include "Matrix.h"
#include "Texture.h"
#include "GlUtil.h"

/* [shaders] */
static const char glVertexShader[] =
        "attribute vec4 vertexPosition;\n"
        "attribute vec2 vertexTextureCord;\n"
        "varying vec2 textureCord;\n"
        "uniform mat4 projection;\n"
        "uniform mat4 modelView;\n"
        "varying float dpt;\n"
		"varying vec4 my_color;\n"
		"vec4 pos;\n"
        "void main()\n"
        "{\n"
		"    dpt = 1.0;\n"
        "    pos = projection * modelView * vertexPosition;\n"
        "    textureCord = vertexTextureCord;\n"
//		"    my_color = gl_Color;\n"
		"    dpt = (pos.z) / 10.0; \n"
		"    gl_Position = pos; \n"
//		"    clamp(dpt, 0.0, 1.0); \n"
        "}\n";

static const char glFragmentShader[] =
        "precision mediump float;\n"
        "uniform sampler2D texture;\n"
        "varying vec2 textureCord;\n"
        "varying float dpt;\n"
		"varying vec4 my_color;\n"
        "void main()\n"
        "{\n"
//		"    dpt=1.0;\n"
//		"    gl_FragColor = vec4(textureCord.x, textureCord.x, textureCord.x, 1.0);\n" //
		"    gl_FragColor = vec4(texture2D(texture, textureCord).xyz, dpt);\n"
        "}\n";
/* [shaders] */

GLuint glProgram;
GLuint vertexLocation;
GLuint samplerLocation;
GLuint projectionLocation;
GLuint modelViewLocation;
GLuint textureCordLocation;
GLuint textureId;

float projectionMatrix[16];
float modelViewMatrix[16];
float angle = 0;

/* [setupGraphicsUpdate] */
bool setupGraphics(int width, int height)
{
    glProgram = createProgram(glVertexShader, glFragmentShader);

    if (!glProgram)
    {
        printf ("Could not create program");
        return false;
    }

    vertexLocation = glGetAttribLocation(glProgram, "vertexPosition");
    textureCordLocation = glGetAttribLocation(glProgram, "vertexTextureCord");
    projectionLocation = glGetUniformLocation(glProgram, "projection");
    modelViewLocation = glGetUniformLocation(glProgram, "modelView");
    samplerLocation = glGetUniformLocation(glProgram, "texture");

    /* Setup the perspective. */
    matrixPerspective(projectionMatrix, 45, (float)width / (float)height, 0.1f, 100);

    /* Load the Texture. */
    textureId = loadSimpleTexture();

    InitTexture();
	LoadTapeTxt();

    if(textureId == 0)
    {
        return false;
    }
    else
    {
        return true;
    }

}
/* [setupGraphicsUpdate] */
/* [verticesAndTexture] */
GLfloat cubeVertices[] = {-1.0f,  1.0f, -1.0f, /* Back. */
                           1.0f,  1.0f, -1.0f,
                          -1.0f, -1.0f, -1.0f,
                           1.0f, -1.0f, -1.0f,
                          -1.0f,  1.0f,  1.0f, /* Front. */
                           1.0f,  1.0f,  1.0f,
                          -1.0f, -1.0f,  1.0f,
                           1.0f, -1.0f,  1.0f,
                          -1.0f,  1.0f, -1.0f, /* Left. */
                          -1.0f, -1.0f, -1.0f,
                          -1.0f, -1.0f,  1.0f,
                          -1.0f,  1.0f,  1.0f,
                           1.0f,  1.0f, -1.0f, /* Right. */
                           1.0f, -1.0f, -1.0f,
                           1.0f, -1.0f,  1.0f,
                           1.0f,  1.0f,  1.0f,
                          -1.0f, 1.0f, -1.0f, /* Top. */
                          -1.0f, 1.0f,  1.0f,
                           1.0f, 1.0f,  1.0f,
                           1.0f, 1.0f, -1.0f,
                          -1.0f, - 1.0f, -1.0f, /* Bottom. */
                          -1.0f,  -1.0f,  1.0f,
                           1.0f, - 1.0f,  1.0f,
                           1.0f,  -1.0f, -1.0f
                         };

GLfloat textureCords[] = { 1.0f, 1.0f, /* Back. */
                           0.0f, 1.0f,
                           1.0f, 0.0f,
                           0.0f, 0.0f,
                           0.0f, 1.0f, /* Front. */
                           1.0f, 1.0f,
                           0.0f, 0.0f,
                           1.0f, 0.0f,
                           0.0f, 1.0f, /* Left. */
                           0.0f, 0.0f,
                           1.0f, 0.0f,
                           1.0f, 1.0f,
                           1.0f, 1.0f, /* Right. */
                           1.0f, 0.0f,
                           0.0f, 0.0f,
                           0.0f, 1.0f,
                           0.0f, 1.0f, /* Top. */
                           0.0f, 0.0f,
                           1.0f, 0.0f,
                           1.0f, 1.0f,
                           0.0f, 0.0f, /* Bottom. */
                           0.0f, 1.0f,
                           1.0f, 1.0f,
                           1.0f, 0.0f
};
/* [verticesAndTexture] */

GLushort indicies[] = {0, 3, 2, 0, 1, 3, 4, 6, 7, 4, 7, 5,  8, 9, 10, 8, 11, 10, 12, 13, 14, 15, 12, 14, 16, 17, 18, 16, 19, 18, 20, 21, 22, 20, 23, 22};

void renderFrame()
{
    glClearColor(0.0f, 0.0f, 0.2f, 1.0f);
    glClear (GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    LoadTapeTxt();
    RenderToTxt();

    glDisable(GL_DEPTH_TEST);
    glViewport(0, 0, 800, 480);

    matrixIdentityFunction(modelViewMatrix);

    matrixRotateX(modelViewMatrix, angle);
    matrixRotateY(modelViewMatrix, angle);

    matrixTranslate(modelViewMatrix, 0.0f, 0.0f, -5.0f);

    glUseProgram(glProgram);
    glVertexAttribPointer(vertexLocation, 3, GL_FLOAT, GL_FALSE, 0, cubeVertices);
    glEnableVertexAttribArray(vertexLocation);

    /* [enableAttributes] */
    glVertexAttribPointer(textureCordLocation, 2, GL_FLOAT, GL_FALSE, 0, textureCords);
    glEnableVertexAttribArray(textureCordLocation);
    glUniformMatrix4fv(projectionLocation, 1, GL_FALSE,projectionMatrix);
    glUniformMatrix4fv(modelViewLocation, 1, GL_FALSE, modelViewMatrix);

    /* Set the sampler texture unit to 0. */
    glUniform1i(samplerLocation, 3);
    /* [enableAttributes] */
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, indicies);
    glDrawArrays(GL_LINES, 0, 24);

    angle += .25;
    if (angle > 360)
    {
        angle -= 360;
    }
}
