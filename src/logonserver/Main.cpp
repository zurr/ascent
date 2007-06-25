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

#include "LogonStdAfx.h"
#include <signal.h>
#include "../shared/svn_revision.h"
#include <TextLogger.h>

#ifdef WIN32
#define PLATFORM_TEXT "Win32"
#define BANNER "Antrix/Win32-2.1.2-%u :: Logon Server                        www.emupedia.com"
#else
#if UNIX_FLAVOUR == UNIX_FLAVOUR_LINUX
#define PLATFORM_TEXT "Linux"
#define BANNER "Antrix/Linux-2.1.2-%u :: Logon Server                        www.emupedia.com"
#elif UNIX_FLAVOUR == UNIX_FLAVOUR_BSD
#define PLATFORM_TEXT "FreeBSD"
#define BANNER "Antrix/FreeBSD-2.1.2-%u :: Logon Server                      www.emupedia.com"
#else
#define PLATFORM_TEXT "Unix"
#define BANNER "Antrix/Unix-2.1.2-%u :: Logon Server                         www.emupedia.com"
#endif
#include <sched.h>
#endif

// Database impl
Database * sLogonSQL;
initialiseSingleton(LogonServer);
bool mrunning = true;
Mutex _authSocketLock;
set<AuthSocket*> _authSockets;
TextLogger * Crash_Log;

/*** Signal Handler ***/
void _OnSignal(int s)
{
	switch (s)
	{
#ifndef WIN32
	case SIGHUP:
	   {
		   sLog.outString("Received SIGHUP signal, reloading accounts.");
		   AccountMgr::getSingleton().ReloadAccounts(true);
	   }break;
#endif
	case SIGINT:
	case SIGTERM:
	case SIGABRT:
#ifdef _WIN32
	case SIGBREAK:
#endif
		mrunning = false;
		break;
	}

	signal(s, _OnSignal);
}

int main(int argc, char** argv)
{
#ifndef WIN32
	rlimit rl;
	if (getrlimit(RLIMIT_CORE, &rl) == -1)
		printf("getrlimit failed. This could be problem.\n");
	else
	{
		rl.rlim_cur = rl.rlim_max;
		if (setrlimit(RLIMIT_CORE, &rl) == -1)
			printf("setrlimit failed. Server may not save core.dump files.\n");
	}
#endif

	new LogonServer;

	// Run!
	LogonServer::getSingleton( ).Run();
}

bool startdb()
{
	string lhostname, lusername, lpassword, ldatabase;
	int lport = 0;
	int ltype = 1;
	// Configure Main Database

	bool result;

	// Configure Logon Database...
	result = Config.MainConfig.GetString("LogonDatabase", "Username", &lusername);
	result = !result ? result : Config.MainConfig.GetString("LogonDatabase", "Password", &lpassword);
	result = !result ? result : Config.MainConfig.GetString("LogonDatabase", "Hostname", &lhostname);
	result = !result ? result : Config.MainConfig.GetString("LogonDatabase", "Name", &ldatabase);
	result = !result ? result : Config.MainConfig.GetInt("LogonDatabase", "Port", &lport);
	result = !result ? result : Config.MainConfig.GetInt("LogonDatabase", "Type", &ltype);

	if(result == false)
	{
		sLog.outString("sql: Logon database parameters not found.");
		return false;
	}

	sLog.SetScreenLoggingLevel(Config.MainConfig.GetIntDefault("LogLevel", "Screen", 0));
	sLogonSQL = CreateDatabaseInterface((DatabaseType)ltype);

	// Initialize it
	if(!sLogonSQL->Initialize(lhostname.c_str(), (unsigned int)lport, lusername.c_str(),
		lpassword.c_str(), ldatabase.c_str(), Config.MainConfig.GetIntDefault("LogonDatabase", "ConnectionCount", 1),
		16384))
	{
		sLog.outError("sql: Logon database initialization failed. Exiting.");
		return false;
	}   

	return true;
}

