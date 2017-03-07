#include "BaseDataCacher.h"
#include <zlib.h>
#include <new>
#include <thread>

using namespace DataFile;

DataBlockHolder::DataBlockHolder(DataType aDataType, int aMaxFrameCount, int aFrameSize)
{
	m_pBuf = m_pDataBuf = new unsigned char [aMaxFrameCount * aFrameSize];
	m_pDataBlock		= nullptr;
	m_ppDataBufIndex	= nullptr;
	m_pMFIndex			= nullptr;
	m_FrameLengthInMF	= nullptr;
	m_ppDataBufIndex	= new unsigned char* [aMaxFrameCount];
	m_pMFIndex			= new unsigned int [aMaxFrameCount];
	m_FrameLengthInMF	= new int [aMaxFrameCount];
	m_Time				= 0;
}


DataBlockHolder::~DataBlockHolder(void)
{
	//Log( L"Запуск ~DataBlockHolder" );
	if( m_pMFIndex != nullptr) { delete [] m_pMFIndex; m_pMFIndex = nullptr; }
	if( m_FrameLengthInMF != nullptr) { delete[] m_FrameLengthInMF; m_FrameLengthInMF = nullptr;}
	if( m_ppDataBufIndex != nullptr) { delete [] m_ppDataBufIndex; m_ppDataBufIndex = nullptr; }
	if( m_pBuf != nullptr) { delete [] m_pBuf; m_pBuf = nullptr; }
}

BaseDataCacher::BaseDataCacher(BaseDataFileReader *aDataFile, int aCacheBlockCount)
	: m_ppCache(nullptr)
	, m_pZippedDataBuf(nullptr)
	, m_pThreadZippedDataBuf(nullptr)
	, m_pCacheThread(nullptr)
{
	if(aCacheBlockCount < 2)
		throw L"Cache Block Count < 2";
	if(nullptr == aDataFile )
		throw L"Invalid argument";
	m_CurrentTime				= 0;
	m_pDataFile					= aDataFile;
	m_DataType					= m_pDataFile->GetDataType();
	m_LastAvgFrameLengthInMF	= 0;
	m_CacheBlockCount			= aCacheBlockCount;
	try
	{
		m_ppCache				= new DataBlockHolder* [m_CacheBlockCount + 1/*for next block*/];
		for(int i = 0; i < m_CacheBlockCount + 1; i++)
			m_ppCache[i] = new DataBlockHolder(m_DataType, m_pDataFile->GetMaxFrameCount(), m_pDataFile->GetFrameSize());
	}
	catch(.../*std::bad_alloc e*/)
	{
		_clearBuffers();
		throw L"Недостаточно памяти для создания кеша данных";
	}
	m_pBlockPrev = m_ppCache[0];
	m_pBlockCurr = m_ppCache[1];
	m_pBlockNext = m_ppCache[m_CacheBlockCount];
    m_hDataReady = Events::CreateEvent(true, true);
    m_hDataNeeded = Events::CreateEvent(false, false);

	m_pCacheThread = new HelperFunctions::XThread< BaseDataCacher >(this, &BaseDataCacher::ThreadExecute, nullptr, "Cache Thread", m_hDataNeeded);
	m_pCacheThread->Resume();
}
//------------------------------------------------------------------------

BaseDataCacher::~BaseDataCacher(void)
{
	//Log( L"Остановка потока BaseDataCacher" );
	SAFE_DELETE(m_pCacheThread);

	//Log( L"Запуск ~BaseDataCacher" );
    Events::DestroyEvent(m_hDataNeeded);
	Events::DestroyEvent(m_hDataReady);
    //DeleteCriticalSection( &m_lockData );
	_clearBuffers();
 }
//------------------------------------------------------------------------
void BaseDataCacher::_clearBuffers()
{
	SAFE_ARRAY_DELETE(m_pThreadZippedDataBuf);
    SAFE_ARRAY_DELETE(m_pZippedDataBuf);
	if(m_ppCache)
		for(int i = 0; i < m_CacheBlockCount + 1; i++)
			SAFE_DELETE(m_ppCache[i]);
    SAFE_ARRAY_DELETE(m_ppCache);
}
int BaseDataCacher::GetFrameCount()
{
	int frames = m_pDataFile->GetFrameCount();
	return frames;
}
MASTERFRAME BaseDataCacher::GetFromMasterFrame()
{
	MASTERFRAME frame = m_pDataFile->FromMF();
	return frame;
}
MASTERFRAME BaseDataCacher::GetToMasterFrame()
{
	MASTERFRAME frame = m_pDataFile->ToMF();
	return frame;
}
const char* BaseDataCacher::GetFilename()
{
	return m_pDataFile->GetCFilename();
}

