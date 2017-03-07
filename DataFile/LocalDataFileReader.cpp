#include "LocalDataFileReader.h"
//#include "..\SS_Log\SS_Log.h"
#include <iostream>
#include <cassert>
#include <fcntl.h>
#include <sys/stat.h>

using namespace DataFile;
using std::wstring;
using namespace std::placeholders;

//#define ROUND_UP_SIZE(Value,Pow2) ((SIZE_T) ((((ULONG)(Value)) + (Pow2) - 1) & (~(((LONG)(Pow2)) - 1))))
//#define ROUND_UP_PTR(Ptr,Pow2)  ((void *) ((((ULONG_PTR)(Ptr)) + (Pow2) - 1) & (~(((LONG_PTR)(Pow2)) - 1))))

LocalDataFileReader::LocalDataFileReader(const std::string& aFileName, bool aAutoFix
										, DataBlocksProc aNewDataBlockProc)
	: BaseLocalDataFileReader(aFileName, std::bind(&LocalDataFileReader::ProcessNewDataBlock, std::ref(*this), _1))
	, m_pfnNewDataBlockProc(aNewDataBlockProc)
	, m_BrokenBlockListSize(0)
{
	DATAFILEPOS pos{0};
	SetFilePos(pos);
	m_pHeader = new DataFileHeader(false);

	unsigned long br{0};
	ssize_t res = read(m_hFile, m_pHeader, sizeof(DataFileHeader));
	if(0 >= res)
		std::cerr << "Failed to read file header" << std::endl;

//	ReadFile(m_hFile, m_pHeader, sizeof(DataFileHeader), &br, NULL);
	if(0 != strncmp(reinterpret_cast<const char*>(m_pHeader->m_Signature), STD_SIGNATURE, strlen(STD_SIGNATURE)))
	{
		std::cerr << "Invalid file signature" << std::endl;
		throw L"Invalid file signature";
	}

	sscanf_s(reinterpret_cast<const char*>(m_pHeader->m_Version), "%d.%d", &m_verMajor, &m_verMinor);
	if(m_verMajor * 10 + m_verMinor > CUR_VERSION_MAJOR * 10 + CUR_VERSION_MINOR)
	{
		std::cout << "Invalid file version. Max supported version 02.00" << std::endl;
		throw L"Invalid file version. Max supported version 02.00";
	}
	//обработать файл версии 01.00, в заголовке которого отсутствует поле FSpecificData и m_Guid
	if(1 == m_verMajor && 0 == m_verMinor)
	{
		m_pHeader->m_SpecificData = 0;
		m_pHeader->m_Guid = GUID_NULL;
		pos = sizeof(DataFileHeader) - sizeof (m_pHeader->m_SpecificData) - sizeof (m_pHeader->m_Guid);
		SetFilePos(pos);
	}
	else if(1 == m_verMajor && 1 == m_verMinor)
	{
		m_pHeader->m_SpecificData = 0;
		m_pHeader->m_Guid = GUID_NULL;
		pos = sizeof(DataFileHeader) - sizeof (m_pHeader->m_Guid);
		SetFilePos(pos);
	}
	else if(m_verMajor * 10 + m_verMinor < 22) //для версий ниже 2.01 нет TripGuid
	{
		m_pHeader->m_TripGuid = GUID_NULL;
		pos = sizeof(DataFileHeader) - sizeof (m_pHeader->m_TripGuid);
		SetFilePos(pos);
	}
	//для версий ниже 2.0 попытаться прочитать GUID в конце файла
	if(m_verMajor * 10 + m_verMinor < 20)
	{
		const size_t size{4};
		char sigGuid[] = {"GUID"};
		SetFilePos(GetFileSize() - strlen(sigGuid) - sizeof(GUID));

		ssize_t res = read(m_hFile, sigGuid, size);
		if(size == res && 0 == strncmp(sigGuid, "GUID", size))
		{
			res = read(m_hFile, &m_pHeader->m_Guid, sizeof(GUID));
			if(sizeof(GUID) != res)
				m_pHeader->m_Guid = GUID_NULL;
		}
		SetFilePos(pos);
	}

	if(GetFileSize() == GetFilePos()) //после заголовка файл заканчивается, значит он пуст
		return;

	m_DataType = DataTypeConverter::ToDataType(reinterpret_cast<const char*>(m_pHeader->m_strDataType));
	m_BrokenBlockListSize = -1;
	bool block_list_ok{true};

	if(INVALID_DATAFILEPOS != m_pHeader->m_BlockListPos && 0 != m_pHeader->m_BlockListPos/*oldschool*/)
	{
		SetFilePos(m_pHeader->m_BlockListPos);
		if(nullptr != m_pfnNewDataBlockProc)
		{
			//std::unique_ptr<DataFileBlockList> dbl = std::make_unique<DataFileBlockList>();
			DataFileBlockList *dbl = new DataFileBlockList();
			dbl->Read(m_hFile);
			size_t count = dbl->GetBlockCount();

			EnterCriticalSection(&m_lockDataBlockQueue);
			for(size_t i = 0; i < count; i++)
			{
				DataFileBlock* db = dbl->GetBlock(i);
				m_queDataBlocks.push(new DataFileBlock(*db));
			}
			LeaveCriticalSection(&m_lockDataBlockQueue);

			if(0 != count)
				SetEvent(m_hNewDataBlockEvent);
			delete dbl;
		}
		else
		{
			LOCK(m_pBlockList);
			
			m_pBlockList->Read(m_hFile);
			DATAFILEPOS oeBlockList = GetFilePos();
			DataFileBlock *block = m_pBlockList->GetBlock(m_pBlockList->GetBlockCount() - 1);
			if(not block)
				block_list_ok = false;
			else
			{
				//между последним блоком данных и таблице индекса есть пространство, поискать в нем еще блоки
				if(block->m_FilePosition + sizeof(DataFileBlock) + block->m_ZippedSize < m_pHeader->m_BlockListPos) 
				{
					DATAFILEPOS eobPos = block->m_FilePosition + sizeof(DataFileBlock) + block->m_ZippedSize;
					SetFilePos(eobPos);
					DATAFILEPOS tmp_pos = INVALID_DATAFILEPOS;
					ssize_t res1 = read(m_hFile, &tmp_pos, sizeof(DATAFILEPOS));
					if(tmp_pos == eobPos) //следующее прочитанное число совпадает с тек позицией => это очередной блок
					{
						DataFileBlock db;
						while(tmp_pos == eobPos)
						{
							SetFilePos(tmp_pos);
							ssize_t res2 = read(m_hFile, &db, sizeof(DataFileBlock));
							if(block->m_FromFrame + block->m_FrameCount == db.m_FromFrame && db.m_ZippedSize > 0 && db.m_FrameCount > 0)
							{
								m_pBlockList->Add(&db);
								block = m_pBlockList->GetBlock(m_pBlockList->GetBlockCount() - 1);
								eobPos = block->m_FilePosition + sizeof(DataFileBlock) + block->m_ZippedSize;
								//попытаемся прочитать после структуры DataFileBlock DATAFILEPOS и если он будет равен предполагаемой позиции начала следующего блока данных
								//значит дальше идет очередная структура DataFileBlock, иначе двигаемся дальше по данным и вытаскивает DataFileBlock's
								ssize_t res3 = read(m_hFile, &tmp_pos, sizeof(DATAFILEPOS));
 								if(0 >= res3 || tmp_pos != eobPos)
								{
 									SetFilePos(eobPos);
 									ssize_t res4 = read(m_hFile, &db, sizeof(DataFileBlock));
									if(0 < res4 && db.m_FilePosition == eobPos)
									{
										tmp_pos = eobPos;
										SetFilePos(eobPos);
									}
								}
							}
							else
							{
								break;
							}
						}
					}
					else
						SetFilePos(eobPos);
				}
				if(m_pBlockList->FromMF() > m_pBlockList->ToMF() || m_pBlockList->ToMF() > block->m_ToMF)
					block_list_ok = false;
				//позиция последнего блока + размер блока + размер структуры блока + размер прочитанного списка блока (с длиной) должна быть равна размеру файла
				//if(block->m_FilePosition + block->m_ZippedSize + sizeof(DataFileBlock) + sizeof(int) + sizeof(DataFileBlock) * m_pBlockList->GetBlockCount()  < GetFileSize())
				if(oeBlockList < GetFileSize())
				{
					block_list_ok = false;
					m_BrokenBlockListSize= m_pBlockList->GetBlockCount();
					std::cout << "Файл " << m_FileName << " поврежден, количество блоков " << m_BrokenBlockListSize
								<< ".  Проиндексированный размер " << block->m_FilePosition + block->m_ZippedSize
								<< " байт из " << GetFileSize() << " байт. Попытка восстановить..." << std::endl;
				} 
				//if(block->m_FilePosition + sizeof(DataFileBlock) + block->m_ZippedSize > m_pHeader->m_BlockListPos) 
				//	block_list_ok = false;
			}
		}
	}
	else
	{
		block_list_ok = false;
	}
	if(!block_list_ok)
		m_bNeedFix = true;
	if(m_pHeader->m_MarkerListPos != INVALID_DATAFILEPOS && m_pHeader->m_MarkerListPos != 0 /*oldschool*/)
	{
		SetFilePos(m_pHeader->m_MarkerListPos);
	}

	if(!block_list_ok && aAutoFix == true)
	{
		try
		{
			Fix();
		}
		catch(const wchar_t* aError)
		{
			std::cerr << "Cannot fix local file. Error " << aError << std::endl;
		}
	}
}

