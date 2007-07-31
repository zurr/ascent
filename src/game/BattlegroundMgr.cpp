/****************************************************************************
 *
 * General Battleground Handler
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

#include "StdAfx.h"

initialiseSingleton(CBattlegroundManager);
typedef CBattleground*(*CreateBattlegroundFunc)(MapMgr* mgr,uint32 iid,uint32 group);

const static uint32 BGMapIds[BATTLEGROUND_NUM_TYPES] = {
	0,		// 0
	30,		// AV
	489,	// WSG
	529,	// AB
	559,	// Nagrand Arena
	562,	// Blades Edge Arena
	566,	// Netherstorm BG
};

const static CreateBattlegroundFunc BGCFuncs[BATTLEGROUND_NUM_TYPES] = {
	NULL,						// 0
	NULL,						// AV
	NULL,						// WSG
	NULL,						// AB
	NULL,						// Nagrand
	NULL,						// Blades Edge
	NULL,						// Netherstorm
};

CBattlegroundManager::CBattlegroundManager() : EventableObject()
{
	m_maxBattlegroundId = 0;
	sEventMgr.AddEvent(this, &CBattlegroundManager::EventQueueUpdate, EVENT_BATTLEGROUND_QUEUE_UPDATE, 10000, 0);
}

CBattlegroundManager::~CBattlegroundManager()
{

}

void CBattlegroundManager::HandleBattlegroundListPacket(WorldSession * m_session, uint32 BattlegroundType)
{
	uint32 LevelGroup = GetLevelGrouping(m_session->GetPlayer()->getLevel());
	uint32 Count = 0;
	WorldPacket data(SMSG_BATTLEFIELD_LIST, 200);
	data << m_session->GetPlayer()->GetGUID();
	data << BattlegroundType;
	data << uint8(2);
	data << uint32(0);		// Count

	/* Append the battlegrounds */
	m_instanceLock.AcquireReadLock();
	for(map<uint32, CBattleground*>::iterator itr = m_instances[BattlegroundType].begin(); itr != m_instances[BattlegroundType].end(); ++itr)
	{
        if(itr->second->GetLevelGroup() == LevelGroup)
		{
			data << itr->first;
			++Count;
		}
	}
	m_instanceLock.ReleaseReadLock();
	*(uint32*)&data.contents()[13] = Count;
	m_session->SendPacket(&data);
}

void CBattlegroundManager::HandleBattlegroundJoin(WorldSession * m_session, WorldPacket & pck)
{
	uint64 guid;
	uint32 pguid = m_session->GetPlayer()->GetGUIDLow();
	uint32 lgroup = GetLevelGrouping(m_session->GetPlayer()->getLevel());
	uint32 bgtype;
	uint32 instance;

	pck >> guid >> bgtype >> instance;
	if(bgtype >= BATTLEGROUND_NUM_TYPES)
		return;		// cheater!

	/* Check the instance id */
	if(instance)
	{
		/* We haven't picked the first instance. This means we've specified an instance to join. */
		m_instanceLock.AcquireReadLock();
		map<uint32, CBattleground*>::iterator itr = m_instances[bgtype].find(instance);

		if(itr == m_instances[bgtype].end())
		{
			sChatHandler.SystemMessage(m_session, "You have tried to join an invalid instance id.");
			m_instanceLock.ReleaseReadLock();
			return;
		}

		m_instanceLock.ReleaseReadLock();
	}
    
	/* Queue him! */
	m_queueLock.Acquire();
	map<uint32, list<uint32> >::iterator itr = m_queuedPlayers[bgtype][lgroup].find(instance);
	if(itr != m_queuedPlayers[bgtype][lgroup].end())
		itr->second.push_back(pguid);
	else
	{
		list<uint32> tmp;
		tmp.push_back(pguid);
		m_queuedPlayers[bgtype][lgroup].insert( make_pair( instance, tmp ) );
	}
	m_queueLock.Release();

	/* We will get updated next few seconds =) */
}

