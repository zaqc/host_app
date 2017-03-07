//---------------------------------------------------------------------------
//#include "stdafx.h"

#include "RailGraphicVC.h"
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//
//  TRailGraphic
//
//---------------------------------------------------------------------------
TRailGraphic::TRailGraphic()
			: graphParams(NULL)
{
	profiles_quant = 0;
	profile_filter = false;
	//memset(Profiles3d1, 0, PROFILES_QUANT * sizeof(float*));
	//memset(ProfilesQuantArr, 0, PROFILES_QUANT * sizeof(int));

	profile_visualisation_type = VIS_TRIANGLES;
	profile_lines_quant = 5;
    beetween_profiles = 10.0f;
	xcenter_2d = 150;
	xcent_front = 50;
	ymin = 130;
	y_dist = 0;
	Rail3dPos = -1;
	Rail3dQuant = 0;
	RailLinesPos = -1;
	Plain3dPos = -1;

	length3d = length2d = 1000;
	railAlpha = 10;
	Profiles3dQuant = 0;
}

TRailGraphic::~TRailGraphic()
{
	clear_graph();
}

void __fastcall TRailGraphic::clear_graph()
{
	Rail3dPos = -1;
	Rail3dQuant = 0;
	RailLinesPos = -1;
	Plain3dPos = -1;
	
	ProfileArrs.clear();
	Profiles3dColor.clear();
	ProfilesQuantArr.clear();
	ProfilesQuant3dArr.clear();
	Rail3dVB.clear();
	Rail3dColor.clear();
	RailLinesInd.clear();
}

void __fastcall TRailGraphic::setTotalRail3d()
{
	if (!(graph_type & RAIL_3D_RT)) return;

	setRail3d(Rail3dVB, Rail3dColor);
	setRailLines3d(Rail3dVB, Rail3dColor);
	//setPlain3d(Rail3d_arr);
}

void __fastcall TRailGraphic::setRail3d(std::vector<Vertex3D> &VertexArr, std::vector<UINT> &ColorArr)
{
	Rail3dQuant = (graph_points.size() * 4 + 2) * 2;
	Rail3dPos = VertexArr.size();
	VertexArr.resize(Rail3dPos + Rail3dQuant);
	ColorArr.resize(Rail3dPos + Rail3dQuant);
	Vertex3D *Rail3d1 = &VertexArr[Rail3dPos],
		Vertex;
	UINT *ColorArr1 = &ColorArr[Rail3dPos];

	// Формирование буфера вершин
	int numb = 0;
	for(UINT i = 0; i < graph_points.size(); ++i)
	{
		Vertex.x = -graphParams->projection3d * length3d;
		Vertex.y = graphParams->projection3d * graph_points[i].y;
		Vertex.z = graphParams->projection3d * graph_points[i].x;//0x01
		Rail3d1[numb++] = Vertex;
		ColorArr1[numb] = int(0x18 * graph_points[i].x / 55 + 0x00) * 0x2000000;//0x040404;
		
		Vertex.x = -Vertex.x;
		Rail3d1[numb++] = Vertex;
		ColorArr1[numb] = ColorArr1[numb - 1];
	}
	for(int i = numb - 2; i >= 0; i -= 2)
	{
		Rail3d1[numb + i] = Rail3d1[numb - i - 2];
		Rail3d1[numb + i].z = -Rail3d1[numb + i].z;
		ColorArr1[numb + i] = ColorArr1[numb - i - 2];
		Rail3d1[numb + i + 1] = Rail3d1[numb - i - 1];
		Rail3d1[numb + i + 1].z = -Rail3d1[numb + i + 1].z;
		ColorArr1[numb + i] = ColorArr1[numb - i - 2];
	}
	numb *= 2;
	Rail3d1[numb++] = Rail3d1[0];
	ColorArr1[numb] = ColorArr1[0];
	Rail3d1[numb++] = Rail3d1[1];
	ColorArr1[numb] = ColorArr1[0];
	for(int i = graph_points.size() * 4; i >= 0; i -= 2)
	{
		Rail3d1[numb + i + 1] = Rail3d1[i];
		ColorArr1[numb + i + 1] = ColorArr1[i];
		Rail3d1[numb + i] = Rail3d1[i + 1];
		ColorArr1[numb + i] = ColorArr1[i + 1];
	}
}

