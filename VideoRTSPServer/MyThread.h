#pragma once

#include <vector>
#include <mutex>
#include <atomic>
#include <Windows.h>
#include "CTool.h"

#ifndef TRACE
#define TRACE CTool::MTrace
#endif

class ThreadFuncBase {};

typedef int(ThreadFuncBase::* FUNCTYPE)();
typedef int(ThreadFuncBase::* FUNCTYPE_ARG)(void*);


class ThreadWorker {
public:
	ThreadWorker() :thiz(NULL), func(NULL),func_arg(NULL), m_arg(NULL), hasArg(false){}
	//ThreadWorker(ThreadFuncBase* obj, FUNCTYPE f) :thiz(obj), func(f) {}
	ThreadWorker(void* obj, FUNCTYPE f) :thiz((ThreadFuncBase*)obj), func(f) {
		hasArg = false;
	}

	ThreadWorker(void* obj, FUNCTYPE_ARG f, void* arg) :thiz((ThreadFuncBase*)obj), func_arg(f), m_arg(arg){
		hasArg = true;
	}

	ThreadWorker(const ThreadWorker& worker) {
		thiz = worker.thiz;
		func = worker.func;
		func_arg = worker.func_arg;
		m_arg = worker.m_arg;
		hasArg = worker.hasArg;
	}

	ThreadWorker& operator=(const ThreadWorker& worker) {
		if (this != &worker) {
			thiz = worker.thiz;
			func = worker.func;
			func_arg = worker.func_arg;
			m_arg = worker.m_arg;
			hasArg = worker.hasArg;
		}
		return *this;
	}

	bool IsVaild() const{
		if (hasArg)
		{
			return (func_arg != NULL) && (m_arg != NULL) && (thiz != NULL);
		}
		else {
			return (thiz != NULL) && (func != NULL);
		}
	}

	int operator()() {
		if (IsVaild()) {
			if (hasArg)
			{
				return (thiz->*func_arg)(m_arg);
			}
			else {
				return (thiz->*func)();
			}
		}
		return -1;
	}

public:
	bool hasArg;
	ThreadFuncBase* thiz;
	FUNCTYPE func;
	FUNCTYPE_ARG func_arg;
	void* m_arg;
};


class CMyThread
{
public:
	CMyThread() : m_hThread(NULL), m_bStatus(false) {}

	~CMyThread() {
		Stop();
	}

	bool IsVaild() {
		if (m_hThread == NULL || m_hThread == INVALID_HANDLE_VALUE) return false;
		return WaitForSingleObject(m_hThread, 0) == WAIT_TIMEOUT;
	}

	bool Start() {
		m_bStatus = true;
		m_hThread = (HANDLE)_beginthread(&CMyThread::threadEntry, 0, this);
		if (!IsVaild()) {
			m_bStatus = false;
		}
		return m_bStatus;
	}

	bool Stop() {
		if (m_bStatus == false) return true;
		m_bStatus = false;

		bool ret =  WaitForSingleObject(m_hThread, INFINITE) == WAIT_OBJECT_0;

		UpdateWorker();

		return ret;
	}

	void UpdateWorker(const ::ThreadWorker& worker = ::ThreadWorker()) {
		// 首先保证 m_worker 置空，之前的指向被删除。
		if (m_worker.load() != NULL && (m_worker.load() != &worker)) {
			::ThreadWorker* pWorker = m_worker.load();
			TRACE("delete pWorker = %08X m_worker = %08X \r\n", pWorker, m_worker.load());
			m_worker.store(NULL);
			delete pWorker;
		}

		if (m_worker.load() == &worker) return;

		if(!worker.IsVaild()){
			m_worker.store(NULL);
			return;
		}

		::ThreadWorker* pWorker = new ::ThreadWorker(worker);
		TRACE("new pWorker = %08X m_worker = %08X \r\n", pWorker, m_worker.load());
		m_worker.store(pWorker);
	}

	// true 忙碌     false 空闲
	bool IsBusy() {
		if (m_worker.load() == NULL) return false;
		return m_worker.load()->IsVaild();  
	}

private:
	static void threadEntry(void* arg) {
		CMyThread* thiz = (CMyThread*)arg;
		if (thiz) {
			thiz->ThreadWorker();
		}
		_endthread();
	}

	// 小于0，结束工作
	// 大于等于0，重复执行
	void ThreadWorker() {
		while (m_bStatus) {

			// 没有添加工作函数，休眠
			if (m_worker.load() == NULL) {
				Sleep(1);
				continue;
			}
			::ThreadWorker worker = *m_worker.load();
			if (worker.IsVaild()) {
				// worker() 返回为0，会一直处理这个任务函数。
				int ret = worker();
				if (ret != 0) {
					TRACE("thread found warning code %d \r\n", ret);
				}
				if (ret < 0) {
					UpdateWorker();
				}
			}
			else {
				Sleep(1);
			}
		}
		
	}

private:
	HANDLE m_hThread;
	bool m_bStatus;		// false表示线程关闭，true表示线程运行

	std::atomic<::ThreadWorker*> m_worker;
};

class CMyThreadPool {
public:
	CMyThreadPool(){
		
	}

	CMyThreadPool(size_t size) {
		m_threads.resize(size);
		for (size_t i = 0; i < size; i++) {
			m_threads[i] = new CMyThread();
		}
	}

	~CMyThreadPool(){
		Stop();
		for (size_t i = 0; i < m_threads.size(); i++) {
			delete m_threads[i];
			m_threads[i] = NULL;
		}
		m_threads.clear();
	}

	bool Invoke() {
		bool ret = true;
		for (size_t i = 0; i < m_threads.size(); i++) {
			if (m_threads[i]->Start() == false) {
				ret = false;
			}
		}
		if (ret == false) {
			for (size_t i = 0; i < m_threads.size(); i++) {
				m_threads[i]->Stop();
			}
		}
		return ret;
	}
	 
	bool Stop() {
		for (size_t i = 0; i < m_threads.size(); i++) {
			m_threads[i]->Stop();
		}
		return true;
	}

	// 返回-1 表示分配失败，所有的线程都在忙；大于等于0，表示第n个线程分配来处理。
	int DispatchWorker(const ThreadWorker& worker) {
		int index = -1;
		m_lock.lock();
		// TODO: 可以改成空闲队列和忙碌队列
		for (size_t i = 0; i < m_threads.size(); i++) {
			if (!m_threads[i]->IsBusy()) {
				m_threads[i]->UpdateWorker(worker);
				index = i;
				break;
			}
		}
		m_lock.unlock();
		return index;
	}

	bool CheckThreadValid(size_t index) {
		if (index < m_threads.size()) {
			return m_threads[index]->IsVaild();
		}
		return false;
	}

private:
	std::mutex m_lock;
	std::vector<CMyThread*> m_threads;
};