/*
 * DScope.h
 *
 *  Created on: Jan 25, 2017
 *      Author: zaqc
 */

#ifndef DSCOPE_H_
#define DSCOPE_H_
//----------------------------------------------------------------------------

#include <stdint.h>

#include <ftdi.h>
//----------------------------------------------------------------------------

#define	ASCAN_CHANNEL		14
#define	CONTROL_CHANNEL		15

#define	PARAM_PULSE			3
#define PARAM_DELAY			4
#define PARAM_ACCUM			5
#define PARAM_DATA_LEN		6
#define PARAM_LOG_OFFSET	7

#define PARAM_MPR1			8
#define MPR1_PRE_AMP_12_dB	1
#define	MPR1_CH_TYPE_VGA	(1 << 1)
#define	MPR1_CH_DISABLE		(1 << 2)
#define MPR1_ASCAN_CH(n)	(n << 4)

#define PARAM_MPR2			13
#define	MPR2_LED_BIT		1
#define MPR2_HV_OFF			(3 << 1)
#define MPR2_HV_30_1		(1 << 1)
#define MPR2_HV_30_2		(2 << 1)
#define MPR2_HV_60			(0)
#define MPR2_COUNTER_EN		(1 << 3)

#define PARAM_SEND_SYNC		14
#define	PARAM_SET_DEFAULTS	15			// reset all channels parameters to default value
//----------------------------------------------------------------------------

enum SyncType {
	stSyncOff, stSyncExt, stSyncInt
};
//----------------------------------------------------------------------------

enum ScopeSide {
	ssNone, ssLeft, ssRight, ssBoth
};
//----------------------------------------------------------------------------

enum HightVoltage {
	hvOff, hv30_1, hv30_2, hv60
};
//----------------------------------------------------------------------------

class DScope;
class DPart;
//----------------------------------------------------------------------------

class DChannelBase {
protected:
	int m_ChNum;
	DScope *m_DScope;
	DPart *m_Part;
	uint32_t send_cmd(uint32_t aParamNum, uint32_t aData);
public:
	DChannelBase(DScope *aDScope, DPart *aPart, int aChNum);
	virtual ~DChannelBase();

	virtual void Load(int aFile) {
	}
	virtual void Save(int aFile) {
	}
};
//----------------------------------------------------------------------------

class DChannel: public DChannelBase {
protected:
	bool m_isEnable;
public:
	DChannel(DScope *aDScope, DPart *aPart, int aChNum);
	virtual ~DChannel();

	virtual uint32_t get_mpr1_value(void);
	uint32_t update_mpr1(void);

	uint32_t SetEanble(bool aEnable);
};
//----------------------------------------------------------------------------

class DLogChannel: public DChannel {
protected:
	bool m_isVGA;
	int m_DataLen; // in 4 bytes (32 bit) (default 32 (32 * 4 == 128))
	int m_ADCAccum; // adc_tick = 25 MHz (default = 31(+1) (40 nSec * 32 == 1.28 nSec)
	int m_Delay;
	int m_PreAmp12dB;
	int m_ADCOffset;
	
	int m_PulseCount;
	int m_Jamming;

	int m_Amp1; // virtual param (for VGA scan view)
	int m_Amp2;
	int m_AmpDelta;
	int m_VRC;
public:
	DLogChannel(DScope *aDScope, DPart *aPart, int aChNum);
	virtual ~DLogChannel();

	virtual void Load(int aFile);
	virtual void Save(int aFile);

	virtual uint32_t get_mpr1_value(void);

	uint32_t SetDataLen(int aDataLen);
	uint32_t SetADCAccum(int aADCAccum);
	uint32_t SetPulseParam(int aPulseCount, int aJamming);
	uint32_t SetDelay(int aDelay);
	uint32_t SetADCOffset(int aADCOffset);

	void SetAmp1(int aAmp1) {
		m_Amp1 = aAmp1;
	}

	void SetAmp2(int aAmp2) {
		m_Amp2 = aAmp2;
	}

	void SetAmpDelta(int aAmpDelta) {
		m_AmpDelta = aAmpDelta;
	}

	void SetVRC(int aVRC) {
		m_VRC = aVRC;
	}

	int GetAmp1(void) {
		return m_Amp1;
	}

	int GetAmp2(void) {
		return m_Amp2;
	}

	int GetAmpDelta(void) {
		return m_AmpDelta;
	}

	int GetVRC(void) {
		return m_VRC;
	}

	int GetADCAccum(void) {
		return m_ADCAccum;
	}

	int GetADCOffset(void) {
		return m_ADCOffset;
	}

	int GetDelay(void) {
		return m_Delay;
	}
};
//----------------------------------------------------------------------------

class DAScan: public DChannel {
protected:
	int m_AScanChannel;
	int m_DataLen; // in 2 Shorts (32 bit) (default (2048 * 2 == 4096))
public:
	DAScan(DScope *aDScope, DPart *aPart);
	virtual ~DAScan();

	virtual uint32_t get_mpr1_value(void);

	uint32_t SetDataLen(int aDataLen);
	uint32_t SetAScanChannel(int aAScanChannel);
};
//----------------------------------------------------------------------------

class DPart: public DChannelBase {
protected:
	unsigned char m_Addr[4];
	bool m_LedOn;
	HightVoltage m_HV;
	bool m_PktCntrEnable;
public:
	DLogChannel *Channel[14];
	DAScan *AScan;

	DPart(DScope *aDScope, unsigned char *aAddr);
	virtual ~DPart();

	virtual void Load(int aFile);
	virtual void Save(int aFile);

	uint32_t update_mpr2(void);

	uint32_t SendPulseCmd(void);

	uint32_t LightOn(bool aLedOn);
	uint32_t SetHightVoltage(HightVoltage aHV);
	uint32_t EnablePktCntr(bool aEnable);

	void GetPartID(unsigned char *aID);
};
//----------------------------------------------------------------------------

class DScope {
protected:
	ftdi_context *m_FTDI;

public:
	DPart *LFish;
	DPart *RFish;

	DScope(ftdi_context *aFTDI);
	virtual ~DScope();

	//void LoadDefaults()

	void LoadParam(void);
	void SaveParam(void);

	virtual void SendBuffer(unsigned char *aBuf, int aSize);

	uint32_t SetSyncType(SyncType aSyncType);
	uint32_t SetIntSyncPeriod(int aPeriod);
};
//----------------------------------------------------------------------------

extern DScope *dscope;
//----------------------------------------------------------------------------

#endif /* DSCOPE_H_ */
