#ifndef BaseDataCacherH
#define BaseDataCacherH
#pragma once
#include "BaseDataFileReader.h"
#include "HelperFunctions.h"
#include "Events.h"
#include "CacheTypes.h"

#define EC_CH_COUNT 16
#define EC_DWORD_COUNT  5

namespace DataFile
{
#pragma pack(push, 1)
struct BaseFrameHeader
{
	unsigned int Masterframe;
	unsigned short Speed;
	unsigned short Buttons;
	int Size;
};
#pragma pack(pop)

class DataBlockHolder
{
public:
    unsigned char*	m_pBuf;
    DataFileBlock*	m_pDataBlock;
    unsigned char*	m_pDataBuf;
    unsigned char**	m_ppDataBufIndex;
    unsigned int*	m_pMFIndex;
	int*			m_FrameLengthInMF; //массив длин фреймов. Длина определяется расстоянием в мастерфреймах между 2-мя соседними фреймами
	int64_t			m_Time; //Время жизни блока в кеше, определяемое счетчиком обращений к данным. Используется для поиска кешируемого блока
public:
	DataBlockHolder(DataType aDataType, int aMaxFrameCount, int aFrameSize);
	virtual ~DataBlockHolder(void);
};


class BaseDataCacher : public HelperFunctions::LockableResource
{
protected:
    DataType				m_DataType;
	BaseDataFileReader*		m_pDataFile;
	DataBlockHolder*		m_pBlockCurr;
    DataBlockHolder*		m_pBlockPrev;
    DataBlockHolder*		m_pBlockNext;
	int						m_CacheBlockCount;	// Количество дополнительных закешированных блоков
    DataBlockHolder**		m_ppCache;	// Хранилище для закешированных блоков
    unsigned char*			m_pZippedDataBuf;
    unsigned char*			m_pThreadZippedDataBuf;
    Events::HANDLE			m_hDataReady;
	Events::HANDLE			m_hDataNeeded;
	Events::HANDLE			m_hCacheThread;
	bool					m_bCacheThread;
	int						m_LastAvgFrameLengthInMF;
	int64_t					m_CurrentTime; //Счетчик запросов данных
	HelperFunctions::XThread<BaseDataCacher>* m_pCacheThread;
public:
    int						m_CurrentFrame;
    int						m_MFCount;
    unsigned short			m_CurrentSpeed;
	unsigned short			m_CurrentButtons;
public:
    BaseDataCacher( BaseDataFileReader *aDataFile, int aCacheBlockCount = 10 );
    virtual	~BaseDataCacher(void);

	int							GetFrameCount();
	MASTERFRAME					GetFromMasterFrame();
	MASTERFRAME					GetToMasterFrame();
	const char*					GetFilename();
	DataType					GetDataType();
	const BaseDataFileReader* 	GetDataFile() { return m_pDataFile; }
	unsigned char*  			GetData(int aFrame, CacheDirection = CacheDirection::cdNone);
	unsigned char*  			GetDataMF(MASTERFRAME aMF, /*OUT*/int* aFrame = NULL, /*OUT*/MASTERFRAME* aRealMF = NULL, /*OUT*/int* aFrameLengthInMF = NULL);
	DataFileBlock *				GetCurrDataBlock(void) { return m_pBlockCurr->m_pDataBlock; }
	MASTERFRAME					GetMFNumber(int aFrame);
	bool						GetFrameInfo(int aFrame, /*OUT*/MASTERFRAME& aMF, /*OUT*/uint16_t& aSpeed, /*OUT*/uint16_t& aButtons );
	int							GetFrameLengthInMF( int aFrame );
	int							GetAvgFrameNumber(MASTERFRAME aMF);
	int64_t						GetSpecificData();
protected:
	//Ищет блок с заданным фреймом (или мастерфреймом) в кеше, если не находит, запращивает его из файла и делает упреждающее кеширование) и записывает его адрес
	//в переменную m_pBlockCurr. Если блок не наден, возвращает false
	bool						GetCachedBlock(MASTERFRAME aFrameOrMF, bool aMasterframe, CacheDirection aCacheDirection);
	int							FindNearestMF(MASTERFRAME aMF);
	virtual void 				CreateIndex( DataBlockHolder *aBH ) = 0;
	void		 				ThreadExecute(void* p);
private:
	void						_clearBuffers();
};
}
#endif
