//---------------------------------------------------------------------------
#pragma hdrstop

#include "ChanelDeffects.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)

__fastcall TChanelDeffects::TChanelDeffects(TGraphParams *Graph, RailFigure *in_rail)
			: rail(in_rail)
			, myGraph(Graph)
			, connected_oscillator(NULL)
			, Using(false)
			, WrongBlocks3d(NULL)
			, WrongBlocksInd(NULL)
			, B_reamer(NULL)
			, side(NO_RAIL)
			, realVBsize(0)
{
	barier = 17;
	step = 2.8f;
    memset(&myMaterial, 0, sizeof(D3DMATERIAL9));
    set_color(color = 0x80FF0000);
	cur_points_quant = total_buffer_length = 0;

	reset_quant();
	//memset(barier_flag, 1, sizeof(byte) * MAX_QUANT);
}

__fastcall TChanelDeffects::~TChanelDeffects()
{
	for(int i = 0; i < points_quant; ++i) if(DefData[i]) delete DefData[i];
    clear_graph();
}

void __fastcall TChanelDeffects::clear_graph()
{
    if(WrongBlocksInd) { WrongBlocksInd->Release(); WrongBlocksInd = NULL; }
	if(WrongBlocks3d) { WrongBlocks3d->Release(); WrongBlocks3d = NULL; }
	realVBsize = 0;
    if(B_reamer) { B_reamer->Release(); B_reamer = NULL; }
}

void __fastcall TChanelDeffects::zero_quant()
{
    points_quant = 0;
}

/*void __fastcall TChanelDeffects::dynamic_recheck()
{
	if(cur_points_quant >= points_quant)
		return;
	int old_pq = cur_points_quant,
		points_quant1 = points_quant;
	points_quant = 0;
	do {
		cur_points_quant += ADD_QUANT;
	} while(cur_points_quant <= points_quant1);
	std::vector<TDeffectsSample*> DefData1(cur_points_quant);
	int i = 0;
	for( ; i < old_pq; ++i) DefData1[i] = DefData[i];
	for( ; i < cur_points_quant; ++i) DefData[i] = new TDeffectsSample(this);
	DefData.swap(DefData1);
	points_quant = points_quant1;
}*/

void __fastcall TChanelDeffects::reset_quant()
{
	if(cur_points_quant == points_quant)
		return;
	int cpq = cur_points_quant;
	cur_points_quant = 0;
	std::vector<TDeffectsSample*> DefData1(points_quant);
    int i = 0;
	if(cpq > points_quant)
    {
		for( ; i < points_quant; ++i) DefData1[i] = DefData[i];
		for( ; i < cpq; ++i) delete DefData[i];
    }
    else
    {
		for( ; i < cpq; ++i) DefData1[i] = DefData[i];
		for( ; i < points_quant; ++i) DefData1[i] = new TDeffectsSample(this);
	}
	DefData.swap(DefData1);
	cur_points_quant = points_quant;
	for(i = 0; i < points_quant; ++i) DefData[i]->get_data();
}

void __fastcall TChanelDeffects::generate_blocks()
{
	if(!Using)
		return;
	for(int i = 0; i < points_quant; ++i) DefData[i]->get_data();
}

void __fastcall TChanelDeffects::set_blocks(UINT* blocks1, int numb)
{
	if(Using) DefData[numb]->get_data(blocks1);
}

void __fastcall TChanelDeffects::set_blocks(unsigned char* blocks1, int numb)
{
	if(Using) DefData[numb]->get_data(blocks1);
}

void TChanelDeffects::get_middle_barrier()
{
	memset(middle_barrier, 0, points_on_line * sizeof(UINT));
	if(with_middle_barrier) for(int j = 0; j < points_on_line; ++j)
	{
		for(int i = 0; i < points_quant; ++i) middle_barrier[j] += DefData[i]->blocks[j];
		middle_barrier[j] = middle_barrier[j] / 1.35f / points_on_line + barier;
	}
	else for(int j = 0; j < points_on_line; ++j) middle_barrier[j] = barier;
}

//#define TEST_REGION 3

void TChanelDeffects::alone_point_filter()
{
	if(!with_alone_point_filter)
		return;
	bool flagf;
	for(int i = 1; i < points_quant - 1; ++i) for(int j = 1; j < points_on_line - 1; ++j)
		if(DefData[i]->wrong_flag[j])
		{
			flagf = true;
			for(int i0 = -1; i0 <= 1 && flagf; ++i0)
				for(int j0 = -1; j0 <= 1 && flagf; ++j0)
					if(DefData[i + i0]->wrong_flag[j + j0]) if(i0 || j0) flagf = false;
			DefData[i]->wrong_flag[j] = !flagf;
		}
}

