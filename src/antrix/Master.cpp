#include "Master.h"
#include "CConsole.h"
#include "../shared/CrashHandler.h"

#include <Network/Network.h>
#include "../shared/Auth/WowCrypt.h"
#include "../shared/CallBack.h"
#include "../game/EventMgr.h"
#include "../game/EventableObject.h"
#include "../game/AreaTrigger.h"
#include "../game/Opcodes.h"
#include "../game/World.h"
#include "../game/Opcodes.h"
#include "../shared/WorldPacket.h"
#include "../game/WorldSocket.h"
#include "../game/ThreadMgr.h"
#include "../game/ScriptMgr.h"
#include "../game/WorldSocket.h"
#include "../game/LogonCommClient.h"
#include "../game/LogonCommHandler.h"
#include "../scriptengine/ScriptEngine.h"

#ifdef WIN32
#define PLATFORM_TEXT "Win32"
#define BANNER "Antrix/Win32-2.1.0-%u :: World Server                        www.emupedia.com"
#else
#if UNIX_FLAVOUR == UNIX_FLAVOUR_LINUX
#define PLATFORM_TEXT "Linux"
#define BANNER "Antrix/Linux-2.1.0-%u :: World Server                        www.emupedia.com"
#elif UNIX_FLAVOUR == UNIX_FLAVOUR_BSD
#define PLATFORM_TEXT "FreeBSD"
#define BANNER "Antrix/FreeBSD-2.1.0-%u :: World Server                      www.emupedia.com"
#else
#define PLATFORM_TEXT "Unix"
#define BANNER "Antrix/Unix-2.1.0-%u :: World Server                         www.emupedia.com"
#endif
#include <sched.h>
#endif

#include "../shared/svn_revision.h"

#include "../game/WorldSession.h"
#include "LogonCommClient.h"

#include <signal.h>

createFileSingleton(Master);
std::string LogFileName;
bool bLogChat;
bool crashed = false;

volatile bool Master::m_stopEvent = false;

// Database defines.
SERVER_DECL Database* Database_Main;

// mainserv defines
TextLogger * Crash_Log;
SessionLogWriter * GMCommand_Log;
SessionLogWriter * Anticheat_Log;

void Master::_OnSignal(int s)
{
	switch (s)
	{
	case SIGINT:
	case SIGTERM:
	case SIGABRT:
#ifdef _WIN32
	case SIGBREAK:
#endif
		Master::m_stopEvent = true;
		break;
	}

	signal(s, _OnSignal);
}

Master::Master()
{
	m_ShutdownTimer = 0;
	m_ShutdownEvent = false;
	m_restartEvent = false;
}

Master::~Master()
{
}

struct Addr
{
	unsigned short sa_family;
	/* sa_data */
	unsigned short Port;
	unsigned long IP; // inet_addr
	unsigned long unusedA;
	unsigned long unusedB;
};


