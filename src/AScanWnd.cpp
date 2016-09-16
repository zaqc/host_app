/*
 * AScanWnd.cpp
 *
 *  Created on: Sep 16, 2016
 *      Author: zaqc
 */

#include "AScanWnd.h"

#include "Label.h"
#include "TrackBar.h"
#include "Window.h"
//----------------------------------------------------------------------------

AScanWnd::AScanWnd(SDL_Renderer *aRnd, int aX, int aY, int aW, int aH) :
		Window(aRnd, aX, aY, aW, aH) {
	m_LAmpOne = NULL;
	m_TBAmpOne = NULL;
}

AScanWnd::~AScanWnd() {
}

void AScanWnd::Init(void) {
	m_LAmpOne = new Label(10, 10, 64, 24, "Amp1");
	AddControl(m_LAmpOne);
	m_TBAmpOne = new TrackBar(10, 32, 64, 400);
	AddControl(m_TBAmpOne);
}

void AScanWnd::Done(void) {
}

void AScanWnd::UpdateControls(void) {
	if (NULL != m_TBAmpOne && NULL != m_LAmpOne) {
		int val = m_TBAmpOne->GetValue();
		char str[128];
		sprintf(str, "%idB", val);
		m_LAmpOne->SetText(std::string(str));
	}
}
