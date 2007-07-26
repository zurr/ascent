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

#include "RStdAfx.h"
#include "../game/ThreadMgr.h"
#include "../shared/svn_revision.h"

TextLogger * Crash_Log;
MySQLDatabase * Database_Character;
MySQLDatabase * Database_World;

int main(int argc, char *argv[])
{
	new ThreadMgr;
	launch_thread(new TextLoggerThread);

	sLog.Init(-1, -1);
	sLog.outString("TexT");
	/* Initialize global timestamp */
	UNIXTIME = time(NULL);

    /* Print Banner */
	Log.Notice("Server", "==============================================================");
	Log.Notice("Server", "| Codename Ascent Cluster - Realm Server                     |");
	Log.Notice("Server", "| Version 0.1, Revision %04u                                 |", g_getRevision());
	Log.Notice("Server", "==============================================================");
	Log.Line();

	Database_Character = new MySQLDatabase();
	Database_World = new MySQLDatabase();
	Log.Success("Database", "Interface Created.");

	new ClusterMgr;
	new ClientMgr;
	Log.Line();
	Log.Notice("Database", "Connecting to database...");
	if(Database_Character->Initialize("localhost", 3306, "moocow", "moo", "antrix-character", 1, 1000) &&
		Database_World->Initialize("localhost", 3306, "moocow", "moo", "antrix-world", 1, 1000))
	{
		Log.Success("Database", "Connections established.");
	}
	else
	{
		Log.Error("Database", "One or more errors occured while connecting to databases.");
		exit(-1);
	}

	Log.Line();
	new CharClassStore("DBC/ChrClasses.dbc");
	new CharRaceStore("DBC/ChrRaces.dbc");
	Log.Success("Storage", "DBC Files Loaded...");
	Storage_Load();

	Log.Line();
	//CreateSocketEngine();
	//sSocketEngine.SpawnThreads();
	new SocketMgr;
	new SocketGarbageCollector;
	sSocketMgr.SpawnWorkerThreads();

	Log.Success("Network", "Network Subsystem Started.");

	Log.Notice("Network", "Opening Client Port...");
	ListenSocket<WorldSocket> * wsl = new ListenSocket<WorldSocket>("0.0.0.0", 8129);
	bool lsc = wsl->IsOpen();
	
	Log.Notice("Network", "Opening Server Port...");
	ListenSocket<WSSocket> * isl = new ListenSocket<WSSocket>("0.0.0.0", 10010);
	bool ssc = isl->IsOpen();

	Config.MainConfig.SetSource("antrix.conf");
	Config.RealmConfig.SetSource("realms.conf");

	if(!lsc || !ssc)
	{
		Log.Error("Network", "Could not open one of the sockets.");
		return 1;
	}

	/* connect to LS */
	new LogonCommHandler;
	sLogonCommHandler.Startup();

	/* main loop */
	for(;;)
	{
        sLogonCommHandler.UpdateSockets();
		wsl->Update();
		isl->Update();
		sClientMgr.Update();
		sClusterMgr.Update();
		Sleep(50);
	}

}

void OnCrash(bool Terminate)
{

}