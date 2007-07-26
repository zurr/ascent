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
