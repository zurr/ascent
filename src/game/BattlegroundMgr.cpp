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
typedef CBattleground*(*CreateBattlegroundFunc)(MapMgr* mgr,uint32 iid,uint32 group, uint32 type);

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
	&WarsongGulch::Create,		// WSG
	NULL,						// AB
	NULL,						// Nagrand
	NULL,						// Blades Edge
	NULL,						// Netherstorm
};

CBattlegroundManager::CBattlegroundManager() : EventableObject()
{
	m_maxBattlegroundId = 0;
	sEventMgr.AddEvent(this, &CBattlegroundManager::EventQueueUpdate, EVENT_BATTLEGROUND_QUEUE_UPDATE, 20000, 0);
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
		Log.Debug("BattlegroundManger", "Created queue for battleground type %u in levelgroup %u", bgtype, lgroup);
		list<uint32> tmp;
		tmp.push_back(pguid);
		m_queuedPlayers[bgtype][lgroup].insert( make_pair( instance, tmp ) );
	}
	Log.Success("BattlegroundMgr", "Player %u is now in battleground queue for instance %u", m_session->GetPlayer()->GetGUIDLow(), instance );

	/* send the battleground status packet */
	SendBattlefieldStatus(m_session->GetPlayer(), 1, bgtype, instance, 0);
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
							Log.Debug("BattlegroundMgr", "Enough players to start battleground type %u for level group %u. Creating.", i, j);
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
							SendBattlefieldStatus(plr, 0, 0, 0, 0);
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
	plr->m_bgIsQueued = false;
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
	uint32 bflag2 = 0;

	switch(m_mapMgr->GetMapId())
	{
	case  489: bflag = 0x0CCD; bflag2 = 0x0CF9; break;
	case  529: bflag = 0x0D1E; break;
	case   30: bflag = 0x0A25; break;
	case  562: bflag = 0x0E76; break;
	}

	WorldPacket data(SMSG_INIT_WORLD_STATES, 10 + (m_worldStates.size() * 8));
	data << m_mapMgr->GetMapId();
	data << bflag;
	data << bflag2;
	data << uint32(m_worldStates.size());

	for(map<uint32, uint32>::iterator itr = m_worldStates.begin(); itr != m_worldStates.end(); ++itr)
		data << itr->first << itr->second;
	plr->GetSession()->SendPacket(&data);
}

CBattleground::CBattleground(MapMgr * mgr, uint32 id, uint32 levelgroup, uint32 type) : m_mapMgr(mgr), m_id(id), m_levelGroup(levelgroup), m_type(type)
{
	m_nextPvPUpdateTime = 0;
	m_countdownStage = 0;
	m_ended = false;
	m_winningteam = 0;
	m_startTime = World::UNIXTIME;
	m_lastResurrect = World::UNIXTIME;
	sEventMgr.AddEvent(this, &CBattleground::EventResurrectPlayers, EVENT_BATTLEGROUND_QUEUE_UPDATE, 30000, 0);
}

CBattleground::~CBattleground()
{
	sEventMgr.RemoveEvents(this);
}

void CBattleground::UpdatePvPData()
{
	if(World::UNIXTIME >= m_nextPvPUpdateTime)
	{
		m_mainLock.Acquire();
		WorldPacket data(MSG_PVP_LOG_DATA, 50);
		BGScore * bs;
		data << uint8(0);
		if(m_ended)
		{
			data << uint8(1);
			data << uint8(m_winningteam);
		}
		else
		{
			data << uint8(0);		// If the game has ended - this will be 1

			data << uint32(m_players[0].size() + m_players[1].size());
			for(uint32 i = 0; i < 2; ++i)
			{
				for(set<Player*>::iterator itr = m_players[i].begin(); itr != m_players[i].end(); ++itr)
				{
					data << (*itr)->GetGUID();
					bs = &(*itr)->m_bgScore;

					data << bs->KillingBlows;
					data << bs->HonorableKills;
					data << bs->Deaths;
					data << bs->BonusHonor;
					data << bs->DamageDone;
					data << bs->HealingDone;
					data << uint32(0x2);
					data << bs->Misc1;
					data << bs->Misc2;
				}
			}
		}

		DistributePacketToAll(&data);
		m_mainLock.Release();

		m_nextPvPUpdateTime = World::UNIXTIME + 2;
	}
}

