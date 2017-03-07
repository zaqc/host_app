#include "GUSDataCacher.h"

using namespace DataFile;

GUSDataCacher::GUSDataCacher(BaseDataFileReader *aFileReader)
	: BaseDataCacher(aFileReader, 30)
{
	aFileReader->GetFileVersion(m_VerMajor, m_VerMinor);
    int buf_size = aFileReader->GetMaxFrameCount() * aFileReader->GetFrameSize();
    m_ResBuf = new unsigned char [buf_size];
    for(int i = 0; i < US_CHANNEL_COUNT; i++)
    {
		m_BlockHolder[i] = NULL;
        m_PrevBlockHolder[i] = NULL;
    }
}


GUSDataCacher::~GUSDataCacher(void)
{
    delete [] m_ResBuf;
}


unsigned char * GUSDataCacher::GetData2(int aFrame, int aChIndex)
{
	if(aChIndex >= 0 && aChIndex < US_CHANNEL_COUNT)
	{
		DataBlockHolder *bh = m_BlockHolder[aChIndex];
    	if(bh != NULL)
        {
        	if(bh->m_pDataBlock->FrameInBlock(aFrame))
            {
				return bh->m_ppDataBufIndex[aFrame - bh->m_pDataBlock->m_FromFrame];
            }
        }
        bh = m_PrevBlockHolder[aChIndex];
		if(bh != NULL)
        {
        	if(bh->m_pDataBlock->FrameInBlock(aFrame))
            {
            	m_PrevBlockHolder[aChIndex] = m_BlockHolder[aChIndex];
                m_BlockHolder[aChIndex] = bh;
            	return bh->m_ppDataBufIndex[aFrame - bh->m_pDataBlock->m_FromFrame];
            }
        }

		unsigned char *ptr = GetData(aFrame);
        if(ptr != NULL)
        {
        	m_PrevBlockHolder[aChIndex] = m_BlockHolder[aChIndex];
			m_BlockHolder[aChIndex] = m_pBlockCurr;
            return ptr;
        }
    }
	return NULL;
}
//---------------------------------------------------------------------------

void GUSDataCacher::PreprocessData(unsigned char *aBuf)
{

}
//---------------------------------------------------------------------------

void GUSDataCacher::CreateIndex(DataBlockHolder *aBH)
{
	unsigned char *src = aBH->m_pDataBuf;
	unsigned char *dst = m_ResBuf;//aBH->FBuf;
	GusFrame *gf = (GusFrame *)dst;

	unsigned int tp[US_CHANNEL_COUNT * 2];
	memset(tp, 0, sizeof(unsigned int) * US_CHANNEL_COUNT * 2);

	for(int n = 0; n < aBH->m_pDataBlock->m_FrameCount; n++)
	{
		aBH->m_pMFIndex[n] = gf->MasterFrame = *(unsigned int*)src; src += sizeof(unsigned int);
		gf->Speed = *(unsigned short*)src; src += sizeof(unsigned short);
		gf->Button = *(unsigned short*)src; src += sizeof(unsigned short);
		if(m_VerMajor * 10 + m_VerMinor >= 21)
			src += sizeof(unsigned int);
		memcpy(gf->MGData, src, sizeof(int) * 10); src += sizeof(int) * 10;


		unsigned int ch_mask = *(unsigned int*)src; src += sizeof(unsigned int);
		unsigned int mask = 1;
		unsigned char *us_ptr = gf->USData;
		for(int i = 0; i < US_CHANNEL_COUNT; i++)
		{
			if(ch_mask & mask)
			{
				memcpy(us_ptr, src, US_TICK_COUNT);
				src += US_TICK_COUNT;
			}
			else
			{
				memset(us_ptr, 0, US_TICK_COUNT);
			}
			us_ptr += US_TICK_COUNT;
			mask <<= 1;
		}

		mask = 1;
		unsigned int tp_mask = *(unsigned int*)src; src += sizeof(unsigned int);
		for(int i = 0; i < US_CHANNEL_COUNT; i++)
		{
			if(tp_mask & mask)
			{
				tp[i * 2] = *(unsigned int*)src; src += sizeof(unsigned int);
				tp[i * 2 + 1] = *(unsigned int*)src; src += sizeof(unsigned int);
			}
			mask <<= 1;
		}
		memcpy(gf->TuneParam, tp, sizeof(gf->TuneParam));

		PreprocessData((unsigned char *)gf);


		aBH->m_ppDataBufIndex[n] = (unsigned char*)gf;
		gf++;
	}

	unsigned char *tmp = aBH->m_pBuf;
	aBH->m_pBuf = aBH->m_pDataBuf = m_ResBuf;
	m_ResBuf = tmp;
}