void __fastcall TRailGraphic::setRailLines3d(std::vector<Vertex3D> &Rail3d_arr, std::vector<UINT> &ColorArr)
{
	RailLinesPos = Rail3d_arr.size();
	Rail3d_arr.resize(RailLinesPos + profile_lines_quant * graph_points.size() * 2);
	ColorArr.resize(RailLinesPos + profile_lines_quant * graph_points.size() * 2);
	Vertex3D *LPVert;
	UINT *LPColor;
	RailLinesInd.resize(RailLinesQuant = profile_lines_quant * graph_points.size() * 2 * 2);
	GLushort *ind_arr;

	float z;
	int v_i;
	for(int j = 0; j < profile_lines_quant; ++j)
	{
		v_i = RailLinesPos + j * graph_points.size() * 2;
		LPVert = &Rail3d_arr[v_i];
		LPColor = &ColorArr[v_i];
		ind_arr = &RailLinesInd[j * graph_points.size() * 2 * 2];
		z = (profile_lines_quant - 1) / 2.0f;
		if((z = (j - z) / z) == 0) z += 0.01f;
        z = length3d * graphParams->projection3d * z;
		for(UINT i = 0; i < graph_points.size(); ++i)
		{
			LPVert[i].x = z;
			LPVert[i].y = graphParams->projection3d * graph_points[i].y;
			LPVert[i].z = graphParams->projection3d * graph_points[i].x;
			(LPVert[graph_points.size() * 2 - i - 1] = LPVert[i]).z = -LPVert[i].z;
			LPColor[graph_points.size() * 2 - i - 1] = LPColor[i] = 0x80FFFFFF;
		}
		for(UINT i = 0; i < graph_points.size() * 2; ++i)
		{
			ind_arr[i * 2] = short(j * graph_points.size() * 2 + i) + RailLinesPos;
			ind_arr[i * 2 + 1] = short(j * graph_points.size() * 2 + i + 1) + RailLinesPos;
		}
		ind_arr[graph_points.size() * 2 * 2 - 1] = short(j * graph_points.size() * 2) + RailLinesPos;
	}
}

void __fastcall TRailGraphic::setPlain3d(std::vector<Vertex3D> &Rail3d_arr, std::vector<UINT> &ColorArr)
{
	Plain3dQuant = graph_points.size() * 4;
	Plain3dPos = Rail3d_arr.size();
	Rail3d_arr.resize(Plain3dPos + Plain3dQuant);
	ColorArr.resize(Plain3dPos + Plain3dQuant);
	Vertex3D *Plain3d1 = &Rail3d_arr[Plain3dPos];
	UINT *PColor = &ColorArr[Plain3dPos];
	int vertex_numb = 0;
	
	// Формирование буфера вершин
	for(UINT i = 0; i < graph_points.size(); ++i)
	{
		PColor[vertex_numb + 1] = PColor[vertex_numb] = 0x88880000;//0x88000000;
		Plain3d1[vertex_numb].x = graphParams->projection3d * length3d;
		Plain3d1[vertex_numb].y = graphParams->projection3d * graph_points[i].y;
		Plain3d1[vertex_numb].z = graphParams->projection3d * graph_points[i].x;
		Plain3d1[vertex_numb + 1] = Plain3d1[vertex_numb];
		Plain3d1[vertex_numb + 1].z = -Plain3d1[vertex_numb + 1].z;
		vertex_numb += 2;
	}
	for(UINT i = 0; i < graph_points.size(); ++i)
	{
		PColor[vertex_numb + 1] = PColor[vertex_numb] = 0x88880000;//0x88000000;
		Plain3d1[vertex_numb].x = -graphParams->projection3d * length3d;
		Plain3d1[vertex_numb].y = graphParams->projection3d * graph_points[i].y;
		Plain3d1[vertex_numb].z = -graphParams->projection3d * graph_points[i].x;
		Plain3d1[vertex_numb + 1] = Plain3d1[vertex_numb];
		Plain3d1[vertex_numb + 1].z = -Plain3d1[vertex_numb + 1].z;
		vertex_numb += 2;
	}
}

#define CUT_ZONE 400.0

