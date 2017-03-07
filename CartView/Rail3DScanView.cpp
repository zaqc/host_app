#include <cstdio>
#include "Rail3DScanView.h"
#include "../utouch/Matrix.h"

using namespace ScreenView;

static const char glVertexShader[] =
"attribute vec4 vertexPosition;\n"
"attribute vec4 vColor;\n"
"attribute vec2 vertexTextureCord;\n"
"varying vec2 textureCord;\n"
"uniform mat4 projection;\n"
"uniform mat4 modelView;\n"
"varying float dpt;\n"
"varying vec4 my_color;\n"
"vec4 pos;\n"
"void main()\n"
"{\n"
"    dpt = 1.0;\n"
"    pos = projection * modelView * vertexPosition;\n"
"    textureCord = vertexTextureCord;\n"
"    my_color = vColor;\n"
"    dpt = (pos.z) / 10.0; \n"
"    gl_Position = pos; \n"
"}\n";

static const char glFragmentShader[] =
"precision mediump float;\n"
"uniform sampler2D texture;\n"
"varying vec2 textureCord;\n"
"varying float dpt;\n"
"varying vec4 my_color;\n"
"void main()\n"
"{\n"
//"    gl_FragColor = vec4(texture2D(texture, textureCord).xyz, dpt);\n"
"	gl_FragColor = my_color;\n"
"}\n";
Rail3DScanView::Rail3DScanView()
			: BaseView()
			, no_oscillators(false)
{
	m_hProgram = ShadersManager::createProgram(glVertexShader, glFragmentShader);
	m_hVertexLocation = glGetAttribLocation(m_hProgram, "vertexPosition");
	m_hProjectionLocation = glGetUniformLocation(m_hProgram, "projection");
	m_hModelViewLocation = glGetUniformLocation(m_hProgram, "modelView");

	matrixPerspective(m_matProjectionMatrix, 45, (float)800 / (float)480, 0.1f, 100);
	
	gParams.SetProgram(m_hProgram);
	rail.graphParams = &gParams;
	rail.Inicialize(RT_R65, RAIL_3D_RT, 500);

	SetChanelsSettings(4, 128, 5);

	RefreshOscillSettings(0, RIGHT_RAIL, Point3F(0, 90, 0), Vector3F(5, -15, 12), 0x008800);
	//ChanelDeffects[0]->step = 2.2f;
	RefreshOscillSettings(1, RIGHT_RAIL, 155, 1, 45, 0, 0x6666FF);
	RefreshOscillSettings(2, RIGHT_RAIL, -155, -1, 45, 0, 0xFF0000);
	RefreshOscillSettings(3, RIGHT_RAIL, -150, 1, 58, 34, 0xFF8844);
}

Rail3DScanView::~Rail3DScanView()
{

}