LocalDataFileReader::~LocalDataFileReader()
{
}

bool LocalDataFileReader::Fix()
{
	//assert(0);
	std::cout << "Run Fix" << std::endl;
	m_hFile = open(m_FileName.c_str(), O_RDWR | O_LARGEFILE, S_IRWXO);
//	if(m_hFile == INVALID_HANDLE_VALUE|| !m_pHeader || (strncmp(m_pHeader->m_Signature, STD_SIGNATURE, strlen(STD_SIGNATURE)) != 0))
//		throw L"Невозможно вызвать Fix. Файл не открыт или имеет неверный формат";
//
//	SAFE_CLOSE_HANDLE(m_hFile);
//
//	m_hFile = CreateFileW(m_FileName.c_str(), GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0);
//	if(m_hFile == INVALID_HANDLE_VALUE)
//	{
//		m_hFile = CreateFileW(m_FileName.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, 0);
//		if(m_hFile == INVALID_HANDLE_VALUE)
//		{
//			wchar_t sError[256] = {};
//			wsprintf(sError, L"Не удалось восстановить хендл файла. Ошибка %d", GetLastError());
//			throw sError;
//		}
//
//		wchar_t sError[256] = {};
//		wsprintf(sError, L"Не удалось открыть файл в монопольное пользование. Ошибка %d", GetLastError());
//		throw sError;
//	}
//
	LOCK(m_pBlockList);

	m_pBlockList->Clear();
	DATAFILEPOS pos = sizeof(DataFileHeader);
//	//поддержка старой версии
//	if(m_verMajor == 1 && m_verMinor == 0)
//	{
//		pos -= sizeof(m_pHeader->m_SpecificData);
//	}
//	else if(m_verMajor * 10 + m_verMinor < 22) //для версий ниже 2.01 нет TripGuid
//	{
//		m_pHeader->m_TripGuid = GUID_NULL;
//		pos = sizeof(DataFileHeader) - sizeof (m_pHeader->m_TripGuid);
//		SetFilePos(pos);
//	}
	SetFilePos(pos);

	int from_frame = 0;
	while(true)
	{
		DataFileBlock db;
		ssize_t res = read(m_hFile, &db, sizeof(DataFileBlock));

		if(db.m_FromFrame != from_frame)
			break;

		if(!res || res != sizeof(DataFileBlock))
			break;
		pos = db.m_ZippedSize;
		MoveFilePos(pos);
		if(nullptr != m_pfnNewDataBlockProc)
		{
			EnterCriticalSection(&m_lockDataBlockQueue);
			m_queDataBlocks.push(new DataFileBlock(db));
			SetEvent(m_hNewDataBlockEvent);
			LeaveCriticalSection(&m_lockDataBlockQueue);
		}

		if(db.m_FromMF > db.m_ToMF) //если что-то сглючило при записи, скорректировать мастерфрейм в блоке, пусть блок пропадет,но следующие спасутся
			db.m_FromMF = db.m_ToMF;

		m_pBlockList->Add(db);
		from_frame += db.m_FrameCount;
//		if(m_pBlockList->GetBlockCount() == static_cast<size_t>(m_BrokenBlockListSize))
//		{
//			Log(L"Успешно прочитано %d блоков. Поиск дополнительного списка блоков", m_BrokenBlockListSize);
//			DATAFILEPOS save_pos = GetFilePos();
//			int count;
//			ReadFile(m_hFile, &count, sizeof(int), &br, NULL);
//			if(count > 0 && count < 1000000 /* разумное количество блоков на файл*/)
//			{
//				DataFileBlock db;
//				ReadFile(m_hFile, &db, sizeof(DataFileBlock), &br, NULL);
//				if(*(m_pBlockList->GetBlock(0)) == db)
//				{
//					DataFileBlockList block_list;
//					SetFilePos(save_pos);
//					block_list.Read(m_hFile);
//					if(block_list.GetBlockCount() == static_cast<size_t>(count))
//					{
//						Log(L"Успешно прочитан список из %d блоков. Продолжаем восстановление файла", count);
//					}
//					else
//					{
//						Log(L"Ошибка чтения списка блоков. Восстановление файла прервано.");
//						break;
//					}
//				}
//				else
//				{
//					Log(L"Найденная последовательноть не явлвяется корректным списком блоков. Восстановление файла прервано.");
//					break;
//				}
//			}
//			else
//			{
//				Log(L"Не найден список блоков в данных. Восстановление файла прервано.");
//				break;
//			}
//		}
	}
//
	pos = 0;
	if(m_pBlockList->GetBlockCount() > 0)
	{
		//перейти в конец файла и запомнить позицию конца в соответствущем поле заголовка
		m_pHeader->m_BlockListPos = GetFilePos();
		//записать в конец файл таблицу блоков
		m_pBlockList->Write(m_hFile);
		SetFilePos(0);
		unsigned long bw;
		if(m_verMajor == 1 && m_verMinor == 0)
			write(m_hFile, m_pHeader, sizeof(DataFileHeader) - sizeof(m_pHeader->m_SpecificData)
			- sizeof(m_pHeader->m_Guid));
		else if(m_verMajor == 1 && m_verMinor == 1)
			write(m_hFile, m_pHeader, sizeof(DataFileHeader) - sizeof(m_pHeader->m_SpecificData));
		else if(m_verMajor * 10 + m_verMinor < 22) //для версий ниже 2.01 нет TripGuid
			write(m_hFile, m_pHeader, sizeof(DataFileHeader) - sizeof(m_pHeader->m_TripGuid));
		else
			write(m_hFile, m_pHeader, sizeof(DataFileHeader));

		m_bNeedFix = false;
	}

	close(m_hFile);
//
//	m_hFile = CreateFileW(m_FileName.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, 0);
//	if(m_hFile == INVALID_HANDLE_VALUE)
//	{
//		wchar_t sError[256] = {};
//		wsprintf(sError, L"Не удалось восстановить хендл файла. Ошибка %d", GetLastError());
//		throw sError;
//	}
//
	return !m_bNeedFix;
}

