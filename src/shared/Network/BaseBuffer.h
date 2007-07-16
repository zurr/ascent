#ifndef _NETLIB_BASE_BUFFER_H
#define _NETLIB_BASE_BUFFER_H

class BaseBuffer
{
public:
	virtual ~BaseBuffer() {}
	virtual void Allocate(size_t size) = 0;
	virtual bool Write(const void * data, size_t bytes) = 0;
	virtual bool Read(void * destination, size_t bytes) = 0;
	virtual void * GetBuffer() = 0;
	virtual void * GetBufferOffset() = 0;
	virtual size_t GetSpace() = 0;
	virtual void IncrementWritten(size_t len) = 0;
	virtual size_t GetSize() = 0;
	virtual void Remove(size_t len) = 0;
};

#endif		// _NETLIB_BASE_BUFFER_H