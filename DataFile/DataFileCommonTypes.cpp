#include "DataFileCommonTypes.h"
#include "common_def.h"
#include <unistd.h>

using namespace DataFile;

DataFileBlock::DataFileBlock()
	: m_FilePosition(INVALID_DATAFILEPOS),
	m_BlockSize(0),
	m_ZippedSize(0),
	m_FromFrame(0),
	m_FrameCount(0),
	m_FromMF(INVALID_MASTERFRAME),
	m_ToMF(INVALID_MASTERFRAME)
{
}

DataFileBlock::DataFileBlock(DATAFILEPOS aFilePosition, int32_t aBlockSize, int32_t aZippedSize
								, int32_t aFromFrame, int32_t aFrameCount, int32_t aBlockIndex
								, MASTERFRAME aFromMF, MASTERFRAME aToMF)
	: m_FilePosition(aFilePosition)
	, m_BlockSize(aBlockSize)
	, m_ZippedSize(aZippedSize)
	, m_FromFrame(aFromFrame)
	, m_FrameCount(aFrameCount)
	, m_FromMF(aFromMF)
	, m_ToMF(aToMF)
{}

int32_t DataFileBlock::FrameFromMF(MASTERFRAME aMF) const
{
	float addition = static_cast<float>(m_FrameCount) / static_cast<float>(m_ToMF - m_FromMF) * static_cast<float>(aMF - m_FromMF);
	int32_t result = m_FromFrame + static_cast<int32_t>(addition);

	return result;
}

bool DataFileBlock::operator== (const DataFileBlock& aBlock) const
{
	return (aBlock.m_FilePosition == m_FilePosition
		&& aBlock.m_BlockSize == m_BlockSize
		&& aBlock.m_FrameCount == m_FrameCount
		&& aBlock.m_FromFrame == m_FromFrame
		&& aBlock.m_FromMF == m_FromMF
		&& aBlock.m_ToMF == m_ToMF
		&& aBlock.m_ZippedSize == m_ZippedSize);
}

void DataFileBlock::Clear()
{
	m_FilePosition = INVALID_DATAFILEPOS;
	m_BlockSize = 0;
	m_ZippedSize = 0;
	m_FromFrame = 0;
	m_FrameCount = 0;
	m_FromMF = INVALID_MASTERFRAME;
	m_ToMF = INVALID_MASTERFRAME;
}

//===========================================================================
//	DataFileBlockList
//===========================================================================

DataFileBlockList::DataFileBlockList(void)
	: m_FrameCount(0)
	, m_FromMF(0)
	, m_ToMF(0)
{
}

void DataFileBlockList::Assign(DataFileBlockList *aDataBlockList)
{
	ClearInternalStorage();

	for(size_t i = 0; i < aDataBlockList->m_vecBlocks.size(); i++)
	{
		DataFileBlock *db = new DataFileBlock(*aDataBlockList->m_vecBlocks[i]);
		m_vecBlocks.push_back(db);
	}

	m_FrameCount = aDataBlockList->m_FrameCount;
	m_FromMF = aDataBlockList->m_FromMF;
	m_ToMF = aDataBlockList->m_ToMF;
}

DataFileBlockList::~DataFileBlockList(void)
{
	ClearInternalStorage();
}

DataFileBlock * DataFileBlockList::FindBlock(int aFrame) const
{
	if((aFrame < 0) || (aFrame >= m_FrameCount))
		return nullptr;

	int lm{0};
	int rm{static_cast<int>(m_vecBlocks.size())};

	try
	{
		while (lm < rm)
		{
			int mdl = (lm + rm) / 2;
			DataFileBlock * db = m_vecBlocks[mdl];

			if (db->m_FromFrame > aFrame)
			{
				if (rm == mdl)
				{
					throw L"Error in block list";
				}
				rm = mdl;
			}
			else
			{
				if (aFrame >= db->m_FromFrame + db->m_FrameCount)
				{
					if (lm == mdl)
					{
						throw L"Error in block list";
					}
					lm = mdl;
				}
				else
				{
					return db;
				}
			}
		}
	}
	catch (const wchar_t* aErr)
	{
		return nullptr;
		//assert(0);
	}

	return nullptr;
}

