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