bool Master::Run()
{
	// Startup banner
	launch_thread(new TextLoggerThread);
	sLog.Init(-1, 3);
	sLog.outString("==============================================================================");
	sLog.outString(BANNER, g_getRevision());
	sLog.outString("");
	sLog.outString("Copyright (c) 2007 Antrix Team. This software is under the QPL license, for");
	sLog.outString("more information look under the COPYING file in this distribution.");
	sLog.outString("==============================================================================");
	sLog.outString("");
	sLog.outString("The key combination <Ctrl-C> will safely shut down the server at any time.");
	sLog.outString("");
	sLog.outString("Initializing File Loggers...");
	Crash_Log = new TextLogger(FormatOutputString("logs", "CrashLog", true).c_str(), false);
    
	sLog.outString("Initializing Random Number Generators...");
	uint32 seed = time(NULL);
	new MTRand(seed);
	srand(seed);

	sLog.outString("Starting Thread Manager...\n");
	new ThreadMgr;
	uint32 LoadingTime = getMSTime();

	sLog.outColor(TNORMAL, "Loading Config Files...\n");
#ifdef WIN32
	sLog.outColor(TYELLOW, "  >> antrix.conf :: ");
	if(Config.MainConfig.SetSource("antrix.conf"))
#else
	sLog.outColor(TYELLOW, "  >> " CONFDIR "/antrix.conf :: ");
	if(Config.MainConfig.SetSource(CONFDIR "/antrix.conf"))
#endif
	{
		sLog.outColor(TGREEN, "ok!");
		sLog.outColor(TNORMAL, "\n");
	}
	else
	{
		sLog.outColor(TRED, "fail.");
		sLog.outColor(TNORMAL, "\n");
		return false;
	}

	/* test for die variables */
	string die;
	if(Config.MainConfig.GetString("die", "msg", &die) || Config.MainConfig.GetString("die2", "msg", &die))
	{
		printf("Die directive received: %s", die.c_str());
		return false;
	}	

#ifdef WIN32
	sLog.outColor(TYELLOW, "  >> realms.conf :: ");
	if(Config.RealmConfig.SetSource("realms.conf"))
#else
	sLog.outColor(TYELLOW, "  >> " CONFDIR "/realms.conf :: ");
	if(Config.RealmConfig.SetSource(CONFDIR "/realms.conf"))
#endif
	{
		sLog.outColor(TGREEN, "ok!");
		sLog.outColor(TNORMAL, "\n\n");
	}
	else
	{
		sLog.outColor(TRED, "fail.");
		sLog.outColor(TNORMAL, "\n\n");
		return false;
	}

	if(!_StartDB())
	{
		return false;
	}

	sLog.outString("");

	ScriptSystem = new ScriptEngine;
	ScriptSystem->Reload();

	new EventMgr;
	new World;

	// open cheat log file
	Anticheat_Log = new SessionLogWriter(FormatOutputString("logs", "cheaters", false).c_str(), false);
	GMCommand_Log = new SessionLogWriter(FormatOutputString("logs", "gmcommand", false).c_str(), false);

	/* load the config file */
	sWorld.Rehash(false);

	// Initialize Opcode Table
	WorldSession::InitPacketHandlerTable();

	string host = Config.MainConfig.GetStringDefault("Listen", "Host", DEFAULT_HOST);
	int wsport = Config.MainConfig.GetIntDefault("Listen", "WorldServerPort", DEFAULT_WORLDSERVER_PORT);

	new ScriptMgr;

	sWorld.SetInitialWorldSettings();


	sWorld.SetStartTime((uint32)time(NULL));
	
	_HookSignals();

	launch_thread(new CConsoleThread);

	uint32 realCurrTime, realPrevTime;
	realCurrTime = realPrevTime = getMSTime();

	// initialize thread system
	sThreadMgr.Initialize();
	
	// Socket loop!
	uint32 start;
	uint32 diff;
	uint32 last_time = now();
	uint32 etime;
	uint32 next_printout = getMSTime(), next_send = getMSTime();

	// Start Network Subsystem
	sLog.outString("Starting network subsystem...");
	new SocketMgr;
	new SocketGarbageCollector;
	sSocketMgr.SpawnWorkerThreads();

	sScriptMgr.LoadScripts();


	sLog.outString("Threading system initialized, currently %u threads are active.", sThreadMgr.GetThreadCount());	

	LoadingTime = getMSTime() - LoadingTime;
	Log::getSingleton( ).outString ("\nServer is ready for connections. Startup time: %ums\n", LoadingTime );
 
	/* write pid file */
	FILE * fPid = fopen("antrix.pid", "w");
	if(fPid)
	{
		uint32 pid;
#ifdef WIN32
		pid = GetCurrentProcessId();
#else
		pid = getpid();
#endif
		fprintf(fPid, "%u", pid);
		fclose(fPid);
	}

	/* Connect to realmlist servers / logon servers */
	new LogonCommHandler();
	sLogonCommHandler.Startup();

	// Create listener
	ListenSocket<WorldSocket> Listener("0.0.0.0", wsport);
	bool listnersockcreate = Listener.IsOpen();


	while(!m_stopEvent && listnersockcreate)
	{
		start = now();
		diff = start - last_time;
		sLogonCommHandler.UpdateSockets();
		sSocketGarbageCollector.Update();
		Listener.Update();

		/* UPDATE */
		last_time = now();
		etime = last_time - start;
		if(m_ShutdownEvent)
		{
			if(getMSTime() >= next_printout)
			{
				if(m_ShutdownTimer > 60000.0f)
				{
					if(!((int)(m_ShutdownTimer)%60000))
						sLog.outString("Server shutdown in %i minutes.", (int)(m_ShutdownTimer / 60000.0f));
				}
				else
					sLog.outString("Server shutdown in %i seconds.", (int)(m_ShutdownTimer / 1000.0f));
					
				next_printout = getMSTime() + 500;
			}
			if(getMSTime() >= next_send)
			{
				// broadcast packet.
				WorldPacket data(20);
				data.SetOpcode(SMSG_SERVER_MESSAGE);
				data << uint32(SERVER_MSG_SHUTDOWN_TIME);
				int time = m_ShutdownTimer / 1000;
				if(time > 0)
				{
					int mins = 0, secs = 0;
					if(time > 60)
						mins = time / 60;
					if(mins)
						time -= (mins*60);
					secs = time;
					char str[20];
					sprintf(str, "%02u:%02u", mins, secs);
					data << str;
					sWorld.SendGlobalMessage(&data, NULL);
				}
				next_send = getMSTime() + 1000;
			}
			if(diff >= m_ShutdownTimer)
				break;
			else
				m_ShutdownTimer -= diff;
		}
		
		sDatabase.CheckConnections();
		sWorld.UpdateQueuedSessions(diff);

		if(50 > etime)
			Sleep(50 - etime);

	}
	_UnhookSignals();

	CConsoleThread *thread = (CConsoleThread*)sThreadMgr.GetThreadByType(THREADTYPE_CONSOLEINTERFACE);
	ASSERT(thread);
	thread->SetThreadState(THREADSTATE_TERMINATE);
	// have to cleanup manually.
	sThreadMgr.RemoveThread(thread);

	
	sLog.outString("Closing listener socket...");
	Listener.Close();
	sLog.outString("Listener closed..");

	sLog.outString("\nDisconnecting all clients....");
	sSocketMgr.CloseAll();
	sLog.outString("All client sockets closed.\n");

	// begin server shutdown
	time_t st = time(NULL);
	sLog.outString("Server shutdown initiated at %s", ctime(&st));

	// send a query to wake it up if its inactive
	sLog.outString("Executing pending database queries and closing database thread...");
	// kill the database thread first so we don't lose any queries/data
	CThread *dbThread = sThreadMgr.GetThreadByType(THREADTYPE_DATABASE);
	CThread *temp = dbThread;
	ASSERT(dbThread);
	// end it
	dbThread->SetThreadState(THREADSTATE_TERMINATE);
	sDatabase.Execute("UPDATE characters SET online = 0");
	MySQLDatabase * db = (MySQLDatabase*)dbThread;

	// wait for it to finish its work
	while(db->ThreadRunning)
	{
		Sleep(100);
	}
	sThreadMgr.RemoveThread(db);

	sLog.outString("All pending database operations cleared.\n");

#ifdef WIN32
	sLog.outString("Killing Socket Threads...");
	sSocketMgr.ShutdownThreads();
	sLog.outString("Done.");
#endif

	sWorld.SaveAllPlayers();
	sLog.outString("");

	delete LogonCommHandler::getSingletonPtr();

	sWorld.ShutdownClasses();
	sLog.outString("\nDeleting World...");
	delete World::getSingletonPtr();

	sLog.outString("Deleting Event Manager...");
	delete EventMgr::getSingletonPtr();

	sLog.outString("Terminating MySQL connections...\n");
	_StopDB();

	sLog.outString("Deleting Network Subsystem...");
	delete SocketMgr::getSingletonPtr();

	sLog.outString("Deleting Script Engine...");
	delete ScriptSystem;

	sLog.outString("\nServer shutdown completed successfully.\n");

	// close the logs
	TextLogger::Thread->Die();

#ifdef WIN32
	WSACleanup();

	// Terminate Entire Application
	//HANDLE pH = OpenProcess(PROCESS_TERMINATE, TRUE, GetCurrentProcessId());
	//TerminateProcess(pH, 0);
	//CloseHandle(pH);

#endif

	return true;
}