DataFileBlock * DataFileBlockList::FindByMF(MASTERFRAME aMF) const
{
	if((aMF < m_FromMF) || (aMF >= m_ToMF))
		return nullptr;

	int lm{0};
	int rm{static_cast<int>(m_vecBlocks.size())};
	int prev_mdl{-1};
	while(lm < rm)
	{
		int mdl = (lm + rm) / 2;
		DataFileBlock *db = m_vecBlocks[mdl];

		if(prev_mdl == mdl) 
			return db;
		prev_mdl = mdl;

		if(db->m_FromMF > aMF)
		{
			rm = mdl;
		}
		else
		{
			if (aMF >= db->m_ToMF)
			{
				lm = mdl;
			}
			else
			{
				return db;
			}
		}
	}

	return nullptr;
}

//int DataFileBlockList::FrameFromMF(MASTERFRAME aMF) const
//{
//	DataFileBlock *db = FindByMF(aMF);
//
//	if(nullptr != db)
//		return db->FrameFromMF(aMF);
//
//	return -1;
//}

DataFileBlock * DataFileBlockList::Add(DataFileBlock& aDataFileBlock)
{
	aDataFileBlock.m_FromFrame = m_FrameCount; //DR bug m_vecBlocks.size();
	DataFileBlock *db = new DataFileBlock(aDataFileBlock);

	if(m_vecBlocks.empty())
		m_FromMF = aDataFileBlock.m_FromMF;

	m_ToMF = aDataFileBlock.m_ToMF;
	m_FrameCount += aDataFileBlock.m_FrameCount;

	m_vecBlocks.push_back(db);

	return db;
}

void DataFileBlockList::Add(DataFileBlock *aDataBlock)
{
	if( m_vecBlocks.empty() )
	{
		m_FromMF = aDataBlock->m_FromMF;
		m_ToMF = aDataBlock->m_ToMF;
	}

	if(m_ToMF < aDataBlock->m_ToMF)
		m_ToMF = aDataBlock->m_ToMF;

	m_vecBlocks.push_back(aDataBlock);
	m_FrameCount += aDataBlock->m_FrameCount;
}

void DataFileBlockList::Read(int aFileId)
{
	Clear();

	uint32_t count{0};

	ssize_t bytes = read(aFileId, static_cast<void*>(&count), sizeof(uint32_t));
	int fileErr{errno};
	assert(-1 != bytes);

	for(uint32_t i = 0; i < count; i++)
	{
		DataFileBlock *db = new DataFileBlock();

		bytes = read(aFileId, static_cast<void*>(db), sizeof(DataFileBlock));
		fileErr = errno;
		assert(-1 != bytes);

		if(sizeof(DataFileBlock) == bytes && db->m_FrameCount > 0 )
		{
			m_vecBlocks.push_back(db);

			m_FrameCount = db->m_FromFrame + db->m_FrameCount;

			if(0 == m_FromMF)
				m_ToMF = m_FromMF = db->m_FromMF;

			if(m_ToMF < db->m_ToMF)
				m_ToMF = db->m_ToMF;
		}
		else
			delete db;
	}
}

void DataFileBlockList::Write(int aFileId)
{
	size_t count{m_vecBlocks.size()};

	ssize_t bytes = write(aFileId, static_cast<const void*>(&count), sizeof(uint32_t));
	int fileErr{errno};
	assert(-1 != bytes);

	for (size_t i = 0; i < count; i++)
	{
		const DataFileBlock *db = m_vecBlocks[i];
		bytes = write(aFileId, static_cast<const void*>(db), sizeof(DataFileBlock));

		int fileErr{errno};
		assert(-1 != bytes);
	}
}

DataFileBlock* DataFileBlockList::GetBlock(size_t aIndex) const
{
	if(aIndex >= m_vecBlocks.size())
		return nullptr;

	DataFileBlock *db{m_vecBlocks[aIndex]};
	return db;
}

void DataFileBlockList::Clear()
{
	ClearInternalStorage();

	m_FrameCount = 0;
	m_FromMF = 0;
	m_ToMF = 0;
}

void DataFileBlockList::ClearInternalStorage()
{
	for(auto block : m_vecBlocks)
		delete block;

	m_vecBlocks.clear();
}
