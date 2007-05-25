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

#ifdef WIN32
#define PLATFORM_TEXT "Win32"
#else
#define PLATFORM_TEXT "Unix"
#endif

// Database impl
Database * sLogonSQL;
uint8 loglevel = 3;
initialiseSingleton(LogonServer);
bool mrunning = true;
Mutex _authSocketLock;
set<AuthSocket*> _authSockets;

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
	result = Config.MainConfig.GetString("LogonDatabase.Username", &lusername);
	result = !result ? result : Config.MainConfig.GetString("LogonDatabase.Password", &lpassword);
	result = !result ? result : Config.MainConfig.GetString("LogonDatabase.Hostname", &lhostname);
	result = !result ? result : Config.MainConfig.GetString("LogonDatabase.Name", &ldatabase);
	result = !result ? result : Config.MainConfig.GetInt("LogonDatabase.Port", &lport);
	result = !result ? result : Config.MainConfig.GetInt("LogonDatabase.Type", &ltype);

	if(result == false)
	{
		sLog.outString("sql: Logon database parameters not found.");
		return false;
	}

	loglevel = (uint8)Config.MainConfig.GetIntDefault("LogLevel", 0);
	sLogonSQL = CreateDatabaseInterface((DatabaseType)ltype);

	// Initialize it
	if(!sLogonSQL->Initialize(lhostname.c_str(), (unsigned int)lport, lusername.c_str(),
		lpassword.c_str(), ldatabase.c_str(), Config.MainConfig.GetIntDefault("LogonDatabase.ConnectionCount", 1),
		16384))
	{
		sLog.outError("sql: Logon database initialization failed. Exiting.");
		return false;
	}   

	return true;
}

void LogonServer::Run()
{
	sLog.Init();
	sLog.outString("===============================================================================");
	sLog.outString("Antrix :: Logon Server (running under %s)", PLATFORM_TEXT);
	sLog.outString("Compatibility is tested with 2.1.0. Revision %u.", g_getRevision());
	sLog.outString("===============================================================================");
	sLog.outString("The key combination <Ctrl-C> will safely shut down the server at any time.");
	sLog.outString("");

	sLog.outColor(TNORMAL, "Loading Config Files...\n");
#ifdef WIN32
	sLog.outColor(TYELLOW, "  >> logonserver.conf :: ");
	if(Config.MainConfig.SetSource("logonserver.conf"))
#else
	sLog.outColor(TYELLOW, "  >> " CONFDIR "logonserver.conf :: ");
	if(Config.MainConfig.SetSource(CONFDIR "logonserver.conf"))
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
	int time = Config.MainConfig.GetIntDefault("AccountRefresh",600);
	time *= 1000;
	//SpawnPeriodicCallThread(AccountMgr, AccountMgr::getSingletonPtr(), &AccountMgr::ReloadAccountsCallback, time);
	PeriodicFunctionCaller<AccountMgr> * pfc = new PeriodicFunctionCaller<AccountMgr>(AccountMgr::getSingletonPtr(),
		&AccountMgr::ReloadAccountsCallback, time);
	launch_thread(pfc);

	// Load conf settings..
	uint32 cport = Config.MainConfig.GetIntDefault("RealmListPort", 3724);
	uint32 sport = Config.MainConfig.GetIntDefault("ServerPort", 8093);
	uint32 threadcount = Config.MainConfig.GetIntDefault("Network.ThreadCount", 5);
	uint32 threaddelay = Config.MainConfig.GetIntDefault("Network.ThreadDelay", 20);
	string host = Config.MainConfig.GetStringDefault("Host", "0.0.0.0");
	string shost = Config.MainConfig.GetStringDefault("ISHost", host.c_str());
	min_build = Config.MainConfig.GetIntDefault("MinClientBuild", 6180);
	max_build = Config.MainConfig.GetIntDefault("MaxClientBuild", 6999);
	
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
		if(diff > 30)		   // More than 30 seconds -> kill the socket.
		{
			sLog.outString("Killing connection from %s:%u due to no activity!", s->GetRemoteIP().c_str(), 
				s->GetRemotePort());
			_authSockets.erase(it2);
			s->removedFromSet = true;
			s->Disconnect();
		}
	}
	_authSocketLock.Release();
}
