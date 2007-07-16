#include "Network.h"

TcpSocket::TcpSocket(int fd, size_t readbuffersize, size_t writebuffersize, bool use_circular_buffer, const sockaddr_in * peer)
{
	SetFd(fd);
	if(use_circular_buffer)
	{
		//
		//
	}
	else
	{
		m_writeBuffer = new StraightBuffer;
		m_readBuffer = new StraightBuffer;
	}

	m_writeBuffer->Allocate(writebuffersize);
	m_readBuffer->Allocate(readbuffersize);
	memcpy(&m_peer, peer, sizeof(sockaddr));

	/* switch the socket to nonblocking mode */
	u_long arg = 1;
	ioctlsocket(fd, FIONBIO, &arg);
	m_writeLock = 0;
	m_deleted = m_connected = true;
}

TcpSocket::~TcpSocket()
{
	delete m_writeBuffer;
	delete m_readBuffer;
}

template<class T>
T * TcpSocket::Connect(const char * hostname, u_short port)
{
	sockaddr_in conn;
	hostent * host;
	
	/* resolve the peer */
	host = gethostbyname(hostname);

	if(!host)
		return NULL;

	/* copy into our address struct */
	memcpy(&conn, host->h_addr_list[0], sizeof(sockaddr_in));

	/* open socket */
	int fd = socket(AF_INET, 0, 0);

	/* set him to blocking mode */
	u_long arg = 0;
	ioctlsocket(fd, FIONBIO, &arg);

	/* try to connect */
	int result = connect(fd, (const sockaddr*)&conn, sizeof(sockaddr_in));

	if(result < 0)
	{
		printf("connect() failed - %u\n", errno);
		closesocket(fd);
		return 0;
	}

	/* create the struct */
	T * s = new T(fd, &conn);
	s->Finalize();
    return s;	
}

void TcpSocket::OnRead(size_t len)
{
	/* This is called when the socket engine gets an event on the socket */

#ifdef NETLIB_IOCP
	/* IOCP is easy. */
	m_readBuffer->IncrementWritten(len);

#else

	/* Any other platform, we have to call recv() to actually get the data. */
	int bytes = recv(m_fd, (char*)m_readBuffer->GetBuffer(), m_readBuffer->GetSpace(), 0);

	/* Under some socket engines, if this returns 0, we're in deep poo poo. */
	/* Check if recv() failed. */
	if(bytes <= 0)
		Disconnect();			// whoopes. :P
	else
	{
		m_readBuffer->IncrementWritten(bytes);
		OnRecvData();
	}
#endif
}

void TcpSocket::OnWrite(size_t len)
{
	/* Lock the buffer so nothing can modify it */
	LockWriteBuffer();

	/* This is called when the socket engine gets an event on the socket */
#ifdef NETLIB_IOCP

#else
	/* Push as much data out as we can in a nonblocking fashion. */
	int bytes = send(m_fd, (const char*)m_writeBuffer->GetBufferOffset(), m_writeBuffer->GetSize(), 0);

	if(bytes < 0)
		Disconnect();
	else
		m_writeBuffer->Remove(bytes);
#endif

	/* Unlock the write buffer, we're finished */
	UnlockWriteBuffer();
}

void TcpSocket::Finalize()
{
	_socketEngine->AddSocket(this);
	OnConnect();
}

bool TcpSocket::Write(const void * data, size_t bytes)
{
	if(!m_connected)
		return false;

	bool rv = m_writeBuffer->Write(data, bytes);
	if(rv)
	{
		if(!m_writeLock)
		{
			// WantWrite()
		}
	}

    return rv;
}

void TcpSocket::Disconnect()
{
	if(!m_connected) return;
	m_connected = false;

	_socketEngine->RemoveSocket(this);
	shutdown(m_fd, SD_BOTH);
	closesocket(m_fd);
	
	if(!m_deleted)
		Delete();
}

void TcpSocket::Delete()
{
	if(m_deleted) return;
	m_deleted = true;
}

void TcpSocket::OnError(int errcode)
{

}

bool TcpSocket::Writable()
{
	return (m_writeBuffer->GetSize() > 0) ? true : false;
}