DataType BaseDataCacher::GetDataType()
{
	DataType dt = m_pDataFile->GetDataType();
	return dt;
}

void BaseDataCacher::ThreadExecute(void* p)
{
	while (m_pCacheThread->IsRunning())
	{
		WaitForSingleObject(m_hDataNeeded, INFINITE);
		if (!m_pCacheThread->IsRunning())
			return;
		if (m_pBlockNext->m_pDataBlock != nullptr)
		{
			try
			{
				int size;
				if (m_pBlockNext->m_pDataBlock->m_ZippedSize < m_pBlockNext->m_pDataBlock->m_BlockSize)
				{
					if (!m_pThreadZippedDataBuf)
					{
						int buf_size = m_pDataFile->GetMaxFrameCount() * m_pDataFile->GetFrameSize();
						try
						{
							m_pThreadZippedDataBuf = new unsigned char[buf_size];
						}
						catch (...)
						{
							throw L"Недостаточно памяти для создания кеша данных";;
						}
					}
					m_pDataFile->ReadBlock(m_pBlockNext->m_pDataBlock, (void *)m_pThreadZippedDataBuf, size);
					unsigned long res_size = m_pBlockNext->m_pDataBlock->m_BlockSize;
					if (uncompress(m_pBlockNext->m_pDataBuf, &res_size, m_pThreadZippedDataBuf, m_pBlockNext->m_pDataBlock->m_ZippedSize) != Z_OK)
						throw L"Can't uncompress DataBuffer in Thread";
				}
				else
				{
					m_pDataFile->ReadBlock(m_pBlockNext->m_pDataBlock, (void *)m_pBlockNext->m_pDataBuf, size);
				}
			}
			catch (...)
			{
				m_pBlockNext->m_pDataBlock = NULL;
			}
		}
		else
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1));// Sleep(1);
		}
		SetEvent(m_hDataReady);
	}
}

bool BaseDataCacher::GetCachedBlock(MASTERFRAME aFrameOrMF, bool aIsMasterframe, CacheDirection aCacheDirection)
{
	m_CurrentTime++;

	if((NULL != m_pBlockCurr->m_pDataBlock) && m_pBlockCurr->m_pDataBlock->IndexInBlock(aFrameOrMF, aIsMasterframe))
		return true;

	if((NULL != m_pBlockPrev->m_pDataBlock) && m_pBlockPrev->m_pDataBlock->IndexInBlock(aFrameOrMF, aIsMasterframe))
	{
		DataBlockHolder *bh = m_pBlockCurr;
		m_pBlockCurr = m_pBlockPrev;
		m_pBlockPrev = bh;

		return true;
	}

	bool frame_in_cache = false;
	for(int i = 0; i < m_CacheBlockCount; i++)
	{
		if((NULL != m_ppCache[i]->m_pDataBlock) && m_ppCache[i]->m_pDataBlock->IndexInBlock(aFrameOrMF, aIsMasterframe))
		{
			m_pBlockPrev = m_pBlockCurr;
			m_pBlockCurr = m_ppCache[i];
			frame_in_cache = true;
			break;
		}
	}

	if(!frame_in_cache)
	{
		try
		{
			// find farest block
			int64_t dif = -1;
			int index = -1;
			for( int i = 0; i < m_CacheBlockCount; i++ )
			{
				if((m_CurrentTime - m_ppCache[i]->m_Time) > dif)
				{
					dif = m_CurrentTime - m_ppCache[i]->m_Time;
					index = i;
				}
			}
			if( index == -1 )
				throw L"Can't find oldest BlockHolder";

			WaitForSingleObject(m_hDataReady, INFINITE);

			if((NULL != m_pBlockNext->m_pDataBlock) && m_pBlockNext->m_pDataBlock->IndexInBlock(aFrameOrMF, aIsMasterframe))
			{
				CreateIndex(m_pBlockNext);
				DataBlockHolder *bh = m_ppCache[index];
				m_ppCache[index] = m_pBlockPrev = m_pBlockCurr = m_pBlockNext;
				m_ppCache[m_CacheBlockCount] = m_pBlockNext = bh;
			}
			else
			{
				DataBlockHolder *bh = m_ppCache[index];
				m_pBlockPrev = m_pBlockCurr;
				m_pBlockCurr = bh;

				m_pBlockCurr->m_pDataBlock = aIsMasterframe ? m_pDataFile->FindByMF(aFrameOrMF) : m_pDataFile->FindBlock((int)aFrameOrMF);

				if(NULL == m_pBlockCurr->m_pDataBlock)
					return false;

				int size;
				if(m_pBlockCurr->m_pDataBlock->m_ZippedSize < m_pBlockCurr->m_pDataBlock->m_BlockSize)
				{
					if(!m_pZippedDataBuf)
					{
						int buf_size = m_pDataFile->GetMaxFrameCount() * m_pDataFile->GetFrameSize();
						try
						{
							m_pZippedDataBuf = new unsigned char[buf_size];
						}
						catch(...)
						{
							throw L"Недостаточно памяти для создания кеша данных";;
						}
					}
					m_pDataFile->ReadBlock(m_pBlockCurr->m_pDataBlock, (void *)m_pZippedDataBuf, size);
					unsigned long res_size = m_pBlockCurr->m_pDataBlock->m_BlockSize;

					if(uncompress(m_pBlockCurr->m_pDataBuf, &res_size, m_pZippedDataBuf, m_pBlockCurr->m_pDataBlock->m_ZippedSize) != Z_OK)
					{
						throw L"Can't uncompress data buffer (main thread)";
					}
				}
				else
				{
					try
					{
						m_pDataFile->ReadBlock(m_pBlockCurr->m_pDataBlock, (void *)m_pBlockCurr->m_pBuf, size);
					}
					catch(const wchar_t *sErr)
					{
						sErr;
						m_pBlockCurr->m_pDataBlock = NULL;
						throw;
					}
				}

				CreateIndex(m_pBlockCurr);
			}

			switch(aCacheDirection)
			{
				case CacheDirection::cdBackward:
					m_pBlockNext->m_pDataBlock = m_pDataFile->FindBlock(m_pBlockCurr->m_pDataBlock->m_FromFrame - 1);
					ResetEvent(m_hDataReady);
					SetEvent(m_hDataNeeded);
					break;
				case CacheDirection::cdForward:
				default:
					m_pBlockNext->m_pDataBlock = m_pDataFile->FindBlock(m_pBlockCurr->m_pDataBlock->m_FromFrame + m_pBlockCurr->m_pDataBlock->m_FrameCount);
					ResetEvent(m_hDataReady);
					SetEvent(m_hDataNeeded);
					break;
			}
		}
		catch(const wchar_t* strErr)
		{
			//Log(L"Не удалось получить данные: %s", strErr);
			return false;
		}
		catch(...)
		{
			return false;
		}
	}

	return true;
}

