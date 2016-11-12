/*
 * KeyParser.h
 *
 *  Created on: Nov 11, 2016
 *      Author: zaqc
 */

#ifndef KEYPARSER_H_
#define KEYPARSER_H_

#include <SDL.h>

#include "Window.h"

enum SpecialKey {
	skF1, // Special function key's
	skF2,
	skF3,
	skF4,
	skF5,
	skF6,
	skLeft, // Arrow joystick key
	skRight,
	skUp,
	skDown,
	skEnter, // Ok
	skEsc // cancel
};

// f1 - View type (Log/VGA)
// f2 - Log - Level / VGA - Amp1
// f3 - VGA - Amp2
// f4 - VRCLen

enum MainState {
	msMainMenu, msAScan, msBScan
};

class GlobalState {
protected:
	MainState m_MainState;
	bool m_ShowMenu;
};

class KeyParser: public Window {
public:
	KeyParser(SDL_Renderer *aRnd, int aX, int aY, int aW, int aH);
	virtual ~KeyParser();
};

#endif /* KEYPARSER_H_ */
