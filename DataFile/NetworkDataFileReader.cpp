#include "NetworkDataFileReader.h"
#include <iostream>
//#include "..\SS_Log\SS_Log.h"

using DataFile::NetworkDataFileReader;
using DataFile::DataFileBlock;
using DataFile::BaseDataFileReader;

NetworkDataFileReader::NetworkDataFileReader(const std::string& aFileName, const std::string& aDstAddr)
	: UpdRcvDataFileReader(aFileName, aDstAddr)
	, m_buffer(nullptr)
	, m_ThreadTerminationEvent()
	, m_ReadBlocksEventArray(nullptr)
	, m_ReadBlocksEventsTotal(2)
	, m_size(0)
{
	m_ThreadTerminationEvent = Events::CreateEvent(true, false);
	m_ReadBlocksEventArray = new Events::HANDLE[m_ReadBlocksEventsTotal]{m_ThreadTerminationEvent, m_GetDataResponseReady};
}

NetworkDataFileReader::~NetworkDataFileReader()
{
	SAFE_ARRAY_DELETE(m_ReadBlocksEventArray);
	Events::DestroyEvent(m_ThreadTerminationEvent);
}

BaseDataFileReader* NetworkDataFileReader::MakeNewInstance()
{
	NetworkDataFileReader* reader = new NetworkDataFileReader(m_FileName, m_SockAddr.ToString());
	reader->PostCreateEvents();
	return static_cast<BaseDataFileReader*>(reader);
}

void NetworkDataFileReader::ReadBlock(const DataFileBlock *aDataBlock, void *aBuffer, int &aSize)
{
	if(true != IsConnected())
	{
		std::cout << "Socket has not connected" << std::endl;
		throw L"Socket has not connected";
	}

	if(true != m_FileSuccessfullyOpened.load())
	{
		std::cout << "File has not opened" << std::endl;
		throw L"File has not opened";
	}

//	m_buffer = static_cast<GetDataResponse*>(aBuffer);
	m_buffer = static_cast<uint8_t*>(aBuffer);

	SendGetDataFileBlockRequest(aDataBlock);

	int index{0};
	Events::WaitForMultipleObjects(m_ReadBlocksEventsTotal, m_ReadBlocksEventArray, false, INFINITE, index);

	if(WAIT_OBJECT_0 == index) // m_ThreadTerminationEvent
	{
		std::cout << "NetworkDataFileReader::ReadBlock" << std::endl;
		LOCK(m_pBlockList);
		m_pBlockList->Clear();

		throw L"Termination event";
	}

	if(WAIT_OBJECT_0 + 1 == index) // m_hResponseEvent m_GetDataResponseReady
		aSize = m_size;
}

void NetworkDataFileReader::ProcessGetDataResponse(const GetDataResponse* aMessage)
{
	if(CommandResult::crError == aMessage->m_Result)
	{
		std::cout << "Requested datablock is not available" << std::endl;
		return;
	}

	m_size = aMessage->Size - sizeof(GetDataResponse);

	if(nullptr != m_buffer)
		memcpy(m_buffer, aMessage->m_pBuf, m_size);
}
