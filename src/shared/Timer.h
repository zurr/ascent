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

#ifndef WOW_TIMER_H
#define WOW_TIMER_H

#if PLATFORM == PLATFORM_WIN32
#   include <windows.h>
#   include <mmsystem.h>
#else
#   include <sys/timeb.h>
#endif

inline uint32 getMSTime()
{
	uint32 time_in_ms = 0;
#if PLATFORM == PLATFORM_WIN32
	time_in_ms = timeGetTime();
#else
	struct timeb tp;
	ftime(&tp);

	time_in_ms = tp.time * 1000 + tp.millitm;
#endif

	return time_in_ms;
}

class IntervalTimer
{
public:
	IntervalTimer() : _interval(0), _current(0) {}

	void Update(time_t diff) { _current += diff; if(_current<0) _current=0;}
	bool Passed() { return _current >= _interval; }
	void Reset() { if(_current >= _interval) _current = _current%_interval;  }
	
	void SetCurrent(time_t current) { _current = current; }
	void SetInterval(time_t interval) {
		_current = 0;
		_interval = interval; 
	}
	time_t GetInterval() const { return _interval; }
	time_t GetCurrent() const { return _current; }

private:
	time_t _interval;
	time_t _current;
};

#endif


