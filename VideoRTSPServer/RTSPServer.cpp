#include "RTSPServer.h"
#include <rpc.h>

#pragma comment(lib, "rpcrt4.lib")

SocketInit RTSPServer::m_init;

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
    m_threadPool.Invoke();
    return 0;
}

// 存在 socket先关闭，但处理Accept的线程还没关闭。 把Stop() 关闭顺序 改一下就行。
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
        RTSPSession session(client);    //这里的client是TCP连接到的，IP是客户端的IP。Port不满足UDP端口设置。
        m_lstSession.PushBack(session);
        m_threadPool.DispatchWorker(ThreadWorker(this, (FUNCTYPE)&RTSPServer::ThreadSession));
    }
    return 0;
}

int RTSPServer::ThreadSession()
{
    RTSPSession session;        // 局部变量，函数结束后，自动析构。连接断开，client关闭。解决方法：PickRequestAndReply while循环处理
    if (m_lstSession.PopFront(session)) {
        session.h_closeUdp = CreateEvent(NULL, TRUE, FALSE, NULL);

        int ret = session.PickRequestAndReply(&RTSPServer::PlayCallBack, this); // 传入回调函数，保证他能在PLAY调用到RTSPServer::PlayCallBack
        // 一个客户端，可能会发多次命令，所以返回0，再次调用方法，直到Recv范围负数。
        CloseHandle(session.h_closeUdp);
        return ret;
    }
    return -1;
}


void RTSPServer::PlayCallBack(RTSPServer* thiz, RTSPSession& session)
{
    MAddress client = session.GetClientUDPAddress();
    session.m_udpAddr = client;
    int ret = thiz->m_threadPool.DispatchWorker(ThreadWorker(thiz, (FUNCTYPE_ARG)&RTSPServer::UdpWorker,(void*)&session));

    /*thiz->UdpWorker(session.GetClientUDPAddress());*/
}

// 发送文件。
// h_closeUdp 放到 RTSPSession里管理。
// rtp发送数据的时候，h_closeUdp和session里的MSocket m_client生命周期类似，切有一定的对应关系（这里假定，一个客户端只会请求一个流，用一个事件管理）。
int RTSPServer::UdpWorker(const RTSPSession& session)
{
    MAddress client = session.m_udpAddr;
    MBuffer frame = m_h264.ReadOneFrame();
    RTPFrame rtp;
    while (frame.size() > 0) {
         //  TODO: 设定一个结束发送帧的事件。
        printf("UdpWorker %p %d\r\n", &session, session.h_closeUdp);
        if (session.h_closeUdp == INVALID_HANDLE_VALUE || (WaitForSingleObject(session.h_closeUdp, 1) == WAIT_OBJECT_0)) {
            break;
        }
        m_hleper.SendMediaFrame(rtp, frame, client);
        frame = m_h264.ReadOneFrame();
    }
    m_h264.Reset();
    return -1;
}

RTSPSession::RTSPSession()
{
    m_port = -1;

    UUID uuid;
    UuidCreate(&uuid);

    m_id.resize(8);
    snprintf((char*)m_id.c_str(), m_id.size(), "%u%u", uuid.Data1, uuid.Data2);
}

RTSPSession::RTSPSession(const MSocket& client)
    :m_client(client)
{
    // 创建一个sessionId
    UUID uuid;
    UuidCreate(&uuid);

    m_id.resize(8);
    snprintf((char*)m_id.c_str(), m_id.size(), "%u%u", uuid.Data1, uuid.Data2);

    m_port = -1;
}

RTSPSession::RTSPSession(const RTSPSession& session)
{
    m_id = session.m_id;
    m_client = session.m_client;
    m_port = session.m_port;
}

RTSPSession& RTSPSession::operator=(const RTSPSession& session)
{
    if (this != &session) {
        m_id = session.m_id;
        m_client = session.m_client;
        m_port = session.m_port;
    }
    return *this;
}

// buffer传入传出参数
MBuffer RTSPSession::PinkOneLine(MBuffer& buffer)
{
    if (buffer.size() <= 0) return MBuffer();
    MBuffer result, temp;
    int i = 0;
    for (; i < (int)buffer.size(); i++) {
        result += buffer.at(i);
        if (buffer.at(i) == '\n') break;
    }
    temp = i + (char*)buffer + 1;
    buffer = temp;
    return result;
}

int RTSPSession::PickRequestAndReply(RTSPPLAYCB cb, RTSPServer* thiz)
{
    int ret = -1;

    do {
        // Pick
        MBuffer buffer = Pick();
        if (buffer.size() <= 0) return -1;

        // Analyse
        RTSPRequest request = AnalyseRequest(buffer);

        if (request.method() < 0) {
            TRACE("method error [%d] \r\n", request.method());
            return -2;
        }
        // Reply
        RTSPReply reply = Reply(request);
        ret = m_client.Send(reply.ToBuffer());

        // 客户端的端口
        if (request.method() == 2) {                // SETUP
            m_port = (short)atoi(request.port());
        }

        if (request.method() == 3) {                // PLAY
            // 回调
            cb(thiz, *this);
        }

        if (request.method() == 4) {                // TEARDOWN
            SetEvent(h_closeUdp);
            printf("PickRequestAndReply TEARDOWN %p %d\r\n", this, h_closeUdp);
        }
    } while (ret >= 0);
   
    if (ret < 0) return ret;

    return 0;
}

