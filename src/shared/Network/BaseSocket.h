#ifndef _NETLIB_BASESOCKET_H
#define _NETLIB_BASESOCKET_H

class BaseSocket
{
public:
	/** Virtual destructor
	 */
	virtual ~BaseSocket() {}

	/** Returns the socket's file descriptor
	 */
	inline int GetFd() { return m_fd; }

	/** Sets the socket's file descriptor
	 * @param new_fd The new file descriptor
	 */
	inline void SetFd(int new_fd) { m_fd = new_fd; }

	/** Is this socket in a read state? Or a write state?
	 */
	virtual bool Writable() = 0;

	/** Virtual OnRead() callback
	 */
	virtual void OnRead(size_t len) = 0;

	/** Virtual OnWrite() callback
	 */
	virtual void OnWrite(size_t len) = 0;

	/** Virtual OnError() callback
	 */
	virtual void OnError(int errcode) = 0;

protected:
	/** This socket's file descriptor
	 */
	int m_fd;
};


#endif		// _NETLIB_BASESOCKET_H