void LogonServer::Run()
{
	launch_thread(new TextLoggerThread);
	sLog.Init(-1, 3);
	Crash_Log = new TextLogger(FormatOutputString("logs", "logonCrashLog", true).c_str(), false);
	
	sLog.outString("==============================================================================");
	sLog.outString(BANNER, g_getRevision());
	sLog.outString("");
	sLog.outString("Copyright (c) 2007 Antrix Team. This software is under the QPL license, for");
	sLog.outString("more information look under the COPYING file in this distribution.");
	sLog.outString("==============================================================================");
	sLog.outString("");
	sLog.outString("The key combination <Ctrl-C> will safely shut down the server at any time.");
	sLog.outString("");
	sLog.outString("Initializing Random Number Generators...");

	sLog.outColor(TNORMAL, "Loading Config Files...\n");
#ifdef WIN32
	sLog.outColor(TYELLOW, "  >> logonserver.conf :: ");
	if(Config.MainConfig.SetSource("logonserver.conf"))
#else
	sLog.outColor(TYELLOW, "  >> " CONFDIR "/logonserver.conf :: ");
	if(Config.MainConfig.SetSource(CONFDIR "/logonserver.conf"))
#endif
	{
		sLog.outColor(TGREEN, "ok!");
		sLog.outColor(TNORMAL, "\n\n");
	}
	else
	{
		sLog.outColor(TRED, "failed.");
		sLog.outColor(TNORMAL, "\n\n");
		return;
	}

   
	sLog.outColor(TNORMAL, "\n >> establishing database connection...");
	sLog.outColor(TGREEN, " ok!\n");

	sLog.outColor(TNORMAL, " >> starting: thread manager...");
		new ThreadMgr;
		ThreadMgr::getSingleton( ).Initialize();
	
	if(!startdb())
		return;

	sLog.outColor(TGREEN, " ok!\n");

	sLog.outColor(TNORMAL, " >> starting: account manager...");
		new AccountMgr;
		new IPBanner;
	sLog.outColor(TGREEN, " ok!\n");

	sLog.outColor(TNORMAL, " >> starting: information core...");
		new InformationCore;
	sLog.outColor(TGREEN, " ok!\n");

	sLog.outColor(TNORMAL, " >> precaching accounts...");
		sAccountMgr.ReloadAccounts(true);
	sLog.outColor(TGREEN, " %u accounts.\n", sAccountMgr.GetCount());
	sLog.outColor(TNORMAL, "\n");

	// Spawn periodic function caller thread for account reload every 10mins
	int time = Config.MainConfig.GetIntDefault("Rates", "AccountRefresh",600);
	time *= 1000;
	//SpawnPeriodicCallThread(AccountMgr, AccountMgr::getSingletonPtr(), &AccountMgr::ReloadAccountsCallback, time);
	PeriodicFunctionCaller<AccountMgr> * pfc = new PeriodicFunctionCaller<AccountMgr>(AccountMgr::getSingletonPtr(),
		&AccountMgr::ReloadAccountsCallback, time);
	launch_thread(pfc);

	// Load conf settings..
	uint32 cport = Config.MainConfig.GetIntDefault("Listen", "RealmListPort", 3724);
	uint32 sport = Config.MainConfig.GetIntDefault("Listen", "ServerPort", 8093);
	uint32 threadcount = Config.MainConfig.GetIntDefault("Network", "ThreadCount", 5);
	uint32 threaddelay = Config.MainConfig.GetIntDefault("Network", "ThreadDelay", 20);
	string host = Config.MainConfig.GetStringDefault("Listen", "Host", "0.0.0.0");
	string shost = Config.MainConfig.GetStringDefault("Listen", "ISHost", host.c_str());
	min_build = Config.MainConfig.GetIntDefault("Client", "MinBuild", 6180);
	max_build = Config.MainConfig.GetIntDefault("Client", "MaxBuild", 6999);
	string logon_pass = Config.MainConfig.GetStringDefault("LogonServer", "RemotePassword", "r3m0t3b4d");
	Sha1Hash hash;
	hash.UpdateData(logon_pass);
	hash.Finalize();
	memcpy(sql_hash, hash.GetDigest(), 20);
	
	launch_thread(new LogonConsoleThread);

	// Spawn garbage collector
	new SocketMgr;
	new SocketGarbageCollector;

	// Spawn network worker threads.
	sSocketMgr.SpawnWorkerThreads();

	// Spawn auth listener
	ListenSocket<AuthSocket> clientListener(host.c_str(), cport);
	bool authsockcreated = clientListener.IsOpen();

	// Spawn interserver listener
	ListenSocket<LogonCommServerSocket> interListener(shost.c_str(), sport);
	bool intersockcreated = interListener.IsOpen();

	// hook signals
	sLog.outString("Hooking signals...");
	signal(SIGINT, _OnSignal);
	signal(SIGTERM, _OnSignal);
	signal(SIGABRT, _OnSignal);
#ifdef _WIN32
	signal(SIGBREAK, _OnSignal);
#else
	signal(SIGHUP, _OnSignal);
#endif

	uint32 loop_counter = 0;

	while(mrunning && authsockcreated && intersockcreated)
	{
		if(!(++loop_counter % 400))	 // 20 seconds
			CheckForDeadSockets();

		clientListener.Update();
		interListener.Update();
		sInfoCore.TimeoutSockets();
		sSocketGarbageCollector.Update();
		CheckForDeadSockets();			  // Flood Protection
		Sleep(50);
	}

	sLog.outString("Shutting down...");
	pfc->kill();

	clientListener.Close();
	interListener.Close();
	sSocketMgr.CloseAll();
	
	sLogonConsole.kill();

	// kill db
	sThreadMgr.RemoveThread((MySQLDatabase*)sLogonSQL);
	((MySQLDatabase*)sLogonSQL)->SetThreadState(THREADSTATE_TERMINATE);

	// send a query to wake up condition
	((MySQLDatabase*)sLogonSQL)->Execute("SELECT count(acct) from accounts");
	sLog.outString("Waiting for database to close..");
	while(((MySQLDatabase*)sLogonSQL)->ThreadRunning)
		Sleep(100);

	DestroyDatabaseInterface(sLogonSQL);

	sThreadMgr.Shutdown();

	delete AccountMgr::getSingletonPtr();
	delete InformationCore::getSingletonPtr();
	delete ThreadMgr::getSingletonPtr();
	delete IPBanner::getSingletonPtr();
#ifdef WIN32
	//TerminateProcess(GetCurrentProcess(), 0);
#else
	printf("Hit any key to exit.\n");
#endif
}

void OnCrash(bool Terminate)
{

}

void LogonServer::CheckForDeadSockets()
{
	_authSocketLock.Acquire();
	time_t t = time(NULL);
	time_t diff;
	set<AuthSocket*>::iterator itr = _authSockets.begin();
	set<AuthSocket*>::iterator it2;
	AuthSocket * s;

	for(itr = _authSockets.begin(); itr != _authSockets.end();)
	{
		it2 = itr;
		s = (*it2);
		++itr;

		diff = t - s->GetLastRecv();
		if(diff > 240)		   // More than 4mins -> kill the socket.
		{
			sLog.outString("Killing connection from %s:%u due to no activity!", inet_ntoa(s->GetRemoteAddress()), 
				s->GetRemotePort());
			_authSockets.erase(it2);
			s->removedFromSet = true;
			s->Disconnect();
		}
	}
	_authSocketLock.Release();
}