void CBattleground::AddPlayer(Player * plr)
{
	m_mainLock.Acquire();

	/* This is called when the player is added, not when they port. So, they're essentially still queued, but not inside the bg yet */
	m_pendPlayers[plr->GetTeam()].insert(plr->GetGUIDLow());

	/* Send a packet telling them that they can enter */
	BattlegroundManager.SendBattlefieldStatus(plr, 2, m_type, m_id, 120000);		// You will be removed from the queue in 2 minutes.

	/* Add an event to remove them in 2 minutes time. */
	sEventMgr.AddEvent(plr, &Player::RemoveFromBattlegroundQueue, EVENT_BATTLEGROUND_QUEUE_UPDATE, 120000, 1);
	plr->m_pendingBattleground = this;

	m_mainLock.Release();
}

void CBattleground::RemovePendingPlayer(Player * plr)
{
	m_mainLock.Acquire();
	
	m_pendPlayers[plr->GetTeam()].erase(plr->GetGUIDLow());
	/* send a null bg update (so they don't join) */
	BattlegroundManager.SendBattlefieldStatus(plr, 0, 0, 0, 0);
	plr->m_pendingBattleground =0;

	m_mainLock.Release();
}

void CBattleground::PortPlayer(Player * plr, bool skip_teleport /* = false*/)
{
	m_mainLock.Acquire();
	if(m_ended)
	{
		sChatHandler.SystemMessage(plr->GetSession(), "You cannot join this battleground as it has already ended.");
		BattlegroundManager.SendBattlefieldStatus(plr, 0, 0, 0, 0);
		plr->m_pendingBattleground = 0;
		m_mainLock.Release();
		return;
	}

	/* remove from any auto queue remove events */
	sEventMgr.RemoveEvents(plr, EVENT_BATTLEGROUND_QUEUE_UPDATE);

	WorldPacket data(SMSG_BATTLEGROUND_PLAYER_JOINED, 8);
	data << plr->GetGUID();
	DistributePacketToAll(&data);

	if(!skip_teleport)
	{
		/* This is where we actually teleport the player to the battleground. */
		plr->m_bgEntryPointX = plr->GetPositionX();
		plr->m_bgEntryPointY = plr->GetPositionY();
		plr->m_bgEntryPointZ = plr->GetPositionZ();
		plr->m_bgEntryPointMap = plr->GetMapId();
		plr->m_bgEntryPointInstance = plr->GetInstanceID();
	
		plr->SafeTeleport(BGMapIds[m_type], m_mapMgr->GetInstanceID(), GetStartingCoords(plr->GetTeam()));
		BattlegroundManager.SendBattlefieldStatus(plr, 3, m_type, m_id, World::UNIXTIME - m_startTime);	// Elapsed time is the last argument
	}

	m_pendPlayers[plr->GetTeam()].erase(plr->GetGUIDLow());
	m_players[plr->GetTeam()].insert(plr);
	plr->m_pendingBattleground = 0;
	plr->m_bg = this;
	
	if(!plr->IsPvPFlagged())
		plr->SetPvPFlag();

	/* Reset the score */
	memset(&plr->m_bgScore, 0, sizeof(BGScore));

	/* send him the world states */
	SendWorldStates(plr);
	
	/* update pvp data */
	UpdatePvPData();

	if(!m_countdownStage)
	{
		m_countdownStage = 1;
		sEventMgr.AddEvent(this, &CBattleground::EventCountdown, EVENT_BATTLEGROUND_COUNTDOWN, 30000, 0);
		sEventMgr.ModifyEventTimeLeft(this, EVENT_BATTLEGROUND_COUNTDOWN, 10000);
	}

	sEventMgr.RemoveEvents(this, EVENT_BATTLEGROUND_CLOSE);
	m_mainLock.Release();
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
	if(sWorldCreator.CreateInstance(BGMapIds[Type], 0, &mgr) == false || !mgr)
	{
		Log.Error("BattlegroundManager", "CreateInstance() call failed for map %u, type %u, level group %u", BGMapIds[Type], Type, LevelGroup);
		return NULL;		// Shouldn't happen
	}

	/* Call the create function */
	iid = ++m_maxBattlegroundId;
	bg = cfunc(mgr, iid, LevelGroup, Type);	
	mgr->m_battleground = bg;
	sEventMgr.AddEvent(bg, &CBattleground::EventCreate, EVENT_BATTLEGROUND_QUEUE_UPDATE, 1, 1);
	Log.Success("BattlegroundManager", "Created battleground type %u for level group %u.", Type, LevelGroup);
	return bg;
}