void TChanelDeffects::check_for_small_peak()
{
	if(!with_small_peak)
		return;
	int points_quant1, block_numb = -1;
	float* test_point, *point0;
	for(int i = TEST_REGION; i < points_quant - TEST_REGION; ++i, (block_numb = -1))
    	for (int k = 0; k < DefData[i]->wrong_blocks_quant; ++k)
		{
			do { ++block_numb; }
			while(!DefData[i]->wrong_flag[block_numb]);
			test_point = DefData[i]->defects_points[k];
			points_quant1 = 0;
			for(int j = -TEST_REGION; j < TEST_REGION; ++j)
				for(int f = 0; f < DefData[i + j]->wrong_blocks_quant; ++f)
				{
					if(f == k && !j)
						continue;
					point0 = DefData[i + j]->defects_points[f];
					if((point0[0] - test_point[0]) * (point0[0] - test_point[0]) +
								(point0[1] - test_point[1]) * (point0[1] - test_point[1]) +
								(point0[0] - test_point[2]) * (point0[2] - test_point[2]) < 42)
						++points_quant1;
				}
			if(points_quant1 < points_quant_barrier)
				DefData[i]->wrong_flag[block_numb] = false;
		}
}

void TChanelDeffects::front_filter()
{
	if(!Using || !with_front_filter)
		return;
	int i, j, counter = 0;
	for(i = DefData.size() - 1; i >= 0; --i, counter = 0)
		for(j = 0; j < points_on_line; ++j)
			if(DefData[i]->wrong_flag[j])
			{
				if(!counter) counter = 5;
				else
				{
					--counter;
					DefData[i]->wrong_flag[j] = false;
				}
			}
			else if(counter) counter = 0;
}

void TChanelDeffects::check_blocks(bool enable3d1)
{
	if(!Using)
		return;
    get_middle_barrier();
	total_buffer_length = 0;
	for(int i = DefData.size() - 1; i >= 0; --i)
	{
		DefData[i]->get_wrongs();
		DefData[i]->check_blocks();
    }
	check_for_small_peak();
	front_filter();
	for(int i = DefData.size() - 1; i >= 0; --i) DefData[i]->check_blocks();

	if(enable3d1 && myGraph)
	{
        blocks3d.clear();
		blocks_ind.clear();
		for(int i = DefData.size() - 1; i >= 0; --i)
			total_buffer_length += DefData[i]->set3d(blocks3d, blocks_ind);
		if(total_buffer_length > 0)
		{
			/*int curVBsize = CUBE_VERTEX_QUANT * total_buffer_length,
				q = 0;
			/*if(realVBsize < curVBsize)
			{
				realVBsize = curVBsize;
				myGraph->CreateVertexBuffer(&WrongBlocks3d, TPositionedVertex::Format(),
								realVBsize * CUBE_VERTEX_QUANT * sizeof(TPositionedVertex));
			}
			TPositionedVertex *timed;
			WrongBlocks3d->Lock(0, curVBsize * CUBE_VERTEX_QUANT, &(void*)timed, 0);
            memcpy(timed, &blocks3d[0], realVBsize * sizeof(TPositionedVertex));
			WrongBlocks3d->Unlock();*/
			int q = 0;
			if(realVBsize < total_buffer_length)
			{
				realVBsize = total_buffer_length;
				myGraph->CreateVertexBuffer(&WrongBlocks3d, TPositionedVertex::Format(),
								total_buffer_length * CUBE_MIN_VERTEX * sizeof(TPositionedVertex));
				myGraph->CreateIndexBuffer(&WrongBlocksInd, D3DFMT_INDEX16,
								total_buffer_length * CUBE_IND_QUANT * sizeof(short));
			}
			void *timed;
			WrongBlocks3d->Lock(0, 0, &timed, 0); // total_buffer_length * CUBE_IND_QUANT * sizeof(TPositionedVertex)
			memcpy(timed, &blocks3d[0], total_buffer_length * CUBE_MIN_VERTEX * sizeof(TPositionedVertex));
			WrongBlocks3d->Unlock();
			WrongBlocksInd->Lock(0, 0, &timed, 0);
			memcpy(timed, &blocks_ind[0], total_buffer_length * CUBE_IND_QUANT * sizeof(short));
			WrongBlocksInd->Unlock();
		}
	}
}