void Rail3DScanView::Render()
{
	glClearColor(.5f, .3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	matrixIdentityFunction(m_matModelViewMatrix);

	matrixRotateZ(m_matModelViewMatrix, 5);
	matrixRotateY(m_matModelViewMatrix, angle);

	matrixTranslate(m_matModelViewMatrix, -1.0f, 0.0f, -7.5f);

	glUseProgram(m_hProgram);
	//glVertexAttribPointer(m_hVertexLocation, 3, GL_FLOAT, GL_FALSE, 0, cubeVertices);
	//glEnableVertexAttribArray(m_hVertexLocation);

	/* [enableAttributes] */
	//glVertexAttribPointer(textureCordLocation, 2, GL_FLOAT, GL_FALSE, 0, textureCords);
	//glEnableVertexAttribArray(textureCordLocation);
	glUniformMatrix4fv(m_hProjectionLocation, 1, GL_FALSE, m_matProjectionMatrix);
	glUniformMatrix4fv(m_hModelViewLocation, 1, GL_FALSE, m_matModelViewMatrix);

	/* Set the sampler texture unit to 0. */
	//glUniform1i(samplerLocation, 3);
	
	for (int i = Oscillators.size() - 1; i >= 0; --i)
	{
		//if(!ChanelDeffects[i]->Using) continue;
		if (!no_oscillators)
		{
			Oscillators[i]->draw3dLines();
			Oscillators[i]->draw3dOscillators();
		}
		//ChanelDeffects[i]->drawBlocks3d();
	}
	rail.drawTotalRail3d();

	//angle += .0025f;
	//if (angle > 360)
	//{
	//	angle -= 360;
	//}

}

void __fastcall Rail3DScanView::SetChanelsQuant(int ch_quant)
{
	int old_q = Oscillators.size();
	for (int i = ch_quant; i < old_q; ++i)
		delete Oscillators[i];
	if (ch_quant < 1) Oscillators.clear();
	else Oscillators.resize(ch_quant, NULL);
	for (int i = old_q; i < ch_quant; ++i)
		Oscillators[i] = new OscillatorGraph(&gParams, &rail);
}

void __fastcall Rail3DScanView::SetPointsQuant(int points_quant)
{
	//EnterCriticalSection(&myGraph->FLock);
	float work_zone,
		beetween = 5.0f;
	//if(points_quant < 100) points_quant = 100;
	work_zone = points_quant / 2 * beetween + 300;

	//for (int i = 0; i < MAX_CHANELS_QUANT; ++i) ChanelDeffects[i]->reset_quant();
	for (UINT i = 0; i < Oscillators.size(); ++i)
		RefreshOscillSettings(i, false);
	rail.clear_graph();
	rail.Inicialize(RT_R65, RAIL_3D_RT, work_zone);

	//LeaveCriticalSection(&myGraph->FLock);
}

void __fastcall Rail3DScanView::SetChanelsSettings(int ch_quant, int points_on_line, float beetween)
{
	SetChanelsQuant(ch_quant);

	//TChanelDeffects::beetween = beetween;
	//TChanelDeffects::points_on_line = points_on_line;
	//EnterCriticalSection(&myGraph->FLock);
	for (UINT i = 0; i < Oscillators.size(); ++i) RefreshOscillSettings(i, false);
	//LeaveCriticalSection(&myGraph->FLock);
}

void __fastcall Rail3DScanView::SetOscillColor(int chanel_numb, int color)
{
	if (chanel_numb < 0 || chanel_numb >= (int)Oscillators.size()) return;
	//ChanelDeffects[chanel_numb]->set_color(color);
	Oscillators[chanel_numb]->color = color;
	//check_data(chanel_numb);
}

int __fastcall Rail3DScanView::GetOscillColor(int chanel_numb)
{
	if (chanel_numb < 0 || chanel_numb >= (int)Oscillators.size()) return 0;
	return Oscillators[chanel_numb]->color;
}

void __fastcall Rail3DScanView::RefreshOscillSettings(int chanel_numb,
	int rail_type, Point3F p, Vector3F v, int color)
{
	if (chanel_numb < 0 || chanel_numb >= (int)Oscillators.size())
		return;
	if (color != -1)
	{
		//ChanelDeffects[chanel_numb]->set_color(color);
		Oscillators[chanel_numb]->color = color;
	}
	Oscillators[chanel_numb]->reset((float*)&p, (float*)&v);
	//ChanelDeffects[chanel_numb]->connect_oscillator(Oscillators[chanel_numb], -TChanelDeffects::points_quant / 2 * TChanelDeffects::beetween);
	//ChanelDeffects[chanel_numb]->side = rail_type;
}

void __fastcall Rail3DScanView::RefreshOscillSettings(int chanel_numb,
	int rail_type, float position, int face_type, float ang_a, float ang_b, int color)
{
	if (chanel_numb < 0 || chanel_numb >= (int)Oscillators.size())
		return;
	if (color != -1)
	{
		//ChanelDeffects[chanel_numb]->set_color(color);
		Oscillators[chanel_numb]->color = color;
	}
	Oscillators[chanel_numb]->reset(position, face_type, ang_a, ang_b);
	//ChanelDeffects[chanel_numb]->connect_oscillator(Oscillators[chanel_numb], -TChanelDeffects::points_quant / 2 * TChanelDeffects::beetween);
	//ChanelDeffects[chanel_numb]->side = rail_type;
}

void __fastcall Rail3DScanView::RefreshOscillSettings(int chanel_numb, bool with_lock)
{
	if (chanel_numb < 0 || chanel_numb >= (int)Oscillators.size())
		return;
	//if (with_lock) EnterCriticalSection(&myGraph->FLock);
	//ChanelDeffects[chanel_numb]->connect_oscillator(Oscillators[chanel_numb], -TChanelDeffects::points_quant / 2 * TChanelDeffects::beetween);
	//check_data(with_lock);
	//if (with_lock) LeaveCriticalSection(&myGraph->FLock);
}
