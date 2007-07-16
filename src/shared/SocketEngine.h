#ifndef _NETLIB_SOCKETENGINE_H
#define _NETLIB_SOCKETENGINE_H

class SocketEngine
{
public:
	virtual ~SocketEngine() {}
	virtual void AddSocket(BaseSocket * s) = 0;
	virtual void RemoveSocket(BaseSocket * s) = 0;
	virtual void WantWrite(BaseSocket * s) = 0;
	virtual void SpawnThreads() = 0;
	virtual void Shutdown() = 0;
	virtual void MessageLoop() = 0;
};

class SocketEngineThread : public ThreadBase
{
	SocketEngine * se;
public:
	SocketEngineThread(SocketEngine * s) : se(s) {}
	void run()
	{
		se->MessageLoop();
	}
};

extern SocketEngine * _socketEngine;

#endif		// _NETLIB_SOCKETENGINE_H
