//---------------------------------------------------------------------------
#pragma hdrstop

#include "glParams.h"
//---------------------------------------------------------------------------

void GlParams::SetProgram(GLuint in_prog_ID)
{
	prog_ID = in_prog_ID;

	AttrArrVert = glGetAttribLocation(prog_ID, "vertexPosition");
	AttrArrColor = glGetAttribLocation(prog_ID, "vColor");
	
	projection2d = 1.f;
	projection3d = 0.007f;
}

void GlParams::SetCube(Vertex3D *VertV, UINT *ColV, float *len, float *pos, UINT color)
{
	ColV[0] = ColV[1] = ColV[2] = ColV[3] = ColV[4] = ColV[5] = ColV[6] = 
				ColV[7] = ColV[8] = ColV[9] = ColV[10] = ColV[11] = ColV[12] = ColV[13] = color;
	VertV[0].x = projection3d * (pos[2] - len[2]);
	VertV[1].x = projection3d * (pos[2] + len[2]);
	VertV[0].y = VertV[1].y = projection3d * (pos[1] + len[1]);
	VertV[0].z = VertV[1].z = projection3d * (pos[0] + len[0]);

	VertV[2] = VertV[7] = VertV[8] = VertV[0];
	VertV[3] = VertV[4] = VertV[5] = VertV[6] = VertV[1];
	VertV[2].z = VertV[3].z = projection3d * (pos[0] - len[0]);

	VertV[4].y = projection3d * (pos[1] - len[1]);
	VertV[4].z = VertV[2].z;
	VertV[6].y = VertV[4].y;
	VertV[9] = VertV[2];
	VertV[8].y = VertV[4].y;

	VertV[10] = VertV[2];
	VertV[11] = VertV[4];
	VertV[10].y = VertV[4].y;

	VertV[12] = VertV[8];
	VertV[13] = VertV[6];
}

void GlParams::SetCubeWithInd(Vertex3D *VertV, UINT *ColV, GLushort *IndV, short IndShift, float *len, float *pos, UINT color)
{
	VertV[3].x = VertV[0].x = projection3d * (pos[2] - len[2]);
	VertV[2].x = VertV[1].x = projection3d * (pos[2] + len[2]);
	VertV[1].z = VertV[0].z = projection3d * (pos[0] - len[0]);
	VertV[3].z = VertV[2].z = projection3d * (pos[0] + len[0]);
	VertV[3].y = VertV[2].y = VertV[1].y = VertV[0].y = projection3d * (pos[1] + len[1]);
	ColV[3] = ColV[2] = ColV[1] = ColV[0] = color;

	memcpy(&VertV[4], VertV, 4 * sizeof(Vertex3D));
	memcpy(&ColV[4], ColV, 4 * sizeof(Vertex3D));
	VertV[7].y = VertV[6].y = VertV[5].y = VertV[4].y = projection3d * (pos[1] - len[1]);

	IndV[0] = 2 + IndShift;
	IndV[1] = 1 + IndShift;
	IndV[2] = 0 + IndShift;
	IndV[3] = 2 + IndShift;
	IndV[4] = 0 + IndShift;
	IndV[5] = 3 + IndShift;

	IndV[6] = 7 + IndShift;
	IndV[7] = 3 + IndShift;
	IndV[8] = 0 + IndShift;
	IndV[9] = 7 + IndShift;
	IndV[10] = 0 + IndShift;
	IndV[11] = 4 + IndShift;

	IndV[12] = 4 + IndShift;
	IndV[13] = 0 + IndShift;
	IndV[14] = 1 + IndShift;
	IndV[15] = 4 + IndShift;
	IndV[16] = 1 + IndShift;
	IndV[17] = 5 + IndShift;

	IndV[18] = 5 + IndShift;
	IndV[19] = 1 + IndShift;
	IndV[20] = 2 + IndShift;
	IndV[21] = 5 + IndShift;
	IndV[22] = 2 + IndShift;
	IndV[23] = 6 + IndShift;

	IndV[24] = 6 + IndShift;
	IndV[25] = 2 + IndShift;
	IndV[26] = 3 + IndShift;
	IndV[27] = 6 + IndShift;
	IndV[28] = 3 + IndShift;
	IndV[29] = 7 + IndShift;

	IndV[30] = 7 + IndShift;
	IndV[31] = 6 + IndShift;
	IndV[32] = 5 + IndShift;
	IndV[33] = 7 + IndShift;
	IndV[34] = 5 + IndShift;
	IndV[35] = 4 + IndShift;
}

void GlParams::DrawCube(int start_pos)
{
	glDrawArrays(GL_TRIANGLE_STRIP, start_pos, CUBE_VERTEX_QUANT - 2);
}

