#pragma once
#ifndef QueueExtractorH
#define QueueExtractorH

#include "HelperFunctions.h"
#include "Events.h"
#include <queue>
#include <functional>

namespace DataFile
{
//указатель на функцию, вызываемую при добавлении нового блока в индекс блоков
template <typename T>
using NewDataBlockProc = std::function<void(T* aDataBlock)>;

template <typename T>
class QueueExtractor
{
private:
	//указатель на функцию которая будет вызываться при добавлении нового блока данных
	NewDataBlockProc<T> m_pfnNewDataBlock;

	//событие генерируемое когда все блоки из FQueue  перенесены в индекс блоков m_pBlockList
	Events::HANDLE m_hDataBlockQueueEmptyEvent;

protected:
	std::queue<T*> m_queDataBlocks;
	
	HelperFunctions::CriticalSection m_lockDataBlockQueue;
	
	Events::HANDLE m_hNewDataBlockEvent;
	HelperFunctions::XThread<QueueExtractor>* m_pExtractThread;

protected:
	void ThreadExecute(void* aParam);

public:
	QueueExtractor(NewDataBlockProc<T> aNewDataBlockProc = nullptr);
	virtual ~QueueExtractor();

	void StartQueueThread();
};

template <typename T>
QueueExtractor<T>::QueueExtractor(NewDataBlockProc<T> aNewDataBlockProc)
	: m_pfnNewDataBlock(aNewDataBlockProc)
	, m_pExtractThread(nullptr)
{
	m_hNewDataBlockEvent = Events::CreateEvent(true, false);
	m_hDataBlockQueueEmptyEvent = Events::CreateEvent(true, false);

	//InitializeCriticalSection(&m_lockDataBlockQueue);
	m_pExtractThread = new HelperFunctions::XThread< QueueExtractor >(this
																		, &QueueExtractor::ThreadExecute
																		, nullptr
																		, std::string("QueueExtractor::Thread")
																		, m_hNewDataBlockEvent);
}

template<typename T>
QueueExtractor<T>::~QueueExtractor(void)
{
	SAFE_DELETE(m_pExtractThread);
	Events::DestroyEvent(m_hNewDataBlockEvent);
	Events::DestroyEvent(m_hDataBlockQueueEmptyEvent);

	while(!m_queDataBlocks.empty())
	{
		delete m_queDataBlocks.front();
		m_queDataBlocks.pop();
	}
}

template<typename T>
void QueueExtractor<T>::ThreadExecute(void* aParam)
{
	while(m_pExtractThread->IsRunning())
	{
		WaitForSingleObject(m_hNewDataBlockEvent, INFINITE);
		if (!m_pExtractThread->IsRunning())
			return;

		ResetEvent(m_hDataBlockQueueEmptyEvent);
		T* dataBlock{nullptr};

		//EnterCriticalSection(&m_lockDataBlockQueue);
		m_lockDataBlockQueue.Lock();
		if(!m_queDataBlocks.empty())
		{
			dataBlock = m_queDataBlocks.front();
			m_queDataBlocks.pop();
		}

		if(m_queDataBlocks.empty())
		{
			ResetEvent(m_hNewDataBlockEvent);
			SetEvent(m_hDataBlockQueueEmptyEvent);
		}
		//LeaveCriticalSection(&m_lockDataBlockQueue);
		m_lockDataBlockQueue.Unlock();

		if(nullptr != dataBlock)
		{
			// callback должен освободить память по указателю dataBlock!!!
			if(nullptr != m_pfnNewDataBlock)
				m_pfnNewDataBlock(dataBlock);
		}
	}
}

template<typename T>
void QueueExtractor<T>::StartQueueThread()
{
	//залочим вызывающий поток, пока не опорожним очередь
	m_pExtractThread->Resume();
	bool bNeedWaitQueue{false};

	//EnterCriticalSection(&m_lockDataBlockQueue);
	m_lockDataBlockQueue.Lock();

	if(!m_queDataBlocks.empty())
		bNeedWaitQueue = true;

	//LeaveCriticalSection(&m_lockDataBlockQueue);
	m_lockDataBlockQueue.Unlock();

	if(bNeedWaitQueue)
	{
		SetEvent(m_hNewDataBlockEvent);
		WaitForSingleObject(m_hDataBlockQueueEmptyEvent, INFINITE);
	}
}

} //end namespace
#endif
