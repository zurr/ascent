#ifndef _NETLIB_SOCKETENGINE_EPOLL_H
#define _NETLIB_SOCKETENGINE_EPOLL_H

#define MAX_DESCRIPTORS 1024

class epollEngine : public SocketEngine
{
	int epoll_fd;
	bool m_running;
	BaseSocket * fds[MAX_DESCRIPTORS];

public:
	epollEngine();
	~epollEngine();

	void AddSocket(BaseSocket * s);
	void RemoveSocket(BaseSocket * s);
	void WantWrite(BaseSocket * s);
	void MessageLoop();
	void SpawnThreads();
	void Shutdown();
};

#endif		// _NETLIB_SOCKETENGINE_EPOLL_H
