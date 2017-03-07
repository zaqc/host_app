#pragma once
#ifndef NetworkDataFileReaderH
#define NetworkDataFileReaderH

#include "UpdRcvDataFileReader.h"

namespace DataFile
{
	class NetworkDataFileReader : public UpdRcvDataFileReader
	{
	private:
		uint8_t* m_buffer;
		Events::HANDLE m_ThreadTerminationEvent;
		Events::HANDLE* m_ReadBlocksEventArray;
		size_t m_ReadBlocksEventsTotal;
		int m_size;

	private:
		void ProcessGetDataResponse(const GetDataResponse* aMessage);

	public:
		NetworkDataFileReader(const std::string& aFileName, const std::string& aDstAddr);
		virtual ~NetworkDataFileReader();
		virtual void ReadBlock(const DataFileBlock *aDataBlock, void *aBuffer, int &aSize);
		virtual BaseDataFileReader* MakeNewInstance();
	};
}
#endif
