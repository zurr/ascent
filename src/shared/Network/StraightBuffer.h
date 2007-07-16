#ifndef _NETLIB_STRAIGHTBUFFER_H
#define _NETLIB_STRAIGHTBUFFER_H

class StraightBuffer : public BaseBuffer
{
	char * m_buffer;
	size_t space;
	size_t written;

public:

	StraightBuffer()
	{
        space = written = 0;
		m_buffer = 0;
	}

	~StraightBuffer()
	{
		free(m_buffer);
	}

	bool Read(void * destination, size_t bytes)
	{
		if(written < bytes)
		{
			/* copy what we have */
			memcpy(destination, m_buffer, bytes);
			bytes = written = 0;
			return false;
		}
		else
		{
			/* read everything */
			memcpy(destination, m_buffer, bytes);
			written -= bytes;

			/* move the buffer "backwards" */
			memcpy(&m_buffer[0], &m_buffer[bytes], written);
			return true;
		}
	}

	bool Write(const void * data, size_t bytes)
	{
		if((written + bytes) > space)
		{
			/* write what we can */
			bytes = space - written;
			if(bytes)
				memcpy(&m_buffer[written], data, bytes);
			written = space;
			return false;
		}
		else
		{
			/* write everything */
			memcpy(&m_buffer[written], data, bytes);
			written += bytes;
			return true;
		}
	}

	size_t GetSpace()
	{
		return (space - written);
	}

	size_t GetSize()
	{
		return written;
	}

	void Remove(size_t len)
	{
		ASSERT(written >= len);
		written -= len;
		if(written)
			memcpy(&m_buffer[0], &m_buffer[written], written);
	}

	void * GetBuffer()
	{
		return &m_buffer[written];
	}

	void Allocate(size_t size)
	{
		m_buffer = (char*)malloc(size);
		space = size;
	}

	void IncrementWritten(size_t len)
	{
		written += len;
		ASSERT(space >= written);
	}

	void * GetBufferOffset()
	{
		return m_buffer;
	}
};

#endif		// _NETLIB_STRAIGHTBUFFER_H
