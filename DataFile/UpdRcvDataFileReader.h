#ifndef UpdRcvDataFileReader_H
#define UpdRcvDataFileReader_H
#pragma once

#include "BaseDataFileReader.h"
#include "ClientSide.h"
#include "DataFileServerTypes.h"
#include "Events.h"
#include <atomic>

namespace DataFile
{
	class UpdRcvDataFileReader : public BaseDataFileReader, public ClientSide
	{
	private:

	protected:
		std::atomic_bool m_FileSuccessfullyOpened;
		Events::HANDLE m_GetDataResponseReady;

	private:
		void ParseMessage(const BaseMessage* aMessage);
		bool ProcessOpenFileResponse(const OpenFileResponse* aMessage);
		bool NewDataBlockResponse(const NewDataBlockMessage* aMessage);
		bool NewDataBloclsListResponse(const DataBlocksListResponse* aMessage);
		bool GetDataBlockResponse(const GetDataResponse* aMessage);

	protected:
		void SendOpenFileRequest();
		void SendGetDataFileBlockRequest(const DataFileBlock * const aDataBlock);
		virtual void UpdateLocalStorage(DataFileBlock* dataBlock){};
		void MessageSeparation(int aMsgType, const void* aMsgBuffer);
		virtual void ProcessGetDataResponse(const GetDataResponse* aMessage){};

	public:
		UpdRcvDataFileReader(const std::string& aFileName, const std::string& aDstAddr);
		virtual ~UpdRcvDataFileReader();

		bool PostCreateEvents();
		bool IsFileOpened();
	};
}
#endif