bool Master::_StartDB()
{
	string hostname, username, password, database;
	int port = 0;
	int type = 1;
	string lhostname, lusername, lpassword, ldatabase;
	int lport = 0;
	int ltype = 1;
	// Configure Main Database
	
	bool result = Config.MainConfig.GetString("Database", "Username", &username);
	Config.MainConfig.GetString("Database", "Password", &password);
	result = !result ? result : Config.MainConfig.GetString("Database", "Hostname", &hostname);
	result = !result ? result : Config.MainConfig.GetString("Database", "Name", &database);
	result = !result ? result : Config.MainConfig.GetInt("Database", "Port", &port);
	result = !result ? result : Config.MainConfig.GetInt("Database", "Type", &type);
	Database_Main = CreateDatabaseInterface((DatabaseType)type);

	if(result == false)
	{
		sLog.outError("sql: One or more parameters were missing from Database directive.");
		return false;
	}

	// Initialize it
	if(!sDatabase.Initialize(hostname.c_str(), (unsigned int)port, username.c_str(),
		password.c_str(), database.c_str(), Config.MainConfig.GetIntDefault("Database", "ConnectionCount", 6),
		16384))
	{
		sLog.outError("sql: Main database initialization failed. Exiting.");
		return false;
	}

	// Configure Logon Database...
	/*result = Config.MainConfig.GetString("LogonDatabase.Username", &lusername);
	result = !result ? result : Config.MainConfig.GetString("LogonDatabase.Password", &lpassword);
	result = !result ? result : Config.MainConfig.GetString("LogonDatabase.Hostname", &lhostname);
	result = !result ? result : Config.MainConfig.GetString("LogonDatabase.Name", &ldatabase);
	result = !result ? result : Config.MainConfig.GetInt("LogonDatabase.Port", &lport);
	result = !result ? result : Config.MainConfig.GetInt("LogonDatabase.Type", &ltype);
	Database_Logon = CreateDatabaseInterface((DatabaseType)ltype);

	if(result == false)
	{
		sLog.outString("sql: Logon database parameters not found. Using main database information.");
		lusername = username;
		lpassword = password;
		lhostname = hostname;
		ldatabase = database;
		lport = port;
	}

	// Initialize it
	if(!sLogonDatabase.Initialize(lhostname.c_str(), (unsigned int)lport, lusername.c_str(),
		lpassword.c_str(), ldatabase.c_str(), Config.MainConfig.GetIntDefault("LogonDatabase.ConnectionCount", 1),
		16384))
	{
		sLog.outError("sql: Logon database initialization failed. Exiting.");
		return false;
	} */  

	return true;
}

