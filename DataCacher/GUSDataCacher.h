#pragma once
#include "BaseDataCacher.h"
#include "USDataFileTypes.h"

namespace DataFile
{
#pragma pack(push, 1)
struct GusFrame
{
	unsigned int MasterFrame;
	unsigned short Speed;
	unsigned short Button;
	int MGData[5 * 2];
	unsigned int TuneParam[2 * US_CHANNEL_COUNT];
	unsigned char USData[US_TICK_COUNT  * US_CHANNEL_COUNT];
};

struct GusFileFrame : public BaseFrameHeader
{
	int MGData[5 * 2];
	unsigned int TuneParam[2 * US_CHANNEL_COUNT];
	unsigned char USData[US_TICK_COUNT  * US_CHANNEL_COUNT];
};
#pragma pack(pop)

class GUSDataCacher : public BaseDataCacher
{
private:
	unsigned char *m_ResBuf;
	DataBlockHolder *m_BlockHolder[US_CHANNEL_COUNT];
	DataBlockHolder *m_PrevBlockHolder[US_CHANNEL_COUNT];
	int m_VerMajor;
	int m_VerMinor;
public:
	GUSDataCacher(BaseDataFileReader *aFileReader);
	virtual ~GUSDataCacher(void);

	unsigned char * GetData2(int aFrame, int aChIndex);

	virtual void PreprocessData(unsigned char *aBuf);

	virtual void CreateIndex(DataBlockHolder *aBH);
};
}