BaseDataFileReader* LocalDataFileReader::MakeNewInstance(void)
{
	return new LocalDataFileReader(m_FileName);
}

bool LocalDataFileReader::NeedFix() const
{
	return m_bNeedFix;
}

bool LocalDataFileReader::TestFileUnbuffered(const std::string& aFileName, int* pTripID, DataType* pDT, int64_t* pFileSize)
{
	assert(0);
	return false;
//	STORAGE_ACCESS_ALIGNMENT_DESCRIPTOR AlignmentDescriptor;
//	std::wstring drive = HelperFunctions::String::Format(L"\\\\.\\%s", HelperFunctions::File::GetDriveLetter(aFileName).c_str());
//	if (HelperFunctions::File::DetectSectorSize(const_cast<WCHAR*>(drive.c_str()), &AlignmentDescriptor) != NO_ERROR)
//		return false;
//	HANDLE hFile = CreateFileW(aFileName.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, 0,
//		OPEN_EXISTING, FILE_FLAG_NO_BUFFERING, 0);
//	if (INVALID_HANDLE_VALUE == hFile)
//		return false;
//
//	size_t Size = ROUND_UP_SIZE(sizeof(DataFileHeader), AlignmentDescriptor.BytesPerPhysicalSector);
//	size_t SizeNeeded = AlignmentDescriptor.BytesPerPhysicalSector + Size;
//	char* pBuffer = new char[SizeNeeded];
//	// Actual alignment happens here.
//	void *pBufferAligned = ROUND_UP_PTR(pBuffer, AlignmentDescriptor.BytesPerPhysicalSector);
//	LARGE_INTEGER pos;
//	GetFileSizeEx(hFile, &pos);
//	if (pFileSize)
//		*pFileSize = pos.QuadPart;
//	//если файл не содержит данных кроме заголовка, считаем его невалидным
//	bool bValidSize = (pos.QuadPart > sizeof(DataFile::DataFileHeader));
//	pos.QuadPart = 0;
//	SetFilePointerEx(hFile, pos, NULL, FILE_BEGIN);
//	unsigned long br;
//	ReadFile(hFile, pBufferAligned, Size, &br, NULL);
//	DataFileHeader* pHeader = reinterpret_cast<DataFileHeader*>(pBufferAligned);
//
//	if (strncmp(pHeader->m_Signature, STD_SIGNATURE, strlen(STD_SIGNATURE)) != 0)
//	{
//		SAFE_CLOSE_HANDLE(hFile);
//		SAFE_ARRAY_DELETE(pBuffer);
//		return false;
//	}
//	int major = 0, minor = 0;
//	char strVer[HDR_VERSION_SIZE + 1]; //Access violation can occur in sscanf_s(pHeader->m_Version, "%d.%d", &major, &minor);
//	memset(strVer, 0, HDR_VERSION_SIZE + 1);
//	strncpy(strVer, pHeader->m_Version, HDR_VERSION_SIZE);
//	sscanf_s(strVer, "%d.%d", &major, &minor);
//
//	//Неподдерживаемая версия
//	if (major * 10 + minor > CUR_VERSION_MAJOR * 10 + CUR_VERSION_MINOR)
//	{
//		SAFE_CLOSE_HANDLE(hFile);
//		SAFE_ARRAY_DELETE(pBuffer);
//		return false;
//	}
//	if (pDT)
//		*pDT = DataTypeConverter::ToDataType(pHeader->m_strDataType);
//	if (pTripID)
//		*pTripID = pHeader->m_TripID;
//	SAFE_CLOSE_HANDLE(hFile);
//	SAFE_ARRAY_DELETE(pBuffer);
//	return bValidSize;
}

