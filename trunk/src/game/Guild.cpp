/*
 * Ascent MMORPG Server
 * Copyright (C) 2005-2007 Ascent Team <http://www.ascentemu.com/>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "StdAfx.h"

static const char * NULLMSG = "";
Guild::Guild()
{
	m_commandLogging=true;
	m_guildId=0;
	m_guildLeader=0;
	m_guildName="Goose";
	m_guildInfo=NULL;
	m_motd=NULL;
	m_backgroundColor=0;
	m_emblemColor=0;
	m_emblemStyle=0;
	m_borderColor=0;
	m_borderStyle=0;
	m_creationTimeStamp=0;
	m_bankBalance =0;
	m_bankTabCount=0;
	creationDay=creationMonth=creationYear=0;
}

Guild::~Guild()
{
	for(GuildMemberMap::iterator itr = m_members.begin(); itr != m_members.end(); ++itr)
	{
		if(itr->second->szOfficerNote)
			free((void*)itr->second->szOfficerNote);
		if(itr->second->szPublicNote)
			free((void*)itr->second->szPublicNote);
		delete itr->second;
	}

	for(GuildRankVector::iterator itr = m_ranks.begin(); itr != m_ranks.end(); ++itr)
	{
		if((*itr) != NULL)
		{
			free((*itr)->szRankName);
			delete (*itr);
		}
	}

	for(GuildLogList::iterator itr = m_log.begin(); itr != m_log.end(); ++itr)
	{
		delete (*itr);
	}

	for(GuildBankTabVector::iterator itr = m_bankTabs.begin(); itr != m_bankTabs.end(); ++itr)
	{
		for(uint32 i = 0; i < MAX_GUILD_BANK_SLOTS; ++i)
			if((*itr)->pSlots[i] != NULL)
				delete (*itr)->pSlots[i];

		delete (*itr);
	}
}

void Guild::SendGuildCommandResult(WorldSession * pClient, uint32 iCmd, const char * szMsg, uint32 iType)
{
	WorldPacket data(SMSG_GUILD_COMMAND_RESULT, 100);
	data << iCmd << szMsg << iType;
	pClient->SendPacket(&data);
}

GuildRank * Guild::FindLowestRank()
{
	GuildRank * r= NULL;
	for(GuildRankVector::iterator itr = m_ranks.begin(); itr != m_ranks.end(); ++itr)
	{
		if((*itr) != NULL)
		{
			if((*itr)->iId != 0 && (!r || (*itr)->iId > r->iId))
				r = *itr;
		}
	}

	return r;
}

GuildRank * Guild::FindHighestRank()
{
	GuildRank * r= NULL;
	for(GuildRankVector::iterator itr = m_ranks.begin(); itr != m_ranks.end(); ++itr)
	{
		if((*itr) != NULL)
		{
			if((*itr)->iId != 0 && (!r || (*itr)->iId < r->iId))
				r = *itr;
		}
	}

	return r;
}

GuildRank * Guild::FindNextLowestRank(GuildRank * r)
{
	GuildRank * rr= r;
	for(GuildRankVector::iterator itr = m_ranks.begin(); itr != m_ranks.end(); ++itr)
	{
		if((*itr) != NULL)
		{
			if((*itr)->iId != 0 && ((*itr)->iId > rr->iId))
				rr = *itr;
		}
	}
	if(rr=r)
		rr=NULL;

	return rr;
}


bool GuildRank::CanPerformCommand(uint32 t)
{
	return ((iRights & t) >0 ? true : false);
}

bool GuildRank::CanPerformBankCommand(uint32 t, uint32 tab)
{
	return ((iTabPermissions[tab].iFlags & t) >0 ? true : false);
}

void Guild::LogGuildEvent(uint8 iEvent, uint8 iStringCount, ...)
{
	if(!m_commandLogging)
		return;

	va_list ap;
	char * strs[4] = {NULL,NULL,NULL,NULL};

	va_start(ap, iStringCount);
	ASSERT(iStringCount <= 4);

	WorldPacket data(SMSG_GUILD_EVENT, 100);
	uint32 i;
	data << iEvent;
	data << iStringCount;

	for(i = 0; i < iStringCount; ++i)
	{
		strs[i] = va_arg(ap, char*);
		data << strs[i];
	}

	va_end(ap);
	SendPacket(&data);
}

void Guild::AddGuildLogEntry(uint8 iEvent, uint8 iParamCount, ...)
{
	if(!m_commandLogging)
		return;

	va_list ap;
	uint32 i;
	GuildLogEvent * ev;

	va_start(ap, iParamCount);
	ASSERT(iParamCount<=3);

	ev = new GuildLogEvent;
	ev->iLogId = GenerateGuildLogEventId();
	ev->iEvent = iEvent;
	ev->iTimeStamp = (uint32)UNIXTIME;

	for(i = 0; i < iParamCount; ++i)
		ev->iEventData[i] = va_arg(ap, uint32);

	for(; i < 3; ++i)
		ev->iEventData[i] = 0;

	CharacterDatabase.Execute("INSERT INTO guild_logs VALUES(%u, %u, %u, %u, %u, %u, %u)",
		m_guildId, ev->iLogId, ev->iTimeStamp, ev->iEvent, ev->iEventData[0], ev->iEventData[1], ev->iEventData[2]);

	m_lock.Acquire();
	if(m_log.size() >= 250)
	{
		// limit it to 250 events.
		// delete the first (oldest) event.
		CharacterDatabase.Execute("DELETE FROM guild_logs WHERE log_id = %u AND guildid = %u", (*(m_log.begin()))->iLogId, m_guildId);
		delete *(m_log.begin());
		m_log.pop_front();
	}

	m_log.push_back(ev);
	m_lock.Release();
}

void Guild::SendPacket(WorldPacket * data)
{
	m_lock.Acquire();
	for(GuildMemberMap::iterator itr = m_members.begin(); itr != m_members.end(); ++itr)
	{
		if(itr->first->m_loggedInPlayer != NULL && itr->first->m_loggedInPlayer->GetSession())
			itr->first->m_loggedInPlayer->GetSession()->SendPacket(data);
	}
	m_lock.Release();
}

GuildRank * Guild::CreateGuildRank(const char * szRankName, uint32 iPermissions, bool bFullGuildBankPermissions)
{
	// find a free id
	uint32 i, j;
	m_lock.Acquire();
	for(i = 0; i < MAX_GUILD_RANKS; ++i)
	{
		if(m_ranks[i] == NULL)
		{
			GuildRank * r = new GuildRank;
			memset(r, 0, sizeof(GuildRank));
			r->iId = i;
			r->iRights=iPermissions;
			r->szRankName = strdup(szRankName);

			if(bFullGuildBankPermissions)
			{
				for(j = 0; j < MAX_GUILD_BANK_TABS; ++j)
				{
					r->iTabPermissions[j].iFlags = 3;			// this is both use tab and withdraw
					r->iTabPermissions[j].iStacksPerDay = 0;	// 0 = unlimited
				}
			}
			m_ranks[i] = r;
			m_lock.Release();

			// save the rank into the database
			CharacterDatabase.Execute("REPLACE INTO guild_ranks VALUES(%u, %u, \"%s\", %u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u)",
				m_guildId, i, CharacterDatabase.EscapeString(string(szRankName)).c_str(),
				r->iRights, r->iGoldLimitPerDay,
				r->iTabPermissions[0].iFlags, r->iTabPermissions[0].iStacksPerDay,
				r->iTabPermissions[1].iFlags, r->iTabPermissions[1].iStacksPerDay,
				r->iTabPermissions[2].iFlags, r->iTabPermissions[2].iStacksPerDay,
				r->iTabPermissions[3].iFlags, r->iTabPermissions[3].iStacksPerDay,
				r->iTabPermissions[4].iFlags, r->iTabPermissions[4].iStacksPerDay,
				r->iTabPermissions[5].iFlags, r->iTabPermissions[5].iStacksPerDay);

			return r;
		}
	}
	m_lock.Release();
	return NULL;
}

// creating from a charter
void Guild::CreateFromCharter(Charter * pCharter, WorldSession * pTurnIn)
{
	uint32 i;
	m_lock.Acquire();

	m_guildId = objmgr.GenerateGuildId();
	m_guildName = strdup(pCharter->GuildName.c_str());
	m_guildLeader = pCharter->LeaderGuid;
	m_ranks.resize(MAX_GUILD_RANKS, NULL);

	// create the guild in the database
	CreateInDB();

	// rest of the fields have been nulled out, create some default ranks.
	GuildRank * leaderRank = CreateGuildRank("Guild Master", GR_RIGHT_ALL, true);
	CreateGuildRank("Officer", GR_RIGHT_ALL, false);
	CreateGuildRank("Veteran", GR_RIGHT_DEFAULT, false);
	CreateGuildRank("Member", GR_RIGHT_DEFAULT, false);
	GuildRank * defRank = CreateGuildRank("Initiate", GR_RIGHT_DEFAULT, false);

	// turn off command logging, we don't wanna spam the logs
	m_commandLogging = false;

	// add the leader to the guild
	AddGuildMember(pTurnIn->GetPlayer()->m_playerInfo, NULL, leaderRank->iId);

	// add all the other people
	for(i = 0; i < pCharter->SignatureCount; ++i)
	{
		PlayerInfo * pi = objmgr.GetPlayerInfo(pCharter->Signatures[i]);
		if(pi)
			AddGuildMember(pi, NULL, defRank->iId);
	}

	// re-enable command logging
	m_commandLogging = true;

	m_lock.Release();
}

void Guild::PromoteGuildMember(PlayerInfo * pMember, WorldSession * pClient)
{
	if(pClient->GetPlayer()->m_playerInfo->guild != this || pMember->guild != this)
		return;

	if(!pClient->GetPlayer()->m_playerInfo->guildRank->CanPerformCommand(GR_RIGHT_PROMOTE))
	{
		SendGuildCommandResult(pClient, GUILD_PROMOTE_S, "", GUILD_PERMISSIONS);
		return;
	}

	// need a proper command result for this
	if(pMember->guildRank->iId == 1)
	{
		pClient->SystemMessage("You cannot promote this member any further.");
		return;
	}

	// find the next highest rank
	uint32 nh = 999999;
	GuildRank * newRank = NULL;

	m_lock.Acquire();
	for(GuildRankVector::iterator vtr = m_ranks.begin(); vtr != m_ranks.end(); ++vtr) 
	{
		if((*vtr) && ((*vtr)->iId < nh || !newRank) && (*vtr)->iId != 0)		// cannot promote to guildmaster
		{
			newRank = (*vtr);
			nh = newRank->iId;
		}
	}

	if(newRank==NULL)
	{
		m_lock.Release();
		pClient->SystemMessage("Could not find a rank to promote this member to.");
		return;
	}

	GuildMemberMap::iterator itr = m_members.find(pMember);
	if(itr == m_members.end())
	{
		// shouldnt happen
		m_lock.Release();
		return;
	}

	itr->second->pRank = newRank;

	// log it
	LogGuildEvent(GUILD_EVENT_PROMOTION, 3, pClient->GetPlayer()->GetName(), pMember->name, newRank->szRankName);
	AddGuildLogEntry(GUILD_LOG_EVENT_PROMOTION, 2, pClient->GetPlayer()->GetGUIDLow(), pMember->guid, newRank->iId);

	// update in the database
	CharacterDatabase.Execute("UPDATE guild_data SET guildRank = %u WHERE playerid = %u AND guildid = %u", newRank->iId, pMember->guid, m_guildId);

	// if the player is online, update his guildrank
	if(pMember->m_loggedInPlayer)
		pMember->m_loggedInPlayer->SetGuildRank(newRank->iId);

	// release lock
	m_lock.Release();
}

void Guild::DemoteGuildMember(PlayerInfo * pMember, WorldSession * pClient)
{
	if(pClient->GetPlayer()->m_playerInfo->guild != this || pMember->guild != this)
		return;

	if(!pClient->GetPlayer()->m_playerInfo->guildRank->CanPerformCommand(GR_RIGHT_DEMOTE))
	{
		SendGuildCommandResult(pClient, GUILD_PROMOTE_S, "", GUILD_PERMISSIONS);
		return;
	}

	// find the next highest rank
	uint32 nh = 0;
	GuildRank * newRank = NULL;

	m_lock.Acquire();
	for(GuildRankVector::iterator vtr = m_ranks.begin(); vtr != m_ranks.end(); ++vtr) 
	{
		if((*vtr) && (*vtr)->iId > nh || !newRank)
		{
			newRank = (*vtr);
			nh = newRank->iId;
		}
	}

	if(newRank==NULL)
	{
		m_lock.Release();
		pClient->SystemMessage("Could not find a rank to demote this member to.");
		return;
	}

	GuildMemberMap::iterator itr = m_members.find(pMember);
	if(itr == m_members.end())
	{
		// shouldnt happen
		m_lock.Release();
		return;
	}

	itr->second->pRank = newRank;

	// log it
	LogGuildEvent(GUILD_EVENT_DEMOTION, 3, pClient->GetPlayer()->GetName(), pMember->name, newRank->szRankName);
	AddGuildLogEntry(GUILD_LOG_EVENT_DEMOTION, 2, pClient->GetPlayer()->GetGUIDLow(), pMember->guid, newRank->iId);

	// update in the database
	CharacterDatabase.Execute("UPDATE guild_data SET guildRank = %u WHERE playerid = %u AND guildid = %u", newRank->iId, pMember->guid, m_guildId);

	// if the player is online, update his guildrank
	if(pMember->m_loggedInPlayer)
		pMember->m_loggedInPlayer->SetGuildRank(newRank->iId);

	// release lock
	m_lock.Release();
}

bool Guild::LoadFromDB(Field * f)
{
	m_guildId = f[0].GetUInt32();
	m_guildName = strdup(f[1].GetString());
	m_guildLeader = f[2].GetUInt32();
	m_emblemStyle = f[3].GetUInt32();
	m_emblemColor = f[4].GetUInt32();
	m_borderStyle = f[5].GetUInt32();
	m_borderColor = f[6].GetUInt32();
	m_backgroundColor = f[7].GetUInt32();
	m_guildInfo = strlen(f[8].GetString()) ? strdup(f[8].GetString()) : NULL;
	m_motd = strlen(f[9].GetString()) ? strdup(f[9].GetString()) : NULL;
	m_creationTimeStamp = f[10].GetUInt32();
	m_bankTabCount = f[11].GetUInt32();
	m_bankBalance = f[12].GetUInt32();

	// load ranks
	uint32 j;
	QueryResult * result = CharacterDatabase.Query("SELECT * FROM guild_ranks WHERE guildId = %u ORDER BY rankId ASC", m_guildId);
	if(result==NULL)
		return false;

	m_ranks.resize(MAX_GUILD_RANKS, NULL);
	uint32 sid = 0;

	do 
	{
		GuildRank * r = new GuildRank;
		Field * f2 = result->Fetch();
		r->iId = f2[1].GetUInt32();
		if(r->iId!=sid)
		{
			Log.Notice("Guild", "Renaming rank %u of guild %s to %u.", r->iId, m_guildName, sid);
			CharacterDatabase.Execute("UPDATE guild_ranks SET rankId = %u WHERE guildId = %u AND rankName = \"%s\"", r->iId,
				m_guildId, CharacterDatabase.EscapeString(string(f2[2].GetString())).c_str());

			r->iId=sid;
		}
		sid++;
		r->szRankName = strdup(f2[2].GetString());
		r->iRights = f2[3].GetUInt32();
		r->iGoldLimitPerDay = f2[4].GetUInt32();
		
		for(j = 0; j < MAX_GUILD_BANK_TABS; ++j)
		{
			r->iTabPermissions[j].iFlags = f2[5+(j*2)].GetUInt32();
			r->iTabPermissions[j].iFlags = f2[6+(j*2)].GetUInt32();
		}

		//m_ranks.push_back(r);
		ASSERT(m_ranks[r->iId] == NULL);
		m_ranks[r->iId] = r;

	} while(result->NextRow());
	delete result;

	// load members
	result = CharacterDatabase.Query("SELECT * FROM guild_data WHERE guildid = %u", m_guildId);
	if(result==NULL)
		return false;

	do 
	{
		Field * f3 = result->Fetch();
		GuildMember * gm = new GuildMember;
		gm->pPlayer = objmgr.GetPlayerInfo(f3[1].GetUInt32());
		if(gm->pPlayer == NULL)
		{
			delete gm;
			continue;
		}

		if(f3[2].GetUInt32() >= MAX_GUILD_RANKS || m_ranks[f3[2].GetUInt32()] == NULL) 
		{
			delete gm;
			continue;
		}

		gm->pRank = m_ranks[f3[2].GetUInt32()];
		if(gm->pRank==NULL)
			gm->pRank=FindLowestRank();
		gm->pPlayer->guild=this;
		gm->pPlayer->guildRank=gm->pRank;
		gm->pPlayer->guildMember=gm;

		if(strlen(f3[3].GetString()))
			gm->szPublicNote = strdup(f3[3].GetString());
		else
			gm->szPublicNote = NULL;

		if(strlen(f3[4].GetString()))
			gm->szOfficerNote = strdup(f3[4].GetString());
		else
			gm->szOfficerNote = NULL;

		gm->uLastWithdrawReset = f3[5].GetUInt32();
		gm->uWithdrawlsSinceLastReset = f3[6].GetUInt32();
		for(j = 0; j < MAX_GUILD_BANK_TABS; ++j)
		{
			gm->uLastItemWithdrawReset[j] = f3[7+(j*2)].GetUInt32();
			gm->uItemWithdrawlsSinceLastReset[j] = f3[8+(j*2)].GetUInt32();
		}

		m_members.insert(make_pair(gm->pPlayer, gm));

	} while(result->NextRow());
	delete result;

	result = CharacterDatabase.Query("SELECT MAX(log_id) FROM guild_logs WHERE guildid = %u", m_guildId);
	m_hiLogId = 1;
	if(result != NULL)
	{
		m_hiLogId = result->Fetch()[0].GetUInt32() + 1;
		delete result;
	}

	// load log
	result = CharacterDatabase.Query("SELECT * FROM guild_logs WHERE guildid = %u ORDER BY timestamp ASC", m_guildId);
	if(result)
	{
		do 
		{
			GuildLogEvent * li = new GuildLogEvent;
			li->iLogId = result->Fetch()[0].GetUInt32();
			li->iEvent = result->Fetch()[3].GetUInt32();
			li->iTimeStamp = result->Fetch()[2].GetUInt32();
			li->iEventData[0] = result->Fetch()[4].GetUInt32();
			li->iEventData[1] = result->Fetch()[5].GetUInt32();
			li->iEventData[2] = result->Fetch()[6].GetUInt32();
		} while(result->NextRow());
		delete result;
	}

	result = CharacterDatabase.Query("SELECT * FROM guild_banktabs WHERE guildId = %u ORDER BY tabId ASC", m_guildId);
	sid = 0;
	if(result)
	{
		do 
		{
			if((sid++) != result->Fetch()[1].GetUInt32())
			{
#ifdef WIN32
				MessageBox(0, "Guild bank tabs are out of order!", "Internal error", MB_OK);
				TerminateProcess(GetCurrentProcess(), 0);
				return false;
#else
				printf("Guild bank tabs are out of order!\n");
				exit(0);
#endif
			}

			QueryResult * res2 = CharacterDatabase.Query("SELECT * FROM guild_bankitems WHERE guildId = %u AND tabId = %u", m_guildId, result->Fetch()[1].GetUInt32());
			GuildBankTab * pTab = new GuildBankTab;
			pTab->iTabId = (uint8)result->Fetch()[1].GetUInt32();
			pTab->szTabName = (strlen(result->Fetch()[2].GetString()) > 0) ? strdup(result->Fetch()[2].GetString()) : NULL;
			pTab->szTabIcon = (strlen(result->Fetch()[3].GetString()) > 0) ? strdup(result->Fetch()[3].GetString()) : NULL;
			
			memset(pTab->pSlots, 0, sizeof(Item*) * MAX_GUILD_BANK_SLOTS);

			if(res2)
			{
				do 
				{
					Item * pItem = objmgr.LoadItem(res2->Fetch()[3].GetUInt64());
					if(pItem == NULL)
					{
						CharacterDatabase.Execute("DELETE FROM guild_bankitems WHERE itemGuid = %u AND guildId = %u AND tabId = %u", res2->Fetch()[3].GetUInt32(), m_guildId, (uint32)pTab->iTabId);
						continue;
					}

					pTab->pSlots[res2->Fetch()[2].GetUInt32()] = pItem;

				} while (res2->NextRow());
				delete res2;
			}

			m_bankTabs.push_back(pTab);

		} while (result->NextRow());
		delete result;
	}

	Log.Debug("Guild", "Loaded guild %s, %u members.", m_guildName, m_members.size());
	return true;
}

void Guild::SetMOTD(const char * szNewMotd, WorldSession * pClient)
{
	if(pClient->GetPlayer()->m_playerInfo->guild != this)
		return;

	if(!pClient->GetPlayer()->m_playerInfo->guildRank->CanPerformCommand(GR_RIGHT_SETMOTD))
	{
		Guild::SendGuildCommandResult(pClient, GUILD_INVITE_S, "", GUILD_PERMISSIONS);
		return;
	}

	if(m_motd)
		free(m_motd);

	if(strlen(szNewMotd))
	{
		m_motd = strdup(szNewMotd);
		CharacterDatabase.Execute("UPDATE guilds SET motd = \"%s\" WHERE guildId = %u", CharacterDatabase.EscapeString(string(szNewMotd)).c_str(), m_guildId);
	}
	else
	{
		m_motd= NULL;
		CharacterDatabase.Execute("UPDATE guilds SET motd = \"\" WHERE guildId = %u", m_guildId);
	}

	LogGuildEvent(GUILD_EVENT_MOTD, 1, szNewMotd);
}

void Guild::SetGuildInformation(const char * szGuildInformation, WorldSession * pClient)
{
	if(pClient->GetPlayer()->m_playerInfo->guild != this)
		return;

	if(!pClient->GetPlayer()->m_playerInfo->guildRank->CanPerformCommand(GR_RIGHT_EGUILDINFO))
	{
		Guild::SendGuildCommandResult(pClient, GUILD_INVITE_S, "", GUILD_PERMISSIONS);
		return;
	}

	if(m_guildInfo)
		free(m_guildInfo);

	if(strlen(szGuildInformation))
	{
		m_guildInfo = strdup(szGuildInformation);
		CharacterDatabase.Execute("UPDATE guilds SET guildInfo = \"%s\" WHERE guildId = %u", CharacterDatabase.EscapeString(string(szGuildInformation)).c_str(), m_guildId);
	}
	else
	{
		m_guildInfo= NULL;
		CharacterDatabase.Execute("UPDATE guilds SET guildInfo = "" WHERE guildId = %u", m_guildId);
	}
}

// adding a member
void Guild::AddGuildMember(PlayerInfo * pMember, WorldSession * pClient, int32 ForcedRank /* = -1 */)
{
	if(pMember->guild != NULL)
		return;

	if(pClient && pClient->GetPlayer()->m_playerInfo->guild != this)
		return;

	if(pClient && !pClient->GetPlayer()->m_playerInfo->guildRank->CanPerformCommand(GR_RIGHT_INVITE))
		return;

	m_lock.Acquire();
	GuildRank * r;
	if(m_members.size())
		r = (ForcedRank<=0) ? FindLowestRank() : m_ranks[ForcedRank];
	else
		r = (ForcedRank<0) ? FindLowestRank() : m_ranks[ForcedRank];

	if(r==NULL)
		r=FindLowestRank();

	if(r==NULL)
	{
		// shouldnt happen
		m_lock.Release();
		return;
	}

	GuildMember * pm = new GuildMember;
	memset(pm, 0, sizeof(GuildMember));
	pm->pPlayer = pMember;
	pm->pRank = r;
	pm->szOfficerNote = pm->szPublicNote = NULL;
	m_members.insert(make_pair(pMember, pm));

	pMember->guild=this;
	pMember->guildRank=r;
	pMember->guildMember=pm;

	if(pMember->m_loggedInPlayer)
	{
		pMember->m_loggedInPlayer->SetGuildId(m_guildId);
		pMember->m_loggedInPlayer->SetGuildRank(r->iId);
	}

	CharacterDatabase.Execute("INSERT INTO guild_data VALUES(%u, %u, %u, '', '', 0, 0, 0, 0, 0)", m_guildId, pMember->guid, r->iId);
	LogGuildEvent(GUILD_EVENT_JOINED, 1, pMember->name);
	AddGuildLogEntry(GUILD_LOG_EVENT_JOIN, 1, pMember->guid);
	m_lock.Release();
}