void CBattlegroundManager::DeleteBattleground(CBattleground * bg)
{
	uint32 i = bg->GetType();
	uint32 j = bg->GetLevelGroup();
	Player * plr;

	m_instanceLock.AcquireWriteLock();
	m_queueLock.Acquire();
	m_instances[i].erase(bg->GetId());
	
	/* erase any queued players */
	map<uint32, list<uint32> >::iterator itr = m_queuedPlayers[i][j].find(bg->GetId());
	list<uint32>::iterator it2;
	if(itr != m_queuedPlayers[i][j].end())
	{
		/* kill him! */
		for(it2 = itr->second.begin(); it2 != itr->second.end(); ++it2)
		{
			plr = objmgr.GetPlayer(*it2);
			if(plr)
			{
				sChatHandler.SystemMessageToPlr(plr, "Your queue on battleground instance %u is no longer valid, the instance no longer exists.", bg->GetId());
				SendBattlefieldStatus(plr, 0, 0, 0, 0);
				plr->m_bgIsQueued = false;
			}
		}
		m_queuedPlayers[i][j].erase(itr);
	}

	m_queueLock.Release();
	m_instanceLock.ReleaseWriteLock();

}

GameObject * CBattleground::SpawnGameObject(uint32 entry,uint32 MapId , float x, float y, float z, float o, uint32 flags, uint32 faction, float scale)
{
	GameObject *go = m_mapMgr->CreateGameObject();

	go->CreateFromProto(entry, MapId, x, y, z, o);

	go->SetUInt32Value(GAMEOBJECT_FACTION,faction);
	go->SetFloatValue(OBJECT_FIELD_SCALE_X,scale);	
	go->SetUInt32Value(GAMEOBJECT_FLAGS, flags);
	go->SetFloatValue(GAMEOBJECT_POS_X, x);
	go->SetFloatValue(GAMEOBJECT_POS_Y, y);
	go->SetFloatValue(GAMEOBJECT_POS_Z, z);
	go->SetFloatValue(GAMEOBJECT_FACING, o);
	go->SetInstanceID(m_mapMgr->GetInstanceID());

	return go;
}

void CBattleground::SendChatMessage(uint32 Type, uint64 Guid, const char * Format, ...)
{
	char msg[500];
	va_list ap;
	va_start(ap, Format);
	vsnprintf(msg, 500, Format, ap);
	va_end(ap);
	WorldPacket * data = sChatHandler.FillMessageData(Type, 0, msg, Guid, 0);
	DistributePacketToAll(data);
	delete data;
}

void CBattleground::DistributePacketToAll(WorldPacket * packet)
{
	m_mainLock.Acquire();
	for(int i = 0; i < 2; ++i)
	{
		for(set<Player*>::iterator itr = m_players[i].begin(); itr != m_players[i].end(); ++itr)
			(*itr)->GetSession()->SendPacket(packet);
	}
	m_mainLock.Release();
}

void CBattleground::DistributePacketToTeam(WorldPacket * packet, uint32 Team)
{
	m_mainLock.Acquire();
	for(set<Player*>::iterator itr = m_players[Team].begin(); itr != m_players[Team].end(); ++itr)
		(*itr)->GetSession()->SendPacket(packet);
	m_mainLock.Release();
}

void CBattleground::PlaySoundToAll(uint32 Sound)
{
	WorldPacket data(SMSG_PLAY_SOUND, 4);
	data << Sound;
	DistributePacketToAll(&data);
}

void CBattleground::PlaySoundToTeam(uint32 Team, uint32 Sound)
{
	WorldPacket data(SMSG_PLAY_SOUND, 4);
	data << Sound;
	DistributePacketToTeam(&data, Team);
}

