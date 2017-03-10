#pragma once
#ifndef BASEDATAFILE_H
#define BASEDATAFILE_H

#ifndef _LARGEFILE64_SOURCE
#define _LARGEFILE64_SOURCE
#endif

#include "DataFileCommonTypes.h"
#include <sys/types.h>
//#include <unistd.h>
#include <cstdio>
#include <cassert>

namespace DataFile
{
class BaseDataFile : public HelperFunctions::LockableResource
{
protected:
#ifdef WINDOWS_OS
	HANDLE m_hFile;
#else
	int m_hFile;
#endif
	DataFileHeader* m_pHeader;

	DataFileBlockList* m_pBlockList;
    DataType m_DataType;
	std::string	m_FileName;

	int m_verMajor;
	int m_verMinor;

public:
	BaseDataFile();
    virtual ~BaseDataFile();
	void SetGUID(const GUID& aGUID, const GUID& aTripGUID);

	inline void GetFileVersion(int& aMajor, int& aMinor)
	{
		if(m_pHeader)
		{
			m_pHeader->GetVersion(m_verMajor, m_verMinor);
		}

		aMajor = m_verMajor;
		aMinor = m_verMinor;
	}

	inline DataFileBlock * GetDataBlock(int aIndex)
	{
		DataFileBlock *db{nullptr};

		LOCK(m_pBlockList);
		db = m_pBlockList->GetBlock(aIndex);
		return db;
	}

	inline int GetMaxFrameCount() const
	{
		if (m_pHeader)
			return m_pHeader->m_MaxFrameCount;

		return -1;
	}

	inline int GetFrameSize() const
	{
		if (m_pHeader)
			return m_pHeader->m_FrameSize;

		return -1;
	}

	inline int64_t GetSpecificData()  const
	{
		if (m_pHeader)
			return m_pHeader->m_SpecificData;

		return -1;
	}

	inline int GetFrameCount()
	{
		int fc{-1};

		LOCK(m_pBlockList);
		fc = m_pBlockList->FrameCount();

		return fc;
	}

	inline size_t GetBlockCount() const
	{
		LOCK(m_pBlockList);
		return m_pBlockList->GetBlockCount();
	}

	inline DataFileBlock* FindBlock(int aFrame)
	{
		DataFileBlock *db{nullptr};

		LOCK(m_pBlockList);
		db = m_pBlockList->FindBlock(aFrame);

		return db;
	}

	inline DataFileBlock * FindByMF(MASTERFRAME aMF) const
	{
		DataFileBlock *db{nullptr};

		LOCK(m_pBlockList);
		db = m_pBlockList->FindByMF(aMF);

		return db;
	}

	inline MASTERFRAME FromMF(void) const
	{
		MASTERFRAME mf{INVALID_MASTERFRAME};

		LOCK(m_pBlockList);
		mf = m_pBlockList->FromMF();

		return mf;
	}

    inline MASTERFRAME ToMF(void) const
	{
		MASTERFRAME mf{INVALID_MASTERFRAME};

		LOCK(m_pBlockList);
		mf = m_pBlockList->ToMF();

		return mf;
	} 

    inline void Clear(void)
	{
		LOCK(m_pBlockList);
		m_pBlockList->Clear();

		return;
	}

	inline const char* GetCFilename() const
	{
		return m_FileName.c_str();
	}

	inline std::string GetFilename() const
	{
		return m_FileName;
	}

    inline DataType GetDataType(void) const
	{
		return m_DataType;
	}

	inline bool IsZipped()
	{
		return m_pHeader ? m_pHeader->m_bZipped : false;
	}

    inline int  GetTripID(void) const
	{
		if(nullptr == m_pHeader)
			return m_pHeader->m_TripID;

		return -1;
	}
	
	inline GUID GetTripGUID(void) const
	{
		if(nullptr == m_pHeader)
			return m_pHeader->m_TripGuid;
		
		return GUID_NULL;
	}

	inline GUID GetGUID(void) const
	{
		if(nullptr == m_pHeader)
			return m_pHeader->m_Guid;
		
		return GUID_NULL;
	}
	
	virtual BaseDataFile* MakeNewInstance(void)
	{
		return nullptr;
	}

	inline DataFileHeader* GetFileHeader() const
	{
		return m_pHeader;
	}

#pragma region File I/O wrappers
    inline bool SetFilePos(DATAFILEPOS pos)
    {
#ifdef WINDOWS_OS
		LARGE_INTEGER li;
		LARGE_INTEGER li_new;
        li.QuadPart = pos;
		return SetFilePointerEx(m_hFile, li, &li_new, FILE_BEGIN) && (li.QuadPart == li_new.QuadPart);
#else
		lseek64(m_hFile, static_cast<off64_t>(pos), SEEK_SET);
		return true;
#endif
    }

    //перемещает текущую позицию в файле относительно текущей вперед или назад на заданное
	//количество байт
    inline bool MoveFilePos(DATAFILEPOS pos)
    {
#ifdef WINDOWS_OS
        LARGE_INTEGER li;
		LARGE_INTEGER li_new;
        li.QuadPart = pos;
        return SetFilePointerEx(m_hFile, li, &li_new, FILE_CURRENT) && (li.QuadPart == li_new.QuadPart);
#else
		lseek64(m_hFile, static_cast<off64_t>(pos), SEEK_CUR);
		//assert(0);
		return true;
#endif
    }

    inline DATAFILEPOS SetFilePosToEnd()
    {
#ifdef WINDOWS_OS
        LARGE_INTEGER li;
        LARGE_INTEGER zero;
        zero.QuadPart = 0;
        SetFilePointerEx(m_hFile, zero, &li, FILE_END);
        return li.QuadPart;
#else
		return lseek64(m_hFile, 0, SEEK_END);
		//return INVALID_DATAFILEPOS;
#endif
    }

    inline DATAFILEPOS GetFilePos()
    {
#ifdef WINDOWS_OS
        LARGE_INTEGER li;
        LARGE_INTEGER zero;
        zero.QuadPart = 0;
        SetFilePointerEx(m_hFile, zero, &li, FILE_CURRENT);
        return li.QuadPart;
#else
		return lseek64(m_hFile, 0, SEEK_CUR);
		//return INVALID_DATAFILEPOS;
#endif
    }

    inline DATAFILEPOS GetFileSize()
    {
#ifdef WINDOWS_OS
		LARGE_INTEGER li;
		GetFileSizeEx(m_hFile, &li);
		return li.QuadPart;
#else
		off64_t oldPos = GetFilePos();
		off64_t size = SetFilePosToEnd();
		SetFilePos(oldPos);

		return size;
#endif
	}

#pragma endregion
};
}
#endif
