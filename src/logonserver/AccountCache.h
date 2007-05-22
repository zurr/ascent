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

typedef struct
{
    uint32 AccountId;
    string Username;
    string Password;
    string GMFlags;
    uint32 AccountFlags;
	uint32 Banned;
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

#ifdef WIN32
    BAN_STATUS CalculateBanStatus(uint32 ip_address);
#else
    BAN_STATUS CalculateBanStatus(const char* ip_address);
#endif

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

        if(itr == AccountDatabase.end())    pAccount = NULL;
        else                                pAccount = &(itr->second);

        setBusy.Release();
        return pAccount;
    }
    void UpdateAccount(Account * acct, Field * field);

    void UpdateAccountLastIP(uint32 accountId, const char * lastIp);

    bool LoadAccount(string Name);    
    void ReloadAccounts(bool silent);
    void ReloadAccountsCallback();

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

        if(itr == AccountDatabase.end())    return NULL;
        else                                return &(itr->second);
    }

#ifdef WIN32
    HM_NAMESPACE::hash_map<string, Account> AccountDatabase;
#else
    std::map<string, Account> AccountDatabase;
#endif

protected:
    Mutex setBusy;
};

typedef struct
{
    string Name;
    string Address;
    uint32 Colour;
    uint32 Icon;
    uint32 TimeZone;
    float Population;
}Realm;

class AuthSocket;
class LogonCommServerSocket;

class InformationCore : public Singleton<InformationCore>
{
    map<uint32, BigNumber*>     m_sessionkeys;
    map<uint32, Realm>          m_realms;
    set<LogonCommServerSocket*> m_serverSockets;
    Mutex realmLock;

    uint32 realmhigh;
    bool usepings;

public:
    InformationCore()
    { 
        realmhigh = 0;
        usepings  = !Config.MainConfig.GetBoolDefault("DisablePings", false);
    }

    // Packets
    void          SendRealms(AuthSocket * Socket);
    
    // Sessionkey Management
    BigNumber*    GetSessionKey(uint32 account_id);
    void          DeleteSessionKey(uint32 account_id);
    void          SetSessionKey(uint32 account_id, BigNumber * key);

    // Realm management
    inline uint32 GenerateRealmID()
    {
        realmhigh++;
        return realmhigh;
    }

    void          AddRealm(uint32 realm_id, Realm * rlm);
    void          RemoveRealm(uint32 realm_id);

    inline void   AddServerSocket(LogonCommServerSocket * sock) { m_serverSockets.insert( sock ); }
    inline void   RemoveServerSocket(LogonCommServerSocket * sock) { m_serverSockets.erase( sock ); }

    void          TimeoutSockets();
};

#define sIPBanner IPBanner::getSingleton()
#define sAccountMgr AccountMgr::getSingleton()
#define sInfoCore InformationCore::getSingleton()

#endif
