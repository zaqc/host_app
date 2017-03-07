#include "DataFileWriter.h"
#include "FreshLocalDataFileReader.h"
#include "DataFrameHeader.h"
#include "zlib.h"
#include <mutex>
#include <thread>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <iostream>
#include "LogHelper.h"

using DataFile::DataFileBlock;
using DataFile::DataFileWriter;
using DataFile::FreshLocalDataFileReader;
using DataFile::NewDataBlockProc;
using DataFile::BaseDataFileReader;
using DataFile::BaseFrameHeader;
using std::string;
using std::wstring;

DataFileWriter::DataFileWriter(DataType aDataType, int aTripID, const GUID& aGuid
								, const GUID& aTripGUID, const string& aFileName
								, int aFrameSize, int aMaxFrameCount, bool aZipped
								, int aMoreDataSize, uint64_t aSpecificData)
	: BaseDataFile()
	, m_WriteToMF(INVALID_MASTERFRAME)
	, m_WriteFrameCount(0)
	, m_WriteBuf(nullptr)
	, m_WriteBufSave(nullptr)
	, m_CompressBuf(nullptr)
	, m_CompressResultBuf(nullptr)
	, m_CompressBuffer(nullptr)
	, m_BufferCompressed(nullptr)
	, m_TerminateCompressThread(nullptr)
	, m_BufSize(aFrameSize * aMaxFrameCount)
{
	m_DataType = aDataType;
	m_FileName = aFileName;
	
	m_hFile = open(m_FileName.c_str(), O_RDWR | O_CREAT | O_LARGEFILE, S_IRWXO);
//	int fileErr{errno};
	assert(-1 != m_hFile);
	if(-1 == m_hFile)
		perror("Cannot open file");

	m_pHeader = new DataFileHeader(aGuid, aTripGUID);
	m_pHeader->GetVersion(m_verMajor, m_verMinor);

	m_pHeader->m_FrameSize = aFrameSize;
	m_pHeader->m_MaxFrameCount = aMaxFrameCount;
	m_pHeader->m_bZipped = aZipped;
	m_pHeader->m_TripID = aTripID;
	m_pHeader->m_SpecificData = aSpecificData;

	m_pHeader->SetDataTypeString(m_DataType);

	ssize_t bytes = write(m_hFile, static_cast<const void*>(m_pHeader), sizeof(DataFileHeader));
//	fileErr = errno;
	assert(-1 != bytes);
	if(-1 == bytes)
		perror("Cannot write file");

	m_WriteBuf = m_WriteBufSave = new unsigned char [m_BufSize]{};
	m_CompressBuf = new unsigned char [m_BufSize]{};

	if(true == aZipped)
		m_CompressResultBuf = new unsigned char [m_BufSize]{};

	m_CompressBuffer = Events::CreateEvent(false, false);
	m_BufferCompressed = Events::CreateEvent(false, true);
	m_TerminateCompressThread = Events::CreateEvent(false, false);

	m_thread = std::thread(&DataFileWriter::ThreadExecute, std::ref(*this));
}

DataFileWriter::~DataFileWriter(void)
{
	Close();

	Events::SetEvent(m_TerminateCompressThread);
	m_thread.join();

	Events::DestroyEvent(m_BufferCompressed);
	m_BufferCompressed = nullptr;
	Events::DestroyEvent(m_CompressBuffer);
	m_CompressBuffer = nullptr;
	Events::DestroyEvent(m_TerminateCompressThread);
	m_TerminateCompressThread = nullptr;

	if(nullptr != m_pHeader && m_pHeader->m_bZipped)
		SAFE_ARRAY_DELETE(m_CompressResultBuf);

	SAFE_ARRAY_DELETE(m_CompressBuf);
	m_WriteBuf = m_WriteBufSave;
	SAFE_ARRAY_DELETE(m_WriteBuf);
	m_WriteBufSave = nullptr;

	for(auto reader : m_ReaderList)
		reader->FileWriterClosed();
}

void DataFileWriter::FlushBuffer()
{
	//Ждем когда завершится сжатие текущего блока
	Events::WaitForSingleObject(m_BufferCompressed, INFINITE);

	m_tmpDataFileBlock.Clear();
	m_tmpDataFileBlock.m_FromMF =*reinterpret_cast<MASTERFRAME*>(m_WriteBufSave);
	m_tmpDataFileBlock.m_ToMF = m_WriteToMF;
	m_tmpDataFileBlock.m_BlockSize = m_WriteBuf - m_WriteBufSave;
	m_tmpDataFileBlock.m_ZippedSize = m_tmpDataFileBlock.m_BlockSize;
	m_tmpDataFileBlock.m_FrameCount = m_WriteFrameCount;

	m_WriteBuf = m_CompressBuf;
	m_CompressBuf = m_WriteBufSave;
	m_WriteBufSave = m_WriteBuf;

	//запустить запись блока
	Events::SetEvent(m_CompressBuffer);
}

