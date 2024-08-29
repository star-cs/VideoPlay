#pragma once

#include "Socket.h"
#include "MyThread.h"
#include "MyQueue.h"
#include <string>
#include <map>

class RTSPRequest
{
public:
	RTSPRequest();
	RTSPRequest(const RTSPRequest& protocol);
	RTSPRequest& operator=(const RTSPRequest& protocol);
	~RTSPRequest();
private:
	int m_method;	//0 OPTIONS  1 DESCRIBE  2 SETUP  3 PLAY  4 TEARDOWN
};

class RTSPReply 
{
public:
	RTSPReply();
	RTSPReply(const RTSPReply& protocol);
	RTSPReply& operator=(const RTSPReply& protocol);
	~RTSPReply();

	MBuffer ToBuffer();
private:
	int m_method;	//0 OPTIONS  1 DESCRIBE  2 SETUP  3 PLAY  4 TEARDOWN
};

class RTSPSession 
{
public:
	RTSPSession();
	RTSPSession(const RTSPSession& protocol);
	RTSPSession& operator=(const RTSPSession& protocol);
	~RTSPSession();

};

class RTSPServer :public ThreadFuncBase
{
public:
	RTSPServer()
		:m_socket(MTYPE::MTCP), m_status(0), m_threadPool(10)
	{
		m_threadMain.UpdateWorker(ThreadWorker(this, (FUNCTYPE)&RTSPServer::ThreadRTSPWorker));
	}

	int Init(const std::string& strIP = "0.0.0.0", short port = 554);
	int Invoke();
	void Stop();

	~RTSPServer();

protected:
	// 返回 0 继续， 返回负数终止，返回非零数警告。
	int ThreadRTSPWorker();
	int ThreadSession();
	RTSPRequest AnalyzeRequest(const std::string& data);
	RTSPReply MakeReplay(const RTSPRequest& request);

private:
	MSocket m_socket;
	MAddress m_addr;

	int m_status;		// 0 未初始化   1 初始化完成   2 正在运行  3 关闭
	CMyThread m_threadMain;
	CMyThreadPool m_threadPool;		// Session需要单独的线程处理
	std::map<std::string, RTSPSession> m_mapSessions;
	
	CMyQueue<MSocket> m_clients;

	static SocketInit init;
};