void TChanelDeffects::generate_b_reamer(int start_x, int start_y,
					float multiple_x, float multiple_y, int *arr, int arr_quant)
{
	if(!Using || !myGraph)
		return;
    TTransformedColoredVertex* Vertex = new TTransformedColoredVertex[points_on_line * points_quant];
	int x, y, numb,
		def;//, def_numb = 0, start = (-connected_oscillator->oscill_point[0].z + 50) / beetween;
	memset(Vertex, 0, points_on_line * points_quant * sizeof(TTransformedColoredVertex));

    if(def_zone) for(x = 0; x < points_quant; ++x)
    {
		bool* wrong_flag = DefData[x]->wrong_flag;
		for(y = 0; y < points_on_line; ++y)
    	{
        	numb = x * points_on_line + y;
			Vertex[numb].x = x * multiple_x + start_x;
			Vertex[numb].y = 128 - y * multiple_y + start_y;
            Vertex[numb].color = 0xFF000000 + ((wrong_flag[y]) ? color : 0);
        }
    }
	else for(x = 0; x < points_quant; ++x)
    {
        UINT *blocks = DefData[x]->blocks;
        for(y = 0; y < points_on_line; ++y)
    	{
        	numb = x * points_on_line + y;
			Vertex[numb].x = x * multiple_x + start_x;
			Vertex[numb].y = 128 * multiple_y - y * multiple_y + start_y;
            Vertex[numb].color = 0xFF000000 + TGraphParams::color_power(color, 4 * blocks[y] / 255.0f);
        }
    }
	myGraph->MakeVertexBuffer(&B_reamer, Vertex, TTransformedColoredVertex::Format(),
				sizeof(TTransformedColoredVertex), points_on_line * points_quant);
	delete[] Vertex;
}

void TChanelDeffects::get_deffects_points(int *deffect_points)
{
	if(!Using || !rail)
		return;
	float top = rail->points[rail->points.size() - 1].y,
		bottom = rail->points[0].y;
	int x, y, i, j, y_max = (top - bottom) / 5;
	for(i = 0; i < points_quant; ++i)
		for(j = DefData[i]->wrong_blocks_quant - 1; j >= 0; --j)
        {
        	float* point = DefData[i]->defects_points[j];
			if (point[2] > work_zone || point[1] > top || point[2] < -work_zone || point[1] < bottom)
				continue;
			x = (point[1] - bottom) / 5;
			y = (point[2] + work_zone) / 5;
			++deffect_points[x + (y * y_max)];
        }
}

void TChanelDeffects::connect_oscillator(OscillatorGraph *oscill, int start_pos)
{
    connected_oscillator = oscill;
    for(int i = 0; i < points_quant; ++i)
        DefData[i]->shift = start_pos + i * beetween;
}

void TChanelDeffects::set_color(DWORD col)
{
    color = col;
	myMaterial.Ambient = myGraph->toD3DCOLORVALUE(color);
}

void __fastcall TChanelDeffects::drawBlocks3d()
{
	if(!Using || !WrongBlocks3d)
		return;
	myGraph->SetMaterial(myMaterial);
    
	myGraph->myDevice->SetStreamSource(0, WrongBlocks3d, 0, sizeof(TPositionedVertex));
	//for(int j = 0; j < total_buffer_length; ++j) DRAWCUBE(j * CUBE_VERTEX_QUANT, myGraph->myDevice);
	myGraph->myDevice->SetIndices(WrongBlocksInd);
	myGraph->myDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,
				0, 0, total_buffer_length * CUBE_MIN_VERTEX, 0, total_buffer_length * CUBE_IND_QUANT / 3);
}

void __fastcall TChanelDeffects::draw_b_reamer()
{
	if(!Using || !B_reamer)
		return;
	myGraph->myDevice->SetStreamSource(0, B_reamer, 0, sizeof(TTransformedColoredVertex));
	myGraph->myDevice->DrawPrimitive(D3DPT_POINTLIST, 0, points_on_line * points_quant);
}

TChanelDeffects& __fastcall TChanelDeffects::operator =(const TChanelDeffects& chanel)
{
	//points_quant = chanel.points_quant;
	color = chanel.color;
	myMaterial = chanel.myMaterial;
	barier = chanel.barier;
	Using = chanel.Using;
	step = chanel.step;
	side = chanel.side;
	reset_quant();

	for(int i = 0; i < points_quant; ++i) *DefData[i] = *chanel.DefData[i];
	return *this;
}
//---------------------------------------------------------------------------

int TChanelDeffects::ChanelsType = STATIC_QUANT;
int TChanelDeffects::points_quant = 800;
int TChanelDeffects::points_on_line = 128;
int TChanelDeffects::points_quant_barrier = 2;
UINT TChanelDeffects::line_check_barrier = 1500;
float TChanelDeffects::max_step = 3.5;
float TChanelDeffects::beetween = 5;
float TChanelDeffects::small_peak_barrier = 4.0f;
float TChanelDeffects::work_zone = 1000;
bool TChanelDeffects::with_middle_barrier = false;
bool TChanelDeffects::with_small_peak = false;
bool TChanelDeffects::with_line_check = false;
bool TChanelDeffects::with_alone_point_filter = false;
bool TChanelDeffects::with_front_filter = false;
bool TChanelDeffects::def_zone = false;
//---------------------------------------------------------------------------


