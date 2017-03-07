#include "UpdRcvDataFileReader.h"
//#include "..\SS_Log\SS_Log.h"
//#include <mstcpip.h>
//#include <WS2tcpip.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <unistd.h>
#include <fcntl.h>
#include <utility>
#include <iostream>
#include "SocketAddr.h"
#include <cstdlib>
#include <ctime>

using DataFile::UpdRcvDataFileReader;
using DataFile::PipeCommand;
using std::string;
using namespace std::placeholders;

UpdRcvDataFileReader::UpdRcvDataFileReader(const std::string& aFileName, const std::string& aDstAddr)
	: BaseDataFileReader()
	, ClientSide(aDstAddr.c_str(), 15152, true)
	, m_FileSuccessfullyOpened(false)
	, m_GetDataResponseReady()
{
	std::srand(static_cast<unsigned int>(std::time(nullptr)));
//	std::cout << "UpdRcvDataFileReader constructor file " << aFileName << std::endl;
	m_FileName = aFileName;

	m_GetDataResponseReady = Events::CreateEvent(false, false);
}

UpdRcvDataFileReader::~UpdRcvDataFileReader()
{
	Events::DestroyEvent(m_GetDataResponseReady);
}

bool UpdRcvDataFileReader::PostCreateEvents()
{
	auto func = std::bind(&UpdRcvDataFileReader::MessageSeparation, std::ref(*this), _1, _2);
	BeginConnect(func);

	while(not IsConnected())
	{
		usleep(1000); //micro sec
		;
//		std::cout << "Client: waiting connection" << std::endl;
	}

	if(true != IsConnected())
	{
		std::cout << "Socket has not been opened for " << m_SockAddr.ToString() << "\\" << m_FileName << std::endl;
		return false;
	}

	return true;
}

void UpdRcvDataFileReader::SendOpenFileRequest()
{
	if(256 < m_FileName.length())
	{
		std::cout << "Size of file name to big: " << m_FileName << std::endl;
		return;
	}

	OpenFileRequest request;
	request.IP = m_SockAddr.GetIPAddr();
	request.Guid = std::rand();
	request.m_FileNameLen = m_FileName.length();

	memcpy(request.m_FileName, m_FileName.c_str(), m_FileName.length());

	bool res = Send(&request, true, 10000);
	if(not res)
	{
		std::cerr << "Socket Send has error or timeout exit" << std::endl;
	}
}

void UpdRcvDataFileReader::SendGetDataFileBlockRequest(const DataFileBlock * const aDataBlock)
{
	GetDataRequest request;
	request.IP = m_SockAddr.GetIPAddr();
	request.Guid = std::rand();
	request.m_DataBlock = *aDataBlock;
	bool res = Send(static_cast<void*>(&request), true, INFINITE);
	if(not res)
	{
//		LOCK(m_pBlockList);
//		m_pBlockList->Clear();

		std::cerr << "Socket Send has error or timeout exit" << std::endl;
	}
}

bool UpdRcvDataFileReader::ProcessOpenFileResponse(const OpenFileResponse* aMessage)
{
//	std::cout << "UpdRcvDataFile: Open File Response" << std::endl;
	if(CommandResult::crOk != aMessage->m_Result)
	{
		std::cerr << "ERROR: Open file response " << m_SockAddr.ToString() << "\\" << m_FileName << "on server-side" << std::endl;
		return false;
	}

	if(not m_pHeader)
		m_pHeader = new DataFileHeader(aMessage->m_FileHeader);
	
	m_DataType = DataTypeConverter::ToDataType(reinterpret_cast<const char*>(aMessage->m_FileHeader.m_strDataType));
	sscanf_s(reinterpret_cast<const char*>(aMessage->m_FileHeader.m_Version), "%d.%d", &m_verMajor, &m_verMinor);

	m_FileSuccessfullyOpened.store(true);

//	std::cout << "Open file response processed " << m_SockAddr.ToString() << "\\" << m_FileName << std::endl;

	return true;
}

bool UpdRcvDataFileReader::NewDataBlockResponse(const NewDataBlockMessage* aMessage)
{
//	std::cout << "UpdRcvDataFile: New Data Block Response" << std::endl;
	LOCK(m_pBlockList);

	if((0xFFFFFFFF != aMessage->m_DataBlock.m_FromMF) && (0xFFFFFFFF != aMessage->m_DataBlock.m_ToMF))
	{
		DataFileBlock* block = new DataFileBlock((aMessage->m_DataBlock));
		m_pBlockList->Add(block);

		UpdateLocalStorage(block);
	}
	//std::cout << "New block has arrived" << std::endl;

	return true;
}

bool UpdRcvDataFileReader::NewDataBloclsListResponse(const DataBlocksListResponse* aMessage)
{
//	std::cout << "UpdRcvDataFile: New Data Block List Response" << std::endl;
	LOCK(m_pBlockList);

	size_t total = aMessage->m_BlockNumber;

	for(size_t i = 0; i < total; ++i)
	{
		if((0xFFFFFFFF != aMessage->m_Blocks[i].m_FromMF) && (0xFFFFFFFF != aMessage->m_Blocks[i].m_ToMF))
		{
			DataFileBlock* block = new DataFileBlock(aMessage->m_Blocks[i]);
			m_pBlockList->Add(block);
			UpdateLocalStorage(block);
		}
	}

//	std::cout << "Array of blocks has arrived" << std::endl;

	return true;
}

bool UpdRcvDataFileReader::GetDataBlockResponse(const GetDataResponse* aMessage)
{
//	std::cout << "UpdRcvDataFile: Get Data Block Response" << std::endl;
	ProcessGetDataResponse(aMessage);
	Events::SetEvent(m_GetDataResponseReady);
	return true;
}

void UpdRcvDataFileReader::ParseMessage(const BaseMessage* aMessage)
{
	//LogDebug(L"ParseMessage");
	switch(static_cast<PipeCommand>(aMessage->Command))
	{
	case PipeCommand::pcOpenFileResponse:
		{
			ProcessOpenFileResponse(static_cast<const OpenFileResponse*>(aMessage));
			break;
		}

	case PipeCommand::pcNewDataBlockMessage:
		{
			NewDataBlockResponse(static_cast<const NewDataBlockMessage*>(aMessage));
			break;
		}

	case PipeCommand::pcDataBlocksResponse:
		{
			NewDataBloclsListResponse(static_cast<const DataBlocksListResponse*>(aMessage));
			break;
		}
	case PipeCommand::pcGetDataResponse:
		{
			GetDataBlockResponse(static_cast<const GetDataResponse*>(aMessage));
			break;
		}
 	default:
 		;
//		__assume(0);
	}
}

void UpdRcvDataFileReader::MessageSeparation(int aMsgType, const void* aMsgBuffer)
{
	switch(aMsgType)
	{
	case WM_CLIENT_CONNECT:
//		std::cout << "Client: callback - client connected" << std::endl;
		SendOpenFileRequest();
		break;
	case WM_CLIENT_DISCONNECT:
//		std::cout << "Client: callback - client disconnected" << std::endl;
		m_FileSuccessfullyOpened.store(false);
		break;
	case WM_MESSAGE_RECEIVED:
//		std::cout << "Client: callback - client has got a message" << std::endl;
		ParseMessage(static_cast<const BaseMessage*>(aMsgBuffer));
		break;
	default:
//		std::cout << "Client: callback" << std::endl;
		;
	}
}

bool UpdRcvDataFileReader::IsFileOpened()
{
	return m_FileSuccessfullyOpened.load();
}
