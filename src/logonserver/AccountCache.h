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

#ifndef __ACCOUNTCACHE_H
#define __ACCOUNTCACHE_H

#include "../shared/Common.h"
#include "../shared/Database/DatabaseEnv.h"

struct Realm;
typedef struct
{
	uint32 AccountId;
	string Username;
	string Password;
	string GMFlags;
	uint32 AccountFlags;
	uint32 Banned;
	uint8 SrpHash[20];
	Realm * LoggedInRealm;
	BigNumber * SessionKey;

	void SetSessionKey(BigNumber * s)
	{
		if(SessionKey)
			delete SessionKey;

		SessionKey = s;
	}

	void ClearSessionKey()
	{
		if(SessionKey)
			delete SessionKey;
		SessionKey = 0;
	}

} Account;

typedef struct 
{
	union
	{
		struct ipfull { uint8 b1, b2, b3, b4; } full;
		uint32 asbytes;
	} ip;
	uint32 ban_expire_time;
} IPBan;

enum BAN_STATUS
{
	BAN_STATUS_NOT_BANNED = 0,
	BAN_STATUS_TIME_LEFT_ON_BAN = 1,
	BAN_STATUS_PERMANANT_BAN = 2,
};

class IPBanner : public Singleton< IPBanner >
{
public:
	void Load();
	void Reload();
	void Remove(set<IPBan*>::iterator ban);

	BAN_STATUS CalculateBanStatus(in_addr ip_address);

protected:
	Mutex setBusy;
	set<IPBan*> banList;
};

class AccountMgr : public Singleton < AccountMgr >
{
public:
	void AddAccount(Field* field);

	inline Account* GetAccount(string Name)
	{
		setBusy.Acquire();
		Account * pAccount = NULL;
		// this should already be uppercase!
#ifdef WIN32
		HM_NAMESPACE::hash_map<string, Account>::iterator itr = AccountDatabase.find(Name);
#else
		map<string, Account>::iterator itr = AccountDatabase.find(Name);
#endif

		if(itr == AccountDatabase.end())	pAccount = NULL;
		else								pAccount = &(itr->second);

		setBusy.Release();
		return pAccount;
	}

	void UpdateAccount(Account * acct, Field * field);
	bool LoadAccount(string Name);	
	void ReloadAccounts(bool silent);
	void ReloadAccountsCallback();
	void RemoveReferencesTo(Realm * realm);

	inline uint32 GetCount() { return AccountDatabase.size(); }

private:
	inline Account* __GetAccount(string Name)
	{
		// this should already be uppercase!
#ifdef WIN32
		HM_NAMESPACE::hash_map<string, Account>::iterator itr = AccountDatabase.find(Name);
#else
		map<string, Account>::iterator itr = AccountDatabase.find(Name);
#endif

		if(itr == AccountDatabase.end())	return NULL;
		else								return &(itr->second);
	}

#ifdef WIN32
	HM_NAMESPACE::hash_map<string, Account> AccountDatabase;
#else
	std::map<string, Account> AccountDatabase;
#endif

protected:
	Mutex setBusy;
};

struct Realm
{
	uint32 ID;
	string Name;
	string Address;
	uint32 Colour;
	uint32 Icon;
	uint32 TimeZone;
	float Population;
	HM_NAMESPACE::hash_map<uint32, uint8> CharacterMap;
};

class AuthSocket;
class LogonCommServerSocket;

class InformationCore : public Singleton<InformationCore>
{
	map<uint32, Realm*>		  m_realms;
	map<Account*, uint32>     m_deletionQueue;
	Mutex m_deletionQueueLock;
	set<LogonCommServerSocket*> m_serverSockets;
	Mutex serverSocketLock;
	Mutex realmLock;

	uint32 realmhigh;
	bool usepings;

public:
	inline Mutex & getServerSocketLock() { return serverSocketLock; }
	inline Mutex & getRealmLock() { return realmLock; }

	InformationCore()
	{ 
		realmhigh = 0;
		usepings  = !Config.MainConfig.GetBoolDefault("LogonServer", "DisablePings", false);
	}

	// Packets
	void		  SendRealms(AuthSocket * Socket);
	LogonCommServerSocket * GetSocketForRealm(uint32 RealmId);

	// Realm management
	inline uint32 GenerateRealmID()
	{
		realmhigh++;
		return realmhigh;
	}

	Realm*		  AddRealm(uint32 realm_id, Realm * rlm);
	Realm*        GetRealm(uint32 realm_id);
	void		  RemoveRealm(uint32 realm_id);

	inline void   AddServerSocket(LogonCommServerSocket * sock) { serverSocketLock.Acquire(); m_serverSockets.insert( sock ); serverSocketLock.Release(); }
	inline void   RemoveServerSocket(LogonCommServerSocket * sock) { serverSocketLock.Acquire(); m_serverSockets.erase( sock ); serverSocketLock.Release(); }

	void		  TimeoutSockets();
	void          TimeoutSessionKeys();

	void AddKey(Account * a)
	{
		m_deletionQueueLock.Acquire();
		m_deletionQueue[a] = time(NULL) + 30;
		m_deletionQueueLock.Release();
	}
};

#define sIPBanner IPBanner::getSingleton()
#define sAccountMgr AccountMgr::getSingleton()
#define sInfoCore InformationCore::getSingleton()

#endif
