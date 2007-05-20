/*
 * Multiplatform Async Network Library
 * Copyright (c) 2007 grep
 *
 * SocketMgr - iocp-based SocketMgr for windows.
 *
 */

#include "Network.h"
#ifdef CONFIG_USE_IOCP

initialiseSingleton(SocketMgr);
SocketMgr::SocketMgr()
{
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2,0), &wsaData);
    m_completionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, (ULONG_PTR)0, 0);

}

SocketMgr::~SocketMgr()
{
    
}

void SocketMgr::SpawnWorkerThreads()
{
    SYSTEM_INFO si;
    GetSystemInfo(&si);

    printf("IOCP: Spawning %u worker threads.\n", si.dwNumberOfProcessors);
    for(uint32 x = 0; x < si.dwNumberOfProcessors; ++x)
        launch_thread(new SocketWorkerThread());
}

void SocketWorkerThread::run()
{
    HANDLE cp = sSocketMgr.GetCompletionPort();
    uint32 len;
    Socket * s;
    OverlappedStruct * ov;
    LPOVERLAPPED ol_ptr;

    while(true)
    {
        if(!GetQueuedCompletionStatus(cp, &len, (LPDWORD)&s, &ol_ptr, 10000))
            continue;

        if(s->IsDeleted())
            continue;

        ov = CONTAINING_RECORD(ol_ptr, OverlappedStruct, m_overlap);

        if(ov->m_event < NUM_SOCKET_IO_EVENTS)
            ophandlers[ov->m_event](s, len);

        delete ov;
    }
}

void HandleReadComplete(Socket * s, uint32 len)
{
    if(!s->IsDeleted())
    {
        if(len)
        {
            s->AddRecvData(len);
            s->OnRead();
            s->SetupReadEvent();
        }
        else
            s->Delete();      // Queue deletion.
    }
}

void HandleWriteComplete(Socket * s, uint32 len)
{
    if(!s->IsDeleted())
    {
        s->BurstBegin();                    // Lock
        s->RemoveWriteBufferBytes(len, false);
        if(s->GetWriteBufferSize() > 0)
            s->WriteCallback();
        else
            s->DecSendLock();
        s->BurstEnd();                      // Unlock
    }
}

void SocketMgr::CloseAll()
{
    list<Socket*> tokill;

    socketLock.Acquire();
    for(set<Socket*>::iterator itr = _sockets.begin(); itr != _sockets.end(); ++itr)
        tokill.push_back(*itr);
    socketLock.Release();
    
    for(list<Socket*>::iterator itr = tokill.begin(); itr != tokill.end(); ++itr)
        (*itr)->Delete();

    uint32 size;
    do
    {
        socketLock.Acquire();
        size = _sockets.size();
        socketLock.Release();
    }while(size);
}

#endif