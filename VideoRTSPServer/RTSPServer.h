#pragma once

#include "Socket.h"
#include "MyThread.h"
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
		:m_socket(MTYPE::MTCP), m_status(0)
	{}

	int Init(const std::string& strIP = "0.0.0.0", short port = 554);
	int Invoke();
	void Stop();

	~RTSPServer() {}

protected:
	int ThreadWorker();
	int ThreadSession();
	RTSPRequest AnalyzeRequest(const std::string& data);
	RTSPReply MakeReplay(const RTSPRequest& request);

private:
	MSocket m_socket;
	int m_status;		// 0 δ��ʼ��   1 ��ʼ�����   2 ��������  3 �ر�
	CMyThread m_threadMain;
	CMyThreadPool m_threadPool;		// Session��Ҫ�������̴߳���
	std::map<std::string, RTSPSession> m_mapSessions;

	static SocketInit init;
};

