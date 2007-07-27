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

#ifndef __CConsole_H
#define __CConsole_H

#include "Common.h"
#include "../game/CThreads.h"

class CConsoleThread : public CThread
{
public:
	CConsoleThread();
	~CConsoleThread();
	void run();
};

class CConsole :  public Singleton < CConsole >
{
	friend class CConsoleThread;

public:						// Public methods:
	bool * running_link;
	~CConsole();
protected:					// Protected methods:
	CConsoleThread *_thread;

	// Process one command
	void ProcessCmd(char *cmd);

	// ver[sion]
	void TranslateVersion(char *str);
	void ProcessVersion();

	// quit | exit
	void TranslateQuit(char *str);
	void ProcessQuit(int delay);
	void CancelShutdown(char *str);

	// help | ?
	void TranslateHelp(char *str);
	void ProcessHelp(char *command);


	void TranslateThreads(char* str);
	void ObjectStats(char *str);

	// getuptime
	void GetUptime(char *str);

	void WideAnnounce(char *str);
	void Announce(char* str);
	void SaveallPlayers(char *str);
	void Kick(char *str);
	void IPBan(char *str);
	void IPUnBan(char *str);
	void BanAccount(char *str);
	void PlayerInfo(char *str);
};

#define sCConsole CConsole::getSingleton()

#endif
