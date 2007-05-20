/*
 * Multiplatform Async Network Library
 * Copyright (c) 2007 grep
 *
 * SocketDefines.h - Any platform-specific defines/includes go here.
 *
 */

#ifndef SOCKET_DEFINES_H
#define SOCKET_DEFINES_H

#include "../Common.h"
using namespace std;

/* Implementation Selection */
#ifdef WIN32        // Easy
#define CONFIG_USE_IOCP
#else

// unix defines
#define SOCKET int
#define SD_BOTH SHUT_RDWR

#if UNIX_FLAVOUR == UNIX_FLAVOUR_LINUX

// select: epoll
#include <sys/epoll.h>
#define CONFIG_USE_EPOLL

#elif UNIX_FLAVOUR == UNIX_FLAVOUR_BSD

// select: kqueue
#include <sys/event.h>
#define CONFIG_USE_KQUEUE

#endif

#endif

/* IOCP Defines */

#ifdef CONFIG_USE_IOCP
enum SocketIOEvent
{
    SOCKET_IO_EVENT_READ_COMPLETE   = 0,
    SOCKET_IO_EVENT_WRITE_END       = 1,
    NUM_SOCKET_IO_EVENTS            = 2,
};

class OverlappedStruct
{
public:
    OVERLAPPED m_overlap;
    SocketIOEvent m_event;
    OverlappedStruct(SocketIOEvent ev) : m_event(ev)
    {
        memset(&m_overlap, 0, sizeof(OVERLAPPED));
    };
};

#endif

#endif
