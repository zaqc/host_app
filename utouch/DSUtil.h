/*
 * DSUtil.h
 *
 *  Created on: Mar 19, 2017
 *      Author: zaqc
 */

#ifndef DSUTIL_H_
#define DSUTIL_H_
//----------------------------------------------------------------------------

#include <vector>
//----------------------------------------------------------------------------

struct ChInfo {
	int m_Color;

	float m_StLevel;
	float m_StStart;
	float m_StEnd;

	float m_Alpha;
	float m_Beta;
	float m_Delay;
	float m_PhyDelay;
	float m_LevelAmp;		// amp for -6dB level
	float m_ConditionAmp;	// value for add to LevelAmp (from table, depence the type of the channel)
	float m_RealAmp;		// real amp
};
//----------------------------------------------------------------------------

struct TrackInfo {
	float m_StartTime;
	float m_EndTime;
	float m_Height;
};
//----------------------------------------------------------------------------

class ChGroup {
protected:
};
//----------------------------------------------------------------------------

class TapeConfig {
protected:
	std::vector<ChInfo*> m_ChInfo;

public:
	TapeConfig();
	virtual ~TapeConfig();
};
//----------------------------------------------------------------------------

#endif /* DSUTIL_H_ */
