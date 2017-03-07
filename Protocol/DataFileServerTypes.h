#pragma once
#include "DataFileCommonTypes.h"
#include "CommonSide.h"
#include <iostream>

namespace DataFile
{
enum class PipeCommand : int32_t
{
	pcOpenFileRequest		= 0x00000100,	// Запрос на открытие файла
	pcOpenFileResponse		= 0x00000300,	// Ответ на запрос открытия файла
	pcGetDataRequest		= 0x00000700,	// Запрос на получение блока данных
	pcGetDataResponse		= 0x00000B00,	// Ответ по запросу на получение блока данных

	pcGetFilesListRequest	= 0x00000B01,	// Запрос списка файлов
	pcGetFilesListResponse	= 0x00000B02,	// Ответ по запросу списка файлов

	pcNewDataBlockMessage	= 0x00001001,	// Сообщение о новом блоке
	pcDataBlocksResponse    = 0x00001010,   // Блочная пересылка блоков
	pcMessageFlag	 		= 0x00001000	// Признак сообщения
};

enum class CommandResult : unsigned int
{
	crOk = 0x56115A7F,
	crError = 0xFACE0023
};

class BufferGuard
{
	uint8_t* m_BuffAddr;
	size_t m_Size;

public:
	BufferGuard(const uint8_t* const aAddr, size_t aSize)
		: m_BuffAddr(const_cast<uint8_t*>(aAddr))
		, m_Size(aSize)
	{
	}

	bool Check(const uint8_t* const addr) const
	{
		return (m_Size == static_cast<int>(addr - m_BuffAddr));
	}
};

#pragma pack(push, 1)
//struct BaseDFMessage
//{
//public:
//	uint32_t m_Size;
//	PipeCommand m_ID;
//
//public:
//	BaseDFMessage(PipeCommand aPipeCommand, int aSize)
//		: m_Size(aSize)
//		, m_ID(aPipeCommand)
//	{
//	}
//};

//========================================================
struct OpenFileRequest : public BaseMessage
{
private:
//	uint8_t* m_buffer;
	static const size_t arraySize{ 256 };
public: 
	uint8_t m_FileName[arraySize];
	uint32_t m_FileNameLen;

protected:
	size_t GetSize() const
	{
		return BaseMessage::GetSize() + sizeof(m_FileName);
	}

public:
	OpenFileRequest()
		: BaseMessage(static_cast<int32_t>(PipeCommand::pcOpenFileRequest))
		//, m_buffer(nullptr)
		, m_FileName()
		, m_FileNameLen(arraySize)
	{
		Size = GetSize();
	}

	~OpenFileRequest()
	{
	//	delete[] m_buffer;
	}

	//void* Serialize()
	//{
	//	size_t buffSize{GetSize()};

	//	Size = buffSize;

	//	if(nullptr != m_buffer)
	//		delete[] m_buffer;

	//	m_buffer = new uint8_t[buffSize]{};
	//	uint8_t* ptr{m_buffer};

	//	BufferGuard bufferGuard(ptr, buffSize);

	//	Buffer<uint32_t>::Push(&ptr, Size);
	//	Buffer<int32_t>::Push(&ptr, Command);
	//	Buffer<int32_t >::Push(&ptr, IP);
	//	Buffer<GUID>::Push(&ptr, Guid);
	//	Buffer<int32_t>::Push(&ptr, Tag);

	//	memcpy(ptr, m_FileName, sizeof(m_FileName));
	//	ptr += sizeof(m_FileName);

	//	Buffer<uint32_t>::Push(&ptr, m_FileNameLen);
	//	Buffer<unsigned int>::Push(&ptr, m_DataType);

	//	if(not bufferGuard.Check(ptr))
	//		std::cout << "Open file request buffer size error while serializing" << std::endl;

	//	return m_buffer;
	//}

	//void Deserialize(const void *aBuffer)
	//{
	//	const uint8_t *ptr{static_cast<const uint8_t *>(aBuffer)};

