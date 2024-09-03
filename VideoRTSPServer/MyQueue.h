#pragma once
#include <list>
#include <atomic>

#include "MyThread.h"

template <class T>
class CMyQueue
{
public:
	// ���ھֲ���������enum��
	enum {
		EQNone,
		EQPush,//���
		EQPop, //����
		EQSize,//��С
		EQClear//���
	};

	typedef void(*PFUNC)(void*);
	typedef struct IocpParam {
		size_t nOperator;	//����
		T Data;				//����
		HANDLE hEvent;		//���� pop ��size ֪ͨ�̻߳�ȡ��������Data
		IocpParam(int op, const T& data, HANDLE hEve = NULL)
		{
			nOperator = op;
			Data = data;
			hEvent = hEve;
		}
		IocpParam() {
			nOperator = EQNone;
		}
	}PPARAM; // Post Param ����Ͷ����Ϣ�Ľṹ��

public:
	CMyQueue() {
		m_lock = false;
		m_hCompeletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 1);//����һ����ɶ˿�
		m_hThread = INVALID_HANDLE_VALUE;
		if (m_hCompeletionPort != NULL)//��ȫ�˿ڴ����ɹ��Ŵ����߳�
		{
			//�����߳�
			m_hThread = (HANDLE)_beginthread(&CMyQueue<T>::threadEntry, 0, this);
		}
	}

	virtual ~CMyQueue() {
		if (m_lock) return;
		m_lock = true;
		PostQueuedCompletionStatus(m_hCompeletionPort, 0, NULL, NULL);
		WaitForSingleObject(m_hThread, INFINITE);
		if (m_hCompeletionPort != NULL) {
			HANDLE hTemp = m_hCompeletionPort;
			m_hCompeletionPort = NULL;
			CloseHandle(hTemp);
		} 
	}

	bool PushBack(const T& data) {
		IocpParam* pParam = new IocpParam(EQPush, data);
		if (m_lock) {
			delete pParam;
			return false;
		}
		BOOL ret = PostQueuedCompletionStatus(m_hCompeletionPort, sizeof(PPARAM), (ULONG_PTR)pParam, NULL);
		if (ret == false) {
			delete pParam;
		}
		return ret;
	}

	virtual bool PopFront(T& data) {
		HANDLE hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
		IocpParam pParam(EQPop, data, hEvent);

		if (m_lock) {
			CloseHandle(hEvent); 
			return false;
		}

		BOOL ret = PostQueuedCompletionStatus(m_hCompeletionPort, sizeof(PPARAM), (ULONG_PTR)&pParam, NULL);
		if (ret == false) {
			CloseHandle(hEvent);
			return false;
		}

		ret = WaitForSingleObject(hEvent, INFINITE) == WAIT_OBJECT_0;	// �¼�����ΪWAIT_OBJECT_0
		if (ret) {
			data = pParam.Data;
		}
		return ret;
	}

	size_t Size() {
		HANDLE hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
		IocpParam pParam(EQSize, T(), hEvent);

		if (m_lock) {
			CloseHandle(hEvent);
			return 0;
		}
		BOOL ret = PostQueuedCompletionStatus(m_hCompeletionPort, sizeof(PPARAM), (ULONG_PTR)&pParam, NULL);
		if (ret == false) {
			CloseHandle(hEvent);
			return false;
		}

		ret = WaitForSingleObject(hEvent, INFINITE) == WAIT_OBJECT_0;	// �¼�����ΪWAIT_OBJECT_0
		if (ret) {
			return pParam.nOperator;
		}
		return -1;
	}

	bool Clear() {
		IocpParam* pParam = new IocpParam(EQClear, T());
		if (m_lock) {
			delete pParam;
			return false;
		}

		BOOL ret = PostQueuedCompletionStatus(m_hCompeletionPort, sizeof(PPARAM), (ULONG_PTR)pParam, NULL);
		if (ret == false) {
			delete pParam;
		}
		return ret;
	}