void Guild::RemoveGuildMember(PlayerInfo * pMember, WorldSession * pClient)
{
	if(pMember->guild != this)
		return;

	if(pClient && pClient->GetPlayer()->m_playerInfo->guild != this)
		return;

	if(pClient && !pClient->GetPlayer()->m_playerInfo->guildRank->CanPerformCommand(GR_RIGHT_REMOVE) && pClient->GetPlayer()->m_playerInfo != pMember)
	{
		Guild::SendGuildCommandResult(pClient, GUILD_CREATE_S, "", GUILD_PERMISSIONS);
		return;
	}

	if(pMember->guildRank->iId==0)
	{
		if(pClient)
			pClient->SystemMessage("You cannot remove the guild master.");

		return;
	}

	CharacterDatabase.Execute("DELETE FROM guild_data WHERE playerid = %u", pMember->guid);

	m_lock.Acquire();
	GuildMemberMap::iterator itr = m_members.find(pMember);
	if(itr != m_members.end())
	{
		// this should always exist.
		if(itr->second->szOfficerNote)
			free((void*)itr->second->szOfficerNote);
		if(itr->second->szPublicNote)
			free((void*)itr->second->szPublicNote);

		delete itr->second;
		m_members.erase(itr);
	}

	LogGuildEvent(GUILD_EVENT_LEFT, 1, pMember->name);
	if(pClient && pClient->GetPlayer()->m_playerInfo != pMember)
	{
		AddGuildLogEntry(GUILD_LOG_EVENT_REMOVAL, 2, pClient->GetPlayer()->GetGUIDLow(), pMember->guid);
	}
	else
	{
		AddGuildLogEntry(GUILD_LOG_EVENT_LEFT, 1, pMember->guid);
	}

	m_lock.Release();

	pMember->guildRank=NULL;
	pMember->guild=NULL;
	pMember->guildMember=NULL;

	if(pMember->m_loggedInPlayer)
	{
		pMember->m_loggedInPlayer->SetGuildRank(0);
		pMember->m_loggedInPlayer->SetGuildId(0);
	}
}

