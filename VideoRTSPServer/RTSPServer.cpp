#include "RTSPServer.h"

int RTSPServer::Init(const std::string& strIP, short port)
{
    m_addr.Update(strIP, port);
    m_socket.Bind(m_addr);
    m_socket.Listen();
    return 0;
}

int RTSPServer::Invoke()
{
    m_threadMain.Start();
    return 0;
}

void RTSPServer::Stop()
{
    m_socket.Close();
    m_threadMain.Stop();
    m_threadPool.Stop();
}

RTSPServer::~RTSPServer()
{
    Stop();
}


int RTSPServer::ThreadRTSPWorker()
{
    MAddress addr;
    MSocket client = m_socket.Accept(addr);
    if (client != INVALID_SOCKET) {
        m_clients.PushBack(client);
        m_threadPool.DispatchWorker(ThreadWorker(this, (FUNCTYPE)RTSPServer::ThreadSession));
    }
    return 0;
}

int RTSPServer::ThreadSession()
{
    MSocket sock;
    m_clients.PopFront(sock);
    
    MBuffer buffer(1024 * 16);
    int len = sock.Recv(buffer);
    if (len <= 0) {
        // TODO£ºÇåÀíclient
        return -1;
    }
    buffer.resize(len);

    RTSPRequest request = AnalyzeRequest(buffer);
    RTSPReply reply = MakeReplay(request);

    sock.Send(reply.ToBuffer());
    return 0;
}

RTSPRequest RTSPServer::AnalyzeRequest(const std::string& data)
{
    return RTSPRequest();
}

RTSPReply RTSPServer::MakeReplay(const RTSPRequest& request)
{
    return RTSPReply();
}
