/*
 * DScope.cpp
 *
 *  Created on: Jan 25, 2017
 *      Author: zaqc
 */
#include <unistd.h>

#include "DScope.h"

//#include "alt_types.h"

//#include "system.h"

//#include "altera_avalon_fifo.h"
//#include "altera_avalon_fifo_regs.h"
//#include "altera_avalon_fifo_util.h"
//----------------------------------------------------------------------------

//============================================================================
//	DChannelBase
//============================================================================

DChannelBase::DChannelBase(int aChNum) {
	m_ChNum = aChNum;
}
//----------------------------------------------------------------------------

DChannelBase::~DChannelBase() {
}
//----------------------------------------------------------------------------

uint32_t DChannelBase::send_cmd(uint32_t aParamNum, uint32_t aData) {
	uint32_t cmd;
	cmd = (m_ChNum << 28) & 0xF0000000; // Channel number
	cmd |= (aParamNum << 24) & 0x0F000000; // Parameter number
	cmd |= aData & 0x0000FFFF; // Command Extended Data

	return cmd;
}
//----------------------------------------------------------------------------

//============================================================================
//	DChannel
//============================================================================

DChannel::DChannel(int aChNum)
		: DChannelBase(aChNum) {
	m_isEnable = true;
}
//----------------------------------------------------------------------------

DChannel::~DChannel() {
}
//----------------------------------------------------------------------------

uint32_t DChannel::get_mpr1_value(void) {
	return m_isEnable ? 0 : MPR1_CH_DISABLE;
}
//----------------------------------------------------------------------------

uint32_t DChannel::update_mpr1(void) {
	return send_cmd(PARAM_MPR1, get_mpr1_value());
}
//----------------------------------------------------------------------------

uint32_t DChannel::SetEanble(bool aEnable) {
	m_isEnable = aEnable;
	return update_mpr1();
}
//----------------------------------------------------------------------------

//============================================================================
//	DLogChannel
//============================================================================

DLogChannel::DLogChannel(int aChNum)
		: DChannel(aChNum) {
	m_isEnable = true;
	m_isVGA = false;
	m_ADCAccum = 31;
	m_Delay = 0;
	m_PreAmp12dB = 0;
	m_ADCOffset = 81;
	m_DataLen = 32;
}
//----------------------------------------------------------------------------

DLogChannel::~DLogChannel() {
}
//----------------------------------------------------------------------------

uint32_t DLogChannel::get_mpr1_value(void) {
	uint32_t data = DChannel::get_mpr1_value();
	data |= m_isVGA ? MPR1_CH_TYPE_VGA : 0;
	data |= m_PreAmp12dB ? MPR1_PRE_AMP_12_dB : 0;
	return data;
}
//----------------------------------------------------------------------------

uint32_t DLogChannel::SetDataLen(int aDataLen) {
	m_DataLen = (aDataLen >> 2) << 2;
	return send_cmd(PARAM_DATA_LEN, m_DataLen >> 2);
}
//----------------------------------------------------------------------------

uint32_t DLogChannel::SetADCAccum(int aADCAccum) {
	return send_cmd(PARAM_ACCUM, aADCAccum);
}
//----------------------------------------------------------------------------

uint32_t DLogChannel::SetDelay(int aDelay) {
	return send_cmd(PARAM_DELAY, aDelay);
}
//----------------------------------------------------------------------------

uint32_t DLogChannel::SetADCOffset(int aADCOffset) {
	m_ADCOffset = (aADCOffset < 0) ? 0 : ((aADCOffset > 255) ? 255 : aADCOffset);
	return send_cmd(PARAM_LOG_OFFSET, m_ADCOffset | (m_ADCOffset << 8));
}
//----------------------------------------------------------------------------

//============================================================================
//	DAScan
//============================================================================

DAScan::DAScan()
		: DChannel(ASCAN_CHANNEL) {
	m_DataLen = 256;
	m_AScanChannel = 0;
}
//----------------------------------------------------------------------------

DAScan::~DAScan() {
}
//----------------------------------------------------------------------------

uint32_t DAScan::get_mpr1_value(void) {
	uint32_t data = DChannel::get_mpr1_value();
	data |= MPR1_ASCAN_CH(m_AScanChannel);
	return data;
}
//----------------------------------------------------------------------------

uint32_t DAScan::SetDataLen(int aDataLen) {
	m_DataLen = (aDataLen >> 1) << 1;
	return send_cmd(PARAM_DATA_LEN, m_DataLen >> 1);
}
//----------------------------------------------------------------------------

uint32_t DAScan::SetAScanChannel(int aAScanChannel) {
	m_AScanChannel = aAScanChannel;
	return update_mpr1();
}
//----------------------------------------------------------------------------

//============================================================================
//	DPart
//============================================================================

DPart::DPart(unsigned char *aAddr)
		: DChannelBase(CONTROL_CHANNEL) {

	m_LedOn = false;
	m_HV = hvOff;
	m_PktCntrEnable = true;

	for (int i = 0; i < 14; i++)
		Channel[i] = new DLogChannel(i);

	AScan = new DAScan();
}
//----------------------------------------------------------------------------

DPart::~DPart() {
	delete AScan;
	for (int i = 0; i < 14; i++)
		delete Channel[i];
}
//----------------------------------------------------------------------------

uint32_t DPart::update_mpr2(void) {
	uint32_t data = (m_LedOn ? MPR2_LED_BIT : 0) | (m_PktCntrEnable ? MPR2_COUNTER_EN : 0);
	switch (m_HV) {
		case hvOff:
			data |= MPR2_HV_OFF;
			break;
		case hv30_1:
			data |= MPR2_HV_30_1;
			break;
		case hv30_2:
			data |= MPR2_HV_30_2;
			break;
		case hv60:
			data |= MPR2_HV_60;
			break;
	}

	return send_cmd(PARAM_MPR2, data);
}
//----------------------------------------------------------------------------

uint32_t DPart::SendPulseCmd(void) {
	return send_cmd(PARAM_SEND_SYNC, 0);
}
//----------------------------------------------------------------------------

uint32_t DPart::LightOn(bool aLedOn) {
	m_LedOn = aLedOn;
	return update_mpr2();
}
//----------------------------------------------------------------------------

uint32_t DPart::SetHightVoltage(HightVoltage aHV) {
	m_HV = aHV;
	return update_mpr2();
}
//----------------------------------------------------------------------------

uint32_t DPart::EnablePktCntr(bool aEnable) {
	m_PktCntrEnable = aEnable;
	return update_mpr2();
}
//----------------------------------------------------------------------------

//============================================================================
//	DScope
//============================================================================

DScope::DScope() {
	unsigned char ls[] = { 0xFF, 0x00, 0x12, 0xDE };
	LFish = new DPart(ls);
	unsigned char rs[] = { 0XFF, 0x00, 0xF0, 0x72 };
	RFish = new DPart(rs);
}
//----------------------------------------------------------------------------

DScope::~DScope() {
}
//----------------------------------------------------------------------------

uint32_t DScope::SetSyncType(SyncType aSyncType) {
	return 0;
}
//----------------------------------------------------------------------------

uint32_t DScope::SetIntSyncPeriod(int aPeriod) {
	return 0;
}
//----------------------------------------------------------------------------