void Guild::SetPublicNote(PlayerInfo * pMember, const char * szNewNote, WorldSession * pClient)
{
	if(pMember->guild != this)
		return;

	if(pClient->GetPlayer()->m_playerInfo->guild != this)
		return;

	if(!pClient->GetPlayer()->m_playerInfo->guildRank->CanPerformCommand(GR_RIGHT_EPNOTE))
	{
		Guild::SendGuildCommandResult(pClient, GUILD_MEMBER_S, "", GUILD_PERMISSIONS);
		return;
	}

	m_lock.Acquire();
	GuildMemberMap::iterator itr = m_members.find(pMember);
	if(itr != m_members.end())
	{
		if(itr->second->szPublicNote)
			free((void*)itr->second->szPublicNote);

		if(szNewNote && szNewNote[0] != '\0')
			itr->second->szPublicNote = strdup(szNewNote);
		else
			itr->second->szPublicNote = NULL;
	}
	m_lock.Release();

	Guild::SendGuildCommandResult(pClient, GUILD_PUBLIC_NOTE_CHANGED_S, pMember->name, 0);
}

void Guild::SetOfficerNote(PlayerInfo * pMember, const char * szNewNote, WorldSession * pClient)
{
	if(pMember->guild != this)
		return;

	if(pClient->GetPlayer()->m_playerInfo->guild != this)
		return;

	if(!pClient->GetPlayer()->m_playerInfo->guildRank->CanPerformCommand(GR_RIGHT_EOFFNOTE))
	{
		Guild::SendGuildCommandResult(pClient, GUILD_MEMBER_S, "", GUILD_PERMISSIONS);
		return;
	}

	m_lock.Acquire();
	GuildMemberMap::iterator itr = m_members.find(pMember);
	if(itr != m_members.end())
	{
		if(itr->second->szOfficerNote)
			free((void*)itr->second->szOfficerNote);

		if(szNewNote && szNewNote[0] != '\0')
			itr->second->szOfficerNote = strdup(szNewNote);
		else
			itr->second->szOfficerNote = NULL;
	}
	m_lock.Release();

	Guild::SendGuildCommandResult(pClient, GUILD_OFFICER_NOTE_CHANGED_S, pMember->name, 0);
}

