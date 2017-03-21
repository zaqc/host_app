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
	//↑↖↗°↘↙↓

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

class ChannelInfo {
public:
	ChannelInfo();
	virtual ~ChannelInfo();
};
//----------------------------------------------------------------------------

#endif /* CHANNELINFO_H_ */
