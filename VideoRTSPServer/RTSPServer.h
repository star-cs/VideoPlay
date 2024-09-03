#pragma once

#include "Socket.h"
#include "MyThread.h"
#include "MyQueue.h"
#include "RTPHelper.h"
#include <string>
#include <map>
#include "MediaFile.h"


class RTSPRequest
{
public:
	RTSPRequest() { m_method = -1; }
	RTSPRequest(const RTSPRequest& protocol);
	RTSPRequest& operator=(const RTSPRequest& protocol);
	~RTSPRequest() { m_method = -1; }

	void SetMethod(const MBuffer& method);
	void SetUrl(MBuffer url);
	void SetSession(MBuffer session);
	void SetSequence(MBuffer seq);
	void SetClientPorts(int ports[]);

	int method() const { return m_method; }
	const MBuffer& url() const { return m_url; }
	const MBuffer& session() const { return m_session; }
	const MBuffer& sequence() const { return m_seq; }
	const MBuffer& port(int index = 0) const { return index ? m_client_port[1] : m_client_port[0]; }
private:
	int m_method;	//0 OPTIONS  1 DESCRIBE  2 SETUP  3 PLAY  4 TEARDOWN
	MBuffer m_url;
	MBuffer m_session;
	MBuffer m_seq;
	MBuffer m_client_port[2];
};

class RTSPReply 
{
public:
	RTSPReply() { m_method = -1; }
	RTSPReply(const RTSPReply& protocol);
	RTSPReply& operator=(const RTSPReply& protocol);
	~RTSPReply() { m_method = -1; }

	MBuffer ToBuffer();
	
	void SetMethod(int method);
	void SetSession(const MBuffer& session);
	void SetOptions(const MBuffer& options);
	void SetSequence(const MBuffer& seq);
	void SetSdp(const MBuffer& sdp);
	void SetClientPort(const MBuffer& port1, const MBuffer& port2);
	void SetServerPort(const MBuffer& port1, const MBuffer& port2);
private:
	int m_method;	//0 OPTIONS  1 DESCRIBE  2 SETUP  3 PLAY  4 TEARDOWN
	MBuffer m_session;
	int m_client_port[2];
	int m_server_port[2];
	MBuffer m_sdp;	//上下文长度，类型 在此
	MBuffer m_seq;
	MBuffer m_options;	
};

class RTSPSession;
class RTSPServer;
typedef void(*RTSPPLAYCB)(RTSPServer* thiz, RTSPSession& session);

class RTSPSession 
{
public:
	RTSPSession();
	RTSPSession(const MSocket& client);
	RTSPSession(const RTSPSession& session);
	RTSPSession& operator=(const RTSPSession& session);
	
	MBuffer PinkOneLine(MBuffer& buffer);

	int PickRequestAndReply(RTSPPLAYCB cb, RTSPServer* thiz);

	MAddress GetClientUDPAddress() const;

	~RTSPSession() {}


private:
	MBuffer Pick();
	RTSPRequest AnalyseRequest(const MBuffer& buffer);
	RTSPReply Reply(const RTSPRequest& request);

private:
	MBuffer m_id;
	MSocket m_client;
	short m_port;		//客户端UDP端口设置。
};


class RTSPServer :public ThreadFuncBase
{
public:
	RTSPServer()
		:m_socket(MTYPE::MTCP), m_status(0), m_threadPool(10)
	{
		m_threadMain.UpdateWorker(ThreadWorker(this, (FUNCTYPE)&RTSPServer::ThreadRTSPWorker));
		m_h264.Open("./test.h264");
	}

	int Init(const std::string& strIP = "0.0.0.0", short port = 554);
	int Invoke();
	void Stop();

	~RTSPServer();

protected:
	// 返回 0 继续， 返回负数终止，返回非零数警告。
	int ThreadRTSPWorker();
	int ThreadSession();
	
	static void PlayCallBack(RTSPServer* thiz, RTSPSession& session);

	int UdpWorker(const MAddress& client);

private:
	static SocketInit m_init;

	MSocket m_socket;
	MAddress m_addr;	// 服务器地址

	int m_status;		// 0 未初始化   1 初始化完成   2 正在运行  3 关闭

	CMyThread m_threadMain;
	CMyThreadPool m_threadPool;		// Session需要单独的线程处理

	// std::map<std::string, RTSPSession> m_mapSessions;
	CMyQueue<RTSPSession> m_lstSession;

	RTPHelper m_hleper;
	MediaFile m_h264;
};

