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
#include "TextLogger.h"

TextLoggerThread * TextLogger::Thread = 0;

void SetLowThreadPriority()
{
#ifdef WIN32
	::SetThreadPriority( ::GetCurrentThread(), THREAD_PRIORITY_LOWEST );
#else
	struct sched_param param;
	param.sched_priority = 0;
	pthread_setschedparam(pthread_self(), SCHED_OTHER, &param);
#endif
}
TextLogger::TextLogger(const char * filename, bool open)
{
	m_isOpen = false;
	m_writeLock = 0;
	m_filename = strnew(filename, false);
	m_file = 0;
	TextLogger::Thread->AddLogger(this);
	if(!open)
		return;

	m_file = fopen(filename, "a");
	if(m_file == 0)
	{
#ifdef WIN32
		CreateDirectory("logs", 0);
		m_file = fopen(filename, "a");
		if(!m_file)
			return;
#else
		return;
#endif
	}
	m_isOpen = true;
}

void TextLogger::Push(const char * str)
{
	queue.Push((char*)str);
	if(m_writeLock)		// data is pending already
		return;

#ifdef WIN32
	InterlockedIncrement(&m_writeLock);
#else
	m_writeLock++;
#endif
	TextLogger::Thread->AddPendingLogger(this);
}

void TextLogger::Update()
{
	char * str = queue.Pop();
	while(str)
	{
		if(m_file)
		{
			fputs(str, m_file);
			fflush(m_file);
		}
		delete [] str;
		str = queue.Pop();
	}
	m_writeLock = 0;
}

string FormatOutputString(const char * Prefix, const char * Description, bool useTimeStamp)
{

	char p[MAX_PATH];
	p[0] = 0;
	time_t t = time(NULL);
	tm * a = gmtime(&t);
	strcat(p, Prefix);
	strcat(p, "/");
	strcat(p, Description);
	if(useTimeStamp)
	{
		char ftime[100];
		snprintf(ftime, 100, "-%-4d-%02d-%02d %02d-%02d-%02d", a->tm_year+1900, a->tm_mon+1, a->tm_mday, a->tm_hour, a->tm_min, a->tm_sec);
		strcat(p, ftime);
	}

	strcat(p, ".log");
	return string(p);
}

void TextLoggerThread::run()
{
	// use the condition variable to wait until we have any data, hold
	// the thread in a suspended state until then so we don't waste time
	// switching to it
	TextLogger * l;
	SetLowThreadPriority();
	while(true)
	{
		m_condition.BeginSynchronized();
		m_active = false;
		m_condition.Wait();

		if(!m_running)
		{
			// we were told to die! muhahah! :)
			break;
		}

		// ooh! we got mail!
		l = updatePendingLoggers.Pop();
		while(l)
		{
			l->Update();
			l = updatePendingLoggers.Pop();
		}

		if(!m_running)
		{
			// we were told to die! muhahah! :)
			break;
		}

		// the end of another loop :> go back to sleep
		m_condition.EndSynchronized();
	}

	/* kill all the registered textloggers */
	for(set<TextLogger*>::iterator itr = loggers.begin(); itr != loggers.end(); ++itr)
		delete (*itr);

	// set will get cleared in destructor (happens at end of thread)
}

