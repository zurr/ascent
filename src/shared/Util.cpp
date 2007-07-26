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

#include "Common.h"

using namespace std;

vector<string> StrSplit(const string &src, const string &sep)
{
	vector<string> r;
	string s;
	for (string::const_iterator i = src.begin(); i != src.end(); i++) {
		if (sep.find(*i) != string::npos) {
			if (s.length()) r.push_back(s);
			s = "";
		} else {
			s += *i;
		}
	}
	if (s.length()) r.push_back(s);
	return r;
}

void SetThreadName(const char* format, ...)
{
	// This isn't supported on nix?
	va_list ap;
	va_start(ap, format);

#ifdef WIN32

	char thread_name[200];
	vsnprintf(thread_name, 200, format, ap);

	THREADNAME_INFO info;
	info.dwType = 0x1000;
	info.dwThreadID = GetCurrentThreadId();
	info.dwFlags = 0;
	info.szName = thread_name;

	__try
	{
		RaiseException(0x406D1388, 0, sizeof(info)/sizeof(DWORD), (DWORD*)&info);
	}
	__except(EXCEPTION_CONTINUE_EXECUTION)
	{

	}

#endif

	va_end(ap);
}

uint32 convTimePeriod ( uint32 dLength, char dType )
{
	if (dLength == 0)
		return 0;
	switch(tolower(dType))
	{
		case 'h':		// hours
			return 60 * 60 * dLength;
			break;
		case 'd':		// days
			return 60 * 60 * 24 * dLength;
			break;
		case 'w':		// weeks
			return 60 * 60 * 24 * 7 * dLength;
			break;
		case 'm':		// months
			return 60 * 60 * 24 * 30 * dLength;
			break;
		case 'y':		// years
			return 60 * 60 * 24 * 365 * dLength;
			break;
		default:		// minutes
			return 60 * dLength;
			break;
	}
	return 0;
}