bool LocalDataFileReader::TestFile(const std::string& aFileName, int* pTripID, DataType* pDT,  MASTERFRAME* pFromMF, MASTERFRAME* pToMF, int64_t* pFileSize, GUID* pGuid)
{
	assert(0);
	return true;
//
//	if(pFromMF)
//		*pFromMF = INVALID_MASTERFRAME;
//	if(pToMF)
//		*pToMF = INVALID_MASTERFRAME;
//	if(pDT)
//		*pDT = dtNONE;
//	if(pTripID)
//		*pTripID = -1;
//	if(pFileSize)
//		*pFileSize = -1;
//	if(pGuid)
//		*pGuid = GUID_NULL;
//	if (!pFromMF && !pToMF && !pGuid)
//		return TestFileUnbuffered(aFileName, pTripID, pDT, pFileSize);
//	HANDLE hFile = CreateFileW(aFileName.c_str(), GENERIC_READ /*| GENERIC_WRITE*/, FILE_SHARE_READ | FILE_SHARE_WRITE, 0,
//				OPEN_EXISTING, 0, 0);
//	if(INVALID_HANDLE_VALUE == hFile)
//	{
//		return false;
//	}
//	LARGE_INTEGER pos;
//	GetFileSizeEx(hFile, &pos);
//	if(pFileSize)
//		*pFileSize = pos.QuadPart;
//	//если файл не содержит данных кроме заголовка, считаем его невалидным
//	bool bValidSize = (pos.QuadPart > sizeof(DataFile::DataFileHeader));
//	pos.QuadPart = 0;
//	SetFilePointerEx(hFile, pos, NULL, FILE_BEGIN);
//	DataFileHeader fh(false);
//	unsigned long br;
//	ReadFile(hFile, &fh, sizeof(DataFileHeader), &br, NULL);
//	if(strncmp(fh.m_Signature, STD_SIGNATURE, strlen(STD_SIGNATURE)) != 0)
//	{
//		SAFE_CLOSE_HANDLE(hFile);
//		return false;
//	}
//	int major = 0, minor = 0;
//	char strVer[HDR_VERSION_SIZE + 1]; //Access violation can occur in sscanf_s(pHeader->m_Version, "%d.%d", &major, &minor);
//	memset(strVer, 0, HDR_VERSION_SIZE + 1);
//	strncpy(strVer, fh.m_Version, HDR_VERSION_SIZE);
//	sscanf_s(strVer, "%d.%d", &major, &minor);
//
//	//Неподдерживаемая версия
//	if(major * 10 + minor > CUR_VERSION_MAJOR * 10 + CUR_VERSION_MINOR )
//	{
//		SAFE_CLOSE_HANDLE(hFile);
//		return false;
//	}
//
//	//обработать файл версии 01.00, в заголовке которого отсутствует поле FSpecificData и m_Guid
//	if(major == 1 && minor == 0)
//	{
//		fh.m_SpecificData	= 0;
//		fh.m_Guid			= GUID_NULL;
//		pos.QuadPart		= sizeof(DataFileHeader) - sizeof (fh.m_SpecificData) - sizeof (fh.m_Guid);
//		SetFilePointerEx(hFile, pos, NULL, FILE_BEGIN);
//	}
//	//обработать файл версии 01.01, в заголовке которого отсутствует поле m_Guid
//	if(major == 1 && minor == 1)
//	{
//		fh.m_Guid		= GUID_NULL;
//		pos.QuadPart	= sizeof(DataFileHeader) - sizeof (fh.m_Guid);
//		SetFilePointerEx(hFile, pos, NULL, FILE_BEGIN);
//	}
//	//для версий ниже 2.0 попытаться прочитать GUID в конце файла
//	if(major * 10 + minor < 20)
//	{
//		char sigGuid [] = "GUID";
//		LARGE_INTEGER newpos;
//		newpos.QuadPart = HelperFunctions::File::GetFileSize(aFileName) - strlen(sigGuid) - sizeof(GUID);
//		SetFilePointerEx(hFile, newpos, NULL, FILE_BEGIN);
//		DWORD br;
//		ReadFile(hFile, sigGuid, 4, &br, NULL);
//		if(br == 4 && strncmp(sigGuid, "GUID", 4) == 0)
//		{
//			ReadFile(hFile, &fh.m_Guid, sizeof(GUID), &br, NULL);
//			if(br != sizeof(GUID))
//				fh.m_Guid = GUID_NULL;
//		}
//		SetFilePointerEx(hFile, pos, NULL, FILE_BEGIN);
//	}
//
//	if(pDT)
//		*pDT = DataTypeConverter::ToDataType(fh.m_strDataType);
//	if(pTripID)
//		*pTripID = fh.m_TripID;
//	if(pGuid)
//		*pGuid = fh.m_Guid;
//	bool block_list_ok = true;
//	if((fh.m_BlockListPos != INVALID_DATAFILEPOS && fh.m_BlockListPos != 0/*oldschool*/) && (pFromMF || pToMF)) //если ни pToMF, pFromMF не указаны, то не читаем блоки
//	{
//		pos.QuadPart = fh.m_BlockListPos;
//		SetFilePointerEx(hFile, pos, NULL, FILE_BEGIN);
//		DataFileBlockList bl;
//		bl.Read(hFile);
//		if(pFromMF)
//			*pFromMF = bl.FromMF();
//		if(pToMF)
//			*pToMF = bl.ToMF();
//	}
//	SAFE_CLOSE_HANDLE(hFile);
//	return bValidSize;
}

void LocalDataFileReader::ProcessNewDataBlock(DataFileBlock* aDataBlock)
{
	std::vector<DataFileBlock*> datablocks;

	datablocks.push_back(aDataBlock);

	EnterCriticalSection(&m_lockDataBlockQueue);

	while(!m_queDataBlocks.empty())
	{
		datablocks.push_back(m_queDataBlocks.front());
		m_queDataBlocks.pop();
	}

	LeaveCriticalSection(&m_lockDataBlockQueue);
		
	if(nullptr != m_pfnNewDataBlockProc)
		m_pfnNewDataBlockProc(datablocks);
}
