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

	virtual int GetWidth(void);

	virtual void ProcessButton(unsigned int &aKey);
	virtual void Render(int aX, int aY);
};
//----------------------------------------------------------------------------

class MenuAScan {
protected:
	TextAScan *m_TextAScan;
	SwitchMenuItem *m_AScanType;
	IntMenuItem *m_Amp1;
	IntMenuItem *m_Amp2;
	IntMenuItem *m_VRC;
	IntMenuItem *m_Level;

	MenuItem *m_Selected;
	std::vector<MenuItem *> m_Items;
//	bool m_Log;	// or VGA
//	int m_LogLevel;
//	int m_Amp2;
//	int m_Vrc;
public:
	MenuAScan(TextAScan *aTextAScan);
	virtual ~MenuAScan();

	void UpdateControl(bool aLogType);

	void ProcessButton(unsigned int &aKey);
	void Render(int aX, int aY);
};
//----------------------------------------------------------------------------

class TextAScan {
protected:
	GLuint m_Prog;
	GLuint m_paramVertexPos;
	GLuint m_paramVertexColor;

	MenuAScan *m_Menu;
public:
	TextAScan();
	virtual ~TextAScan();

	void DrawLine(int aX1, int aY1, int aX2, int aY2);
	void FillRect(int aX1, int aY1, int aX2, int aY2, GLfloat *aColor = NULL);

	void ProcessButton(unsigned int &aKey);
	void DrawBuf(int aX1, int aY1, int aX2, int aY2, unsigned char *aBuf, int aSize);
};
//----------------------------------------------------------------------------

#endif /* UTOUCH_TEXTASCAN_H_ */
