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

#include "StdAfx.h"
initialiseSingleton(LogonCommHandler);

LogonCommHandler::LogonCommHandler()
{
	idhigh = 1;
	next_request = 1;
	pings = !Config.MainConfig.GetBoolDefault("DisablePings", false);
	string logon_pass = Config.MainConfig.GetStringDefault("LogonServer.RemotePassword", "r3m0t3");
	
	// sha1 hash it
	Sha1Hash hash;
	hash.UpdateData(logon_pass);
	hash.Finalize();
	memcpy(sql_passhash, hash.GetDigest(), 20);
}

LogonCommHandler::~LogonCommHandler()
{
	for(set<LogonServer*>::iterator i = servers.begin(); i != servers.end(); ++i)
		delete (*i);

	for(set<Realm*>::iterator i = realms.begin(); i != realms.end(); ++i)
		delete (*i);
}

LogonCommClientSocket * LogonCommHandler::ConnectToLogon(string Address, uint32 Port)
{
	// Create the socket.
	LogonCommClientSocket * conn = new LogonCommClientSocket(0);
	if(!conn->Connect(Address.c_str(), Port))
	{
		delete conn;
		return 0;
	}
	

	// Send the initial ping
	conn->SendPing();

	return conn;
}

void LogonCommHandler::RequestAddition(LogonCommClientSocket * Socket)
{
	set<Realm*>::iterator itr = realms.begin();
	WorldPacket data(RCMSG_REGISTER_REALM, 100);
	for(; itr != realms.end(); ++itr)
	{
		data.clear();

		// Add realm to the packet
		Realm * realm = *itr;
		data << realm->Name;
		data << realm->Address;
		data << realm->Colour;
		data << realm->Icon;
		data << realm->TimeZone;
		data << realm->Population;
		Socket->SendPacket(&data);
	}
}

void LogonCommHandler::Startup()
{
	// Try to connect to all logons.
	sLog.outColor(TNORMAL, "\n >> loading realms and logon server definitions... ");
	LoadRealmConfiguration();

	sLog.outColor(TNORMAL, " >> attempting to connect to all logon servers... \n");
	for(set<LogonServer*>::iterator itr = servers.begin(); itr != servers.end(); ++itr)
		Connect(*itr);

	sLog.outColor(TNORMAL, "\n");
}

void LogonCommHandler::Connect(LogonServer * server)
{
	sLog.outColor(TNORMAL, "	>> connecting to `%s` on `%s:%u`...", server->Name.c_str(), server->Address.c_str(), server->Port);
	server->RetryTime = time(NULL) + 10;
	server->Registered = false;
	LogonCommClientSocket * conn = ConnectToLogon(server->Address, server->Port);
	logons[server] = conn;
	if(conn == 0)
	{
		sLog.outColor(TRED, " fail!\n	   server connection failed. I will try again later.");
		sLog.outColor(TNORMAL, "\n");
		return;
	}
	sLog.outColor(TGREEN, " ok!\n");
	sLog.outColor(TNORMAL, "	   >> registering realms... ");
	conn->_id = server->ID;

	RequestAddition(conn);

	uint32 st = time(NULL) + 10;

	// Wait for register ACK
	while(server->Registered == false)
	{
		// Don't wait more than.. like 10 seconds for a registration
		if(time(NULL) >= st)
		{
			sLog.outColor(TYELLOW, "timeout.");
			break;
		}
		Sleep(50);
	}

	if(!server->Registered)
		return;

	// Wait for all realms to register
	Sleep(200);

	sLog.outColor(TNORMAL, "\n	   >> ping test: ");
	sLog.outColor(TYELLOW, "%ums", conn->latency);
	sLog.outColor(TNORMAL, "\n");
}

void LogonCommHandler::AdditionAck(uint32 ID, uint32 ServID)
{
	map<LogonServer*, LogonCommClientSocket*>::iterator itr = logons.begin();
	for(; itr != logons.end(); ++itr)
	{
		if(itr->first->ID == ID)
		{
			itr->first->ServerID = ServID;
			itr->first->Registered = true;
			return;
		}
	}
}

void LogonCommHandler::UpdateSockets()
{
	mapLock.Acquire();

	map<LogonServer*, LogonCommClientSocket*>::iterator itr = logons.begin();
	LogonCommClientSocket * cs;
	uint32 t = time(NULL);
	for(; itr != logons.end(); ++itr)
	{
		cs = itr->second;
		if(cs != 0)
		{
			if(pings && (t - cs->last_ping) > 10)
			{
				if(!cs->gotpong)
				{
					// flag for dc
					cs->_id = 0;
					sLog.outString(" >> realm id %u connection was dropped unexpectedly (ping timeout). reconnecting next loop.", itr->first->ID);
					cs->Disconnect();
					itr->second = 0;
					continue;
				}
				else
				{
					cs->SendPing();
				}
			}
		}
		else
		{
			// check retry time
			if(t >= itr->first->RetryTime)
			{
				Connect(itr->first);
			}
		}
	}
	mapLock.Release();
}