MAddress RTSPSession::GetClientUDPAddress() const
{
    MAddress addr;
    int len = addr.size();
    getsockname(m_client, addr, &len);  //获取客户端地址
    addr.Fresh();
    addr.setPort(m_port);               //设置客户端的udp端口。
    return addr;
}




MBuffer RTSPSession::Pick()
{
    // buffer(1) 防止buffer设置过大，一次性接收到两个包。这里并没有使用index序号对 第二个包的情况处理。
    MBuffer result, buffer(1);  
    int ret = 1;
    while (ret > 0) {
        buffer.Zero();
        ret = m_client.Recv(buffer);
        result += buffer;
        if (ret > 0 && result.size() >= 4) {
            UINT val = *(UINT*)(result.size() - 4 + (char*)result);
            if (val == *(UINT*)"\r\n\r\n") {        // RTSP完整包 \r\n\r\n 结尾
                break;
            } 
        } 
    }
    return result;
}

RTSPRequest RTSPSession::AnalyseRequest(const MBuffer& buffer)
{ 
    // TODO：字符串解析
    RTSPRequest request;

    printf("%s\r\n", (char*)buffer);
    if (buffer.size() <= 0) return request;
    
    MBuffer data = buffer;
    MBuffer line = PinkOneLine(data);
    MBuffer method(32), url(1024), version(16), seq(64);
    if (sscanf(line, "%s %s %s\r\n", (char*)method, (char*)url, (char*)version) < 3) {      // method url version
        TRACE("Error at:[%s] \r\n", (char*)line);
        return request;
    }

    line = PinkOneLine(data);
    if (sscanf(line, "CSeq: %s\r\n", (char*)seq) < 1) {
        TRACE("Error at:[%s] \r\n", (char*)line);
        return request;
    }

    request.SetMethod(method);
    request.SetUrl(url);
    request.SetSequence(seq);
    
    if ((strcmp(method, "OPTIONS") == 0) || (strcmp(method, "DESCRIBE") == 0)) {
        return request;
    }
    else if (strcmp(method, "SETUP") == 0) {        // client_port
        do {
            line = PinkOneLine(data);
            if (strstr((const char*)line, "client_port=") == NULL) continue;
            break;
        } while (line.size() > 0);
        int ports[2] = { 0,0 };
        if (sscanf(line, "Transport: RTP/AVP;unicast;client_port=%d-%d\r\n", ports, ports + 1) == 2) {
            request.SetClientPorts(ports);
            return request;
        }
    }
    else if ((strcmp(method, "PLAY") == 0) || (strcmp(method, "TEARDOWN") == 0)){         // session           range不管，默认从头到尾
        line = PinkOneLine(data);
        MBuffer session(64);
        if (sscanf(line, "Session: %s\r\n", (char*)session) == 1) {
            request.SetSession(session);
            return request;
        }
    }

    return request;
}
 

RTSPReply RTSPSession::Reply(const RTSPRequest& request)
{
    RTSPReply reply;
    reply.SetSequence(request.sequence());
    if (request.session().size() > 0) {
        reply.SetSession(request.session());
    }
    else {
        reply.SetSession(m_id);
    }
       
    reply.SetMethod(request.method());
    switch (request.method())
    {
    case 0:     // OPTIONS
        reply.SetOptions("Public: OPTIONS, DESCRIBE, SETUP, PLAY, TEARDOWN\r\n");
        break;

    case 1:     // DESCRIBE
    {
        // TODO: sdp
        MBuffer sdp;
       
        // MBuffer重载<<运算符 
        sdp << "v=0\r\n";
        sdp << "o=- " << (char*)m_id << " 1 IN IP4 127.0.0.1\r\n";
        sdp << "t=0 0\r\n" << "a=control:*\r\n" << "m=video 0 RTP/AVP 96\r\n";
        sdp << "a=framerate:24\r\n";
        sdp << "a=rtpmap:96 H264/90000\r\n" << "a=control:track0\r\n";

        reply.SetSdp(sdp);
    } 
        break;

    case 2:     // SETUP   从SETUP S->C 之后，通信之间都需要session
        // Transport: RTP/AVP;unicast;client_port=54492-54493;server_port=55000-55001\r\n
        reply.SetClientPort(request.port(0), request.port(1));
        reply.SetServerPort("55000", "55001");
        reply.SetSession(m_id);
        break;

    case 3:     // PLAY
        break;

    case 4:     // TEARDOWN
        break;

    }

    return reply;
}

