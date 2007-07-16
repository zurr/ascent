#ifndef _NETLIB_SELECTENGINE_H
#define _NETLIB_SELECTENGINE_H

class BaseSocket;
class SelectEngine : public SocketEngine
{
public:
	SelectEngine();
	~SelectEngine();

	void AddSocket(BaseSocket * s);
	void RemoveSocket(BaseSocket * s);
	void WantWrite(BaseSocket * s);
	void SpawnThreads();
	void Shutdown();
	void MessageLoop();

protected:
	bool m_running;
	Mutex m_socketLock;
	map<int, BaseSocket*> m_sockets;
};

#endif		// _NETLIB_SELECTENGINE_H
