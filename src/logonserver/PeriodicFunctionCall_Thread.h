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

#include "../shared/CallBack.h"

template<class Type>
class PeriodicFunctionCaller : public ThreadBase
{
public:
	template<class T>
		PeriodicFunctionCaller(T * callback, void (T::*method)(), uint32 Interval)
	{
		cb = new CallbackP0<T>(callback, method);
		interval = Interval;
		running = true;
		delete_after_use=false;
	}

	~PeriodicFunctionCaller()
	{
		delete cb;
	}

	void run()
	{
#ifndef WIN32
		uint32 start = getMSTime();
		uint32 end;
		uint32 etime;
		// initial rest
		Sleep(interval);

		while(running && mrunning)
		{
			start = getMSTime();
			
			// execute the callback
			cb->execute();

			end = getMSTime();
			etime = end - start;
			if(etime < interval)
				Sleep(interval - etime);
		}
#else
		thread_active=true;
		hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
		for(;;)
		{
			WaitForSingleObject(hEvent, interval);

			if(!running)
				break;	/* we got killed */

			/* times up */
			ResetEvent(hEvent);
			cb->execute();
		}
		thread_active=false;
#endif

	}

	void kill()
	{
		running = false;
#ifdef WIN32
		/* push the event */
		SetEvent(hEvent);
		printf("Waiting for PFC thread to exit...");
		/* wait for the thread to exit */
		while(thread_active)
		{
			Sleep(100);
		}
		printf(" done.\n");
#endif
	}

private:
	CallbackBase * cb;
	uint32 interval;
	bool running;
#ifdef WIN32
	bool thread_active;
	HANDLE hEvent;
#endif
};

#define SpawnPeriodicCallThread(otype, ptr, method, interval) \
	launch_thread(new PeriodicFunctionCaller<otype>(ptr, method, interval));