void CBattlegroundManager::SendBattlefieldStatus(Player * plr, uint32 Status, uint32 Type, uint32 InstanceID, uint32 Time)
{
	WorldPacket data(SMSG_BATTLEFIELD_STATUS, 30);
	if(Status == 0)
		data << uint64(0) << uint32(0);
	else
	{
		data << uint32(0);

		if(Type < 4 || Type == 7)
			data << uint8(0) << uint8(2);
		else
			data << uint8(2) << uint8(10);

		data << Type;
		data << uint16(0x1F90);
		data << InstanceID;
		data << uint8(plr->GetTeam());
		data << Status;

		switch(Status)
		{
		case 1:					// Waiting in queue
			data << uint32(60) << uint32(0);				// Time / Elapsed time
			break;
		case 2:					// Ready to join!
			data << BGMapIds[Type] << Time;
			break;
		case 3:
			data << BGMapIds[Type] << uint32(0) << Time << uint8(1);
			break;
		}
	}

	plr->GetSession()->SendPacket(&data);
}

void CBattleground::RemovePlayer(Player * plr)
{
	WorldPacket data(SMSG_BATTLEGROUND_PLAYER_LEFT, 30);
	data << plr->GetGUID();

	m_mainLock.Acquire();
	m_players[plr->GetTeam()].erase(plr);
	DistributePacketToAll(&data);

	plr->m_bg = 0;
	memset(&plr->m_bgScore, 0, sizeof(BGScore));
	OnRemovePlayer(plr);
	
	/* teleport out */
	LocationVector vec(plr->m_bgEntryPointX, plr->m_bgEntryPointY, plr->m_bgEntryPointZ, plr->m_bgEntryPointO);
	plr->SafeTeleport(plr->m_bgEntryPointMap, plr->m_bgEntryPointInstance, vec);

	BattlegroundManager.SendBattlefieldStatus(plr, 0, 0, 0, 0);

	/* send some null world states */
	data.Initialize(SMSG_INIT_WORLD_STATES);
	data << uint32(plr->m_bgEntryPointMap) << uint32(0) << uint32(0);
	plr->GetSession()->SendPacket(&data);

	if(!m_ended && m_players[0].size() == 0 && m_players[1].size() == 0)
	{
		/* create an inactive event */
		sEventMgr.RemoveEvents(this, EVENT_BATTLEGROUND_CLOSE);						// 10mins
		sEventMgr.AddEvent(this, &CBattleground::Close, EVENT_BATTLEGROUND_CLOSE, 600000, 1);
	}

	m_mainLock.Release();
}

void CBattleground::SendPVPData(Player * plr)
{
	m_mainLock.Acquire();
	WorldPacket data(MSG_PVP_LOG_DATA, 50);
	BGScore * bs;
	data << uint8(0);
	if(m_ended)
	{
		data << uint8(1);
		data << uint8(m_winningteam);
	}
	else
	{
		data << uint8(0);		// If the game has ended - this will be 1

		data << uint32(m_players[0].size() + m_players[1].size());
		for(uint32 i = 0; i < 2; ++i)
		{
			for(set<Player*>::iterator itr = m_players[i].begin(); itr != m_players[i].end(); ++itr)
			{
				data << (*itr)->GetGUID();
				bs = &(*itr)->m_bgScore;

				data << bs->KillingBlows;
				data << bs->HonorableKills;
				data << bs->Deaths;
				data << bs->BonusHonor;
				data << bs->DamageDone;
				data << bs->HealingDone;
				data << uint32(0x2);
				data << bs->Misc1;
				data << bs->Misc2;
			}
		}
	}

	plr->GetSession()->SendPacket(&data);
	m_mainLock.Release();
}

void CBattleground::EventCreate()
{
	OnCreate();
}

int32 CBattleground::event_GetInstanceID()
{
	return m_mapMgr->GetInstanceID();
}

