#include "FreshLocalDataFileReader.h"
#include "DataFileWriter.h"

using namespace DataFile;
using namespace HelperFunctions;
using std::wstring;
using std::string;

FreshLocalDataFileReader::FreshLocalDataFileReader(DataFileWriter* aDataFileWriter, const string& aFileName
													, DataFileBlockList *aBlockList, DataFileHeader* aHeader
													, NewDataBlockProc<DataFileBlock> aNewDataBlockProc)
	: BaseLocalDataFileReader(aFileName, aNewDataBlockProc)
	, m_pDataFileWriter(aDataFileWriter)
{
	m_DataType = aDataFileWriter->GetDataType();
	m_pHeader = new DataFileHeader(*aHeader);

    size_t count = aBlockList->GetBlockCount();
    if(count != 0)
    {
		EnterCriticalSection(&m_lockDataBlockQueue);
		for(size_t i = 0; i < count; i++)
			m_queDataBlocks.push(new DataFileBlock(*aBlockList->GetBlock(i)));
    	LeaveCriticalSection(&m_lockDataBlockQueue);
    }
}

FreshLocalDataFileReader::~FreshLocalDataFileReader()
{
	if(nullptr != m_pDataFileWriter)
		m_pDataFileWriter->Unsubscribe(this);
}

void FreshLocalDataFileReader::NewDataBlockHandler(const DataFileBlock& aDataBlock)
{
    EnterCriticalSection(&m_lockDataBlockQueue);

	m_queDataBlocks.push(new DataFileBlock(aDataBlock));
	SetEvent(m_hNewDataBlockEvent);

	LeaveCriticalSection(&m_lockDataBlockQueue);
}

BaseDataFileReader* FreshLocalDataFileReader::MakeNewInstance(void)
{
    if(nullptr != m_pDataFileWriter)
    {
        FreshLocalDataFileReader* pFile = m_pDataFileWriter->GetReader(nullptr, nullptr);
		if(nullptr != pFile)
		{
			pFile->StartQueueThread();
			return pFile;
		}
    }
    return nullptr;
}

void FreshLocalDataFileReader::FileWriterClosed()
{ 
	m_pDataFileWriter = nullptr;
}
