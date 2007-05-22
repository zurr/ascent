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

#ifndef _THREADING_MUTEX_H
#define _THREADING_MUTEX_H

#ifdef WIN32
#include <windows.h>

class SERVER_DECL Mutex
{
public:
	Mutex()
	{
		InitializeCriticalSection(&cs);
	}

	~Mutex()
	{
		DeleteCriticalSection(&cs);
	}

	void Acquire()
	{
		EnterCriticalSection(&cs);
	}

	void Release()
	{
		LeaveCriticalSection(&cs);
	}

	bool AttemptAcquire()
	{
		return TryEnterCriticalSection(&cs);
	}

protected:
	CRITICAL_SECTION cs;
};

/*******
 * this type of mutex will prevent re-entry from the same thread
 * its needed for the event system.
*/

class SERVER_DECL RMutex
{
public:
	RMutex()
	{
		InitializeCriticalSection(&cs);
		locked = false;
	}

	~RMutex()
	{
		DeleteCriticalSection(&cs);
	}

	void Acquire()
	{
		EnterCriticalSection(&cs);
		locked = true;
	}

	void Release()
	{
		LeaveCriticalSection(&cs);
		locked = false;
	}

	bool AttemptAcquire()
	{
		if(TryEnterCriticalSection(&cs))
		{
			if(locked == false)
			{
				locked = true;
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}

protected:
	CRITICAL_SECTION cs;
	bool locked;
};

#else

/***********************************
  pthread_mutex implementation
  **********************************/

class Mutex
{
public:
    Mutex()
    {
        pthread_mutex_init(&mutex, NULL);
    }

    ~Mutex()
    {
        pthread_mutex_destroy(&mutex);
    }

    inline void Acquire()
    {
        pthread_mutex_lock(&mutex);
    }

    inline void Release()
    {
        pthread_mutex_unlock(&mutex);
    }

    inline bool AttemptAcquire()
    {
        return (pthread_mutex_trylock(&mutex) == 0);
    }

protected:
	friend class Condition;
	pthread_mutex_t mutex;
};

#define RMutex Mutex

#endif

#endif

