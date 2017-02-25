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
#include <libxml/parser.h>
//----------------------------------------------------------------------------

struct ColorMap {
	unsigned int m_Color[256][256];
};

struct Channel {
	bool UseIt; // Use this chennel for rendering
	int Number;
	int ColorIndex; // Color index
	int Side; // Side 1-left 2-right
	int DataIndex; // start data in data buffer returned by GetData(void)
	int DataSize; // size of channel data (tick count)
};

struct Track {
	bool ShowIt; // show this track on tape
	int Side; // 1-left 2-right
	int DefaultHeight; // Default height of track (default value is 128 tick's)
	int RealHeight; // Real height of Track on Tape (calculate at real time)
	int TrackTop; // Y position of Track on Tape
	int MinTrackHeight; // Minimal track height (default 8 pixels)
	bool AutoHeight; // Auto Calculate or Fixed track height
	struct Channel *Channel[4]; // Channel number
};

struct ScreenOut {
	bool UseIt; // use data from this channel for draw or not
	bool FirstUse; // use this TapePoint in first time (clear data if true)
	int Value1; // Maximum Value
	int Value2; // Value equal or less then Value1
	int Index; // Color Index of Value1 (maximum value)
	int Y; // tape vertical position (use for draw)
	int H; // from 1 to N (depends of h-zoom, more then one if real height greater defaults)
};
//----------------------------------------------------------------------------

class TapeConfig {
protected:
	xmlDoc *m_Doc;
	xmlNode *m_RootNode;

	std::vector<Channel> m_Channel;
	std::vector<Track> m_Track;
public:
	TapeConfig();
	virtual ~TapeConfig();

	int GetChannelCount(void);
	Channel GetChannel(int aIndex);

	int GetTrackCount(void);
	Track GetTrack(int aIndex);

	void ParseChannelItem(xmlNode *aNode);
	void DecodeChannelParam(xmlAttr *aParam);
	void ParseTrackItem(xmlNode *aNode);
	void DecodeTrackParam(xmlAttr *aParam);

	void LoadConfig();
};
//----------------------------------------------------------------------------

#define MAX_CHANNEL_COUNT	32
#define	MAX_TRACK_COUNT		10
#define	MAX_TAPE_HEIGHT		1024
#define	MAX_DATA_SIZE		4096

class RealTapeScroller {
protected:
	unsigned int *m_FB; // Frame Buffer
	unsigned int *m_DB; // Draw Buffer
	int m_W;
	int m_H;

	Channel m_Channel[MAX_CHANNEL_COUNT];
	Track m_Track[MAX_TRACK_COUNT];
	ScreenOut m_SO[MAX_TAPE_HEIGHT];
	ScreenOut *m_Index[MAX_DATA_SIZE]; // for each Data element

	class DataGenerator *m_DataGenerator;
	SDL_Texture *m_Txt;
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
//----------------------------------------------------------------------------

class BScanWnd: public Window {
protected:
	RealTapeScroller *m_RTS;
public:
	BScanWnd(SDL_Renderer *aRnd, int aX, int aY, int aW, int aH);
	virtual ~BScanWnd();

	virtual void Init(void);
	virtual void Done(void);

	virtual void PaintWindow(void);
};
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
//	Test routines stuff
//----------------------------------------------------------------------------
#define MAX_GEN_LINE	16

struct LineGenStruct {
	float DX;
	float CurX;
	int YCount;
	int YMaxCount;
	bool Valid;
};

class DataGenerator {
protected:
	std::vector<LineGenStruct> m_Gen;
	unsigned char m_OutData[4096];
public:
	DataGenerator();
	virtual ~DataGenerator();

	unsigned char *GetData(void);
};
//----------------------------------------------------------------------------

#endif /* SRC_BSCANWND_H_ */
