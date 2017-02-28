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

DChannelBase::DChannelBase(alt_u32 aBaseAddr, int aChNum) {
	m_BaseAddr = aBaseAddr;
	m_ChNum = aChNum;
}
//----------------------------------------------------------------------------

DChannelBase::~DChannelBase() {
}
//----------------------------------------------------------------------------

void DChannelBase::send_cmd(alt_u32 aParamNum, alt_u32 aData) {
	alt_u32 cmd;
	cmd = (m_ChNum << 28) & 0xF0000000; // Channel number
	cmd |= (aParamNum << 24) & 0x0F000000; // Parameter number
	cmd |= aData & 0x0000FFFF; // Command Extended Data
/*
	int level = IORD_ALTERA_AVALON_FIFO_LEVEL(m_BaseAddr);
	IOWR_ALTERA_AVALON_FIFO_DATA(m_BaseAddr, cmd);
*/
}
//----------------------------------------------------------------------------

//============================================================================
//	DChannel
//============================================================================

DChannel::DChannel(alt_u32 aBaseAddr, int aChNum) :
		DChannelBase(aBaseAddr, aChNum) {
	m_isEnable = true;
}
//----------------------------------------------------------------------------

DChannel::~DChannel() {
}
//----------------------------------------------------------------------------

alt_u32 DChannel::get_mpr1_value(void) {
	return m_isEnable ? 0 : MPR1_CH_DISABLE;
}
//----------------------------------------------------------------------------

void DChannel::update_mpr1(void) {
	send_cmd(PARAM_MPR1, get_mpr1_value());
}
//----------------------------------------------------------------------------

void DChannel::SetEanble(bool aEnable) {
	m_isEnable = aEnable;
	update_mpr1();
}
//----------------------------------------------------------------------------

//============================================================================
//	DLogChannel
//============================================================================
DLogChannel::DLogChannel(alt_u32 aBaseAddr, int aChNum) :
		DChannel(aBaseAddr, aChNum) {
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

alt_u32 DLogChannel::get_mpr1_value(void) {
	alt_u32 data = DChannel::get_mpr1_value();
	data |= m_isVGA ? MPR1_CH_TYPE_VGA : 0;
	data |= m_PreAmp12dB ? MPR1_PRE_AMP_12_dB : 0;
	return data;
}
//----------------------------------------------------------------------------

void DLogChannel::SetDataLen(int aDataLen) {
	m_DataLen = (aDataLen >> 2) << 2;
	send_cmd(PARAM_DATA_LEN, m_DataLen >> 2);
}
//----------------------------------------------------------------------------

void DLogChannel::SetADCAccum(int aADCAccum) {
	send_cmd(PARAM_ACCUM, aADCAccum);
}
//----------------------------------------------------------------------------

void DLogChannel::SetDelay(int aDelay) {
	send_cmd(PARAM_DELAY, aDelay);
}
//----------------------------------------------------------------------------

void DLogChannel::SetADCOffset(int aADCOffset) {
	m_ADCOffset =
			(aADCOffset < 0) ? 0 : ((aADCOffset > 255) ? 255 : aADCOffset);
	send_cmd(PARAM_LOG_OFFSET, m_ADCOffset | (m_ADCOffset << 8));
}
//----------------------------------------------------------------------------

//============================================================================
//	DAScan
//============================================================================
DAScan::DAScan(alt_u32 aBaseAddr) :
		DChannel(aBaseAddr, ASCAN_CHANNEL) {
}

DAScan::~DAScan() {
}

alt_u32 DAScan::get_mpr1_value(void) {
	alt_u32 data = DChannel::get_mpr1_value();
	data |= MPR1_ASCAN_CH(m_AScanChannel);
	return data;
}
//----------------------------------------------------------------------------

void DAScan::SetDataLen(int aDataLen) {
	m_DataLen = (aDataLen >> 1) << 1;
	send_cmd(PARAM_DATA_LEN, m_DataLen >> 1);
}
//----------------------------------------------------------------------------

void DAScan::SetAScanChannel(int aAScanChannel) {
	m_AScanChannel = aAScanChannel;
	update_mpr1();
}
//----------------------------------------------------------------------------

//============================================================================
//	DPart
//============================================================================

DPart::DPart(alt_u32 aBaseAddr) :
		DChannelBase(aBaseAddr, CONTROL_CHANNEL) {

	for (int i = 0; i < 14; i++)
		Channel[i] = new DLogChannel(aBaseAddr, i);

	AScan = new DAScan(aBaseAddr);
}
//----------------------------------------------------------------------------

DPart::~DPart() {
	delete AScan;
	for (int i = 0; i < 14; i++)
		delete Channel[i];
}
//----------------------------------------------------------------------------

void DPart::update_mpr2(void) {
	alt_u32 data = (m_LedOn ? MPR2_LED_BIT : 0)
			| (m_PktCntrEnable ? MPR2_COUNTER_EN : 0);
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

	send_cmd(PARAM_MPR2, data);
}
//----------------------------------------------------------------------------

void DPart::SendPulseCmd(void) {
	send_cmd(PARAM_SEND_SYNC, 0);
}
//----------------------------------------------------------------------------

void DPart::LightOn(bool aLedOn) {
	m_LedOn = aLedOn;
	update_mpr2();
}
//----------------------------------------------------------------------------

void DPart::SetHightVoltage(HightVoltage aHV) {
	m_HV = aHV;
	update_mpr2();
}
//----------------------------------------------------------------------------

void DPart::EnablePktCntr(bool aEnable) {
	m_PktCntrEnable = aEnable;
	update_mpr2();
}
//----------------------------------------------------------------------------

//============================================================================
//	DScope
//============================================================================
DScope::DScope() {
	LFish = new DPart(0);//CMD_FIFO_BASE);
}
//----------------------------------------------------------------------------

DScope::~DScope() {
}
//----------------------------------------------------------------------------

void DScope::SetSyncType(SyncType aSyncType) {

}
//----------------------------------------------------------------------------

void DScope::SetIntSyncPeriod(int aPeriod) {

}
//----------------------------------------------------------------------------
