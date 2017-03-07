#pragma once
#ifndef DATA_FILE_WRITER_H
#define DATA_FILE_WRITER_H

#include "HelperFunctions.h"
#include "BaseDataFile.h"
#include "QueueExtractor.h"
#include "common_def.h"
#include <thread>
#include <mutex>
#include <condition_variable>
#include "Events.h"
//#include "uuid.h"

namespace DataFile
{

class FreshLocalDataFileReader;
struct BaseFrameHeader;

class DataFileWriter : public BaseDataFile
{
private:
	std::thread m_thread;
	//по какой мастерфрейм включительно есть данные в буфере записи
	MASTERFRAME m_WriteToMF;

	int	m_WriteFrameCount;
	unsigned char* m_WriteBuf;
	unsigned char* m_WriteBufSave;
    unsigned char* m_CompressBuf;
    unsigned char* m_CompressResultBuf;
    Events::HANDLE m_CompressBuffer;
    Events::HANDLE m_BufferCompressed;
	Events::HANDLE m_TerminateCompressThread;
    int m_BufSize;
    DataFileBlock m_tmpDataFileBlock;
    std::vector<FreshLocalDataFileReader*> m_ReaderList;
    
    mutable std::mutex m_lockReaderList;

private:
	void FlushBuffer();

protected:
	virtual void ThreadExecute();
	void WriteBlock(void *aBuffer, DataFileBlock& aDataFileBlock);

public:
	DataFileWriter(DataType aDataType, int aTripID, const GUID& aGuid, const GUID& aTripGUID, const std::string& aFileName
						, int aFrameSize, int aMaxFrameCount, bool aZipped = false
						, int aMoreDataSize = 0, uint64_t aSpecificData = 0);
    virtual ~DataFileWriter(void);
    void Close();

    void DirectWriteBlock(void * aRawBuffer);
	//void WriteFrame(unsigned int aDataMask, WSABUF *aMF, WSABUF *aBuf);
	//void WriteFrame(unsigned int aDataMask, WSABUF *aMF, WSABUF *aBuf1, WSABUF *aBuf2);
	//				pIsBlockFinished/*не обязательный выходной параметр, установлен в true когда закончено наполнение текущего блока*/
	void WriteFrame(const DataFile::BaseFrameHeader& aFrameHeader, const char* aBuffer, bool* pIsBlockFinished = nullptr/*OUT*/);
	//void WriteFrame(MASTERFRAME aMasterFrameNumber, uint16_t aButtons, uint16_t aSpeed, const char* aBuffer
	//					, int aSize, bool* pIsBlockFinished = nullptr/*OUT*/);

	FreshLocalDataFileReader* GetReader(NewDataBlockProc<DataFileBlock> aNewDataBlockSender, void* aNewDataBlockParam = nullptr);
	void Unsubscribe(FreshLocalDataFileReader* aFreshServerDataFileReader);
};
}
#endif
