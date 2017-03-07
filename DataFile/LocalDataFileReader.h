#pragma once
#ifndef LocalDataFileReaderH
#define LocalDataFileReaderH
#include "BaseLocalDataFileReader.h"

namespace DataFile
{
	class LocalDataFileReader : public BaseLocalDataFileReader
	{
	protected:
		DataBlocksProc m_pfnNewDataBlockProc;
		int m_BrokenBlockListSize;
	
	public:
		//если aAutoFix = true, файл с поврежденным заголовком восстанавливается автоматически, иначе
		//о необходимости восстановления можно узнать из метода NeedFix и восстановить методом Fix
		LocalDataFileReader(const std::string& aFileName, bool aAutoFix = true
							, DataBlocksProc aNewDataBlockProc = nullptr);
		virtual ~LocalDataFileReader();
		bool NeedFix() const;
		bool Fix();
		static bool TestFile(const std::string& aFileName, int* pTripID = nullptr
								, DataType* pDT = nullptr, MASTERFRAME* pFromMF = nullptr
								, MASTERFRAME* pToMF = nullptr, int64_t* pFileSize = nullptr
								, GUID* pGuid = nullptr);
		static bool TestFileUnbuffered(const std::string& aFileName, int* pTripID = nullptr
										, DataType* pDT = nullptr, int64_t* pFileSize = nullptr);
		virtual BaseDataFileReader* MakeNewInstance(void);
		void ProcessNewDataBlock(DataFileBlock *aDataBlock);
	};
}
#endif
