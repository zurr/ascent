
#include "CConsole.h"
#include "Log.h"
#include "Master.h"
#include "../game/StdAfx.h"


createFileSingleton(CConsole);
CConsole::~CConsole()
{
	*running_link = false;
	delete _thread;
}

void CConsoleThread::run()
{
	SetThreadName("Console Interpreter");
	CThread::run();
	sCConsole._thread = this;
	delete_after_use = false;
	char i = 0;
	char cmd[96];
	bool running = true;
	sCConsole.running_link = &running;	

	while (ThreadState != THREADSTATE_TERMINATE && running)
	{
		if(ThreadState == THREADSTATE_PAUSED)
		{
			while(ThreadState == THREADSTATE_PAUSED)
			{
				Sleep(200);
			}
		}
		
		// Make sure our buffer is clean to avoid Array bounds overflow
		memset(cmd,NULL,sizeof(cmd)); 
		// Read in single line from "stdin"
		fgets(cmd, 80, stdin);

		if(ThreadState == THREADSTATE_TERMINATE)
			return;

		for( i = 0 ; i < 80 || cmd[i] != '\0' ; i++ )
		{
			if( cmd[i] =='\n' )
			{
				cmd[i]='\0';
				sCConsole.ProcessCmd(cmd);
				fflush(stdin);
				break;
			}
		}
	}
}

//------------------------------------------------------------------------------
// Protected methods:
//------------------------------------------------------------------------------
// Process one command
void CConsole::ProcessCmd(char *cmd)
{
	typedef void (CConsole::*PTranslater)(char *str);
	struct SCmd
	{
		char *name;
		PTranslater tr;
	};

	SCmd cmds[] =
	{

		{ "?", &CConsole::TranslateHelp}, { "help", &CConsole::TranslateHelp},
		{ "ver", &CConsole::TranslateVersion}, { "version", &CConsole::TranslateVersion},
		{ "uptime", &CConsole::GetUptime},
		{ "threads", &CConsole::TranslateThreads},
		{ "cancelshutdown", &CConsole::CancelShutdown },
		{ "status", &CConsole::ObjectStats },
		{ "announce", &CConsole::Announce },
		{ "wannounce", &CConsole::WideAnnounce },
		{ "saveall", &CConsole::SaveallPlayers },
		{ "quit", &CConsole::TranslateQuit}, { "exit", &CConsole::TranslateQuit}, 
	};

	if(_thread->GetThreadState() == THREADSTATE_TERMINATE)
	{
		printf("Exit confirmed. \n");
		return;
	}
	char cmd2[80];
	strcpy(cmd2, cmd);
	for(int i = 0; i < strlen(cmd); ++i)
		cmd2[i] = tolower(cmd[i]);

	for (int i = 0; i < sizeof(cmds)/sizeof(SCmd); i++)
		if (strncmp(cmd2, cmds[i].name, strlen(cmds[i].name)) == 0)
		{
			(this->*(cmds[i].tr)) (cmd + strlen(cmds[i].name));
			return;
		}

		printf("Console: Unknown console command (use \"help\" for help).\n");
}

void CConsole::CancelShutdown(char *str)
{
	printf("Shutdown aborted.\n");
	WorldPacket data(20);
	data.SetOpcode(SMSG_SERVER_MESSAGE);
	data << uint32(SERVER_MSG_SHUTDOWN_CANCELLED);
	sWorld.SendGlobalMessage(&data);
	sMaster.m_ShutdownEvent = false;
	sMaster.m_ShutdownTimer = 0;
}

void CConsole::GetUptime(char *str)
{
	uint32 count = objmgr._players.size();

	sLog.outString("Console: Server has been running for %s There are currently %d online players.",
		sWorld.GetUptimeString().c_str(), count);
}

//------------------------------------------------------------------------------
// ver[sion]
void CConsole::TranslateVersion(char *str)
{
	ProcessVersion();
}
void CConsole::ProcessVersion()
{
	sLog.outString("Console: Server %s", _FULLVERSION);
}
//------------------------------------------------------------------------------
// quit | exit
void CConsole::TranslateQuit(char *str)
{
	int delay = str != NULL ? atoi(str) : 5000;
	if(!delay)
		delay = 5000;
	else
		delay *= 1000;

	ProcessQuit(delay);
}
void CConsole::ProcessQuit(int delay)
{
	sMaster.m_ShutdownTimer = delay;
	sMaster.m_ShutdownEvent = true;
}
//------------------------------------------------------------------------------
// help | ?
void CConsole::TranslateHelp(char *str)
{
	ProcessHelp(NULL);
}
void CConsole::ProcessHelp(char *command)
{
	if (command == NULL)
	{
		sLog.outString("Console:--------help--------");
		sLog.outString("   help, ?: print this text");
		sLog.outString("   uptime: print uptime of the server");
		sLog.outString("   version, ver: print version");
		sLog.outString("   cancelshutdown: cancels the shutdown of the server");
		sLog.outString("   announce: announces a msg to the server.");
		sLog.outString("   wannounce: announces a widescreen msg to the server");
		sLog.outString("   saveall: saves all players");
		sLog.outString("   quit, exit: close program");
	}
}
//------------------------------------------------------------------------------

CConsoleThread::CConsoleThread() : CThread()
{
	ThreadType = THREADTYPE_CONSOLEINTERFACE;
}

void CConsole::TranslateThreads(char* str)
{
	std::string threads = sThreadMgr.ShowStatus();
	printf(threads.c_str());
}

CConsoleThread::~CConsoleThread()
{

}

void CConsole::ObjectStats(char *str)
{
	printf("\n");
	printf("Loaded object information:\n");
	printf("\n");
}

void CConsole::Announce(char* str)
{
	if(!str)
		return;

	char msg[500];
	sprintf(msg, "%sConsole:%s%s", "|cff00ccff", "|r", str);
	sWorld.SendWorldText(msg, 0);
}

void CConsole::WideAnnounce(char *str)
{
	if(!str)
		return;

	char msg[500];
	sprintf(msg, "%sConsole:%s%s", "|cff00ccff", "|r", str);
	sWorld.SendWorldText(msg, 0);
	sWorld.SendWorldWideScreenText(msg, 0);
}

void CConsole::SaveallPlayers(char *str)
{
	HM_NAMESPACE::hash_map<uint32, Player*>::const_iterator itr;
	uint32 stime = now();
	uint32 count = 0;
	objmgr._playerslock.AcquireReadLock();
	for (itr = objmgr._players.begin(); itr != objmgr._players.end(); itr++)
	{
		if(itr->second->GetSession())
		{
			itr->second->SaveToDB(false);
			count++;
		}
	}
	objmgr._playerslock.ReleaseReadLock();
	char msg[100];
	sprintf(msg, "Saved all %d online players in %d msec.", count, (uint32)now() - stime);
	sWorld.SendWorldText(msg);
	sWorld.SendWorldWideScreenText(msg);
}
