#include "DataFilePipe.h"
#include "LocalDataFileReader.h"
#include <iostream>

using namespace DataFile;
using namespace std::placeholders;

DataFilePipe::DataFilePipe(SOCKET aSocket, DataFileSourceGetter aCallback, bool bAutoStart
							, SockAddr* aSockAddr, int aReadBufferSize, int aWriteBufferSize)
	: SocketTCPConnector(aSocket, bAutoStart, aSockAddr, aReadBufferSize, aWriteBufferSize)
	, m_TryGetFreshDataReader(aCallback)
	, m_pDataFile(nullptr)
	, m_DataBlockMsgBuf(nullptr)
{
	auto func = std::bind(&DataFilePipe::OnMessageReceived, std::ref(*this), _1, _2);
	SetMessageCallback(func);
}

DataFilePipe::~DataFilePipe()
{
	SAFE_DELETE(m_pDataFile);
	SAFE_ARRAY_DELETE(m_DataBlockMsgBuf);
}

void DataFilePipe::OnMessageReceived(const void* aMsgBuffer, sockaddr_in* aAddrFrom)
{
	ProcessCommand(static_cast<const BaseMessage*>(aMsgBuffer));
}

void DataFilePipe::ProcessCommand(const BaseMessage* aMessage)
{
	switch(static_cast<PipeCommand>(aMessage->Command))
	{
	case PipeCommand::pcOpenFileRequest:
		{
			SendOpenFileResponse(static_cast<const OpenFileRequest *>(aMessage));
		}
		break;

	case PipeCommand::pcGetDataRequest:
		{
			SendGetDataResponse(static_cast<const GetDataRequest *>(aMessage));
		}
		break;
	}
}

void DataFilePipe::SendOpenFileResponse(const OpenFileRequest *aRequest)
{
//	std::cout << "Send open file response" << std::endl;
	std::string sFilename(reinterpret_cast<const char*>(aRequest->m_FileName));
//	std::cout << "Requested file is " <<  sFilename.c_str() << std::endl;

	bool result = CreateReader(sFilename);

	OpenFileResponse response;
	response.IP = m_SockAddr.GetIPAddr();
	response.Guid = aRequest->Guid;

	if(true == result)
	{
		// file is opened. m_pDataFile should not be equal nullptr
		assert(nullptr != m_pDataFile);

		const DataFileHeader* header = m_pDataFile->GetFileHeader();
		if(nullptr != header)
		{
			response.m_FileHeader = *header;
			response.m_Result = CommandResult::crOk;
		}
	}
	// else send CommandResult::crError

	if(not Send(&response, response.Size, 10000))
		std::cerr << "Socket send has error or timeout exit" << std::endl;

	if(true == result)
	{
		int bufSize = sizeof(GetDataResponse) + m_pDataFile->GetFrameSize() * m_pDataFile->GetMaxFrameCount();
		m_DataBlockMsgBuf = new uint8_t [bufSize]{};

		m_pDataFile->StartQueueThread();
	}
}

bool DataFilePipe::CreateReader(const std::string& aFileName)
{
	// try to create fresh reader
	auto func1 = std::bind(&DataFilePipe::SendNewDataBlock, std::ref(*this), _1);
	m_pDataFile = m_TryGetFreshDataReader(aFileName, func1);

	if(nullptr != m_pDataFile)
	{
//		std::cout << "Request accepted for realtime file" <<  std::endl;
		return true;
	}

//	std::cout << "Cannot create FreshReader. Continue" <<  std::endl;

	try
	{
		auto func2 = std::bind(&DataFilePipe::SendDataBlocksList, std::ref(*this), _1);
		m_pDataFile = new LocalDataFileReader(aFileName, false, func2);

//		std::cout << "Request accepted for stored file" << std::endl;
	}
	catch(const wchar_t* aError)
	{
		m_pDataFile = nullptr;
		std::cerr << "Cannot create LocalDataFileReader. Error "<< aError << std::endl;
		return false;
	}

	return true;
}

void DataFilePipe::SendGetDataResponse(const GetDataRequest *aRequest)
{
//	std::cout << "Send get data response" << std::endl;

	if(nullptr == m_pDataFile)
		return;

	GetDataResponse* getDataResponse = new (m_DataBlockMsgBuf) GetDataResponse();
	getDataResponse->IP = m_SockAddr.GetIPAddr();
	getDataResponse->Guid = aRequest->Guid;
		
	int size{0};
	bool readResult{true};
	try
	{
		m_pDataFile->ReadBlock(&aRequest->m_DataBlock, getDataResponse->m_pBuf, size);
	}
	catch(const char* aError)
	{
		readResult = false;
		getDataResponse->Size = sizeof(GetDataResponse);

		std::cerr << "Cannot ReadBlock. " << aError << std::endl;
	}

	if(true == readResult)
	{
		getDataResponse->m_Result = CommandResult::crOk;
		getDataResponse->Size = sizeof(GetDataResponse) + size;
	}

	if(not Send(getDataResponse, getDataResponse->Size, 10000))
		std::cerr << "Socket send has error or timeout exit" << std::endl;
}

void DataFilePipe::SendNewDataBlock(DataFileBlock *aDataBlock)
{
	if(false == m_NeedDelete.load())
		return;

	NewDataBlockMessage* msg = new (m_DataBlockMsgBuf) NewDataBlockMessage();
	msg->IP = m_SockAddr.GetIPAddr();
	msg->m_DataBlock = *aDataBlock;

	if(not Send(msg, msg->Size, 10000))
		std::cerr << "Socket send has error or timeout" << std::endl;
}

void DataFilePipe::SendDataBlocksList(std::vector<DataFileBlock*>& aDataBlocks)
{
//	if(false == m_NeedDelete.load())
//		return;

	size_t total{aDataBlocks.size()};
	size_t sizeInBytes = sizeof(DataBlocksListResponse) + total * sizeof(DataFileBlock);

	uint8_t* buff = new uint8_t[sizeInBytes]{};
	DataBlocksListResponse* msg = new (buff) DataBlocksListResponse();

	msg->IP = m_SockAddr.GetIPAddr();
	msg->m_BlockNumber = total;
	msg->Size = sizeInBytes;

	for(size_t i = 0; i < total; ++i)
		msg->m_Blocks[i] = *aDataBlocks[i];

	if(not Send(buff, sizeInBytes, 10000))
		std::cerr << "Socket send has error or timeout" << std::endl;

	SAFE_ARRAY_DELETE(buff);
}