void LogonCommHandler::ConnectionDropped(uint32 ID)
{
	mapLock.Acquire();
	map<LogonServer*, LogonCommClientSocket*>::iterator itr = logons.begin();
	for(; itr != logons.end(); ++itr)
	{
		if(itr->first->ID == ID && itr->second != 0)
		{
			sLog.outColor(TNORMAL, " >> realm id %u connection was dropped unexpectedly. reconnecting next loop.", ID);
			sLog.outColor(TNORMAL, "\n");
			itr->second = 0;
			break;
		}
	}
	mapLock.Release();
}

uint32 LogonCommHandler::ClientConnected(string AccountName, WorldSocket * Socket)
{
	uint32 request_id = next_request++;
	sLog.outDebug ( " >> sending request for account information: `%s` (request %u).", AccountName.c_str(), request_id);
  //  sLog.outColor(TNORMAL, "\n");
	
	// Send request packet to server.
	map<LogonServer*, LogonCommClientSocket*>::iterator itr = logons.begin();
	if(logons.size() == 0 || itr->second == 0)
	{
		// No valid logonserver is connected.
		return (uint32)-1;
	}
	pendingLock.Acquire();

	WorldPacket data(RCMSG_REQUEST_SESSION, 100);
	data << request_id;
	data << AccountName;
	itr->second->SendPacket(&data);

	pending_logons[request_id] = Socket;
	pendingLock.Release();

	return request_id;
}

void LogonCommHandler::UnauthedSocketClose(uint32 id)
{
	pendingLock.Acquire();
	pending_logons.erase(id);
	pendingLock.Release();
}

void LogonCommHandler::RemoveUnauthedSocket(uint32 id)
{
	pending_logons.erase(id);
}

void LogonCommHandler::LogonDatabaseSQLExecute(const char* str, ...)
{
	va_list ap;
	va_start(ap, str);
	char query[1024];
	vsprintf(query, str, ap);
	va_end(ap);
	WorldPacket data(RCMSG_SQL_EXECUTE, strlen(query)+ 17);
	data.append(sql_passhash, 16);
	data << query;
	
	// Send request packet to server.
	map<LogonServer*, LogonCommClientSocket*>::iterator itr = logons.begin();
	if(logons.size() == 0 || itr->second == 0)
	{
		// No valid logonserver is connected.
		return;
	}
	itr->second->SendPacket(&data);
}

void LogonCommHandler::LogonDatabaseReloadAccounts()
{
	WorldPacket data(RCMSG_RELOAD_ACCOUNTS, 16);
	data.append(sql_passhash, 16);

	map<LogonServer*, LogonCommClientSocket*>::iterator itr = logons.begin();
	if(logons.size() == 0 || itr->second == 0)
	{
		// No valid logonserver is connected.
		return;
	}
	itr->second->SendPacket(&data);
}

void LogonCommHandler::LoadRealmConfiguration()
{
	uint32 logoncount = Config.RealmConfig.GetIntDefault("LogonServerCount", 0);
	if(logoncount == 0)
	{
		sLog.outColor(TRED, "\n   >> no logon servers found. this server will not be online anywhere!\n");
	}
	else
	{
		for(uint32 i = 1; i < logoncount+1; ++i)
		{
			LogonServer * ls = new LogonServer;
			ls->ID = idhigh++;
			ls->Name = Config.RealmConfig.GetStringVA("UnkLogon", "LogonServer%u.Name", i);
			ls->Address = Config.RealmConfig.GetStringVA("127.0.0.1", "LogonServer%u.Address", i);
			ls->Port = Config.RealmConfig.GetIntVA(8093, "LogonServer%u.Port", i);
			servers.insert(ls);
		}
		sLog.outColor(TYELLOW, "%u servers, ", logoncount);
	}

	uint32 realmcount = Config.RealmConfig.GetIntDefault("RealmCount", 1);
	if(realmcount == 0)
	{
		sLog.outColor(TRED, "\n   >> no realms found. this server will not be online anywhere!\n");
	}
	else
	{
		for(uint32 i = 1; i < realmcount+1; ++i)
		{
			Realm * realm = new Realm;
			realm->Name = Config.RealmConfig.GetStringVA("SomeRealm", "Realm%u.Name", i);
			realm->Address = Config.RealmConfig.GetStringVA("127.0.0.1:8129", "Realm%u.Address", i);
			realm->Colour = Config.RealmConfig.GetIntVA(1, "Realm%u.Colour", i);
			realm->TimeZone = Config.RealmConfig.GetIntVA(1, "Realm%u.TimeZone", i);
			realm->Population = Config.RealmConfig.GetFloatVA(0, "Realm%u.Population", i);
			string rt = Config.RealmConfig.GetStringVA("Normal", "Realm%u.Icon", i);
			uint32 type;

			// process realm type
			if(!stricmp(rt.c_str(), "pvp"))
				type = REALMTYPE_PVP;
			else if(!stricmp(rt.c_str(), "rp"))
				type = REALMTYPE_RP;
			else if(!stricmp(rt.c_str(), "rppvp"))
				type = REALMTYPE_RPPVP;
			else
				type = REALMTYPE_NORMAL;

			_realmType = type;

			realm->Icon = type;
			realms.insert(realm);
		}
		sLog.outColor(TBLUE, "%u realms.\n", realmcount);
	}
}
