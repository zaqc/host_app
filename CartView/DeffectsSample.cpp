//---------------------------------------------------------------------------
#pragma hdrstop

#include "DeffectsSample.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)

__fastcall TDeffectsSample::TDeffectsSample(TChanelDeffects *Chanel1)
			: Chanel(Chanel1)
			, wrong_blocks_quant(0)
{
	memset(wrong_flag, 0, sizeof(bool) * MAX_QUANT);
	get_data();
}

void __fastcall TDeffectsSample::get_data()
{
    memset(blocks, 0, MAX_QUANT * sizeof(unsigned int));
}

void __fastcall TDeffectsSample::get_data(unsigned int* blocks1)
{
    memcpy(blocks, blocks1, TChanelDeffects::points_on_line * sizeof(unsigned int));
}

void __fastcall TDeffectsSample::get_data(unsigned char* blocks1)
{
	for(int i = TChanelDeffects::points_on_line - 1; i >= 0; --i) blocks[i] = blocks1[i];
}

void __fastcall TDeffectsSample::get_data(TDeffectsSample *ch)
{
    memcpy(blocks, ch->blocks, TChanelDeffects::points_on_line * sizeof(UINT));
}

void __fastcall TDeffectsSample::get_wrongs()
{
	for(int i = TChanelDeffects::points_on_line - 3; i >= 0; --i)
        wrong_flag[i] = (Chanel->middle_barrier[i] <= blocks[i]);
	if(!TChanelDeffects::with_line_check)
		return;
    UINT summator = 0, summator1 = 0;
	for(int i = TChanelDeffects::points_on_line; i >= 0; --i)
		if(wrong_flag[i]) { summator += blocks[i]; ++summator1; }
    if(summator > TChanelDeffects::line_check_barrier || summator1 > 10)
		memset(wrong_flag, 0, TChanelDeffects::points_on_line * sizeof(bool));
}

void __fastcall TDeffectsSample::check_blocks()
{
	wrong_blocks_quant = 0;
	if(!Chanel || !Chanel->connected_oscillator)
		return;
	std::vector<Point3F> &pts = Chanel->connected_oscillator->points;
	int a = pts.size(), z,
		j = 0;
	for(int k = 0; k < a; ++k) if(wrong_flag[k])
	{
		defects_points[j][0] = pts[k].x;
		defects_points[j][1] = pts[k].y;
		z = pts[k].z + shift;
		#ifdef CUT_SCENE
			if(z > TChanelDeffects::work_zone || z < -TChanelDeffects::work_zone)
				continue;
		#endif
		defects_points[j][2] = z;
		++j;
	}
	wrong_blocks_quant = j;
}

void __fastcall TDeffectsSample::check_front()
{
	int quant = 0, numb;
    for(int i = 0; i < TChanelDeffects::points_on_line; ++i)
    {
        if((!wrong_flag[i] && quant != 0) || quant > 6)
        {
            numb = i - quant / 2 - 1;
			for(int j = i - quant; j < i; ++j) if(j != numb && wrong_flag[j])
				wrong_flag[j] = false;
			quant = 0;
        }
        if(wrong_flag[i]) ++quant;
    }
}

int __fastcall TDeffectsSample::set3d(std::vector<TPositionedVertex> &blocks3d, std::vector<short> &blocks_ind)
{
	if(!Chanel || !Chanel->connected_oscillator)
		return 0;
	int b_s = blocks3d.size(),
		i_s = blocks_ind.size();
	blocks3d.resize(b_s + wrong_blocks_quant * CUBE_MIN_VERTEX);
	blocks_ind.resize(i_s + wrong_blocks_quant * CUBE_IND_QUANT);
	float len[] = { Chanel->beetween / 2,
				Chanel->step * Chanel->connected_oscillator->Vectors[0].y / 2, Chanel->beetween / 2};
	UINT color = 0x88000000 | Chanel->connected_oscillator->color;
	for(int i = wrong_blocks_quant - 1; i >= 0; --i)
		//Chanel->myGraph->SetCube(&blocks3d[b_s + i * CUBE_VERTEX_QUANT], len, defects_points[i], color);//blocks_Alpha[i]);
		Chanel->myGraph->SetCubeWithInd(&blocks3d[b_s + i * CUBE_MIN_VERTEX], &blocks_ind[i_s + i * CUBE_IND_QUANT],
										b_s + i * CUBE_MIN_VERTEX, len, defects_points[i], color);
	return wrong_blocks_quant;
}

TDeffectsSample& __fastcall TDeffectsSample::operator=(const TDeffectsSample& itm)
{
	Chanel = itm.Chanel;

	memcpy(defects_points, itm.defects_points, sizeof(defects_points));
	memcpy(blocks, itm.blocks, sizeof(blocks));
	wrong_blocks_quant = itm.wrong_blocks_quant;
	//memcpy(blocks_Alpha, itm.blocks_Alpha, sizeof(blocks_Alpha));
	memcpy(wrong_flag, itm.wrong_flag, sizeof(wrong_flag));
	shift = itm.shift;
	return *this;
}