void DataFileWriter::Close()
{
	FlushBuffer();
	//Ждем когда завершится сжатие последнего блока
	WaitForSingleObject(m_BufferCompressed, INFINITE);

	m_pHeader->m_BlockListPos = GetFilePos();

	LOCK(m_pBlockList);
	m_pBlockList->Write(m_hFile);

	SetFilePos(0);
	ssize_t bytes = write(m_hFile, static_cast<const void*>(m_pHeader), sizeof(DataFileHeader));
//	int fileErr{errno};
	assert(-1 != bytes);
	if(-1 == bytes)
		perror("Cannot write file");

	int res = close(m_hFile);
//	fileErr = errno;
	assert(-1 != res);
	if(-1 == res)
		perror("Closing file error");

	m_hFile = -1;
}

void DataFileWriter::ThreadExecute()
{
	const int arraySize{2};
	Events::HANDLE events[arraySize]{m_TerminateCompressThread, m_CompressBuffer};

	while(true)
	{
		int waitIndex{0};
		int res = Events::WaitForMultipleObjects(arraySize, events, false, INFINITE, waitIndex);
		
		if(WAIT_OBJECT_0 == waitIndex) //m_TerminateCompressThread
			break;

		try
		{
			if (0 < m_tmpDataFileBlock.m_BlockSize)
			{
				int result{-1};
				uLongf res_size{static_cast<uLongf>(m_BufSize)};

				if (m_pHeader->m_bZipped)
				{
					const int compressLevel{3};
					result = compress2(m_CompressResultBuf, &res_size
											, m_CompressBuf, static_cast<uLongf>(m_tmpDataFileBlock.m_BlockSize)
											, compressLevel);
					assert(Z_OK == result);
				}

				if ((Z_OK == result) && (res_size < m_tmpDataFileBlock.m_BlockSize))
				{
					m_tmpDataFileBlock.m_ZippedSize = res_size;
					WriteBlock(m_CompressResultBuf, m_tmpDataFileBlock);
				}
				else
					WriteBlock(m_CompressBuf, m_tmpDataFileBlock);
			}
		}
		catch(...)
		{
			std::cerr << "Error occurred in WriteBlock" << std::endl;
		}

		Events::SetEvent(m_BufferCompressed);
	}
}

void DataFileWriter::WriteBlock(void *aBuffer, DataFileBlock& aDataFileBlock)
{
	LOCK(m_pBlockList);

	if(-1 == m_hFile)
		throw L"Invalid file handler";

	aDataFileBlock.m_FilePosition = GetFilePos();
	m_pBlockList->Add(aDataFileBlock); // <-- изменяет структуру

	if(0 != aDataFileBlock.m_FrameCount || 0 != aDataFileBlock.m_ZippedSize || 0 != aDataFileBlock.m_BlockSize)
	{
		ssize_t bytes = write(m_hFile, &aDataFileBlock, sizeof(DataFileBlock));
//		int ferr{errno};
		assert(-1 != bytes);
		if (-1 == bytes)
			LOGE("Cannot write file");
		else
			LOGI("Success write %d bytes to file", bytes);
		if(bytes != sizeof(DataFileBlock))
			throw L"Invalid block size";

		bytes = write(m_hFile, static_cast<const void*>(aBuffer), aDataFileBlock.m_ZippedSize);
//		ferr = errno;
		assert(-1 != bytes);
		if(-1 == bytes)
			LOGE("Cannot write file");

		if(bytes != aDataFileBlock.m_ZippedSize)
			throw L"Uncompressing error";
	}

	for(auto reader : m_ReaderList)
		reader->NewDataBlockHandler(aDataFileBlock);
}

void DataFileWriter::DirectWriteBlock(void * aRawBuffer)
{
	DataFileBlock* dataBlockHeader = static_cast<DataFileBlock*>(aRawBuffer);

	uint8_t* buff = static_cast<uint8_t*>(aRawBuffer);
	buff += sizeof(DataFileBlock);

	WriteBlock(buff, *dataBlockHeader);
}

//void DataFileWriter::WriteFrame(unsigned int aDataMask, WSABUF *aMF, WSABUF *aBuf)
//{
//	int len = sizeof(unsigned int) + sizeof(int) * 2 + aMF->len + aBuf->len;
//
//	m_WriteToMF = *(unsigned int *)(aMF->buf);
//
//	if((m_WriteBuf - m_WriteBufSave + len >= m_pHeader->m_FrameSize * m_pHeader->m_MaxFrameCount)
//		|| (m_WriteFrameCount + 1 > m_pHeader->m_MaxFrameCount))
//	{
//		WaitForSingleObject(m_BufferCompressed, INFINITE);
//		m_CompressFromMF = *(unsigned int *)(m_WriteBufSave + 8);
//		m_CompressToMF = m_WriteToMF;
//		m_CompressFrameCount = m_WriteFrameCount;
//		m_WriteFrameCount = 0;
//		m_CompressSize = m_WriteBuf - m_WriteBufSave;
//		m_WriteBuf = m_CompressBuf;
//		m_CompressBuf = m_WriteBufSave;
//		m_WriteBufSave = m_WriteBuf;
//		SetEvent(m_CompressBuffer);
//	}
//
//	*(unsigned int *)m_WriteBuf = aDataMask; m_WriteBuf += sizeof(unsigned int);
//
//	*(int *)m_WriteBuf = aMF->len; m_WriteBuf += sizeof(int);
//	memcpy(m_WriteBuf, aMF->buf, aMF->len); m_WriteBuf += aMF->len;
//
//	*(int *)m_WriteBuf = aBuf->len; m_WriteBuf += sizeof(int);
//	memcpy(m_WriteBuf, aBuf->buf, aBuf->len); m_WriteBuf += aBuf->len;
//
//	m_WriteFrameCount++;
//}

