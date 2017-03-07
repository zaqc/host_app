//---------------------------------------------------------------------------
#ifndef glParamsH
#define glParamsH
//---------------------------------------------------------------------------

#include "myMath.h"
#include <string>
#include <vector>

#include "ShadersManager.h"
//---------------------------------------------------------------------------

#define CUBE_VERTEX_QUANT 14
//#define CUBE_VERTEX_QUANT 36
#define CUBE_MIN_VERTEX 8
#define CUBE_IND_QUANT 36
//---------------------------------------------------------------------------

class Vertex3D {
public:
	GLfloat x, y, z;
};

class GlParams
{
	/*TPointF lock_pos, cur_pos;
	bool is_fullScreen;
	int mouse_down;
	ULONG last_time, fvf;
	float deb_float;*/

	static int total_memory_allocated;

public:
	float projection2d, projection3d;
	//D3DCOLOR backgroundcolor;
	GLuint prog_ID;
	GLuint AttrArrVert,
		AttrArrColor;

	void SetProgram(GLuint in_prog_ID);
	void SetCube(Vertex3D *VertV, UINT *ColV, float *len, float *pos, UINT color);// = 0x80);
	void SetCubeWithInd(Vertex3D *VertV, UINT *ColV, GLushort *IndV, short IndShift, float *len, float *pos, UINT color);
	void DrawCube(int start_pos = 0);
};
//---------------------------------------------------------------------------

#define DRAWCUBE(pos) glDrawArrays(GL_TRIANGLE_STRIP, pos, CUBE_VERTEX_QUANT - 2);
//---------------------------------------------------------------------------

#endif
