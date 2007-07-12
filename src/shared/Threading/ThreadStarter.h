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

#ifndef _THREADING_STARTER_H
#define _THREADING_STARTER_H

class SERVER_DECL ThreadBase
{
public:
	ThreadBase() : delete_after_use(true) {}
	virtual ~ThreadBase() {}
	virtual void run() = 0;
	bool delete_after_use;
};

#ifdef WIN32

#include <process.h>

static unsigned int __stdcall thread_func(void * args)
{
	ThreadBase * ptr = (ThreadBase*)args;
	bool delete_it = ptr->delete_after_use;
	ptr->run();

	// delete the thread to call the destructor, our threads remove themselves here.
	if(delete_it)
		delete ptr;

	return 0;
}

// Use _beginthreadex to start the thread (MT runtime lib needed)
inline void launch_thread(ThreadBase * thread)
{
	_beginthreadex(0, 0, &thread_func, (void*)thread, 0, 0);
}

#else

static void * thread_func(void * args)
{
	ThreadBase * ptr = (ThreadBase*)args;
	bool delete_it = ptr->delete_after_use;
	ptr->run();

	// delete the thread to call the destructor, our threads remove themselves here.
	if(delete_it)
		delete ptr;
	
	return 0;
}

// Use pthread_create to start the thread
inline void launch_thread(ThreadBase * thread)
{
	pthread_t t;
	pthread_create(&t, 0, thread_func, (void*)thread);
}

#endif

#endif