void CBattlegroundManager::EventQueueUpdate()
{
	vector<Player*> tempPlayerVec[2];
	uint32 i,j,k;
	Player * plr;
	CBattleground * bg;
	map<uint32, CBattleground*>::iterator itr;
	map<uint32, list<uint32> >::iterator it2, it5;
	list<uint32>::iterator it3, it4;
	vector<Player*>::iterator it6;
	m_queueLock.Acquire();
	for(i = 0; i < BATTLEGROUND_NUM_TYPES; ++i)
	{
		for(j = 0; j < MAX_LEVEL_GROUP; ++j)
		{
			if(!m_queuedPlayers[i][j].size())
				continue;

			/* Process instance id 0 first. */
			if( (it2 = m_queuedPlayers[i][j].find( 0 )) != m_queuedPlayers[i][j].end() )
			{
				/* These players have chosen to join the "first available" instance. */

				/* Try and find a free instance. */
				bg=0;
				m_instanceLock.AcquireReadLock();
				for(itr = m_instances[i].begin(); itr != m_instances[i].end(); ++itr)
				{
					if(!itr->second->IsFull())
					{
						bg = itr->second;

						/* Add as many players as possible to this battleground. */
						for(it4 = it2->second.begin(); it4 != it2->second.end();)
						{
							it3 = it4;
							++it4;

							plr = objmgr.GetPlayer(*it3);
							if(!plr)
							{
								it2->second.erase(it3);
								continue;
							}

							if(bg->HasFreeSlots(plr->GetTeam()))
							{
								plr->m_bgIsQueued = false;
								if(GetLevelGrouping(plr->getLevel()) == j)
									bg->AddPlayer(plr);

								it2->second.erase(it3);
							}
						}

						/* No players left? */
						if(!it2->second.size())
							break;
					}
				}

				m_instanceLock.ReleaseReadLock();

				/* Do we still have players left over? */
				if(it2->second.size())
				{
					if(CanCreateInstance(i, j))
					{
						// No free instances.
						// Do we have enough players to create a new instance?
						for(it4 = it2->second.begin(); it4 != it2->second.end();)
						{
							it3 = it4;
							++it4;

							plr = objmgr.GetPlayer(*it3);
							if(!plr || GetLevelGrouping(plr->getLevel()) != j)
							{
								it2->second.erase(it3);
								continue;
							}

							tempPlayerVec[plr->GetTeam()].push_back(plr);
						}

						if(tempPlayerVec[0].size() >= MINIMUM_PLAYERS_ON_EACH_SIDE_FOR_BG &&
							tempPlayerVec[1].size() >= MINIMUM_PLAYERS_ON_EACH_SIDE_FOR_BG)
						{
							/* Woot! Let's create a new instance! */
							bg = CreateInstance(i, j);

							/* Dump all the players into the bg */
							for(k=0;k<2;++k)
							{
								for(it6=tempPlayerVec[k].begin(); it6 != tempPlayerVec[k].end(); ++it6)
								{
									bg->AddPlayer(*it6);

									/* This is gonna be costly. :P */
									for(it4 = it2->second.begin(); it4 != it2->second.end(); ++it4)
									{
										if((*it4) == (*it6)->GetGUIDLow())
										{
											it2->second.erase(it4);
											break;
										}
									}
									
									if(!bg->HasFreeSlots(k))
										break;
								}
							}
						}

						tempPlayerVec[0].clear();
						tempPlayerVec[1].clear();
					}
				}

				/* If there is no players queued on instance id 0 anymore, kill the iterator (Saves memory) */
				if(!it2->second.size())
					m_queuedPlayers[i][j].erase(it2);
			}

			// Now process the players bound to a specific instance id.
			m_instanceLock.AcquireReadLock();

			for(it5 = m_queuedPlayers[i][j].begin(); it5 != m_queuedPlayers[i][j].end(); ++it5)
			{
				it2 = it5;
				++it5;

				/* Skip the '0' instance id (handled differently) */
				if(it2->first == 0)
					continue;

				if(!it2->second.size())		// No queued players on this instance any more
				{
					/* Kill the iterator */
					m_queuedPlayers[i][j].erase(it2);
					continue;
				}

				/* Find the instance that these players are queued on. */
				itr = m_instances[i].find(it2->first);
				if(itr == m_instances[i].end())
				{
					/* Whoops! That instance is no longer valid. */
					/* Kill all the players on it. */
					for(it3 = it2->second.begin(); it3 != it2->second.end(); ++it3)
					{
						plr = objmgr.GetPlayer(*it3);
						if(plr)
						{
							sChatHandler.SystemMessageToPlr(plr, "Your queue on battleground instance %u is no longer valid, the instance no longer exists.", it2->first);
							plr->m_bgIsQueued = false;
						}
					}
					m_queuedPlayers[i][j].erase(it2);
					continue;
				}
				else
				{
					/* Do we have free slots? */
					if(!itr->second->IsFull())
					{
						/* Add as many players as possible. */
						for(it4 = it2->second.begin(); it4 != it2->second.end();)
						{
							it3 = it4;
							++it4;

							plr = objmgr.GetPlayer(*it3);
							if(!plr)
								it2->second.erase(it3);
							else
							{
								if(itr->second->HasFreeSlots(plr->GetTeam()))
								{
									plr->m_bgIsQueued = false;
									if(GetLevelGrouping(plr->getLevel()) == j)
									{
										itr->second->AddPlayer(plr);
									}
								}
							}
						}
					}
				}
			}

			m_instanceLock.ReleaseReadLock();
		}
	}
	m_queueLock.Release();
}