	//	Size = Buffer<uint32_t>::Pop(&ptr);
	//	if(/*Size > 1000 ||*/ Size < 10)
	//		return;

	//	BufferGuard bufferGuard(static_cast<const uint8_t*>(aBuffer), Size);

	//	Command = Buffer<int32_t>::Pop(&ptr);
	//	IP = Buffer<int32_t>::Pop(&ptr);
	//	Guid = Buffer<GUID>::Pop(&ptr);
	//	Tag = Buffer<int32_t>::Pop(&ptr);

	//	memcpy(m_FileName, ptr, sizeof(m_FileName));
	//	ptr += sizeof(m_FileName);

	//	m_FileNameLen = Buffer<uint32_t>::Pop(&ptr);
	//	m_DataType = Buffer<unsigned int>::Pop(&ptr);

	//	if(not bufferGuard.Check(ptr))
	//		std::cout << "Open file request buffer size error while deserializing" << std::endl;
	//}

};

//========================================================
struct OpenFileResponse : public BaseMessage
{
public: 
	bool m_bNewFile;		// Изменяемый (нoвый) файл
	CommandResult m_Result;	// Результат выполнения операции открытия файла
	DataFileHeader m_FileHeader;

protected:
	size_t GetSize() const
	{
		return BaseMessage::GetSize() + sizeof(m_bNewFile) + sizeof(m_Result) + sizeof(m_FileHeader);
	}

public:
	OpenFileResponse()
		: BaseMessage(static_cast<int32_t>(PipeCommand::pcOpenFileResponse))
		, m_bNewFile(false)		   
		, m_Result(CommandResult::crError)
	{
		Size = GetSize();
	}
};

//========================================================
struct GetDataRequest : public BaseMessage
{
public: 
	DataFileBlock m_DataBlock;	// блока TDataBlock

protected:
	size_t GetSize() const
	{
		return BaseMessage::GetSize() + sizeof(m_DataBlock);
	}

public:
	GetDataRequest()
		: BaseMessage(static_cast<int32_t>(PipeCommand::pcGetDataRequest))
	{
		Size = GetSize();
	}
};

//========================================================
struct GetDataResponse : public BaseMessage
{
public:
	CommandResult m_Result;
#pragma warning(disable:4200)
	char m_pBuf[];
#pragma warning(default:200)

public:
	GetDataResponse()
		: BaseMessage(static_cast<int32_t>(PipeCommand::pcGetDataResponse))
		, m_Result(CommandResult::crError)
		, m_pBuf()
	{}
};

//========================================================
struct NewDataBlockMessage : public BaseMessage
{
public: 
	DataFileBlock m_DataBlock;	// Блок данных

protected:
	size_t GetSize() const
	{
		return BaseMessage::GetSize() + sizeof(m_DataBlock);
	}

public:
	NewDataBlockMessage()
		: BaseMessage(static_cast<int32_t>(PipeCommand::pcNewDataBlockMessage))
	{
		Size = GetSize();
	}
};

//========================================================
struct DataBlocksListResponse : public BaseMessage
{
public: 
		uint32_t m_BlockNumber;
#pragma warning(disable:4200)
		DataFileBlock m_Blocks[];
#pragma warning(default:200)

public:
		DataBlocksListResponse()
		: BaseMessage(static_cast<int32_t>(PipeCommand::pcDataBlocksResponse))
		, m_BlockNumber(0)
		, m_Blocks()
	{}
};

//========================================================
struct GetFilesListRequeste : public BaseMessage
{
public:
	GetFilesListRequeste()
		: BaseMessage(static_cast<int32_t>(PipeCommand::pcGetFilesListRequest))
	{}
};

//========================================================
struct GetFilesListResponse : public BaseMessage
{
public:
	size_t m_FilesCount;
	struct
	{
		int8_t m_fileName[];
	};

public:
	GetFilesListResponse()
		: BaseMessage(static_cast<int32_t>(PipeCommand::pcGetFilesListResponse))
		, m_FilesCount(0)
		{}
};
//========================================================
#pragma pack(pop)
}
