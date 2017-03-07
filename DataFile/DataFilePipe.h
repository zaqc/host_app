#pragma once

#include "BaseLocalDataFileReader.h"
#include "DataFileServerTypes.h"
#include "SocketTCPConnector.h"
//#include <WinSock2.h>
#include <functional>
#include <atomic>

namespace DataFile
{

class DataFileServer;
class BaseLocalDataFileReader;

using DataFileSourceGetter = std::function<BaseLocalDataFileReader*(const std::string& aFileName
																	, NewDataBlockProc<DataFileBlock> aNewDataBlockProc)>;

class DataFilePipe : public SocketTCPConnector
{
private:
	DataFileSourceGetter m_TryGetFreshDataReader;
    BaseLocalDataFileReader* m_pDataFile;
    uint8_t* m_DataBlockMsgBuf;

public:

protected:
    void SendNewDataBlock(DataFileBlock *aDataBlock);
	void SendDataBlocksList(std::vector<DataFileBlock*>& aDataBlocks);
	void OnMessageReceived(const void* aMsgBuffer, sockaddr_in* aAddrFrom);
	bool CreateReader(const std::string& aFileName);

public:
	DataFilePipe(SOCKET aSocket, DataFileSourceGetter aCallback, bool bAutoStart, SockAddr* aSockAddr
					, int aReadBufferSize, int aWriteBufferSize);
	
    virtual ~DataFilePipe();

    void AddDebugString();
    void ProcessCommand(const BaseMessage* aMessage);

    void SendOpenFileResponse(const OpenFileRequest *aRequest);
    void SendGetDataResponse(const GetDataRequest *aRequest);
};
}
