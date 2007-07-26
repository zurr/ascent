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

#ifndef LOGONCOMMHANDLER_H
#define LOGONCOMMHANDLER_H

typedef struct
{
	uint32 ID;
	string Name;
	string Address;
	uint32 Port;
	uint32 ServerID;
	uint32 RetryTime;
	bool   Registered;
}LogonServer;

typedef struct
{
	string Name;
	string Address;
	uint32 Colour;
	uint32 Icon;
	uint32 TimeZone;
	float Population;
}Realm;

enum RealmType
{
	REALMTYPE_NORMAL = 0,
	REALMTYPE_PVP	= 4,
	REALMTYPE_RP	 = 6,
	REALMTYPE_RPPVP  = 8,
};

class SocketLoadBalancer;

class LogonCommHandler : public Singleton<LogonCommHandler>
{
	map<LogonServer*, LogonCommClientSocket*> logons;
	map<uint32, WorldSocket*> pending_logons;
	set<Realm*> realms;
	set<LogonServer*> servers;
	uint32 idhigh;
	uint32 next_request;
	Mutex mapLock;
	Mutex pendingLock;
	bool pings;
	uint32 _realmType;

public:
	uint8 sql_passhash[20];

	LogonCommHandler();
	~LogonCommHandler();

	LogonCommClientSocket * ConnectToLogon(string Address, uint32 Port);
	void UpdateAccountCount(uint32 account_id, uint8 add);
	void RequestAddition(LogonCommClientSocket * Socket);
	void CheckAllServers();
	void Startup();
	void ConnectionDropped(uint32 ID);
	void AdditionAck(uint32 ID, uint32 ServID);
	void UpdateSockets();
	void Connect(LogonServer * server);
	void LogonDatabaseSQLExecute(const char* str, ...);
	void LogonDatabaseReloadAccounts();

	void LoadRealmConfiguration();
	void AddServer(string Name, string Address, uint32 Port);

	inline uint32 GetRealmType() { return _realmType; }
	void SetRealmType(uint32 type) { _realmType = type; }

	/////////////////////////////
	// Worldsocket stuff
	///////

	uint32 ClientConnected(string AccountName, WorldSocket * Socket);
	void UnauthedSocketClose(uint32 id);
	void RemoveUnauthedSocket(uint32 id);
	inline WorldSocket* GetSocketByRequest(uint32 id)
	{
		//pendingLock.Acquire();

		WorldSocket * sock;
		map<uint32, WorldSocket*>::iterator itr = pending_logons.find(id);
		sock = (itr == pending_logons.end()) ? 0 : itr->second;

		//pendingLock.Release();
		return sock;
	}
	inline Mutex & GetPendingLock() { return pendingLock; }		
};

#define sLogonCommHandler LogonCommHandler::getSingleton()

#endif

