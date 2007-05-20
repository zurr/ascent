/*
 * Multiplatform Async Network Library
 * Copyright (c) 2007 grep
 *
 * Socket implementable class.
 *
 */

#ifndef SOCKET_H
#define SOCKET_H

#include "SocketDefines.h"

class SERVER_DECL Socket
{
public:
    // Constructor. If fd = 0, it will be assigned 
    Socket(SOCKET fd, uint32 sendbuffersize, uint32 recvbuffersize);
    
    // Destructor.
    virtual ~Socket();

    // Open a connection to another machine.
    bool Connect(const char * Address, uint32 Port);

    // Disconnect the socket.
    void Disconnect();

    // Accept from the already-set fd.
    void Accept(sockaddr_in * address);

/* Implementable methods */

    // Called when data is received.
    virtual void OnRead() {}

    // Called when a connection is first successfully established.
    virtual void OnConnect() {}

    // Called when the socket is disconnected from the client (either forcibly or by the connection dropping)
    virtual void OnDisconnect() {}

/* Buffer Operations */

    // Remove x bytes from the front of the read buffer.
    void __fastcall RemoveReadBufferBytes(uint32 size, bool lock);

    // Remove x bytes from the front of the send buffer.
    void __fastcall RemoveWriteBufferBytes(uint32 size, bool lock);

    // Returns receive buffer offset.
    inline uint8 * GetReadBuffer(uint32 offset) { return m_readBuffer + offset; }

    // Increments the read size x bytes.
    inline void AddRecvData(uint32 size) { m_readByteCount += size; }

    // Reads x bytes from the buffer into the destination array.
    void __fastcall Read(uint32 size, uint8 * buffer);

/* Sending Operations */

    // Locks sending mutex, adds bytes, unlocks mutex.
    bool Send(const uint8 * Bytes, uint32 Size);

    // Burst system - Locks the sending mutex.
    inline void BurstBegin() { m_writeMutex.Acquire(); }

    // Burst system - Adds bytes to output buffer.
    bool BurstSend(const uint8 * Bytes, uint32 Size);

    // Burst system - Pushes event to queue - do at the end of write events.
    void BurstPush();

    // Burst system - Unlocks the sending mutex.
    inline void BurstEnd() { m_writeMutex.Release(); }

/* Client Operations */

    // Get the client's ip in numerical form.
    string GetRemoteIP();
    inline uint32 GetRemotePort() { return ntohs(m_client.sin_port); }
    inline SOCKET GetFd() { return m_fd; }
    
/* Platform-specific methods */

    void SetupReadEvent();
    void ReadCallback(uint32 len);
    void WriteCallback();

    inline bool IsDeleted() { return m_deleted; }
    inline bool IsConnected() { return m_connected; }

    inline uint32 GetReadBufferSize() { return m_readByteCount; }
    inline uint32 GetWriteBufferSize() { return m_writeByteCount; }
    inline sockaddr_in & GetRemoteStruct() { return m_client; }

/* Deletion */
    void Delete();

private:

    // We are deleted? Stop us from posting events.
    bool m_deleted;

    // we are connected? stop from posting events.
    bool m_connected;

    // Called when connection is opened.
    void _OnConnect();
  
/* Buffers */
    uint8 * m_readBuffer;
    uint8 * m_writeBuffer;
    Mutex m_writeMutex;
    Mutex m_readMutex;
    uint32 m_readBufferSize;
    uint32 m_writeBufferSize;
    uint32 m_readByteCount;
    uint32 m_writeByteCount;

/* Other Member Variables */
    SOCKET m_fd;
    sockaddr_in m_client;

/* Win32 - IOCP Specific Calls */
#ifdef CONFIG_USE_IOCP

public:

    // Set completion port that this socket will be assigned to.
    inline void SetCompletionPort(HANDLE cp) { m_completionPort = cp; }

    // Post an I/O event to the FIFO queue.
    void PostSocketMessage(SocketIOEvent type);
    
    // Atomic wrapper functions for increasing read/write locks
    inline void IncSendLock() { InterlockedIncrement(&m_writeLock); }
    inline void DecSendLock() { InterlockedDecrement(&m_writeLock); }
    inline bool AcquireSendLock() { return (InterlockedCompareExchange(&m_writeLock, 1, 0) == 0); }

private:
    // Completion port socket is assigned to
    HANDLE m_completionPort;
    
    // Write lock, stops multiple write events from being posted.
    long m_writeLock;
    
    // Assigns the socket to his completion port.
    void AssignToCompletionPort();

#endif

/* Linux - EPOLL Specific Calls */
#ifdef CONFIG_USE_EPOLL
public:
    // Posts a epoll event with the specifed arguments.
    void PostEvent(uint32 events);

    // Atomic wrapper functions for increasing read/write locks
    inline void IncSendLock() { m_writeLockMutex.Acquire(); m_writeLock++; m_writeLockMutex.Release(); }
    inline void DecSendLock() { m_writeLockMutex.Acquire(); m_writeLock--; m_writeLockMutex.Release(); }
    inline bool HasSendLock() { bool res; m_writeLockMutex.Acquire(); res = (m_writeLock != 0); m_writeLockMutex.Release(); return res; }
    bool AcquireSendLock()
    {
      bool rv;
      m_writeLockMutex.Acquire();
      if(m_writeLock != 0)
        rv = false;
      else
      {
        rv = true;
	m_writeLock++;
      }
      m_writeLockMutex.Release();
      return rv;
    }

private:
    unsigned int m_writeLock;
    Mutex m_writeLockMutex;
#endif

/* FreeBSD - kqueue specific calls */
#ifdef CONFIG_USE_KQUEUE
public:
    // Posts a epoll event with the specifed arguments.
    void PostEvent(int events, bool oneshot);

    // Atomic wrapper functions for increasing read/write locks
    inline void IncSendLock() { m_writeLockMutex.Acquire(); m_writeLock++; m_writeLockMutex.Release(); }
    inline void DecSendLock() { m_writeLockMutex.Acquire(); m_writeLock--; m_writeLockMutex.Release(); }
    inline bool HasSendLock() { bool res; m_writeLockMutex.Acquire(); res = (m_writeLock != 0); m_writeLockMutex.Release(); return res; }
    bool AcquireSendLock()
    {
        bool rv;
        m_writeLockMutex.Acquire();
        if(m_writeLock != 0)
            rv = false;
        else
        {
            rv = true;
            m_writeLock++;
        }
        m_writeLockMutex.Release();
        return rv;
    }

private:
    unsigned int m_writeLock;
    Mutex m_writeLockMutex;
#endif
};

/* Socket Garbage Collector */
#define SOCKET_GC_TIMEOUT 15

class SocketGarbageCollector : public Singleton<SocketGarbageCollector>
{
    map<Socket*, time_t> deletionQueue;
    Mutex lock;
public:
    void Update()
    {
        map<Socket*, time_t>::iterator i, i2;
        time_t t = time(NULL);
        lock.Acquire();
        for(i = deletionQueue.begin(); i != deletionQueue.end();)
        {
            i2 = i++;
            if(i2->second <= t)
            {
                delete i2->first;
                deletionQueue.erase(i2);
            }
        }
        lock.Release();
    }

    void QueueSocket(Socket * s)
    {
        lock.Acquire();
        deletionQueue.insert( map<Socket*, time_t>::value_type( s, time(NULL) + SOCKET_GC_TIMEOUT ) );
        lock.Release();
    }
};

#define sSocketGarbageCollector SocketGarbageCollector::getSingleton()

#endif
