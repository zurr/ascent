#include "Network.h"

SocketEngine * _socketEngine;

SelectEngine::SelectEngine()
{
	m_running = true;
}

SelectEngine::~SelectEngine()
{

}

void SelectEngine::AddSocket(BaseSocket * s)
{
	m_socketLock.Acquire();
	m_sockets.insert( make_pair( s->GetFd(), s ) );
	m_socketLock.Release();
}

void SelectEngine::RemoveSocket(BaseSocket * s)
{
	m_socketLock.Acquire();
	m_sockets.erase(s->GetFd());
	m_socketLock.Release();
}

void SelectEngine::MessageLoop()
{
	FD_SET readable;
	FD_SET writable;
	FD_SET exception;
	map<int, BaseSocket*>::iterator itr, it2;
	int nfds;
	timeval timeout;
	timeout.tv_sec = 1;
	timeout.tv_usec = 0;
	while(m_running)
	{
		if(!m_sockets.size())
		{
			Sleep(20);
			continue;
		}

		FD_ZERO(&readable);
		FD_ZERO(&writable);
		FD_ZERO(&exception);

        m_socketLock.Acquire();
		for(itr = m_sockets.begin(); itr != m_sockets.end(); ++itr)
		{
			FD_SET( itr->second->GetFd(), itr->second->Writable() ? &writable : &readable );
		}
		m_socketLock.Release();

		nfds = select(FD_SETSIZE, &readable, &writable, &exception, &timeout);
		if(nfds > 0)
		{
			m_socketLock.Acquire();
			for(it2 = m_sockets.begin(); it2 != m_sockets.end();)
			{
				itr = it2++;
				if( FD_ISSET( itr->second->GetFd(), &exception ) )
					itr->second->OnError(errno);
				else if( FD_ISSET( itr->second->GetFd(), &readable ) )
					itr->second->OnRead(0);
				else if( FD_ISSET( itr->second->GetFd(), &writable ) )
					itr->second->OnWrite(0);
			}
			m_socketLock.Release();
		}
	}
}

void SelectEngine::SpawnThreads()
{
	launch_thread(new SocketEngineThread(this));
}

void SelectEngine::Shutdown()
{
	m_running = false;
}

void SelectEngine::WantWrite(BaseSocket * s)
{

}
