#pragma once
#ifndef BASEDATAFILEREADER_H
#define BASEDATAFILEREADER_H

#include "BaseDataFile.h"

namespace DataFile
{
class BaseDataFileReader : public BaseDataFile
{
public:
	BaseDataFileReader() : BaseDataFile() {}
	virtual ~BaseDataFileReader() {}
	virtual void ReadBlock(const DataFileBlock *aDataBlock, void *aBuffer, int &aSize) = 0;
	virtual BaseDataFileReader* MakeNewInstance()
	{
		return nullptr;
	}
};
}
#endif