void Guild::RemoveGuildRank(WorldSession * pClient)
{
	m_lock.Acquire();

	GuildRank * pLowestRank = FindLowestRank();
	if(pLowestRank = NULL)
	{
		pClient->SystemMessage("Cannot find a rank to delete.");
		m_lock.Release();
		return;		
	}

	// check for players that need to be promoted
	GuildRank * pNewLowestRank = FindNextLowestRank(pLowestRank);
	GuildMemberMap::iterator itr = m_members.begin();
	for(; itr != m_members.end(); ++itr)
	{
		if(itr->second->pRank == pLowestRank)
		{
			if(pNewLowestRank == NULL)
			{
				pClient->SystemMessage("There are still members using this rank and we cannot find them another rank to use!");
				m_lock.Release();
				return;
			}

			itr->second->pRank = pNewLowestRank;
			itr->first->guildRank = pNewLowestRank;
		}
	}

	m_ranks[pLowestRank->iId] = NULL;
	delete pLowestRank;
	m_lock.Release();
}

void Guild::Disband()
{
	m_lock.Acquire();
	for(GuildMemberMap::iterator itr = m_members.begin(); itr != m_members.end(); ++itr)
	{
		itr->first->guild=NULL;
		itr->first->guildRank=NULL;
		if(itr->first->m_loggedInPlayer != NULL)
		{
			itr->first->m_loggedInPlayer->SetGuildId(0);
			itr->first->m_loggedInPlayer->SetGuildRank(0);
		}

		delete itr->second;
	}
	m_members.clear();
	objmgr.RemoveGuild(m_guildId);
	CharacterDatabase.Execute("DELETE FROM guilds WHERE guildId = %u", m_guildId);
	CharacterDatabase.Execute("DELETE FROM guild_logs WHERE guildid = %u", m_guildId);
	CharacterDatabase.Execute("DELETE FROM guild_ranks WHERE guildId = %u", m_guildId);
	CharacterDatabase.Execute("DELETE FROM guild_data WHERE guildid = %u", m_guildId);
	CharacterDatabase.Execute("DELETE FROM guild_bankitems WHERE guildId = %u", m_guildId);
	CharacterDatabase.Execute("DELETE FROM guild_banktabs WHERE guildId = %u", m_guildId);
	m_lock.Release();
	delete this;
}

