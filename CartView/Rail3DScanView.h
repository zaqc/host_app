#pragma once

#include "BaseView.h"
#include "ShadersManager.h"
#include "RailGraphicVC.h"
#include "OscillatorGraph.h"

namespace ScreenView
{
	class Rail3DScanView : public BaseView
	{
	private:
		GLuint m_hProgram;
		GLuint m_hVertexLocation;
		GLuint m_hProjectionLocation;
		GLuint m_hModelViewLocation;

		float m_matProjectionMatrix[16];
		float m_matModelViewMatrix[16];
		float angle = 60;
		GlParams gParams;
		TRailGraphic rail;
		std::vector<OscillatorGraph*> Oscillators;

	public:
		bool no_oscillators;


		Rail3DScanView();
		~Rail3DScanView();
		virtual void Render();

		void __fastcall SetChanelsQuant(int ch_quant);

		void __fastcall SetPointsQuant(int points_quant = -1);
		void __fastcall SetChanelsSettings(int ch_quant, int points_on_line, float beetween);

		//void __fastcall SetOscillVisibility(int chanel_numb, bool flag);
		//bool __fastcall GetOscillVisibility(int chanel_numb);
		void __fastcall SetOscillColor(int chanel_numb, int color);
		int __fastcall GetOscillColor(int chanel_numb);
		//void __fastcall SetBarrier(int chanel_numb, int barrier);
		//void __fastcall SetBarrier(int barrier);
		void __fastcall RefreshOscillSettings(int chanel_numb, int rail_type, Point3F p, Vector3F v, int color);
		void __fastcall RefreshOscillSettings(int chanel_numb, int rail_type,
					float position, int face_type, float ang_a, float ang_b, int color = -1);
		void __fastcall RefreshOscillSettings(int chanel_numb, bool with_lock = true);
		//void __fastcall DataFilters(int numb = FILTER_NONE);

		/*void __fastcall fill_data(int chanel_numb, int pos, UCHAR *work_arr);
		void __fastcall fill_data_chanel(int chanel_numb, UCHAR *work_arr);
		void __fastcall fill_data_pos(int pos, UCHAR *work_arr);
		void __fastcall fill_data(UCHAR *work_arr);*/
		/*void __fastcall fast_take_data(long position, bool with_graph = true);
		void __fastcall take_data(long position = -1, bool with_graph = true);
		void __fastcall get_data(long frame1);*/

		//void __fastcall check_data(int chanel_numb, bool with_lock = true);
		//void __fastcall check_data(bool with_graph = true);

		//void __fastcall find_reflectors(int ch_numb);
		//void __fastcall find_reflectors();
		//void __fastcall get_deffects_area(int pos0, int rail_side = BOTH_RAILS);
	};
}