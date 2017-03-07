//---------------------------------------------------------------------------

#ifndef DeffectsSampleH
#define DeffectsSampleH
//---------------------------------------------------------------------------

#include "ChanelDeffects.h"

#define EASY_CHANEL 1
#define HEAVY_CHANEL 2

#define EASY_QUANT 60
#define HEAVY_QUANT 128
#define MAX_QUANT 256

#define EASY_MAX_VALUE 4
#define HEAVY_MAX_VALUE 256

#define REFLECTIONS_COUNT 8

class TDeffectsSample
{
	float defects_points[MAX_QUANT][3];


public:
	class TChanelDeffects *Chanel;

	UINT blocks[MAX_QUANT];
    int wrong_blocks_quant;
    //int blocks_Alpha[MAX_QUANT];
	bool wrong_flag[MAX_QUANT];
	float shift;

    
    __fastcall TDeffectsSample(TChanelDeffects *Chanel1);

	void __fastcall get_data();
	void __fastcall get_data(unsigned int* blocks1);
	void __fastcall get_data(unsigned char* blocks1);
    void __fastcall get_data(TDeffectsSample *ch);
    
    void __fastcall get_wrongs();

    void __fastcall check_front();
	void __fastcall check_blocks();

	int __fastcall set3d(std::vector<TPositionedVertex> &blocks3d, std::vector<short> &blocks_ind);

	TDeffectsSample& __fastcall operator=(const TDeffectsSample& itm);

    friend class TChanelDeffects;
};

#endif