void Guild::ChangeGuildMaster(PlayerInfo * pNewMaster, WorldSession * pClient)
{
	if(pClient->GetPlayer()->GetGUIDLow() != m_guildLeader)
	{
		Guild::SendGuildCommandResult(pClient, GUILD_PROMOTE_S, "", GUILD_PERMISSIONS);
		return;
	}

	m_lock.Acquire();
	GuildRank * newRank = FindHighestRank();
	if(newRank==NULL)
	{
		m_lock.Release();
		return;
	}

	GuildMemberMap::iterator itr = m_members.find(pNewMaster);
	GuildMemberMap::iterator itr2 = m_members.find(pClient->GetPlayer()->m_playerInfo);
	ASSERT(m_ranks[0]!=NULL);
	if(itr==m_members.end() || itr2==m_members.end())
	{
		m_lock.Release();
		return;
	}

	itr->second->pRank = m_ranks[0];
	itr->first->guildRank = itr->second->pRank;
	itr2->second->pRank = newRank;
	itr2->first->guildRank = newRank;
	CharacterDatabase.Execute("UPDATE guild_data SET guildRank = 0 WHERE playerid = %u AND guildid = %u", itr->first->guid, m_guildId);
	CharacterDatabase.Execute("UPDATE guild_data SET guildRank = %u WHERE playerid = %u AND guildid = %u", newRank->iId, itr->first->guid, m_guildId);
	CharacterDatabase.Execute("UPDATE guilds SET guildMaster = %u WHERE guildId = %u", itr->first->guid, m_guildId);
	m_guildLeader = itr->first->guid;
	m_lock.Release();
}

