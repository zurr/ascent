#ifndef _NETLIB_TCPSOCKET_H
#define _NETLIB_TCPSOCKET_H

class TcpSocket : public BaseSocket
{
public:
	/** Constructor
	 * @param fd File descriptor to use with this socket
	 * @param readbuffersize Incoming data buffer size
	 * @param writebuffersize Outgoing data buffer size
	 * @param use_circular_buffer Use a circular buffer or normal buffer?
	 * @param peer Connection
	 */
	TcpSocket(int fd, size_t readbuffersize, size_t writebuffersize, bool use_circular_buffer, const sockaddr_in * peer);

	/** Destructor
	 */
	~TcpSocket();

	/** Connect to a server.
	 * @param hostname Hostname or IP address to connect to
	 * @param port Port to connect to
	 * @return templated type if successful, otherwise null
	 */
	template<class T>
	static T* Connect(const char * hostname, u_short port);

	/** Locks the socket's write buffer so you can begin a write operation
	 */
	inline void LockWriteBuffer() { m_writeMutex.Acquire(); }

	/** Unlocks the socket's write buffer so others can write to it
	 */
	inline void UnlockWriteBuffer() { m_writeMutex.Release(); }

	/** Writes the specified data to the end of the socket's write buffer
	 */
	bool Write(const void * data, size_t bytes);

	/** Reads the count of bytes from the buffer and put it in the specified pointer
	 */
	bool Read(void * destination, size_t bytes)
	{
		return m_readBuffer->Read(destination, bytes);
	}

	/** If for some reason we need to access the buffers directly 
	 * (shouldn't happen) these will return them
	 */
	inline BaseBuffer * GetReadBuffer() { return m_readBuffer; }
	inline BaseBuffer * GetWriteBuffer() { return m_writeBuffer; }

	/** Disconnects the socket, removing it from the socket engine, and queues
	 * deletion.
	 */
	void Disconnect();

	/** Queues the socket for deletion, and disconnects it, if it is connected
	 */
	void Delete();

	/** Implemented OnRead()
	 */
	void OnRead(size_t len);

	/** Implemented OnWrite()
	 */
	void OnWrite(size_t len);

	/** Are we connected?
	 */
	inline bool IsConnected() { return m_connected; }

	/** When we read data this is called
	 */
	virtual void OnRecvData() {}
	virtual void OnConnect() {}
	virtual void OnDisconnect() {}
	
	void Finalize();

	/** Get IP in numerical form
	 */
	const char * GetIP() { return inet_ntoa(m_peer.sin_addr); }

	/** Are we writable?
	 */
	bool Writable();
	void OnError(int errcode);

protected:

	/** Connected peer
	 */
	sockaddr_in m_peer;

	/** Socket's write buffer protection
	 */
	Mutex m_writeMutex;

	/** Read (inbound) buffer
	 */
	BaseBuffer * m_readBuffer;

	/** Write (outbound) buffer
	 */
	BaseBuffer * m_writeBuffer;
	
	/** Write mutex (so we don't post a write event twice
	 */
	long m_writeLock;

	/** deleted/disconnected markers
	 */
	bool m_deleted;
	bool m_connected;
};

#endif		// _NETLIB_TCPSOCKET_H
