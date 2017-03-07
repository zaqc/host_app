//---------------------------------------------------------------------------
#ifndef OscillatorGraphH
#define OscillatorGraphH
//---------------------------------------------------------------------------

#include "Oscillator.h"
#include "glParams.h"
//#include "ChanelDeffects.h"
//---------------------------------------------------------------------------

class OscillatorGraph : public TOscillator
{
	std::vector<Vertex3D> oscillatorVerts,
		linesVerts;
	std::vector<UINT> oscillatorCols,
		linesCols;
	GlParams *Graph;


public:
	UINT color;

	OscillatorGraph(GlParams *inGraph, RailFigure *inRail);
	virtual ~OscillatorGraph();

	virtual void generate_reflections();

	void ClearGraph();
    void SetGraph();
	void set3dLines();
	void set3dOscillators();
	void draw3dLines();
	void draw3dOscillators();

};
//---------------------------------------------------------------------------

#endif