uint32 Guild::GenerateGuildLogEventId()
{
	uint32 r;
	m_lock.Acquire();
	r = ++m_hiLogId;
	m_lock.Release();
	return r;
}

void Guild::GuildChat(const char * szMessage, WorldSession * pClient, uint32 iType)
{
	if(pClient->GetPlayer()->m_playerInfo->guild != this)
		return;

	if(!pClient->GetPlayer()->m_playerInfo->guildRank->CanPerformCommand(GR_RIGHT_GCHATSPEAK))
	{
		Guild::SendGuildCommandResult(pClient, GUILD_MEMBER_S, "", GUILD_PERMISSIONS);
		return;
	}

	WorldPacket * data = sChatHandler.FillMessageData( CHAT_MSG_GUILD, iType==CHAT_MSG_ADDON?CHAT_MSG_ADDON:LANG_UNIVERSAL, szMessage,
		pClient->GetPlayer()->GetGUID());

	m_lock.Acquire();
	for(GuildMemberMap::iterator itr = m_members.begin(); itr != m_members.end(); ++itr)
	{
		if(itr->second->pRank->CanPerformCommand(GR_RIGHT_GCHATLISTEN) && itr->first->m_loggedInPlayer)
			itr->first->m_loggedInPlayer->GetSession()->SendPacket(data);
	}
	m_lock.Release();

	delete data;
}

void Guild::OfficerChat(const char * szMessage, WorldSession * pClient, uint32 iType)
{
	if(pClient->GetPlayer()->m_playerInfo->guild != this)
		return;

	if(!pClient->GetPlayer()->m_playerInfo->guildRank->CanPerformCommand(GR_RIGHT_OFFCHATSPEAK))
	{
		Guild::SendGuildCommandResult(pClient, GUILD_MEMBER_S, "", GUILD_PERMISSIONS);
		return;
	}

	WorldPacket * data = sChatHandler.FillMessageData( CHAT_MSG_OFFICER, iType==CHAT_MSG_ADDON?CHAT_MSG_ADDON:LANG_UNIVERSAL, szMessage,
		pClient->GetPlayer()->GetGUID());

	m_lock.Acquire();
	for(GuildMemberMap::iterator itr = m_members.begin(); itr != m_members.end(); ++itr)
	{
		if(itr->second->pRank->CanPerformCommand(GR_RIGHT_OFFCHATLISTEN) && itr->first->m_loggedInPlayer)
			itr->first->m_loggedInPlayer->GetSession()->SendPacket(data);
	}
	m_lock.Release();

	delete data;
}

