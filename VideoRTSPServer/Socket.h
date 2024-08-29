#pragma once
#include <WinSock2.h>
#include <memory>
#include <string>

enum MTYPE {
	MTCP,
	MUDP,
};


class MBuffer : public std::string
{
public:
	MBuffer(size_t size = 0) :std::string() {
		if (size > 0) {
			resize(size);
			memset(*this, 0, this->size());
		}
	}

	MBuffer(const char* str) {
		resize(strlen(str));
		memcpy((void*)c_str(), str, size());
	}

	MBuffer(void* buffer, size_t size) :std::string() {
		resize(size);
		memcpy((void*)c_str(), buffer, size);
	}

	~MBuffer() {
		std::string::~basic_string();
	}

	operator char* () const { return (char*)c_str(); }
	operator const char* () const { return c_str(); }
	operator BYTE* () const { return (BYTE*)c_str(); }
	operator void* () const { return (void*)c_str(); }

	MBuffer operator=(const char* str) {
		std::string::operator=(str);
		return *this;
	}

	void Updata(void* buffer, size_t size) {
		resize(size);
		memcpy((void*)c_str(), buffer, size);
	}
};

class Socket
{
public:
	Socket(MTYPE type = MTCP){
		if (type == MTCP) {
			m_sock = socket(PF_INET, SOCK_STREAM, 0);
		}
		else if (type == MUDP) {
			m_sock = socket(PF_INET, SOCK_DGRAM, 0);
		}
	}

	~Socket() {
		Close();
	}

	Socket(SOCKET s) {
		m_sock = s;
	}

	void Close() {
		if (m_sock != INVALID_SOCKET) {
			SOCKET t = m_sock;
			m_sock = INVALID_SOCKET;
			closesocket(t);
		}
	}

	operator SOCKET() {
		return m_sock;
	}

private:
	SOCKET m_sock;

};

class MAddress
{
public:
	MAddress() {
		m_port = -1;
		memset(&m_addr, 0, sizeof(m_addr));
		m_addr.sin_family = AF_INET;
	}

	MAddress(const MAddress& addr) {
		m_ip = addr.m_ip;
		m_port = addr.m_port;
		memcpy(&m_addr, &addr.m_addr, sizeof(sockaddr_in));
	}

	MAddress& operator=(const MAddress& addr) {
		if (this != &addr) {
			m_ip = addr.m_ip;
			m_port = addr.m_port;
			memcpy(&m_addr, &addr.m_addr, sizeof(sockaddr_in));
		}
		return *this;
	}

	~MAddress() {}

	void Update(const std::string& ip, short port) {
		m_ip = ip;
		m_port = port;
		m_addr.sin_port = htons(port);
		m_addr.sin_addr.s_addr = inet_addr(m_ip.c_str());
	}

	operator const sockaddr* () const{
		return (sockaddr*)&m_addr;
	}

	operator sockaddr* () {
		return (sockaddr*)&m_addr;
	}


	operator sockaddr_in* () {
		return &m_addr;
	}

	int size() const { return sizeof(sockaddr_in); }

private:
	std::string m_ip;
	short m_port;
	sockaddr_in m_addr;
};

class MSocket
{
public:
	MSocket(MTYPE type = MTCP) : m_socket(new Socket(type)), m_type(type){}

	MSocket(SOCKET sock, MTYPE type) :m_socket(new Socket(sock)), m_type(type){}

	MSocket(const MSocket& sock) : m_socket(sock.m_socket) {}

	MSocket& operator=(const MSocket& sock) {
		if (this != &sock) {
			m_socket = sock.m_socket;
		}
		return *this;
	}

	~MSocket() { m_socket.reset(); }

	operator SOCKET() {
		return *m_socket;
	}

	int Close() {
		m_socket.reset();
		return 0;
	}

	int Bind(const MAddress& addr) {
		if (m_socket == nullptr) {				// Close() 后可能会调用 Bind()
			m_socket.reset(new Socket(m_type));
		}
		return ::bind(*m_socket, addr, addr.size());
	}

	int Listen(int backlog = 5) {
		return ::listen(*m_socket, backlog);
	}

	MSocket Accept(MAddress& addr) {
		int len = sizeof(addr);
		SOCKET sock = ::accept(*m_socket, addr, &len);
		return MSocket(sock, m_type);		// 返回得到的套接字，和m_socket是同类的
	}

	int Connect(const MAddress& addr) {
		return ::connect(*m_socket, addr, addr.size());
	}

	int Recv(MBuffer& buffer) {
		return ::recv(*m_socket, buffer, buffer.size(), 0);
	}

	int Send(const MBuffer& buffer) {
		// TODO：待优化buffer较长的情况。
		return ::send(*m_socket, buffer, buffer.size(), 0);
	}

private:     
	std::shared_ptr<Socket> m_socket;
	MTYPE m_type;
};


class SocketInit
{
public:
	SocketInit(){
		WSADATA wsa;
		WSAStartup(MAKEWORD(2, 2), &wsa);
	}

	~SocketInit(){
		WSACleanup();
	}
};