void Master::_StopDB()
{
	sDatabase.Shutdown();
	DestroyDatabaseInterface(Database_Main);
}

void Master::_HookSignals()
{
	signal(SIGINT, _OnSignal);
	signal(SIGTERM, _OnSignal);
	signal(SIGABRT, _OnSignal);
#ifdef _WIN32
	signal(SIGBREAK, _OnSignal);
#endif
}

void Master::_UnhookSignals()
{
	signal(SIGINT, 0);
	signal(SIGTERM, 0);
	signal(SIGABRT, 0);
#ifdef _WIN32
	signal(SIGBREAK, 0);
#endif

}

#ifdef WIN32

Mutex m_crashedMutex;

// Crash Handler
void OnCrash(bool Terminate)
{
	sLog.outString("Advanced crash handler initialized.");
	if(!m_crashedMutex.AttemptAcquire())
		TerminateProcess(GetCurrentProcess(), 0);

	try
	{
		if(World::getSingletonPtr() != 0 && ThreadMgr::getSingletonPtr() != 0)
		{
			sLog.outString("Waiting for all database queries to finish...");
			MySQLDatabase* dbThread = (MySQLDatabase*)sThreadMgr.GetThreadByType(THREADTYPE_DATABASE);
			if(dbThread != 0)
			{
				// end it
				dbThread->SetThreadState(THREADSTATE_TERMINATE);
				sDatabase.Execute("UPDATE characters SET online = 0");
				// wait for it to finish its work
				while(dbThread->ThreadRunning)
				{
					Sleep(100);
				}
			}
			sLog.outString("All pending database operations cleared.\n");
			sWorld.SaveAllPlayers();
			sLog.outString("Data saved.");
		}
	}
	catch(...)
	{
		sLog.outString("Threw an exception while attempting to save all data.");
	}

	sLog.outString("Closing.");
	
	// beep
	//printf("\x7");
	
	// Terminate Entire Application
	if(Terminate)
	{
		HANDLE pH = OpenProcess(PROCESS_TERMINATE, TRUE, GetCurrentProcessId());
		TerminateProcess(pH, 1);
		CloseHandle(pH);
	}
}

#endif
