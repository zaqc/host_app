#pragma once
#ifndef DATAFILECOMMONTYPES_H
#define DATAFILECOMMONTYPES_H

#include "DataType.h"
#include <vector>
#include "HelperFunctions.h"
#include <cstring>
#include <cstdint>

typedef uint64_t DATAFILEPOS;
#define INVALID_DATAFILEPOS 0xFFFFFFFFFFFFFFFF //UINT64_MAX

#define STD_SIGNATURE "MVD"
#define CUR_VERSION "02.02"

#define CUR_VERSION_MAJOR 2
#define CUR_VERSION_MINOR 2

namespace DataFile
{
#pragma pack(push, 1)
	struct DataFileHeader
	{
	public:
		int8_t 		m_Signature[3];			// Signature MVD
		int8_t		m_Version[5];			// Версия файла
		int8_t		m_strDataType[16];		// тип файла например "USM", "LineVideo1" или "FrameVideo1"
		int32_t		m_TripID;				// Идентификатор проезда в базе, к которому привязан файл, -1 если без привязки
		DATAFILEPOS m_BlockListPos;			// позиция списка блоков
		DATAFILEPOS m_MarkerListPos;		// for future use
		int32_t		m_FrameSize;			// размер фрейма
		int32_t		m_MaxFrameCount;		// количество фреймов в блоке
		bool		m_bZipped;				// применяется сжатие блоков данных
		int64_t		m_SpecificData;			// Дополнительные данные специфичные для каждого устройства
		GUID		m_Guid;					// Уникальный идентификатор файла
		GUID		m_TripGuid;				// Уникальный идентификатор проезда в базе данных, к которому привязан файл

	public:
		DataFileHeader(bool bInit = true)
			: m_Signature()
			, m_Version()
			, m_strDataType()
			, m_TripID(-1)
			, m_BlockListPos(INVALID_DATAFILEPOS)
			, m_MarkerListPos(INVALID_DATAFILEPOS)
			, m_FrameSize(0)
			, m_MaxFrameCount(0)
			, m_bZipped(false)
			, m_SpecificData(0)
			, m_Guid(GUID_NULL)
			, m_TripGuid(GUID_NULL)
		{
			if(true == bInit)
			{
				memcpy(m_Signature, STD_SIGNATURE, strlen(STD_SIGNATURE));

				char sVer[8]{};
				sprintf_s(sVer, sizeof(sVer), "%02d.%02d", static_cast<int>(CUR_VERSION_MAJOR), static_cast<int>(CUR_VERSION_MINOR));
				memcpy(m_Version, sVer, sizeof(m_Version));

				m_FrameSize	= 4096;
				m_MaxFrameCount = 1024;
			}
		}

		DataFileHeader(const GUID& aGuid, const GUID& aTripGUID)
			: m_Signature()
			, m_Version()
			, m_strDataType()
			, m_TripID(-1)
			, m_BlockListPos(INVALID_DATAFILEPOS)
			, m_MarkerListPos(INVALID_DATAFILEPOS)
			, m_FrameSize(4096)
			, m_MaxFrameCount(1024)
			, m_bZipped(false)
			, m_SpecificData(0)
			, m_Guid(aGuid)
			, m_TripGuid(aTripGUID)
		{
			memcpy(m_Signature, STD_SIGNATURE, strlen(STD_SIGNATURE));

			char sVer[8]{};
			sprintf_s(sVer, sizeof(sVer), "%02d.%02d", static_cast<int>(CUR_VERSION_MAJOR), static_cast<int>(CUR_VERSION_MINOR));
			memcpy(m_Version, sVer, sizeof(m_Version));
		}

		void GetVersion(int& aMajor, int& aMinor) const
		{
			sscanf_s(reinterpret_cast<const char*>(m_Version), "%d.%d", &aMajor, &aMinor);
		}

		void SetDataTypeString(DataType aDataType)
		{
			const char *data_type_str = DataTypeConverter::ToString(aDataType);
			strncpy_s(reinterpret_cast<char*>(m_strDataType), sizeof(m_strDataType), data_type_str, 16);
		}
	};
#pragma pack(pop)

#pragma pack(push, 4)
	struct DataFileBlock
	{
	public:
		DATAFILEPOS m_FilePosition;
		int32_t m_BlockSize;
		int32_t	m_ZippedSize;
		int32_t	m_FromFrame;
		int32_t	m_FrameCount;

		MASTERFRAME	m_FromMF;
		MASTERFRAME	m_ToMF;

	public:
		DataFileBlock();
		DataFileBlock(DATAFILEPOS aFilePosition, int32_t aBlockSize, int32_t aZippedSize
							, int32_t aFromFrame, int32_t aFrameCount, int32_t aBlockIndex
							, MASTERFRAME aFromMF, MASTERFRAME aToMF);
		int32_t FrameFromMF(MASTERFRAME aMF) const;
		bool operator== (const DataFileBlock& aBlock) const;
		void GetVersion(int& aMajor, int& aMinor) const;
		void Clear();

		inline bool FrameInBlock(int aFrame) const
		{
			return ((aFrame >= m_FromFrame) && (aFrame < m_FromFrame + m_FrameCount));
		}

		inline bool MFInBlock(MASTERFRAME aMF) const
		{
			return ((aMF >= m_FromMF) && (aMF < m_ToMF));
		}

		inline bool IndexInBlock(MASTERFRAME aFrameOrMF, bool aIsMasterframe) const
		{
			return aIsMasterframe ? MFInBlock(aFrameOrMF) : FrameInBlock((int)aFrameOrMF);
		}
	};
#pragma pack(pop)

	class DataFileBlockList :public HelperFunctions::LockableResource
	{
	private:
		std::vector<DataFileBlock*>	m_vecBlocks;
		int	m_FrameCount;
		MASTERFRAME m_FromMF;
		MASTERFRAME m_ToMF;

	private:
		void ClearInternalStorage();

	public:
		DataFileBlockList();
		void Assign(DataFileBlockList *aDataBlockList);
		virtual ~DataFileBlockList();

		DataFileBlock * FindBlock(int aFrame) const;
		DataFileBlock * FindByMF(MASTERFRAME aMF) const;
		//int FrameFromMF(MASTERFRAME aMF) const;
		DataFileBlock * Add(DataFileBlock& aDataFileBlock);
		void Add(DataFileBlock *aDataBlock);
		void Read(int aFileId);
		void Write(int aFileId);
		DataFileBlock * GetBlock(size_t aIndex) const;
		void Clear();

		int FrameCount() const 
		{ 
			return m_FrameCount; 
		}

		MASTERFRAME FromMF() const 
		{ 
			return m_FromMF; 
		}

		MASTERFRAME ToMF() const 
		{ 
			return m_ToMF; 
		}

		size_t GetBlockCount() const 
		{ 
			return m_vecBlocks.size(); 
		}
	};
}
#endif