void CBattlegroundManager::RemovePlayerFromQueues(Player * plr)
{
	uint32 lgroup = GetLevelGrouping(plr->getLevel());
	m_queueLock.Acquire();
	ASSERT(plr->m_bgQueueType < BATTLEGROUND_NUM_TYPES);
	map<uint32, list<uint32> >::iterator itr = m_queuedPlayers[plr->m_bgQueueType][lgroup].find(plr->m_bgQueueInstanceId);
	list<uint32>::iterator it2;
	if(itr != m_queuedPlayers[plr->m_bgQueueType][lgroup].end())
	{
		for(it2 = itr->second.begin(); it2 != itr->second.end(); ++it2) {
			if((*it2) == plr->GetGUIDLow())
			{
				itr->second.erase(it2);
				break;
			}
		}
	}
    m_queueLock.Release();
}

bool CBattlegroundManager::CanCreateInstance(uint32 Type, uint32 LevelGroup)
{
	uint32 lc = 0;
	for(map<uint32, CBattleground*>::iterator itr = m_instances[Type].begin(); itr != m_instances[Type].end(); ++itr)
	{
		if(itr->second->GetLevelGroup() == LevelGroup)
		{
			lc++;
			if(lc >= MAXIMUM_BATTLEGROUNDS_PER_LEVEL_GROUP)
				return false;
		}
	}

	return true;
}

void CBattleground::SendWorldStates(Player * plr)
{
	if(!m_worldStates.size())
		return;

	uint32 bflag = 0;

	switch(m_mapMgr->GetMapId())
	{
	case  489: bflag = 0x0CCD; break;
	case  529: bflag = 0x0D1E; break;
	case   30: bflag = 0x0A25; break;
	case  562: bflag = 0x0E76; break;
	}

	WorldPacket data(SMSG_INIT_WORLD_STATES, 10 + (m_worldStates.size() * 8));
	for(map<uint32, uint32>::iterator itr = m_worldStates.begin(); itr != m_worldStates.end(); ++itr)
		data << itr->first << itr->second;
	plr->GetSession()->SendPacket(&data);
}

void CBattleground::AddPlayer(Player * plr)
{

}

CBattleground * CBattlegroundManager::CreateInstance(uint32 Type, uint32 LevelGroup)
{
	CreateBattlegroundFunc cfunc = BGCFuncs[Type];
	MapMgr * mgr = 0;
	CBattleground * bg;
	uint32 iid;

	if(cfunc == NULL)
	{
		Log.Error("BattlegroundManager", "Could not find CreateBattlegroundFunc pointer for type %u level group %u", Type, LevelGroup);
		return NULL;
	}

	/* Create Map Manager */
	if(sWorldCreator.CreateInstance(NULL, NULL, BGMapIds[Type], 0, 0, &mgr, 0) == false || !mgr)
	{
		Log.Error("BattlegroundManager", "CreateInstance() call failed for map %u, type %u, level group %u", BGMapIds[Type], Type, LevelGroup);
		return NULL;		// Shouldn't happen
	}

	/* Call the create function */
	iid = ++m_maxBattlegroundId;
	bg = cfunc(mgr, iid, LevelGroup);	
	Log.Success("BattlegroundManager", "Created battleground type %u for level group %u.", Type, LevelGroup);
	return bg;
}

void CBattlegroundManager::DeleteBattleground(CBattleground * bg)
{

}