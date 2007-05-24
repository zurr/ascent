/*
 * Multiplatform Async Network Library
 * Copyright (c) 2007 grep
 *
 * ListenSocket<T>: Creates a socket listener on specified address and port,
 *				  requires Update() to be called every loop.
 *
 */

#ifndef LISTEN_SOCKET_WIN32_H
#define LISTEN_SOCKET_WIN32_H

#ifdef CONFIG_USE_IOCP

template<class T>
class SERVER_DECL ListenSocket
{
public:
	ListenSocket(const char * ListenAddress, uint32 Port)
	{
		m_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
		SocketOps::ReuseAddr(m_socket);
		SocketOps::Nonblocking(m_socket);

		m_address.sin_family = AF_INET;
		m_address.sin_port = ntohs((u_short)Port);
		m_address.sin_addr.s_addr = htonl(INADDR_ANY);
		m_opened = false;

		if(strcmp(ListenAddress, "0.0.0.0"))
		{
			struct hostent * hostname = gethostbyname(ListenAddress);
			if(hostname != 0)
				memcpy(&m_address.sin_addr.s_addr, hostname->h_addr_list[0], hostname->h_length);
		}

		// bind.. well attempt to.
		int ret = bind(m_socket, (const sockaddr*)&m_address, sizeof(m_address));
		if(ret != 0)
		{
			printf("Bind unsuccessful on port %u.", Port);
			return;
		}

		ret = listen(m_socket, 5);
		if(ret != 0) 
		{
			printf("Unable to listen on port %u.", Port);
			return;
		}

		m_opened = true;
		len = sizeof(sockaddr_in);
		m_cp = sSocketMgr.GetCompletionPort();
	}

	~ListenSocket()
	{
		Close();	
	}

	void Update()
	{
		aSocket = WSAAccept(m_socket, (sockaddr*)&m_tempAddress, (socklen_t*)&len, NULL, NULL);
		if(aSocket == INVALID_SOCKET)
			return;

		socket = new T(aSocket);
		socket->SetCompletionPort(m_cp);
		socket->Accept(&m_tempAddress);
	}

	void Close()
	{
		if(m_opened)
			SocketOps::CloseSocket(m_socket);
		m_opened = false;
	}

	inline bool IsOpen() { return m_opened; }

private:
	SOCKET m_socket;
	struct sockaddr_in m_address;
	struct sockaddr_in m_tempAddress;
	bool m_opened;
	uint32 len;
	SOCKET aSocket;
	T * socket;
	HANDLE m_cp;
};

#endif
#endif
