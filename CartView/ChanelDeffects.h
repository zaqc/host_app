//---------------------------------------------------------------------------

#ifndef ChanelDeffectsH
#define ChanelDeffectsH

//---------------------------------------------------------------------------
#include "GraphParams.h"
#include "OscillatorGraph.h"
#include "DeffectsSample.h"
//---------------------------------------------------------------------------

#define CUT_SCENE
#define STATIC_QUANT 0
#define DYNAMIC_QUANT 1

#define MAX_QUANT 256
#define ADD_QUANT 128
#define TEST_REGION 3

//#define NO_RAIL 0
//#define LEFT_RAIL 1
//#define RIGHT_RAIL 2
//#define BOTH_RAILS (LEFT_RAIL | RIGHT_RAIL)
//---------------------------------------------------------------------------

class TDeffectsSample;
//---------------------------------------------------------------------------

class TChanelDeffects
{
	int color, cur_points_quant,
		realVBsize;
    class OscillatorGraph *connected_oscillator;
	RailFigure *rail;
	unsigned int middle_barrier[MAX_QUANT];
	std::vector<TPositionedVertex> blocks3d;
	std::vector<short> blocks_ind;

	//void __fastcall dynamic_recheck();

public:
	std::vector<TDeffectsSample*> DefData;

	static int points_on_line, points_quant_barrier, points_quant, ChanelsType;
	static UINT line_check_barrier;
	static float max_step, beetween, small_peak_barrier, work_zone;
	static bool with_middle_barrier, with_small_peak, with_line_check,
		with_alone_point_filter, with_front_filter, def_zone;

	D3DMATERIAL9 myMaterial;
    TGraphParams *myGraph;
	IDirect3DVertexBuffer9 *WrongBlocks3d, *B_reamer;
	IDirect3DIndexBuffer9 *WrongBlocksInd;
	int total_buffer_length, side;
	unsigned int barier;
	bool Using;
	float step;


	__fastcall TChanelDeffects(TGraphParams *Graph, RailFigure *in_rail);
	__fastcall ~TChanelDeffects();

    void __fastcall clear_graph();

    void __fastcall zero_quant();
	void __fastcall reset_quant();
	void __fastcall generate_blocks();
	void __fastcall set_blocks(UINT* blocks1, int numb);
	void __fastcall set_blocks(unsigned char* blocks1, int numb);

    void get_middle_barrier();
	void check_for_small_peak();
	void alone_point_filter();
	void front_filter();
	void check_blocks(bool enable3d1 = true);

	void generate_b_reamer(int start_x, int start_y, float multiple_x, float multiple_y,
							int *arr = NULL, int arr_quant = 0);

    void get_deffects_points(int *);

	void connect_oscillator(OscillatorGraph *oscill, int start_pos);
	void set_color(DWORD);

	void __fastcall drawBlocks3d();
	void __fastcall draw_b_reamer();

	TChanelDeffects& __fastcall operator =(const TChanelDeffects& chanel);

    friend class TDeffectsSample;
};

#endif