void CBattleground::EventCountdown()
{
	if(m_countdownStage == 1)
	{
		m_countdownStage = 2;
		SendChatMessage(CHAT_MSG_BATTLEGROUND_EVENT, 0, "One minute until the battle for %s begins!", GetName());
	}
	else if(m_countdownStage == 2)
	{
		m_countdownStage = 3;
		SendChatMessage(CHAT_MSG_BATTLEGROUND_EVENT, 0, "Thirty seconds until the battle for %s begins!", GetName());
	}
	else if(m_countdownStage == 3)
	{
		m_countdownStage = 4;
		SendChatMessage(CHAT_MSG_BATTLEGROUND_EVENT, 0, "Fifteen seconds until the battle for %s begins!", GetName());
		sEventMgr.ModifyEventTime(this, EVENT_BATTLEGROUND_COUNTDOWN, 15000);
		sEventMgr.ModifyEventTimeLeft(this, EVENT_BATTLEGROUND_COUNTDOWN, 15000);
	}
	else
	{
		SendChatMessage(CHAT_MSG_BATTLEGROUND_EVENT, 0, "The battle for %s has begun!", GetName());
		sEventMgr.RemoveEvents(this, EVENT_BATTLEGROUND_COUNTDOWN);
		Start();
	}
}

void CBattleground::Start()
{
	OnStart();
}

void CBattleground::SetWorldState(uint32 Index, uint32 Value)
{
	map<uint32, uint32>::iterator itr = m_worldStates.find(Index);
	if(itr == m_worldStates.end())
		m_worldStates.insert( make_pair( Index, Value ) );
	else
		itr->second = Value;

	WorldPacket data(SMSG_UPDATE_WORLD_STATE, 8);
	data << itr->first << itr->second;
	DistributePacketToAll(&data);
}

void CBattleground::Close()
{
	/* remove all players from the battleground */
	m_mainLock.Acquire();
	m_ended = true;
	for(uint32 i = 0; i < 2; ++i)
	{
		set<Player*>::iterator itr;
		set<uint32>::iterator it2;
		uint32 guid;
		Player * plr;
		for(itr = m_players[i].begin(); itr != m_players[i].end();)
		{
			plr = *itr;
			++itr;
			RemovePlayer(plr);
		}
        
		for(it2 = m_pendPlayers[i].begin(); it2 != m_pendPlayers[i].end();)
		{
			guid = *it2;
			++it2;
			plr = objmgr.GetPlayer(guid);

			if(plr)
				RemovePendingPlayer(plr);
			else
				m_pendPlayers[i].erase(guid);
		}
	}

	/* call the virtual onclose for cleanup etc */
	OnClose();

	/* shut down the map thread. this will delete the battleground from the corrent context. */
	m_mapMgr->SetThreadState(THREADSTATE_TERMINATE);
	m_mapMgr->delete_pending = true;

	m_mainLock.Release();
}

