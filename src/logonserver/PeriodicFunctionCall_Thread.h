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
	}

	~PeriodicFunctionCaller()
	{
		delete cb;
	}

	void run()
	{
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
	}

	void kill()
	{
		running = false;
	}

private:
	CallbackBase * cb;
	uint32 interval;
	bool running;
};

#define SpawnPeriodicCallThread(otype, ptr, method, interval) \
	launch_thread(new PeriodicFunctionCaller<otype>(ptr, method, interval));
