//---------------------------------------------------------------------------
#ifndef RailGraphicH
#define RailGraphicH
//---------------------------------------------------------------------------

#include "RailFigure.h"
#include "glParams.h"
//---------------------------------------------------------------------------
#define __fastcall
class TRailGraphic: public RailFigure
{
	int graph_type;
	int railAlpha, profile_lines_quant;
	int Rail3dPos, Rail3dQuant, Plain3dPos, Plain3dQuant, RailLinesPos, RailLinesQuant, Profiles3dQuant;
	std::vector<int> ProfilesQuantArr, ProfilesQuant3dArr;
    
	std::vector<float> ProfileArrs;
	float length2d, length3d, beetween_profiles;

	std::vector<std::vector<Vertex3D> > Profiles3d1;
	std::vector<Vertex3D> Profiles3d2;
	std::vector<Vertex3D> Rail3dVB;
	std::vector<UINT> Rail3dColor, Profiles3dColor;
	std::vector<GLushort> RailLinesInd;


	void  setRail3d(std::vector<Vertex3D> &VertexArr, std::vector<UINT> &ColorArr);
	void  setRailLines3d(std::vector<Vertex3D> &VertexArr, std::vector<UINT> &ColorArr);
	void  setPlain3d(std::vector<Vertex3D> &VertexArr, std::vector<UINT> &ColorArr);
    void  GenerateProfile3d(std::vector<Vertex3D> &VertexArr, float *arrF, int &quant, float z_pos);

	void  drawRail3d();
	void  drawRailLines3d();
	void  drawPlain3d();
	void  drawProfile3d();

public:
	GlParams *graphParams;

	int profiles_quant;
    bool profile_filter;

	int xcenter_2d, xcent_front, ymin, ymax, y_dist, profile_visualisation_type;
    double ballance_point;

	TRailGraphic();
	~TRailGraphic();
	void __fastcall clear_graph();
	void __fastcall SetProfileQuant(int quant);
	void __fastcall Inicialize(int in_type, int in_graph_type, float length = 500);

    void __fastcall SetProfile3D(void *arr, int quant, int change_pos);

	void __fastcall setTotalRail3d();
	void __fastcall drawTotalRail3d();
    void __fastcall ElvaluateProfiles();
};
//---------------------------------------------------------------------------

#endif


