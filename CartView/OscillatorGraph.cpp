//---------------------------------------------------------------------------
#pragma hdrstop
#include "OscillatorGraph.h"
//---------------------------------------------------------------------------

OscillatorGraph::OscillatorGraph(GlParams *inGraph, RailFigure *inRail)
			: TOscillator(inRail)
			, Graph(inGraph)
{
    color = 0xFFFF0000;
}

OscillatorGraph::~OscillatorGraph()
{
    ClearGraph();
}

void OscillatorGraph::ClearGraph()
{
	oscillatorVerts.clear();
	linesVerts.clear();
	oscillatorCols.clear();
	linesCols.clear();
}

void OscillatorGraph::generate_reflections()
{
	TOscillator::generate_reflections();
	SetGraph();
}

void OscillatorGraph::SetGraph()
{
	set3dLines();
	set3dOscillators();
}

void OscillatorGraph::set3dLines()
{
	linesVerts.clear();
	linesCols.clear();
	if(!Graph)
		return;
	linesVerts.resize(reflect_count + 1);
	linesCols.resize(reflect_count + 1);
	for(int reflect = 0; reflect <= reflect_count; ++reflect)
	{
		linesVerts[reflect].x = Graph->projection3d * oscill_point[reflect].z;
		linesVerts[reflect].y = Graph->projection3d * oscill_point[reflect].y;
		linesVerts[reflect].z = Graph->projection3d * oscill_point[reflect].x;
		linesCols[reflect] = color | 0xFF000000;
	}
}

void OscillatorGraph::set3dOscillators()
{
	if(!reflect_count || !Graph || !rail)
		return;
	float len[] = { 5, 10, 5 },
		pos[] = { oscill_point[0].x, oscill_point[0].y + 10, oscill_point[0].z };
	oscillatorVerts.resize(CUBE_VERTEX_QUANT);
	oscillatorCols.resize(CUBE_VERTEX_QUANT);
	Graph->SetCube(&oscillatorVerts[0], &oscillatorCols[0], len, pos, color | 0x88000000);
}

void OscillatorGraph::draw3dLines()
{
	if (linesVerts.size())
	{
		glEnableVertexAttribArray(Graph->AttrArrVert);
		glVertexAttribPointer(Graph->AttrArrVert, 3, GL_FLOAT, GL_FALSE, 0, &linesVerts[0]);
		glEnableVertexAttribArray(Graph->AttrArrColor);
		glVertexAttribPointer(Graph->AttrArrColor, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, &linesCols[0]);
		
		//glDrawArrays(GL_LINE_STRIP, 0, begin_work_reflect);
		glDrawArrays(GL_LINE_STRIP, 0, reflect_count + 1);
	}
}

void OscillatorGraph::draw3dOscillators()
{
	if(oscillatorVerts.size())
	{
		glEnableVertexAttribArray(Graph->AttrArrVert);
		glVertexAttribPointer(Graph->AttrArrVert, 3, GL_FLOAT, GL_FALSE, 0, &oscillatorVerts[0]);
		glEnableVertexAttribArray(Graph->AttrArrColor);
		glVertexAttribPointer(Graph->AttrArrColor, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, &oscillatorCols[0]);
		DRAWCUBE(0);
	}
}

