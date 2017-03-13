/*
 * TextAScan.h
 *
 *  Created on: Mar 6, 2017
 *      Author: zaqc
 */

#ifndef UTOUCH_TEXTASCAN_H_
#define UTOUCH_TEXTASCAN_H_
//----------------------------------------------------------------------------

#include <vector>
#include <GLES2/gl2.h>
#include "DScopeStream.h"
//----------------------------------------------------------------------------

class TextAScan;
//----------------------------------------------------------------------------

class MenuItem {
protected:
	TextAScan *m_TextAScan;
	bool m_Selected;
	bool m_IsChanged;
public:
	MenuItem(TextAScan *aTextAScan);
	virtual ~MenuItem();

	virtual void SetSelected(bool aSelected);
	virtual int GetWidth(void);
	virtual void ProcessButton(unsigned int &aKey);
	virtual void Render(int aX, int aY);

	virtual bool IsChanged(void);
};
//----------------------------------------------------------------------------

class SwitchMenuItem: public MenuItem {
protected:
	char *m_Caption;
	char *m_I1;
	char *m_I2;
	int m_ItemNumber;
public:
	SwitchMenuItem(TextAScan *aTextAScan, char *aCaption, char* aI1, char* aI2);
	virtual ~SwitchMenuItem();

	virtual int GetWidth(void);
	int SelectedItem(void);

	virtual void ProcessButton(unsigned int &aKey);
	virtual void Render(int aX, int aY);
};
//----------------------------------------------------------------------------

class IntMenuItem: public MenuItem {
protected:
	char *m_Caption;
	int m_Val;
	int m_Min;
	int m_Max;
	unsigned int m_PrevKey;
	int m_HoldTickCount;
public:
	IntMenuItem(TextAScan *aTextAScan, char *aCaption, int aVal, int aMin, int aMax);
	virtual ~IntMenuItem();

	virtual int GetValue(void);
	virtual void SetValue(int aVal, bool aIsChanged = false);

	virtual int GetWidth(void);

	virtual void ProcessButton(unsigned int &aKey);
	virtual void Render(int aX, int aY);
};
//----------------------------------------------------------------------------

class MenuAScan {
protected:
	TextAScan *m_TextAScan;
	SwitchMenuItem *m_Side;
	IntMenuItem *m_ChNumber;
	SwitchMenuItem *m_AScanType;
	IntMenuItem *m_Delay;
	IntMenuItem *m_ADCAccum;
	IntMenuItem *m_Amp1;
	IntMenuItem *m_Amp2;
	IntMenuItem *m_VRC;
	IntMenuItem *m_Level;

	MenuItem *m_Selected;
	std::vector<MenuItem *> m_Items;

	int m_AmpDelta;

	float m_VRCTab[128];
	float m_LogTab[256];

	bool m_ShowAllChannel;
public:
	MenuAScan(TextAScan *aTextAScan);
	virtual ~MenuAScan();

	int GetSide(void);
	int GetChNumber(void);
	bool GetShowAllChannel(void);

	bool IsLogView(void);

	void UpdateControl(bool aLogType);

	void ProcessButton(unsigned int &aKey);
	virtual void HandleEvent(void);

	void FillVRC(void);
	void CalcVGA(float *aVal, unsigned char *aBuf, int aSize);

	void Render(int aX, int aY);
};
//----------------------------------------------------------------------------

class TextAScan {
protected:
	GLuint m_Prog;
	GLuint m_paramVertexPos;
	GLuint m_paramVertexColor;

	GLfloat *m_V;	// vertex
	GLfloat *m_C;	// color
	GLushort *m_Ndx;	// line index
	int m_PtCount;	// Point count

	MenuAScan *m_Menu;
public:
	TextAScan();
	virtual ~TextAScan();

	void DrawLine(int aX1, int aY1, int aX2, int aY2, GLfloat *aColor = NULL);
	void FillRect(int aX1, int aY1, int aX2, int aY2, GLfloat *aColor = NULL);

	void ProcessButton(unsigned int &aKey);

	void CalcAScan(int aX1, int aY1, int aX2, int aY2, unsigned char *aBuf, int aSize);

	void DrawBuf(DScopeStream *aDSS, int aX1, int aY1, int aX2, int aY2);
	void DrawGrid(int aX1, int aY1, int aX2, int aY2, bool aLog);
};
//----------------------------------------------------------------------------

#endif /* UTOUCH_TEXTASCAN_H_ */
