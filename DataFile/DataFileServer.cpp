#include "DataFileServer.h"
#include "DataFilePipe.h"
//#include "..\SS_Log\SS_Log.h"

using namespace DataFile;
using namespace std;

DataFileServer::DataFileServer(DataFileSourceGetter aCallback)
	: ServerSide(15152)
	, m_TryGetFreshDataReader(aCallback)
{
}

DataFileServer::~DataFileServer()
{
}

void DataFileServer::CreateSocket(SocketConnector*& aSocketConnector, SOCKET aSocket, bool bAutoStart
							, SockAddr* aSockAddr, int aReadBufferSize, int aWriteBufferSize)
{
	aSocketConnector = new DataFilePipe(aSocket, m_TryGetFreshDataReader, bAutoStart, aSockAddr
											, aReadBufferSize, aWriteBufferSize);
}
