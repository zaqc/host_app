/*
 * DScope.cpp
 *
 *  Created on: Jan 25, 2017
 *      Author: zaqc
 */
#include <unistd.h>
#include <stdint.h>
#include <memory.h>
#include <fcntl.h>

#include <ftdi.h>

#include "DScope.h"
//----------------------------------------------------------------------------

DScope *dscope = NULL;
//----------------------------------------------------------------------------

//============================================================================
//	DChannelBase
//============================================================================

DChannelBase::DChannelBase(DScope *aDScope, DPart *aPart, int aChNum) {
	m_DScope = aDScope;
	m_Part = aPart;
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

	unsigned char buf[16];
	memset(buf, 0, sizeof(buf));
	buf[0] = 0x55;
	buf[1] = 0x55;
	buf[2] = 0x55;
	buf[3] = 0xD5;
	m_Part->GetPartID(&buf[4]);
	buf[8] = (cmd >> 24) & 0xFF;
	buf[9] = (cmd >> 16) & 0xFF;
	buf[10] = (cmd >> 8) & 0xFF;
	buf[11] = cmd & 0xFF;

	m_DScope->SendBuffer(buf, 16);

	return cmd;
}
//----------------------------------------------------------------------------

//============================================================================
//	DChannel
//============================================================================

DChannel::DChannel(DScope *aDScope, DPart *aPart, int aChNum)
		: DChannelBase(aDScope, aPart, aChNum) {
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

DLogChannel::DLogChannel(DScope *aDScope, DPart *aPart, int aChNum)
		: DChannel(aDScope, aPart, aChNum) {
	m_isEnable = true;
	m_isVGA = false;
	m_ADCAccum = 31;
	m_Delay = 0;
	m_PreAmp12dB = 0;
	m_ADCOffset = 102;
	m_DataLen = 32;

	m_Amp1 = 48;
	m_Amp2 = 53;
	m_VRC = 5;
}
//----------------------------------------------------------------------------

DLogChannel::~DLogChannel() {
}
//----------------------------------------------------------------------------

void DLogChannel::Load(int aFile){
	int val;
	read(aFile, &val, sizeof(int));
	SetADCAccum(val);
	read(aFile, &val, sizeof(int));
	SetDelay(val);
	read(aFile, &val, sizeof(int));
	SetADCOffset(val);

	read(aFile, &m_Amp1, sizeof(int));
	read(aFile, &m_Amp2, sizeof(int));
	read(aFile, &m_VRC, sizeof(int));
}
//----------------------------------------------------------------------------

void DLogChannel::Save(int aFile){
	write(aFile, &m_ADCAccum, sizeof(int));
	write(aFile, &m_Delay, sizeof(int));
	write(aFile, &m_ADCOffset, sizeof(int));

	write(aFile, &m_Amp1, sizeof(int));
	write(aFile, &m_Amp2, sizeof(int));
	write(aFile, &m_VRC, sizeof(int));
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
	m_ADCAccum = aADCAccum;
	return send_cmd(PARAM_ACCUM, aADCAccum);
}
//----------------------------------------------------------------------------

uint32_t DLogChannel::SetDelay(int aDelay) {
	m_Delay = aDelay;
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

DAScan::DAScan(DScope *aDScope, DPart *aPart)
		: DChannel(aDScope, aPart, ASCAN_CHANNEL) {
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

DPart::DPart(DScope *aDScope, unsigned char *aAddr)
		: DChannelBase(aDScope, this, CONTROL_CHANNEL) {

	m_LedOn = false;
	m_HV = hvOff;
	m_PktCntrEnable = true;

	*(uint32_t*) m_Addr = *(uint32_t*) aAddr;

	for (int i = 0; i < 14; i++)
		Channel[i] = new DLogChannel(aDScope, this, i);

	AScan = new DAScan(aDScope, this);
}
//----------------------------------------------------------------------------

DPart::~DPart() {
	delete AScan;
	for (int i = 0; i < 14; i++)
		delete Channel[i];
}
//----------------------------------------------------------------------------

void DPart::Load(int aFile) {
	for(int i = 0; i < 14; i++)
		Channel[i]->Load(aFile);
}
//----------------------------------------------------------------------------

void DPart::Save(int aFile) {
	for(int i = 0; i < 14; i++)
		Channel[i]->Save(aFile);
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

void DPart::GetPartID(unsigned char *aID) {
	memcpy(aID, m_Addr, 4);
}
//----------------------------------------------------------------------------

//============================================================================
//	DScope
//============================================================================

DScope::DScope(ftdi_context *aFTDI) {
	m_FTDI = aFTDI;
	unsigned char ls[] = { 0xFF, 0x00, 0x12, 0xDE };
	LFish = new DPart(this, ls);
	unsigned char rs[] = { 0XFF, 0x00, 0xF0, 0x72 };
	RFish = new DPart(this, rs);

	mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
	int f = open((char *) "./config.dat", O_RDWR, mode);
	if (f != -1) {
		LFish->Load(f);
		RFish->Load(f);
		close(f);
	}
}
//----------------------------------------------------------------------------

DScope::~DScope() {
	mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
	int f = open("./config.dat", O_RDWR | O_CREAT, mode);
	if (f != -1) {
		LFish->Save(f);
		RFish->Save(f);
		close(f);
	}

	delete RFish;
	delete LFish;
}
//----------------------------------------------------------------------------

void DScope::SendBuffer(unsigned char *aBuf, int aSize) {
	if (m_FTDI)
		ftdi_write_data(m_FTDI, aBuf, aSize);
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
