#pragma once
#ifndef DATAFILESERVER_H
#define DATAFILESERVER_H

#include "ServerSide.h"
#include "DataFilePipe.h"

class SocketConnector;

namespace DataFile
{
class DataFileServer : public ServerSide
{
protected:
	DataFileSourceGetter m_TryGetFreshDataReader;

protected:
	virtual void CreateSocket(SocketConnector*& aSocketConnector, SOCKET aSocket, bool bAutoStart
								, SockAddr* aSockAddr, int aReadBufferSize, int aWriteBufferSize);
public:
    DataFileServer(DataFileSourceGetter aCallback);
    virtual ~DataFileServer();
};
}
#endif
