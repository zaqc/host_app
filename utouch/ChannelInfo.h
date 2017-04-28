/*
 * ChannelInfo.h
 *
 *  Created on: Mar 20, 2017
 *      Author: zaqc
 */

#ifndef CHANNELINFO_H_
#define CHANNELINFO_H_
//----------------------------------------------------------------------------

#include <string>
#include <vector>

#include <GLES2/gl2.h>
//----------------------------------------------------------------------------

class ChannelItem {
protected:
	int m_X;
	int m_Y;
	int m_W;
	int m_H;

	std::string m_Caption;
	bool m_Selected;
public:
	ChannelItem(int aX, int aY, int aW, int aH);
	virtual ~ChannelItem();

	void SetBkColor(float aR, float aG, float aB, float aA);
};
//----------------------------------------------------------------------------

enum ChannelDirection {
	cdUpLeft, cdUp, cdUpRight, cdDirect, cdDownLeft, cdDown, cdDownRight
};
//----------------------------------------------------------------------------

class ChannelGroup {
	// 45°↓
	// ↑↖↗°↘↙↓

	//   ↑70°
	// ↖70° ↗70°
	// ↖58° ↗58°
	// ↑45° ↓45°
	//   ⊚ 0°
	// ↙58° ↘58°
	// ↙70° ↘70°
	//   ↓70°
};
//----------------------------------------------------------------------------

struct ChannelParam {
	int m_ChNum;		// from 0 to 13

	bool m_Visible;

	int m_Side;			// left(up)/right(bottom)

	float m_FromTime;	// time of first tick
	float m_TickTime;	// time of one data element

	int m_DataOffset;	// position in data frame
	int m_DataSize;		// count of element from channel

	int m_ColorIndex;
};
//----------------------------------------------------------------------------

struct TrackParam {
	std::vector<ChannelParam *> m_Channel;

	float m_FromTime;	// show data from this time
	float m_TimeScale;	// default time scale

	bool m_UseIt;

	bool m_Fixed;			// use FixedHeight else calculate Height with use DefaultHeight
	int m_FixedHeight;

	int m_MinHeight;		// in pixel
	float m_DefaultHeight;	// in percent (default value is 100%, if track greater or less than others, value changes from this value)

	int m_YPos;				// in pixel
	int m_RealHeight;		// in pixel
};
//----------------------------------------------------------------------------

class TapeSchema {
private:
	std::vector<ChannelParam*> m_Channel;
	std::vector<TrackParam*> m_Track;
	unsigned short *m_PreCalc;
public:
	TapeSchema();
	virtual ~TapeSchema();

	void SimpleSchema(void);

	void AlignTracks(void);
	void PrepareSchema(void);
};
//----------------------------------------------------------------------------

class ChannelInfo {
protected:
	GLuint m_Prog;
	GLuint m_paramVertexPos;
	GLuint m_paramTexture;
	GLuint m_paramGlobalTime;
	GLuint m_paramLeftLevelMask;
	GLuint m_Text;
public:
	ChannelInfo();
	virtual ~ChannelInfo();

	void Render(void);
};
//----------------------------------------------------------------------------

#endif /* CHANNELINFO_H_ */
