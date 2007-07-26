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


