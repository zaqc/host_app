#include "BaseDataFile.h"
#include <errno.h>

using namespace DataFile;

BaseDataFile::BaseDataFile(void)
	: HelperFunctions::LockableResource()
	, m_hFile(-1)
	, m_pHeader(nullptr)
	, m_pBlockList(nullptr)
	, m_DataType(dtNONE)
	, m_verMajor(-1)
	, m_verMinor(-1)
{
	m_pBlockList = new DataFileBlockList();
}

BaseDataFile::~BaseDataFile(void)
{
	SAFE_DELETE(m_pBlockList);
	SAFE_DELETE(m_pHeader);

	if(-1 != m_hFile)
	{
		int res = close(m_hFile);
		int fileErr{errno};
		assert(-1 != res);

		m_hFile = -1;
	}
}

void BaseDataFile::SetGUID(const GUID& aGUID, const GUID& aTripGUID)
{
	if(nullptr != m_pHeader && -1 != m_hFile)
	{
		m_pHeader->m_Guid = aGUID;
		m_pHeader->m_TripGuid = aTripGUID;
		m_pHeader->GetVersion(m_verMajor, m_verMinor);
		int64_t pos = GetFilePos();

		//если старая весия, дописать GUID в конец файла (может убить содержимое, если файл используется для записи данных )
		if (m_verMajor * 10 + m_verMinor < 20)
		{
			SetFilePos(GetFileSize());
			char sigGuid[]{"GUID"};

			ssize_t bytes = write(m_hFile, static_cast<const void*>(sigGuid), sizeof(sigGuid));
			int err{errno};
			assert(-1 != bytes);

			
			bytes = write(m_hFile, static_cast<const void*>(&aGUID), sizeof(GUID));
			err = errno;
			assert(-1 != bytes);
		}
		else
		{
			SetFilePos( 0 );
			ssize_t bytes = write(m_hFile, static_cast<const void*>(m_pHeader), sizeof(DataFileHeader));
			int err{errno};
			assert(-1 != bytes);
		}
		SetFilePos(pos);
	}
}
