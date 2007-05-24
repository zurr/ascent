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

#ifndef __LOGONCONSOLE_H
#define __LOGONCONSOLE_H

#include "Common.h"
#include "../game/CThreads.h"

class LogonConsoleThread : public ThreadBase
{
public:
	LogonConsoleThread();
	~LogonConsoleThread();
	void run();
};

class LogonConsole :  public Singleton < LogonConsole >
{
	friend class LogonConsoleThread;

public:						// Public methods:
	void kill() { running = false; };

protected:					// Protected methods:
	LogonConsoleThread *_thread;

	// Process one command
	void ProcessCmd(char *cmd);

	// quit | exit
	void TranslateQuit(char *str);
	void ProcessQuit(int delay);
	void CancelShutdown(char *str);

	// help | ?
	void TranslateHelp(char *str);
	void ProcessHelp(char *command);

	void ReloadAccts(char *str);
	bool running;
};

#define sLogonConsole LogonConsole::getSingleton()

#endif