void __fastcall TRailGraphic::ElvaluateProfiles()
{
	if(!(graph_type & RAIL_PROFILE_RT))
		return;
	int Profiles3dQuant1 = 0;
    float start_pos = beetween_profiles * profiles_quant / 2;
	std::vector<int> ProfilesQuant3dArr1(profiles_quant);
    ballance_point = 0;
    int cut_zone;
    memcpy(&ProfilesQuant3dArr1[0], &ProfilesQuantArr[0], profiles_quant * sizeof(int));
    for(int i = 0; i < profiles_quant; ++i)
    {
        cut_zone = (int)std::min(CUT_ZONE, static_cast<double>(ProfilesQuant3dArr1[i]));
		for(int k = 0; k < cut_zone; ++k) ballance_point += ProfileArrs[MAX_PROFILE_ARR * i + k * 2 + 1] / cut_zone;
    }
	ballance_point /= profiles_quant;
	for(int i = 0; i < profiles_quant; ++i)
    {
		GenerateProfile3d(Profiles3d1[i], &ProfileArrs[MAX_PROFILE_ARR * i], ProfilesQuant3dArr1[i], i * beetween_profiles - start_pos);
    	Profiles3dQuant1 += ProfilesQuant3dArr1[i];
    }
    int numb = 0;
	for(int i = 0; i < profiles_quant; ++i) numb += ProfilesQuant3dArr1[i];
	Profiles3d2.resize(numb);
	numb = 0;
	for(int i = 0; i < profiles_quant; ++i)
	{
        memcpy(&Profiles3d2[numb], &(Profiles3d1[i])[0], ProfilesQuant3dArr1[i] * sizeof(Vertex3D));
    	numb += ProfilesQuant3dArr1[i];
    }
    memcpy(&ProfilesQuant3dArr[0], &ProfilesQuant3dArr1[0], sizeof(int) * profiles_quant);
    Profiles3dQuant = Profiles3dQuant1;
}

void __fastcall TRailGraphic::SetProfile3D(void *arr, int quant, int change_pos)
{
    memcpy(&ProfileArrs[MAX_PROFILE_ARR * change_pos], arr, quant * 2 * sizeof(float));
    ProfilesQuantArr[change_pos] = quant;
}

#define CHECK_AREA 3

void __fastcall TRailGraphic::GenerateProfile3d(std::vector<Vertex3D> &Plain3d1, float *arrF, int &quant, float z_pos)
{
	Plain3d1.resize(quant * 4);
	Vertex3D Vertex;
	int vertex_numb = 0, continue_flag, color, summator = 0, min_numb, max_numb, barrier;

    min_numb = -CHECK_AREA + 1;
    max_numb = CHECK_AREA;
    barrier = (max_numb - min_numb) / 2 + 1;
	for(int i = 0; i < quant; ++i)
	{
        continue_flag = 0;
		if(i > 0 && std::fabs(arrF[i * 2 + 1] - arrF[(i - 1 - summator) * 2 + 1]) +
						std::fabs(arrF[i * 2] - arrF[(i - 1 - summator) * 2]) < 1.8f)
        {
            ++summator;
			continue;
        }
        summator = 0;

		if(i < CUT_ZONE && std::fabs(ballance_point - arrF[i * 2 + 1]) > 18)
			continue;
		if(!profile_filter)
        {
			for(int j = min_numb; j < max_numb; ++j)
        	{
				if(j == 0) ++j;
				if(j + i < 0 || j + i >= quant)
				{
					++continue_flag;
					continue;
				}
				if(fabs(arrF[i * 2 + 1] - arrF[(j + i) * 2 + 1]) + fabs(arrF[i * 2] - arrF[(j + i) * 2]) < 1.5f)
					continue_flag += 2;
        	}
			if(continue_flag < barrier)
				continue;
        }

		if((color = (int)((arrF[i * 2 + 1] + 4) * 3.0f)) > 0x100)
		{	if((color -= 0x100) > 0x100) { color -= 0x100; color = (color << 16) + 0xFF; } }
        else { color = ((0x100 - color) << 8); }
        //Vertex.color = 0x58000000 + color;
		Vertex.x = graphParams->projection3d * z_pos;
		Vertex.y = -graphParams->projection3d * arrF[i * 2 + 1];
		Vertex.z = graphParams->projection3d * arrF[i * 2];
		Plain3d1[vertex_numb++] = Vertex;

        Vertex.x = graphParams->projection3d * (z_pos + beetween_profiles);
		Plain3d1[vertex_numb++] = Vertex;
	}
    int quant1 = vertex_numb;
    vertex_numb -= 2;
	while(vertex_numb >= 0) { Plain3d1[quant1++] = Plain3d1[vertex_numb--]; }
    quant = quant1;
}

