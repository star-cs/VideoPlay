#pragma once
#include <WinSock2.h>
#include <memory>

enum MTYPE {
	MTCP,
	MUDP,
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

class MSocket
{
public:
	MSocket(MTYPE type = MTCP) : m_socket(new Socket(type)) {}

	MSocket(const MSocket& sock) 
		: m_socket(sock.m_socket) {}

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

private:     
	std::shared_ptr<Socket> m_socket;
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
