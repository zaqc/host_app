/*
 * BScanWnd.h
 *
 *  Created on: Sep 28, 2016
 *      Author: zaqc
 */

#ifndef SRC_BSCANWND_H_
#define SRC_BSCANWND_H_
//----------------------------------------------------------------------------

#include "Window.h"
//----------------------------------------------------------------------------

struct ColorMap {
	unsigned int m_Color[256][256];
};

struct Channel {
	bool UseIt;		// Use this chennel for rendering
	int ColorIndex;	// Color index in CLUT
	int DataIndex;	// start data in data buffer returned by GetData(void)
	int DataSize;	// size of channel data
};

struct Track {
	bool ShowIt;		// show this track on tape
	int DefaultHeight;	// Real height of track (default value is 128 tick's)
	int RealHeight;		// Real height of Track on Tape (calculate at real time)
	int TrackTop;		// Y position of Track on Tape
	int MinTrackHeight;	// Minimal track height (default 8 pixels)
	bool AutoHeight;	// Auto Calculate track height
	int Channel[4];		// Channel number
};

struct Preparser {
	bool UseIt;			// use data from this channel for draw or not
	bool FirstUse;		// use this TapePoint in first time (clear data if true)
	int Value1;
	int Index1;
	int Value2;
	int Index2;
	int DisplayHeight;	// from 1 to N (depends of height zoom)
};

struct Tape {
	SDL_Color BkColor;	// Color of track dividers
	int TrackCount;		// Tracks Count on tape (max=10)
	struct Track Track[10];
	int PPCount;		// PreParseed point count
	struct Preparser PP[4096]; // max screen height (480)
};
//----------------------------------------------------------------------------

class RealTapeScroller {
protected:
	unsigned int *m_FB;		// Frame Buffer
	unsigned int *m_DB;		// Draw Buffer
	int m_W;
	int m_H;

	Tape m_Tape;
public:
	RealTapeScroller(int aW, int aH);
	virtual ~RealTapeScroller();

	void InitTapeConfig(void);

	void CalcPreparserTable(void);

	virtual unsigned char * GetData(void);
	virtual int GetDataLength(void);

	void Show(SDL_Renderer *aRnd, int aX, int aY);

	void PrepareDrawBuffer(void);
	void ScrollAndDraw(void);
};

class BScanWnd: public Window {
public:
	BScanWnd(SDL_Renderer *aRnd, int aX, int aY, int aW, int aH);
	virtual ~BScanWnd();
};
//----------------------------------------------------------------------------

#endif /* SRC_BSCANWND_H_ */