void Guild::SendGuildLog(WorldSession * pClient)
{
	WorldPacket data(MSG_GUILD_LOG, 18*m_log.size()+1);
	GuildLogList::iterator itr;

	data << uint8(m_log.size() >= 250 ? 250 : m_log.size());
	m_lock.Acquire();
	for(itr = m_log.begin(); itr != m_log.end(); ++itr) {
		data << uint8((*itr)->iEvent);
		switch((*itr)->iEvent)
		{
		case GUILD_LOG_EVENT_DEMOTION:
		case GUILD_LOG_EVENT_PROMOTION:
			{
				data << uint64((*itr)->iEventData[0]);
				data << uint64((*itr)->iEventData[1]);
				data << uint8((*itr)->iEventData[2]);
			}break;
			
		case GUILD_LOG_EVENT_INVITE:
		case GUILD_LOG_EVENT_REMOVAL:
			{
				data << uint64((*itr)->iEventData[0]);
				data << uint64((*itr)->iEventData[1]);
			}break;

		case GUILD_LOG_EVENT_JOIN:
		case GUILD_LOG_EVENT_LEFT:
			{
				data << uint64((*itr)->iEventData[0]);
			}break;
		}

		data << uint32(UNIXTIME - (*itr)->iTimeStamp);
	}
	m_lock.Release();

	pClient->SendPacket(&data);
}

void Guild::SendGuildRoster(WorldSession * pClient)
{
	WorldPacket data(SMSG_GUILD_ROSTER, (60*10) + (100 * m_members.size()) + 100);
	GuildMemberMap::iterator itr;
	GuildRank * r;
	Player * pPlayer;
	uint32 i;
	GuildRank * myRank;
	bool ofnote;
	if(pClient->GetPlayer()->m_playerInfo->guild != this)
		return;

	myRank = pClient->GetPlayer()->m_playerInfo->guildRank;
	ofnote = myRank->CanPerformCommand(GR_RIGHT_VIEWOFFNOTE);

	m_lock.Acquire();

	data << uint32(m_members.size());
	if(m_motd)
		data << m_motd;
	else
		data << uint8(0);

	if(m_guildInfo)
		data << m_guildInfo;
	else
		data << uint8(0);

	data << uint32(MAX_GUILD_RANKS);

	for(i = 0; i < MAX_GUILD_RANKS; ++i)
	{
		r = m_ranks[i];
		if(r == NULL)
		{
			data << uint64(0);
			data << uint64(0);
			data << uint64(0);
			data << uint64(0);
			data << uint64(0);
			data << uint64(0);
			data << uint64(0);
		}
		else
		{
			data << r->iRights;
			data << r->iGoldLimitPerDay;

			// i would do this with one big memcpy but grr grr struct alignment
			data << r->iTabPermissions[0].iFlags;
			data << r->iTabPermissions[0].iStacksPerDay;
			data << r->iTabPermissions[1].iFlags;
			data << r->iTabPermissions[1].iStacksPerDay;
			data << r->iTabPermissions[2].iFlags;
			data << r->iTabPermissions[2].iStacksPerDay;
			data << r->iTabPermissions[3].iFlags;
			data << r->iTabPermissions[3].iStacksPerDay;
			data << r->iTabPermissions[4].iFlags;
			data << r->iTabPermissions[4].iStacksPerDay;
			data << r->iTabPermissions[5].iFlags;
			data << r->iTabPermissions[5].iStacksPerDay;
		}
	}

	for(itr = m_members.begin(); itr != m_members.end(); ++itr)
	{
		pPlayer = itr->second->pPlayer->m_loggedInPlayer;

		data << itr->first->guid;
		data << uint32(0);			// highguid
		data << uint8( (pPlayer!=NULL) ? 1 : 0 );
		data << itr->first->name;
		data << itr->second->pRank->iId;
		data << uint8(itr->first->lastLevel);
		data << uint8(itr->first->cl);
		data << itr->first->lastZone;

		if(!pPlayer)
			data << float((UNIXTIME-itr->first->lastOnline)/86400.0);

		if(itr->second->szPublicNote)
			data << itr->second->szPublicNote;
		else
			data << uint8(0);

		if(ofnote && itr->second->szOfficerNote != NULL)
			data << itr->second->szOfficerNote;
		else
			data << uint8(0);
	}

	m_lock.Release();

	pClient->SendPacket(&data);
}

void Guild::SendGuildQuery(WorldSession * pClient)
{
	uint32 i = 0;
	GuildRank * r;
	WorldPacket data(SMSG_GUILD_QUERY_RESPONSE, 300);
	data << m_guildId;
	data << m_guildName;

	m_lock.Acquire();
	for(i = 0; i < MAX_GUILD_RANKS; ++i)
	{
		r = m_ranks[i];
		if(r != NULL)
			data << r->szRankName;
		else
			data << uint8(0);
	}

	m_lock.Release();

	data << m_emblemStyle;
	data << m_emblemColor;
	data << m_borderStyle;
	data << m_borderColor;
	data << m_backgroundColor;

	pClient->SendPacket(&data);
}

void Guild::CreateInDB()
{
	CharacterDatabase.Execute("INSERT INTO guilds VALUES(%u, \"%s\", %u, %u, %u, %u, %u, %u, '', '', %u, 0, 0)",
		m_guildId, CharacterDatabase.EscapeString(string(m_guildName)).c_str(), m_guildLeader, m_emblemStyle, m_emblemColor, m_borderColor, m_borderStyle,
		m_backgroundColor, m_creationTimeStamp);
}

Guild* Guild::Create()
{
	return new Guild();
}

