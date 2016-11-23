/*
 * DScope.h
 *
 *  Created on: Nov 22, 2016
 *      Author: zaqc
 */
#ifndef DSCOPE_H_
#define DSCOPE_H_
//----------------------------------------------------------------------------

#include <string.h>
//----------------------------------------------------------------------------

void DScopeInit(void);
void DScopeDone(void);
//----------------------------------------------------------------------------

class DScopeChannel{
public:
	std::wstring m_Caption;		// л1..л14 п1..п14
	std::wstring m_ShortTxt;	// 0°, 45°/0°, 58°/34°, 70°/0° - H или O <- ->
	std::wstring m_LongTxt;		// Левая сторона, Канал №1, Угол ввода 45°, Разворот 0°
	DScopeChannel();
	virtual ~DScopeChannel();

	void Load(void);
};
//----------------------------------------------------------------------------

class DScope {
public:
	DScope();
	virtual ~DScope();
};
//----------------------------------------------------------------------------

extern DScope *g_DScope;
//----------------------------------------------------------------------------

#endif /* DSCOPE_H_ */
