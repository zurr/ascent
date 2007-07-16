#ifndef _NETLIB_LISTENSOCKET_H
#define _NETLIB_LISTENSOCKET_H

template<class T>
class ListenSocket : public BaseSocket
{
	int new_fd;
	sockaddr_in new_peer;
	sockaddr_in address;
public:
	void OnRead(size_t len)
	{
		int len2 = sizeof(sockaddr_in);
		new_fd = accept(m_fd, (sockaddr*)&new_peer, &len2);
		if(new_fd > 0)
		{
			T * s = new T(new_fd, &new_peer);
			s->Finalize();
		}
	}

	void OnWrite(size_t len) {}
	void OnError(int err) {}
	
	void Open(const char * hostname, u_short port)
	{
		m_fd = socket(AF_INET, 0, 0);
		address.sin_family = AF_INET;
		address.sin_port = ntohs(port);
		address.sin_addr.s_addr = htonl(INADDR_ANY);

		if(bind(m_fd, (const sockaddr*)&address, sizeof(sockaddr_in)) < 0)
			return;

		if(listen(m_fd, 5) < 0)
			return;

		// add to mgr
		_socketEngine->AddSocket(this);
	}

	bool Writable() { return false; }
};

#endif		// _NETLIB_LISTENSOCKET_H
