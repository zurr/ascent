#include "Network.h"
#include "SocketEngine_epoll.h"

epollEngine::epollEngine()
{
	epoll_fd = epoll_create(MAX_DESCRIPTORS);
	assert(epoll_fd != -1);
}

epollEngine::~epollEngine()
{
	close(epoll_fd);
}

void epollEngine::AddSocket(BaseSocket * s)
{
	assert(fds[s->GetFd()] == 0);
	fds[s->GetFd()] = s;

	struct epoll_event ev;
	memset(&ev, 0, sizeof(epoll_event));
	ev.data.fd = s->GetFd();
	ev.events = (s->Writable()) ? EPOLLOUT : EPOLLIN;
	ev.events |= EPOLLET;

	epoll_ctl(epoll_fd, EPOLL_CTL_ADD, s->GetFd(), &ev);
}

void epollEngine::RemoveSocket(BaseSocket * s)
{
	assert(fds[s->GetFd()] == s);
	fds[s->GetFd()] = 0;

	struct epoll_event ev;
	memset(&ev, 0, sizeof(epoll_event));
	ev.data.fd = s->GetFd();
	ev.events = (s->Writable()) ? EPOLLOUT : EPOLLIN;
	ev.events |= EPOLLET;

	epoll_ctl(epoll_fd, EPOLL_CTL_DEL, s->GetFd(), &ev);
}

void epollEngine::WantWrite(BaseSocket * s)
{
	struct epoll_event ev;
	memset(&ev, 0, sizeof(epoll_event));
	ev.data.fd = s->GetFd();
	ev.events = EPOLLOUT | EPOLLET;

	epoll_ctl(epoll_fd, EPOLL_CTL_MOD, s->GetFd(), &ev);
}

void epollEngine::MessageLoop()
{
	const static int maxevents = 1024;
	struct epoll_event events[1024];
	int nfds, i;
	BaseSocket * s;
	while(m_running)
	{
        nfds = epoll_wait(epoll_fd, events, maxevents, 1000);
		for(i = 0; i < nfds; ++i)
		{
            s = fds[events[i].data.fd];
			if(s == 0)
			{
				printf("epoll returned invalid fd %u\n", events[i].data.fd);
				continue;
			}

			if(events[i].events & EPOLLHUP || events[i].events & EPOLLERR)
			{
				s->OnError(errno);
			}
			else if(events[i].events & EPOLLIN)
			{
				s->OnRead(0);
				if(s->Writable())
					WantWrite(s);
			}
			else if(events[i].events & EPOLLOUT)
			{
				s->OnWrite();
				if(!s->Writable())
				{
					/* change back to read state */
					struct epoll_event ev;
					memset(&ev, 0, sizeof(epoll_event));
					ev.data.fd = s->GetFd();
					ev.events = EPOLLIN | EPOLLET;

					epoll_ctl(epoll_fd, EPOLL_CTL_MOD, s->GetFd(), &ev);
				}
			}
		}
	}
}