void __fastcall TRailGraphic::SetProfileQuant(int quant)
{
	if(profiles_quant == quant || quant < 0)
		return;
	if(profiles_quant < quant)
	{
		Profiles3d1.resize(quant);
		ProfileArrs.resize(quant * MAX_PROFILE_ARR);
		ProfilesQuantArr.resize(quant);
		ProfilesQuant3dArr.resize(quant);
		memset(&ProfilesQuantArr[0], 0, quant * sizeof(int));
		memset(&ProfilesQuant3dArr[0], 0, quant * sizeof(int));
	}
	profiles_quant = quant;
}

void __fastcall TRailGraphic::Inicialize(int in_type, int in_graph_type, float length)
{
	RailFigure::Inicialize(in_type);
    graph_type = in_graph_type;
	length3d = length;
	clear_graph();
	setTotalRail3d();
}

void __fastcall TRailGraphic::drawTotalRail3d()
{
    try {
		if(graph_type & RAIL_3D_RT)
		{
			glEnableVertexAttribArray(graphParams->AttrArrVert);
			glVertexAttribPointer(graphParams->AttrArrVert, 3, GL_FLOAT, GL_FALSE, 0, &Rail3dVB[0]);
			glEnableVertexAttribArray(graphParams->AttrArrColor);
			glVertexAttribPointer(graphParams->AttrArrColor, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, &Rail3dColor[0]);
			//drawPlain3d();
			drawRail3d();
			drawRailLines3d();
			//drawProfile3d();
			glDisableVertexAttribArray(graphParams->AttrArrVert);
			glDisableVertexAttribArray(graphParams->AttrArrColor);
		}
    }
    catch(...) {}
}

void __fastcall TRailGraphic::drawRail3d()
{
	if(Rail3dQuant > 0 || Rail3dPos >= 0)
		glDrawArrays(GL_TRIANGLE_STRIP, Rail3dPos, Rail3dQuant - 2);
}

void __fastcall TRailGraphic::drawRailLines3d()
{
	if(RailLinesPos < 0)
		return;
	glDrawElements(GL_LINES, RailLinesQuant, GL_UNSIGNED_SHORT, &RailLinesInd[0]);
}

void __fastcall TRailGraphic::drawPlain3d()
{
	if(Plain3dPos < 0)
		return;
	int d_gp_size = graph_points.size() * 2;
	for(int i = d_gp_size - 2; i >= 0; i -= 2)
	{
		glDrawArrays(GL_TRIANGLE_STRIP, Plain3dPos + i, 2);
		glDrawArrays(GL_TRIANGLE_STRIP, Plain3dPos + i + d_gp_size, 2);
	}
	//while (++i < rail_graph_parts * 2 - 1) myGraph->myDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, i * 2, 2);
}

void __fastcall TRailGraphic::drawProfile3d()
{
	if(!Profiles3dQuant || !(graph_type & RAIL_PROFILE_RT))
		return;
	glEnableVertexAttribArray(graphParams->AttrArrVert);
    int loc_number = 0;
	if(profile_visualisation_type == VIS_TRIANGLES)
    {
		for(int i = 0; i < profiles_quant; ++i)
    	{
			glDrawArrays(GL_TRIANGLE_STRIP, loc_number, ProfilesQuant3dArr[i] - 2);
        	loc_number += ProfilesQuant3dArr[i];
    	}
	}
    else
    {
		if(profile_visualisation_type == VIS_POINTS) for(int i = 0; i < profiles_quant; ++i)
    	{
			glDrawArrays(GL_POINTS, loc_number, (ProfilesQuant3dArr[i] + 1) / 2);
        	loc_number += ProfilesQuant3dArr[i];
        }
        else if(profile_visualisation_type == VIS_LINES) for(int i = 0; i < profiles_quant; ++i)
    	{
			glDrawArrays(GL_LINES, loc_number, (ProfilesQuant3dArr[i] + 1) / 4);
        	loc_number += ProfilesQuant3dArr[i];
        }
    }
}
//---------------------------------------------------------------------------