unsigned char * BaseDataCacher::GetData(int aFrame, CacheDirection aCD)
{
	if ((m_pDataFile == NULL) || (aFrame < 0) || (aFrame >= m_pDataFile->GetFrameCount()))
		return NULL;
	LOCK(this);
	if(GetCachedBlock((MASTERFRAME)aFrame, false, aCD))
	{
		unsigned char *ptr = m_pBlockCurr->m_ppDataBufIndex[aFrame - m_pBlockCurr->m_pDataBlock->m_FromFrame];
		m_pBlockCurr->m_Time = m_CurrentTime;
		if(ptr != NULL)
		{
			m_CurrentSpeed	= *(unsigned short*)(ptr + 4);
			m_CurrentButtons = *(unsigned short*)(ptr + 6);
		}
		return ptr;
	}
	return NULL;
}


int BaseDataCacher::FindNearestMF(MASTERFRAME aMF)
{
    int lm = 0;
    int rm = m_pBlockCurr->m_pDataBlock->m_FrameCount - 1;

	if( aMF >= m_pBlockCurr->m_pMFIndex[ rm ] )
		return rm;

    while(rm - lm > 1)
    {
        int mdl = (lm + rm) / 2;
        if(m_pBlockCurr->m_pMFIndex[mdl] > aMF)
        {
            rm = mdl;
        }
        else
        {
            if(m_pBlockCurr->m_pMFIndex[mdl] < aMF)
            {
                lm = mdl;
            }
            else
            {
                return mdl;
            }
        }
    }
	if( aMF != m_pBlockCurr->m_pMFIndex[ rm ] )
    {
        return lm;
    }
    else
    {
        return rm;
    }
}


unsigned char * BaseDataCacher::GetDataMF(MASTERFRAME aMF, int* aFrame, MASTERFRAME* aRealMF, int* aFrameLengthInMF)
{
	if((m_pDataFile == NULL) || (aMF < m_pDataFile->FromMF()) || (aMF >= m_pDataFile->ToMF())) return NULL;
	LOCK(this);
	if(GetCachedBlock(aMF, true, CacheDirection::cdForward))
	{
		int offset = FindNearestMF(aMF);
		if( aRealMF != NULL )
    		*aRealMF = m_pBlockCurr->m_pMFIndex[offset];
		if( aFrameLengthInMF != NULL )
			*aFrameLengthInMF = m_pBlockCurr->m_FrameLengthInMF[ offset ];
		m_CurrentFrame = m_pBlockCurr->m_pDataBlock->m_FromFrame + offset;
		if( aFrame != NULL )
			*aFrame = m_CurrentFrame;
		m_MFCount = m_pBlockCurr->m_pDataBlock->m_ToMF - m_pBlockCurr->m_pDataBlock->m_FromMF;

		unsigned char *ptr = m_pBlockCurr->m_ppDataBufIndex[offset];
		if(ptr != NULL)
		{
			m_CurrentSpeed = *(unsigned short*)(ptr + 4);
			m_CurrentButtons = *(unsigned short*)(ptr + 6);
		}
		return ptr;
	}
	return NULL;
}


