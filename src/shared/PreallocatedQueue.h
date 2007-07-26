/****************************************************************************
 *
 * General Object Type File
 * Copyright (c) 2007 Team Ascent
 *
 * This work is licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 3.0
 * License. To view a copy of this license, visit
 * http://creativecommons.org/licenses/by-nc-sa/3.0/ or send a letter to Creative Commons,
 * 543 Howard Street, 5th Floor, San Francisco, California, 94105, USA.
 *
 * EXCEPT TO THE EXTENT REQUIRED BY APPLICABLE LAW, IN NO EVENT WILL LICENSOR BE LIABLE TO YOU
 * ON ANY LEGAL THEORY FOR ANY SPECIAL, INCIDENTAL, CONSEQUENTIAL, PUNITIVE OR EXEMPLARY DAMAGES
 * ARISING OUT OF THIS LICENSE OR THE USE OF THE WORK, EVEN IF LICENSOR HAS BEEN ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGES.
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