void CBattleground::SpawnSpiritGuide(float x, float y, float z, float o, uint32 horde)
{
	if(horde > 1)
		horde = 1;

	CreatureInfo * pInfo = CreatureNameStorage.LookupEntry(13116 + horde);
	if(pInfo == 0)
	{
		return;
	}

	Creature * pCreature = m_mapMgr->CreateCreature();

	pCreature->Create(pInfo->Name, m_mapMgr->GetMapId(), x, y, z, o);

	pCreature->SetInstanceID(m_mapMgr->GetInstanceID());
	pCreature->SetUInt32Value(OBJECT_FIELD_ENTRY, 13116 + horde);
	pCreature->SetFloatValue(OBJECT_FIELD_SCALE_X, 1.0f);

	pCreature->SetUInt32Value(UNIT_FIELD_HEALTH, 100000);
	pCreature->SetUInt32Value(UNIT_FIELD_POWER1, 4868);
	pCreature->SetUInt32Value(UNIT_FIELD_POWER3, 200);
	pCreature->SetUInt32Value(UNIT_FIELD_POWER5, 2000000);

	pCreature->SetUInt32Value(UNIT_FIELD_MAXHEALTH, 10000);
	pCreature->SetUInt32Value(UNIT_FIELD_MAXPOWER1, 4868);
	pCreature->SetUInt32Value(UNIT_FIELD_MAXPOWER3, 200);
	pCreature->SetUInt32Value(UNIT_FIELD_MAXPOWER5, 2000000);

	pCreature->SetUInt32Value(UNIT_FIELD_LEVEL, 60);
	pCreature->SetUInt32Value(UNIT_FIELD_FACTIONTEMPLATE, 84 - horde);
	pCreature->SetUInt32Value(UNIT_FIELD_BYTES_0, 0 | (2 << 8) | (1 << 16));

	pCreature->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_DISPLAY, 22802);
	pCreature->SetUInt32Value(UNIT_VIRTUAL_ITEM_INFO, 2 | (0xA << 8) | (2 << 16) | (0x11 << 24));
	pCreature->SetUInt32Value(UNIT_VIRTUAL_ITEM_INFO_01, 2);

	pCreature->SetUInt32Value(UNIT_FIELD_FLAGS, 4928);

	pCreature->SetUInt32Value(UNIT_FIELD_AURA, 22011);
	pCreature->SetUInt32Value(UNIT_FIELD_AURAFLAGS, 9);
	pCreature->SetUInt32Value(UNIT_FIELD_AURALEVELS, 0x3C);
	pCreature->SetUInt32Value(UNIT_FIELD_AURAAPPLICATIONS, 0xFF);

	pCreature->SetUInt32Value(UNIT_FIELD_BASEATTACKTIME, 2000);
	pCreature->SetUInt32Value(UNIT_FIELD_BASEATTACKTIME_01, 2000);
	pCreature->SetFloatValue(UNIT_FIELD_BOUNDINGRADIUS, 0.208f);
	pCreature->SetFloatValue(UNIT_FIELD_COMBATREACH, 1.5f);

	pCreature->SetUInt32Value(UNIT_FIELD_DISPLAYID, 13337 + horde);
	pCreature->SetUInt32Value(UNIT_FIELD_NATIVEDISPLAYID, 13337 + horde);

	pCreature->SetUInt32Value(UNIT_CHANNEL_SPELL, 22011);
	pCreature->SetUInt32Value(UNIT_MOD_CAST_SPEED, 1065353216);

	pCreature->SetUInt32Value(UNIT_NPC_FLAGS, 64);
	pCreature->SetUInt32Value(UNIT_FIELD_BYTES_2, 1 | (0x10 << 8));

	pCreature->DisableAI();
	pCreature->PushToWorld(m_mapMgr);
}

void CBattleground::QueuePlayerForResurrect(Player * plr)
{
	m_resurrectQueue[plr->GetTeam()].insert(plr->GetGUIDLow());
}

#define RESURRECT_SPELL 21074   // Spirit Healer Res
void CBattleground::EventResurrectPlayers()
{
	Player * plr;
	set<uint32>::iterator itr;
	WorldPacket data(50);
	for(uint32 i = 0; i < 2; ++i)
	{
		for(itr = m_resurrectQueue[i].begin(); itr != m_resurrectQueue[i].end(); ++itr)
		{
			plr = m_mapMgr->GetPlayer(*itr);
			if(plr && plr->isDead())
			{
                data.Initialize(SMSG_SPELL_START);
				data << plr->GetNewGUID() << plr->GetNewGUID() << uint32(RESURRECT_SPELL) << uint16(0) << uint32(0) << uint16(2) << plr->GetGUID();
				plr->SendMessageToSet(&data, true);

				data.Initialize(SMSG_SPELL_GO);
				data << plr->GetNewGUID() << plr->GetNewGUID() << uint32(RESURRECT_SPELL) << uint8(0) << uint8(1) << uint8(1) << plr->GetGUID() << uint8(0) << uint16(2)
					<< plr->GetGUID();
				plr->SendMessageToSet(&data, true);

				plr->ResurrectPlayer();
				plr->SetUInt32Value(UNIT_FIELD_HEALTH, plr->GetUInt32Value(UNIT_FIELD_HEALTH));
				plr->SetUInt32Value(UNIT_FIELD_POWER1, plr->GetUInt32Value(UNIT_FIELD_MAXPOWER1));
				plr->SetUInt32Value(UNIT_FIELD_POWER4, plr->GetUInt32Value(UNIT_FIELD_MAXPOWER4));
			}
		}
		m_resurrectQueue[i].clear();
	}
	m_lastResurrect = World::UNIXTIME;
}