//void DataFileWriter::WriteFrame(unsigned int aDataMask, WSABUF *aMF, WSABUF *aBuf1, WSABUF *aBuf2)
//{
//	int len = sizeof(unsigned int) + sizeof(int) * 3 + aMF->len + aBuf1->len + aBuf2->len;
//
//	m_WriteToMF = *(unsigned int *)(aMF->buf);
//
//	if((m_WriteBuf - m_WriteBufSave + len >= m_pHeader->m_FrameSize * m_pHeader->m_MaxFrameCount)
//		|| (m_WriteFrameCount + 1 > m_pHeader->m_MaxFrameCount))
//	{
//		WaitForSingleObject(m_BufferCompressed, INFINITE);
//		m_CompressFromMF = *(unsigned int *)(m_WriteBufSave + 8);
//		m_CompressToMF = m_WriteToMF;
//		m_CompressFrameCount = m_WriteFrameCount;
//		m_WriteFrameCount = 0;
//		m_CompressSize = m_WriteBuf - m_WriteBufSave;
//		m_WriteBuf = m_CompressBuf;
//		m_CompressBuf = m_WriteBufSave;
//		m_WriteBufSave = m_WriteBuf;
//		SetEvent(m_CompressBuffer);
//	}
//	*(unsigned int *)m_WriteBuf = aDataMask; m_WriteBuf += sizeof(unsigned int);
//
//	*(int *)m_WriteBuf = aMF->len; m_WriteBuf += sizeof(int);
//	memcpy(m_WriteBuf, aMF->buf, aMF->len); m_WriteBuf += aMF->len;
//
//	*(int *)m_WriteBuf = aBuf1->len; m_WriteBuf += sizeof(int);
//	memcpy(m_WriteBuf, aBuf1->buf, aBuf1->len); m_WriteBuf += aBuf1->len;
//
//	*(int *)m_WriteBuf = aBuf2->len; m_WriteBuf += sizeof(int);
//	memcpy(m_WriteBuf, aBuf2->buf, aBuf2->len); m_WriteBuf += aBuf2->len;
//
//	m_WriteFrameCount++;
//}

void DataFileWriter::WriteFrame(const DataFile::BaseFrameHeader& aFrameHeader, const char* aBuffer, bool* pIsBlockFinished/*OUT*/)
{
	bool isBlockFinished{false};

	m_WriteToMF = aFrameHeader.Masterframe;

	size_t len = sizeof(BaseFrameHeader) + aFrameHeader.Size;
	if((m_WriteBuf - m_WriteBufSave + len >= m_BufSize)
		|| (m_WriteFrameCount + 1 > m_pHeader->m_MaxFrameCount)) //изменил условие на строгое, при нестрогом и максимальном колчесве фреймов в блоке 1 блок терялся
	{
		FlushBuffer();

		m_WriteFrameCount = 0;
		isBlockFinished = true;
	}

	if(nullptr != pIsBlockFinished)
		*pIsBlockFinished = isBlockFinished;

	memcpy(m_WriteBuf, &aFrameHeader, sizeof(BaseFrameHeader));
	m_WriteBuf += sizeof(BaseFrameHeader);

	memcpy(m_WriteBuf, aBuffer, aFrameHeader.Size);
	m_WriteBuf += aFrameHeader.Size;

	m_WriteFrameCount++;
}

FreshLocalDataFileReader* DataFileWriter::GetReader(NewDataBlockProc<DataFileBlock> aNewDataBlockProc, void* aNewDataBlockParam)
{
	FreshLocalDataFileReader* reader{nullptr};
	LOCK(m_pBlockList);
	reader = new FreshLocalDataFileReader(this, m_FileName, m_pBlockList, m_pHeader, aNewDataBlockProc);

	std::lock_guard<std::mutex>lock(m_lockReaderList);
	m_ReaderList.push_back(reader);

	return reader;
}

void DataFileWriter::Unsubscribe(FreshLocalDataFileReader* aFreshServerDataFileReader)
{
	std::lock_guard<std::mutex>lock(m_lockReaderList);

	for(auto iList = m_ReaderList.begin(); iList != m_ReaderList.end(); ++iList)
		if(*iList == aFreshServerDataFileReader)
		{
			m_ReaderList.erase(iList);
			break;
		}
}