MASTERFRAME BaseDataCacher::GetMFNumber(int aFrame)
{
	if( GetData(aFrame) &&
		m_pBlockCurr->m_pMFIndex != NULL &&
		aFrame - m_pBlockCurr->m_pDataBlock->m_FromFrame >= 0 &&
		aFrame - m_pBlockCurr->m_pDataBlock->m_FromFrame < m_pBlockCurr->m_pDataBlock->m_FromFrame + m_pBlockCurr->m_pDataBlock->m_FrameCount )
		return m_pBlockCurr->m_pMFIndex[aFrame - m_pBlockCurr->m_pDataBlock->m_FromFrame];
	return INVALID_MASTERFRAME;
}
int BaseDataCacher::GetAvgFrameNumber(MASTERFRAME aMF)
{
	DataFileBlock* pBlock = m_pDataFile->FindByMF(aMF);
	if(pBlock)
	{
		return pBlock->m_FromFrame + (int)(pBlock->m_FrameCount * ((float)(aMF - pBlock->m_FromMF) / (pBlock->m_ToMF - pBlock->m_FromMF)));
	}
	return -1;
}

bool BaseDataCacher::GetFrameInfo(int aFrame, /*OUT*/MASTERFRAME& aMF, /*OUT*/uint16_t& aSpeed, /*OUT*/uint16_t& aButtons )
{
	if( GetData(aFrame) &&
		m_pBlockCurr->m_pMFIndex != NULL &&
		aFrame - m_pBlockCurr->m_pDataBlock->m_FromFrame >= 0 &&
		aFrame - m_pBlockCurr->m_pDataBlock->m_FromFrame < m_pBlockCurr->m_pDataBlock->m_FromFrame + m_pBlockCurr->m_pDataBlock->m_FrameCount )
	//if( m_pBlockCurr->m_pMFIndex == NULL ||
	//	aFrame - m_pBlockCurr->m_pDataBlock->m_FromFrame < 0 ||
	//	aFrame - m_pBlockCurr->m_pDataBlock->m_FromFrame >= m_pBlockCurr->m_pDataBlock->m_FromFrame + m_pBlockCurr->m_pDataBlock->m_FrameCount )
	//{
	//	GetData( aFrame );
	//}
	//if( m_pBlockCurr->m_pMFIndex != NULL &&
	//	aFrame - m_pBlockCurr->m_pDataBlock->m_FromFrame >= 0 &&
	//	aFrame - m_pBlockCurr->m_pDataBlock->m_FromFrame < m_pBlockCurr->m_pDataBlock->m_FromFrame + m_pBlockCurr->m_pDataBlock->m_FrameCount )
	{
		aMF			= m_pBlockCurr->m_pMFIndex[aFrame - m_pBlockCurr->m_pDataBlock->m_FromFrame];
		aSpeed		= *((uint16_t*)(m_pBlockCurr->m_ppDataBufIndex[aFrame - m_pBlockCurr->m_pDataBlock->m_FromFrame] + 4));
		aButtons	= *((uint16_t*)(m_pBlockCurr->m_ppDataBufIndex[aFrame - m_pBlockCurr->m_pDataBlock->m_FromFrame] + 6));
		return true;
	}
		
	return false;
}

int BaseDataCacher::GetFrameLengthInMF( int aFrame )
{
	if( GetData(aFrame) && 
		m_pBlockCurr && 
		m_pBlockCurr->m_pDataBlock->m_FromFrame <= aFrame && 
		m_pBlockCurr->m_pDataBlock->m_FrameCount + m_pBlockCurr->m_pDataBlock->m_FromFrame > aFrame )
	{
		return m_pBlockCurr->m_FrameLengthInMF[ aFrame - m_pBlockCurr->m_pDataBlock->m_FromFrame ];
	}
	else 
	{
		return -1;
	}
}
int64_t	BaseDataCacher::GetSpecificData()
{
	if(m_pDataFile)
		return m_pDataFile->GetSpecificData();
	else
		return 0;
}