RTSPRequest::RTSPRequest(const RTSPRequest& protocol)
{
    m_method = protocol.m_method;
    m_url = protocol.m_url;
    m_session = protocol.m_session;
    m_seq = protocol.m_seq;
    m_client_port[0] = protocol.m_client_port[0];
    m_client_port[1] = protocol.m_client_port[1];
}

RTSPRequest& RTSPRequest::operator=(const RTSPRequest& protocol)
{
    if (this != &protocol)
    {
        m_method = protocol.m_method;
        m_url = protocol.m_url;
        m_session = protocol.m_session;
        m_seq = protocol.m_seq;
        m_client_port[0] = protocol.m_client_port[0];
        m_client_port[1] = protocol.m_client_port[1];
    }
    return *this;
}

void RTSPRequest::SetMethod(const MBuffer& method)
{
    if (strcmp(method, "OPTIONS") == 0) m_method = 0;
    else if (strcmp(method, "DESCRIBE") == 0) m_method = 1;
    else if (strcmp(method, "SETUP") == 0) m_method = 2;
    else if (strcmp(method, "PLAY") == 0) m_method = 3;
    else if (strcmp(method, "TEARDOWN") == 0) m_method = 4;
}

void RTSPRequest::SetUrl(MBuffer url)
{
    m_url = (char*)url;
}

void RTSPRequest::SetSession(MBuffer session)
{
    m_session = (char*)session;
}

void RTSPRequest::SetSequence(MBuffer seq)
{
    // (char*)seq，去除seq后面多余的\0
    m_seq = (char*)seq;
}

void RTSPRequest::SetClientPorts(int ports[])
{
    m_client_port[0] << ports[0];
    m_client_port[1] << ports[1];
}

RTSPReply::RTSPReply(const RTSPReply& protocol)
{
    m_method = protocol.m_method;
    m_session = protocol.m_session;
    m_client_port[0] = protocol.m_client_port[0];
    m_client_port[1] = protocol.m_client_port[1];
    m_server_port[0] = protocol.m_server_port[0];
    m_server_port[1] = protocol.m_server_port[1];
    m_sdp = protocol.m_sdp;
    m_options = protocol.m_options;
    m_seq = protocol.m_seq;
}

RTSPReply& RTSPReply::operator=(const RTSPReply& protocol)
{
    if (this != &protocol) {
        m_method = protocol.m_method;
        m_session = protocol.m_session;
        m_client_port[0] = protocol.m_client_port[0];
        m_client_port[1] = protocol.m_client_port[1];
        m_server_port[0] = protocol.m_server_port[0];
        m_server_port[1] = protocol.m_server_port[1];
        m_sdp = protocol.m_sdp;
        m_options = protocol.m_options;
        m_seq = protocol.m_seq;
    }
    return *this;
}

void RTSPReply::SetMethod(int method)
{
    m_method = method;
}

void RTSPReply::SetSession(const MBuffer& session)
{
    m_session = session;
}

void RTSPReply::SetOptions(const MBuffer& options)
{
    m_options = options;
}

void RTSPReply::SetSequence(const MBuffer& seq)
{
    m_seq = seq;
}

void RTSPReply::SetSdp(const MBuffer& sdp)
{
    m_sdp = sdp;
}

void RTSPReply::SetClientPort(const MBuffer& port1, const MBuffer& port2)
{
    port1 >> m_client_port[0];
    port2 >> m_client_port[1];
}

void RTSPReply::SetServerPort(const MBuffer& port1, const MBuffer& port2)
{
    port1 >> m_server_port[0];
    port2 >> m_server_port[1];
}

MBuffer RTSPReply::ToBuffer()
{
    MBuffer reply;
    reply << "RTSP/1.0 200 OK\r\n" << "CSeq: " << m_seq << "\r\n";

    switch (m_method)
    {
    case 0:     // OPTIONS
        reply << m_options << "\r\n";
        break;

    case 1:     // DESCRIBE
        reply << "Content-Base: 127.0.0.1" << "\r\n";
        reply << "Content-Type: application/sdp\r\n";
        reply << "Content-Length: " << m_sdp.size() << "\r\n\r\n";      // 这里拼接的sdp，一定要多添加一个\r\n
        reply << m_sdp;                                                 // 这里不用\r\n
        break;

    case 2:     // SETUP
        reply << "Transport: RTP/AVP;unicast;client_port=" << m_client_port[0] << "-" << m_client_port[1] << 
            ";server_port=" << m_server_port[0] << "-" << m_server_port[1] << "\r\n";

        reply << "Session: " << (char*)m_session << "\r\n\r\n";
        break;

    case 3:     // PLAY
        reply << "Range: npt=0.000-\r\n";
        reply << "Session: " << (char*)m_session << "\r\n\r\n";
        break;

    case 4:     // TEARDOWN
        reply << "Session: " << (char*)m_session << "\r\n\r\n";
        break;

    }
    return reply;
}


