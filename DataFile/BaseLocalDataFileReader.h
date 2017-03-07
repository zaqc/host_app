#pragma once
#ifndef BASE_LOCAL_DATA_FILE_READER_H
#define BASE_LOCAL_DATA_FILE_READER_H

#include "QueueExtractor.h"
#include "BaseDataFileReader.h"

namespace DataFile
{
struct DataFileBlock;

using DataBlocksProc = std::function<void(std::vector<DataFileBlock*>& aDataBlocks)>;

class BaseLocalDataFileReader : public BaseDataFileReader, public QueueExtractor<DataFileBlock>
{
private:
	NewDataBlockProc<DataFileBlock> m_Proc;

protected:
	bool m_bNeedFix;

private:
	void NewDataBlock(DataFileBlock* aDataBlock);
	
public:
	BaseLocalDataFileReader(const std::string& aFileName
								, NewDataBlockProc<DataFileBlock> aNewDataBlockProc = nullptr);
	virtual ~BaseLocalDataFileReader(void);

	virtual void ReadBlock(const DataFileBlock* aDataBlock, void* aBuffer, int& aSize);
};
}
#endif
