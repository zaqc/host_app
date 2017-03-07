#pragma once
#ifndef FreshLocalDataFileReaderH
#define FreshLocalDataFileReaderH

#include "BaseLocalDataFileReader.h"

namespace DataFile
{
class DataFileWriter;

class FreshLocalDataFileReader : public BaseLocalDataFileReader
{
protected:
	DataFileWriter* m_pDataFileWriter;

public:
	FreshLocalDataFileReader(DataFileWriter* aDataFileWriter, const std::string& aFileName
							, DataFileBlockList *aBlockList, DataFileHeader* pHeader
							, NewDataBlockProc<DataFileBlock> aNewDataBlockProc = nullptr);
	virtual ~FreshLocalDataFileReader();

	void NewDataBlockHandler(const DataFileBlock& aDataBlock);
	//Вызывается из DataFileWriter когда тот закрывается, чтобы предотвратить отписку
	//от несущесвующего DataFileWriter при закрытии этого файла
	void FileWriterClosed();
	virtual BaseDataFileReader* MakeNewInstance();
};
}

#endif
