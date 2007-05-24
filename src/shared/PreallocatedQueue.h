/****************************************************************************
 *
 * General Object Type File
 * Copyright (c) 2007 Antrix Team
 *
 * This file may be distributed under the terms of the Q Public License
 * as defined by Trolltech ASA of Norway and appearing in the file
 * COPYING included in the packaging of this file.
 *
 * This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 * WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 */

#ifndef _PREALLOCATED_QUEUE_H
#define _PREALLOCATED_QUEUE_H

template<typename Type>
class PreallocatedQueue
{
public:
	PreallocatedQueue(uint32 size, uint32 realloc_size = 100)
	{
		// create buffer
		Buffer = (uint8*)malloc(size * sizeof(Type));
		m_readpos = m_writepos = 0;
		m_buffersize = size * sizeof(Type);
		type_size = sizeof(Type);
		m_reallocsize = realloc_size;
	}

	~PreallocatedQueue()
	{
		free(Buffer);
	}

	void push_back(Type ptr)
	{
		if((type_size + m_writepos) > m_buffersize)
			reallocate(m_buffersize + (m_reallocsize * type_size));

		memcpy(&Buffer[m_writepos], &ptr, type_size);
		m_writepos += type_size;
	}

	Type pop_front()
	{
		assert((m_readpos + type_size) <= m_writepos);
		Type returner;
		memcpy(&returner, &Buffer[m_readpos], type_size);
		m_readpos += type_size;

		// clear buffer completely if we're at the end of the buffer
		if(m_readpos == m_writepos)
			clear();

		return returner;
	}

	void clear()
	{
		m_readpos = m_writepos = 0;
	}

	bool empty()
	{
		if(m_readpos == m_writepos)
			return true;
		return false;
	}

	void reallocate(uint32 size)
	{
		Buffer = (uint8*)realloc(Buffer, size);
		assert(Buffer);
		m_buffersize = size;
	}

protected:

	uint8 * Buffer;

	uint32 m_readpos;
	uint32 m_writepos;
	uint32 m_writesize;
	uint32 m_buffersize;
	uint32 type_size;
	uint32 m_reallocsize;
};

#endif