protected:
	static void threadEntry(void* arg) {
		CMyQueue<T>* thiz = (CMyQueue<T>*)arg;
		thiz->threadMain();
		_endthread();
	}

	void threadMain() {
		PPARAM* pParam = NULL;
		DWORD dwTransferred = 0;
		ULONG_PTR CompletionKey = 0;
		OVERLAPPED* pOverlapped = NULL;
		while (GetQueuedCompletionStatus(m_hCompeletionPort, &dwTransferred, &CompletionKey, &pOverlapped, INFINITE))
		{
			if ((dwTransferred == 0) && (CompletionKey == NULL)) {
				printf("thread is prepare to exit \r\n");
				break;
			}

			pParam = (PPARAM*)CompletionKey;
			DealParam(pParam);
		}


		while (GetQueuedCompletionStatus(m_hCompeletionPort, &dwTransferred, &CompletionKey, &pOverlapped, 0)) {
			if ((dwTransferred == 0) && (CompletionKey == NULL)) {
				printf("thread is prepare to exit \r\n");
				break;
			}
			pParam = (PPARAM*)CompletionKey;
			DealParam(pParam);
		}

		// �ر���ȫ�˿�ӳ��
		HANDLE hTemp = m_hCompeletionPort;
		m_hCompeletionPort = NULL;
		CloseHandle(hTemp);
	}

	virtual void DealParam(PPARAM* pParam) {
		switch (pParam->nOperator)
		{
		case EQPush:
			m_lstData.push_back(pParam->Data);
			delete pParam;
			break;

		case EQPop:
			if (m_lstData.size() > 0) {
				pParam->Data = m_lstData.front();
				m_lstData.pop_front();
			}
			if (pParam->hEvent != NULL) {
				SetEvent(pParam->hEvent);
			}
			break;

		case EQSize:
			pParam->nOperator = m_lstData.size();
			if (pParam->hEvent != NULL) {
				SetEvent(pParam->hEvent);
			}
			break;

		case EQClear:
			m_lstData.clear();
			delete pParam;
			break;

		default:
			TRACE("unlnown operator! \r\n");
			break;
		}
	}


protected:
	std::list<T> m_lstData;
	HANDLE m_hCompeletionPort;
	HANDLE m_hThread;

	std::atomic<bool> m_lock;	// �����Ƿ���������

};



template<class T>
class CMySendQueue : public CMyQueue<T>, public ThreadFuncBase
{

public:
	typedef int (ThreadFuncBase::* MYCALLBACK)(T& data);

	CMySendQueue(ThreadFuncBase* obj, MYCALLBACK callback)
		:CMyQueue<T>(), m_base(obj), m_callback(callback)
	{
		// ����һ���̣߳�ר�����ڷ������е����ݡ�
		m_thread.Start();
		m_thread.UpdateWorker(::ThreadWorker(this, (FUNCTYPE)&CMySendQueue<T>::threadTick));
	}  

	virtual ~CMySendQueue() {
		m_base = NULL;
		m_callback = NULL;
		m_thread.Stop();
	}

protected:
	int threadTick() {
		// ȷ������ Queue �� ������Ϣ�̴߳���
		if (WaitForSingleObject(CMyQueue<T>::m_hThread, 0) != WAIT_TIMEOUT) return -1;

		if (CMyQueue<T>::m_lstData.size() > 0) {
			PopFront();
		}
		Sleep(1);
		return 0;		// ����0���̻߳᲻ͣ�������������
	}

	virtual bool PopFront(T& data) { return false; };

	bool PopFront() {
		typename CMyQueue<T>::IocpParam* pParam = new typename CMyQueue<T>::IocpParam(CMyQueue<T>::EQPop, T());
		if (CMyQueue<T>::m_lock) {
			delete pParam;
			return false;
		}

		BOOL ret = PostQueuedCompletionStatus(CMyQueue<T>::m_hCompeletionPort, sizeof(*pParam), (ULONG_PTR)&pParam, NULL);
		if (ret == false) {
			delete pParam;
			return false;
		}

		return ret;
	}

	//virtual void DealParam(PPARAM* pParam) {
	virtual void DealParam(typename CMyQueue<T>::PPARAM* pParam) {
		switch (pParam->nOperator)
		{
		case CMyQueue<T>::EQPush:
			CMyQueue<T>::m_lstData.push_back(pParam->Data);
			delete pParam;
			break;

		case CMyQueue<T>::EQPop:
			if (CMyQueue<T>::m_lstData.size() > 0) {
				pParam->Data = CMyQueue<T>::m_lstData.front();
				// �ļ����ܴܺ�
				if((m_base->*m_callback)(pParam->Data) == 0)
					CMyQueue<T>::m_lstData.pop_front();
			}
			
			delete pParam;
			break;

		case CMyQueue<T>::EQSize:
			pParam->nOperator = CMyQueue<T>::m_lstData.size();
			if (pParam->hEvent != NULL) {
				SetEvent(pParam->hEvent);
			}
			break;

		case CMyQueue<T>::EQClear:
			CMyQueue<T>::m_lstData.clear();
			delete pParam;
			break;

		default:
			TRACE("unlnown operator! \r\n");
			break;
		}
	}

private:
	ThreadFuncBase* m_base;
	MYCALLBACK m_callback;		// �󶨣�Pop��ʱ����� MyClient::SendData ����
	CMyThread m_thread;
};


typedef CMySendQueue<std::vector<char>>::MYCALLBACK SENDCALLBACK;