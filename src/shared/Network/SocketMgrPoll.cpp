/*
 * Multiplatform Async Network Library
 * Copyright (c) 2007 Burlex
 *
 * SocketMgr - Select manager.
 *
 */

#include "Network.h"
#ifdef CONFIG_USE_POLL

initialiseSingleton(SocketMgr);

SocketMgr::SocketMgr()
{
	memset(fds, 0, sizeof(void*) * SOCKET_HOLDER_SIZE);
	memset(poll_events, 0, sizeof(pollfd) * SOCKET_HOLDER_SIZE);
	highest_fd = 0;
	socket_count = 0;
}

SocketMgr::~SocketMgr()
{

}

void SocketMgr::AddSocket(Socket * s)
{
	if(socket_count >= SOCKET_HOLDER_SIZE || fds[s->GetFd()] != 0)
	{
		s->Delete();
		return;
	}

	/* add it to the big set */
	fds[s->GetFd()] = s;
	poll_events[s->GetFd()].fd = s->GetFd();
	
	if(s->GetWriteBufferSize() > 0)
		poll_events[s->GetFd()].events = POLLOUT;
	else
		poll_events[s->GetFd()].events = POLLOUT;

	if(s->GetFd() > highest_fd)
		highest_fd = s->GetFd();

	socket_count++;
}

void SocketMgr::RemoveSocket(Socket * s)
{
	if(socket_count >= SOCKET_HOLDER_SIZE || fds[s->GetFd()] == 0)
	{
		return;
	}

	fds[s->GetFd()] = 0;
	socket_count--;
}

void SocketMgr::WantWrite(int fd)
{
	m_setLock.Acquire();
	m_setLock.Release();
}

void SocketMgr::thread_function()
{
	Socket * s;
	uint32 i, j, k;

	for(;;)
	{
		/* dump all the fd's into the poll() struct */
		for(i = 0, j = 0; i < highest_fd; ++i)
		{
			if(fds[i] != 0)
			{
				if(fds[i]->GetWriteBufferSize() > 0)
					poll_events[j].events = POLLOUT;

				poll_events[j].events = POLLIN;
				poll_events[j++].fd = fds[i]->GetFd();
			}
		}

		if(!j)
		{
			Sleep(20);
			continue;
		}

		k = poll(poll_events, j, 20);
		if(!k || k < 0)
			continue;

		for(i = 0; i < k; ++i)
		{
            s = fds[poll_events[i].fd];
			if(!s) continue;

			if(poll_events[i].revents & POLLERR || poll_events.revents & POLLHUP)
			{
				s->Delete();
				continue;
			}

			if(poll_events[i].revents & POLLIN)
				s->ReadCallback(0);

			if(poll_events[i].revents & POLLOUT)
			{
				s->BurstBegin();
				s->WriteCallback();
				s->BurstEnd();
			}
		}
	}
}

void SocketWorkerThread::run()
{
	sSocketMgr.thread_function();
}

void SocketMgr::CloseAll()
{

}

void SocketMgr::SpawnWorkerThreads()
{
	int tc = 1;
	for(int i = 0; i < tc; ++i)
		launch_thread(new SocketWorkerThread());
}

void SocketMgr::ShutdownThreads()
{

}

#endif
