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
	int m_ChNum;

public:
	DChannelBase() {
		m_ChNum = 0;
	}
	DChannelBase(int aChNum);
	virtual ~DChannelBase();

	uint32_t send_cmd(uint32_t aParamNum, uint32_t aData);
};
//----------------------------------------------------------------------------

class DChannel: public DChannelBase {
protected:
	bool m_isEnable;
public:
	DChannel(int aChNum);
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
public:
	DLogChannel(int aChNum);
	virtual ~DLogChannel();

	virtual uint32_t get_mpr1_value(void);

	uint32_t SetDataLen(int aDataLen);
	uint32_t SetADCAccum(int aADCAccum);
	uint32_t SetDelay(int aDelay);
	uint32_t SetADCOffset(int aADCOffset);
};
//----------------------------------------------------------------------------

class DAScan: public DChannel {
protected:
	int m_AScanChannel;
	int m_DataLen; // in 2 Shorts (32 bit) (default (2048 * 2 == 4096))
public:
	DAScan();
	virtual ~DAScan();

	virtual uint32_t get_mpr1_value(void);

	uint32_t SetDataLen(int aDataLen);
	uint32_t SetAScanChannel(int aAScanChannel);
};
//----------------------------------------------------------------------------

class DPart: public DChannelBase {
protected:
	bool m_LedOn;
	HightVoltage m_HV;
	bool m_PktCntrEnable;

public:
	DChannel *Channel[14];
	DAScan *AScan;

	DPart();
	virtual ~DPart();

	uint32_t update_mpr2(void);

	uint32_t SendPulseCmd(void);

	uint32_t LightOn(bool aLedOn);
	uint32_t SetHightVoltage(HightVoltage aHV);
	uint32_t EnablePktCntr(bool aEnable);
};
//----------------------------------------------------------------------------

class DScope {
protected:
public:
	DPart *LFish;
	DPart *RFish;

	DScope();
	virtual ~DScope();

	uint32_t SetSyncType(SyncType aSyncType);
	uint32_t SetIntSyncPeriod(int aPeriod);
};
//----------------------------------------------------------------------------

#endif /* DSCOPE_H_ */