void Guild::BuyBankTab(WorldSession * pClient)
{
	if(pClient && pClient->GetPlayer()->GetGUIDLow() != m_guildLeader)
		return;

	m_lock.Acquire();

	if(m_bankTabCount>=MAX_GUILD_BANK_TABS)
		return;
	
	GuildBankTab * pTab = new GuildBankTab;
	pTab->iTabId = m_bankTabCount;
	memset(pTab->pSlots, 0, sizeof(Item*)*MAX_GUILD_BANK_SLOTS);
	pTab->szTabName=NULL;
	pTab->szTabIcon=NULL;

	m_bankTabs.push_back(pTab);
	m_bankTabCount++;

	CharacterDatabase.Execute("INSERT INTO guild_banktabs VALUES(%u, %u, '', '')", m_guildId, (uint32)pTab->iTabId);
	CharacterDatabase.Execute("UPDATE guilds SET bankTabCount = %u WHERE guildId = %u", m_bankTabCount, m_guildId);
	m_lock.Release();
}

uint32 GuildMember::CalculateAllowedItemWithdraws(uint32 tab)
{
	if(pRank->iTabPermissions[tab].iStacksPerDay == 0)		// Unlimited
		return 0xFFFFFFFF;

	if((UNIXTIME - uLastItemWithdrawReset[tab]) >= TIME_DAY)
		return pRank->iTabPermissions[tab].iStacksPerDay;
	else
		return (pRank->iTabPermissions[tab].iStacksPerDay - uItemWithdrawlsSinceLastReset[tab]);
}

void GuildMember::OnItemWithdraw(uint32 tab)
{
	if(pRank->iTabPermissions[tab].iStacksPerDay == 0)		// Unlimited
		return;

	// reset the counter if a day has passed
	if(((uint32)UNIXTIME - uLastItemWithdrawReset[tab]) >= TIME_DAY)
	{
		uLastItemWithdrawReset[tab] = (uint32)UNIXTIME;
		uItemWithdrawlsSinceLastReset[tab] = 1;
		CharacterDatabase.Execute("UPDATE guild_data SET lastItemWithdrawReset%u = %u, itemWithdrawlsSinceLastReset%u = 1 WHERE playerid = %u",
			tab, uLastItemWithdrawReset, tab, pPlayer->guid);
	}
	else
	{
		// increment counter
		uItemWithdrawlsSinceLastReset[tab]++;
		CharacterDatabase.Execute("UPDATE guild_data SET itemWithdrawlsSinceLastReset%u = %u WHERE playerid = %u",
			tab, uItemWithdrawlsSinceLastReset, pPlayer->guid);
	}
}

uint32 GuildMember::CalculateAvailableAmount()
{
	if(pRank->iGoldLimitPerDay == 0)		// Unlimited
		return 0xFFFFFFFF;

	if((UNIXTIME - uLastWithdrawReset) >= TIME_DAY)
		return pRank->iGoldLimitPerDay;
	else
		return (pRank->iGoldLimitPerDay - uWithdrawlsSinceLastReset);
}

void GuildMember::OnMoneyWithdraw(uint32 amt)
{
	if(pRank->iGoldLimitPerDay == 0)		// Unlimited
		return;

	// reset the counter if a day has passed
	if(((uint32)UNIXTIME - uLastWithdrawReset) >= TIME_DAY)
	{
		uLastWithdrawReset = (uint32)UNIXTIME;
		uWithdrawlsSinceLastReset = amt;
		CharacterDatabase.Execute("UPDATE guild_data SET lastWithdrawReset = %u, withdrawlsSinceLastReset = %u WHERE playerid = %u",
			uLastWithdrawReset, uWithdrawlsSinceLastReset, pPlayer->guid);
	}
	else
	{
		// increment counter
		uWithdrawlsSinceLastReset++;
		CharacterDatabase.Execute("UPDATE guild_data SET withdrawlsSinceLastReset = %u WHERE playerid = %u",
			uWithdrawlsSinceLastReset, pPlayer->guid);
	}
}

void Guild::DepositMoney(WorldSession * pClient, uint32 uAmount)
{
	if(pClient->GetPlayer()->GetUInt32Value(PLAYER_FIELD_COINAGE) < uAmount)
		return;

	// add to the bank balance
	m_bankBalance += uAmount;

	// update in db
	CharacterDatabase.Execute("UPDATE guilds SET bankBalance = %u WHERE guildId = %u", m_bankBalance, m_guildId);

	// take the money, oh noes gm pls gief gold mi hero poor
	pClient->GetPlayer()->ModUInt32Value(PLAYER_FIELD_COINAGE, -(int32)uAmount);

	// broadcast guild event telling everyone the new balance
	char buf[20];
	snprintf(buf, 20, I64FMT, (uint64)m_bankBalance);
	LogGuildEvent(GUILD_EVENT_SETNEWBALANCE, 1, buf);
}

void Guild::WithdrawMoney(WorldSession * pClient, uint32 uAmount)
{
	GuildMember * pMember = pClient->GetPlayer()->m_playerInfo->guildMember;
	if(pMember==NULL)
		return;

	// sanity checks
	if(pMember->pRank->iGoldLimitPerDay)
		if(pMember->CalculateAvailableAmount() < uAmount)
			return;

	if(m_bankBalance < uAmount)
		return;

	// update his bank state
	pMember->OnMoneyWithdraw(uAmount);

	// give the gold! GM PLS GOLD PLS 1 COIN
	pClient->GetPlayer()->ModUInt32Value(PLAYER_FIELD_COINAGE, (uint32)uAmount);

	// subtract the balance
	m_bankBalance -= uAmount;

	// update in db
	CharacterDatabase.Execute("UPDATE guilds SET bankBalance = %u WHERE guildId = %u", (m_bankBalance>0)?m_bankBalance:0, m_guildId);

	// notify everyone with the new balance
	char buf[20];
	snprintf(buf, 20, I64FMT, (uint64)m_bankBalance);
	LogGuildEvent(GUILD_EVENT_SETNEWBALANCE, 1, buf);
}
