#include "BaseLocalDataFileReader.h"
#include "HelperFunctions.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <cassert>
#include <cwchar>
#include <cstdio>
#include <errno.h>

using namespace DataFile;
using namespace std::placeholders;
using std::wstring;
using std::string;

BaseLocalDataFileReader::BaseLocalDataFileReader(const string& aFileName
													, NewDataBlockProc<DataFileBlock> aNewDataBlockProc)
	: BaseDataFileReader()
	, QueueExtractor<DataFileBlock>(std::bind(&BaseLocalDataFileReader::NewDataBlock, std::ref(*this), _1))
	, m_Proc(aNewDataBlockProc)
	, m_bNeedFix(false)
{
	m_FileName = aFileName;

	m_hFile = open(m_FileName.c_str(), O_RDONLY | O_LARGEFILE, S_IRWXU);
	int fileErr{errno};
	assert(-1 != m_hFile);

	if(-1 == m_hFile)
	{
		wchar_t sError[256]{};
		std::swprintf(sError, sizeof(sError), L"Не удалось открыть файл. Ошибка %d", fileErr);
		throw sError;
	}
}

BaseLocalDataFileReader::~BaseLocalDataFileReader()
{
}

void BaseLocalDataFileReader::NewDataBlock(DataFileBlock *aDataBlock)
{
	if(nullptr == aDataBlock)
		return;
	
	LOCK(m_pBlockList);

	// указатели aDataBlock освободятся в m_pBlockList
	m_pBlockList->Add(aDataBlock);

	if(nullptr != m_Proc)
		m_Proc(aDataBlock);
}

void BaseLocalDataFileReader::ReadBlock(const DataFileBlock* aDataBlock, void* aBuffer, int& aSize)
{
	if(nullptr != aDataBlock)
	{
		DATAFILEPOS filePosition = aDataBlock->m_FilePosition + sizeof(DataFileBlock);
		
		if(!SetFilePos(filePosition))
			throw "Can't set file pointer on position";

		ssize_t nr = static_cast<ssize_t>(aDataBlock->m_ZippedSize);
		unsigned char* buf = static_cast<unsigned char*>(aBuffer);

		aSize = 0;
		while(nr > 0)
		{
			ssize_t bytes = read(m_hFile, static_cast<void*>(buf), nr);
			int fileErr{errno};
			assert(-1 != bytes);
			if(-1 != bytes)
			{
				buf += bytes;
				nr -= bytes;
				aSize += bytes;
			}
			else
				throw "Can't read data from DataFile";
		}
	}
	else
		throw "Invalid parametr value: aDataBlock is nullptr";
}
