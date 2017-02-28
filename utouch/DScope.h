/*
 * DScope.h
 *
 *  Created on: Jan 25, 2017
 *      Author: zaqc
 */

#ifndef DSCOPE_H_
#define DSCOPE_H_
//----------------------------------------------------------------------------

//#include "alt_types.h"
#define	alt_u32		unsigned int
//----------------------------------------------------------------------------

#define	ASCAN_CHANNEL		14
#define	CONTROL_CHANNEL		15

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

class DChannelBase {
protected:
	alt_u32 m_BaseAddr;
	int m_ChNum;

public:
	DChannelBase() {
		m_BaseAddr = 0;
		m_ChNum = 0;
	}
	DChannelBase(alt_u32 aBaseAddr, int aChNum);
	virtual ~DChannelBase();

	void send_cmd(alt_u32 aParamNum, alt_u32 aData);
};
//----------------------------------------------------------------------------

class DChannel: public DChannelBase {
protected:
	bool m_isEnable;
public:
	DChannel(alt_u32 aBaseAddr, int aChNum);
	virtual ~DChannel();

	virtual alt_u32 get_mpr1_value(void);
	void update_mpr1(void);

	void SetEanble(bool aEnable);
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
public:
	DLogChannel(alt_u32 aBaseAddr, int aChNum);
	virtual ~DLogChannel();

	virtual alt_u32 get_mpr1_value(void);

	void SetDataLen(int aDataLen);
	void SetADCAccum(int aADCAccum);
	void SetDelay(int aDelay);
	void SetADCOffset(int aADCOffset);
};
//----------------------------------------------------------------------------

class DAScan: public DChannel {
protected:
	int m_AScanChannel;
	int m_DataLen; // in 2 Shorts (32 bit) (default (2048 * 2 == 4096))
public:
	DAScan(alt_u32 aBaseAddr);
	virtual ~DAScan();

	virtual alt_u32 get_mpr1_value(void);

	void SetDataLen(int aDataLen);
	void SetAScanChannel(int aAScanChannel);
};
//----------------------------------------------------------------------------

class DPart: public DChannelBase {
protected:
	alt_u32 m_BaseAddr;
	bool m_LedOn;
	HightVoltage m_HV;
	bool m_PktCntrEnable;

public:
	DChannel *Channel[14];
	DAScan *AScan;

	DPart(alt_u32 aBaseAddr);
	virtual ~DPart();

	void update_mpr2(void);

	void SendPulseCmd(void);

	void LightOn(bool aLedOn);
	void SetHightVoltage(HightVoltage aHV);
	void EnablePktCntr(bool aEnable);

//	void ResetToDefaults(void);
};
//----------------------------------------------------------------------------

class DScope {
protected:
public:
	DPart *LFish;
	DPart *RFish;

	DScope();
	virtual ~DScope();

	void SetSyncType(SyncType aSyncType);
	void SetIntSyncPeriod(int aPeriod);
};
//----------------------------------------------------------------------------

#endif /* DSCOPE_H_ */
