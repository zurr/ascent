/****************************************************************************
 *
 * Areatrigger Handler
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
void WorldSession::HandleAreaTriggerOpcode(WorldPacket & recv_data)
{
	 if(!_player->IsInWorld()) return;
    CHECK_PACKET_SIZE(recv_data, 4);
    uint32 id ;
    recv_data >> id;
	_HandleAreaTriggerOpcode(id);
}

void WorldSession::_HandleAreaTriggerOpcode(uint32 id)
{    
	sLog.outDebug("AreaTrigger: %u", id);

    WorldPacket data(80);
    AreaTrigger * pAreaTrigger = sWorld.GetAreaTrigger(id);

    // Search quest log, find any exploration quests
    sQuestMgr.OnPlayerExploreArea(GetPlayer(),id);
    
    // if in BG handle is triggers
    if(GetPlayer()->m_bgInBattleground && GetPlayer()->GetCurrentBattleground() != NULL)
    {
        GetPlayer()->GetCurrentBattleground()->HandleBattlegroundAreaTrigger(GetPlayer(), id);
        return;
    }   

    if(pAreaTrigger && pAreaTrigger->Type == ATTYPE_BATTLEGROUND)
    {
        if(pAreaTrigger->Mapid == 489)        // hack fix
            pAreaTrigger->Mapid = 2;
        else if(pAreaTrigger->Mapid == 529)
            pAreaTrigger->Mapid = 3;
        else if(pAreaTrigger->Mapid == 30)
            pAreaTrigger->Mapid = 1;
            
        WorldPacket *pkt = sBattlegroundMgr.BuildBattlegroundListPacket(GetPlayer()->GetGUID(), _player,
            pAreaTrigger->Mapid);
        SendPacket(pkt);
        delete pkt;
        return;
    }

    if(pAreaTrigger)
    {
        bool bFailedPre = false;
        std::string failed_reason;

        if(pAreaTrigger->required_level)
        {
            if(GetPlayer()->getLevel() < pAreaTrigger->required_level)
            {
                bFailedPre = true;
                if(failed_reason.size() > 0)
                    failed_reason += ", and ";
                else
                    failed_reason = "You must be ";

                // mm hacky
                char lvltext[30];
                sprintf(lvltext, "at least level %d", pAreaTrigger->required_level);
                failed_reason += lvltext;
            }
        }
        if(bFailedPre)
        {
            failed_reason += " before you're allowed through here.";
            WorldPacket msg;
            msg.Initialize(SMSG_AREA_TRIGGER_MESSAGE);
            msg << uint32(0) << failed_reason << uint8(0);
            SendPacket(&msg);
            sLog.outDebug("Player %s failed areatrigger prereq - %s", GetPlayer()->GetName(), failed_reason.c_str());
            return;
        }
        switch(pAreaTrigger->Type)
        {
        case ATTYPE_INSTANCE:
            {
                if(GetPlayer()->GetPlayerStatus() != TRANSFER_PENDING) //only ports if player is out of pendings
                {
                    GetPlayer()->SaveEntryPoint();
                    //death system check.
                    Corpse *pCorpse = NULL;
//                    CorpseData *pCorpseData = NULL;
                    MapInfo *pMapinfo = NULL;

                    pMapinfo = sWorld.GetMapInformation(pAreaTrigger->Mapid);
                    if(pMapinfo && !pMapinfo->HasFlag(WMI_INSTANCE_ENABLED))
                    {
                        WorldPacket msg;
                        msg.Initialize(SMSG_AREA_TRIGGER_MESSAGE);
                        msg << uint32(0) << "This instance is currently unavailable." << uint8(0) << uint8(0);
                        SendPacket(&msg);
                        return;
                    }

                    if(pMapinfo && pMapinfo->HasFlag(WMI_INSTANCE_XPACK_01) && !HasFlag(ACCOUNT_FLAG_XPACK_01))
                    {
                        WorldPacket msg;
                        msg.Initialize(SMSG_BROADCAST_MSG);
                        msg << uint32(3) << "You must have The Burning Crusade Expansion to access this content." << uint8(0);
                        SendPacket(&msg);
                        return;
                    }

                    if(pMapinfo && pMapinfo->type != INSTANCE_MULTIMODE && GetPlayer()->iInstanceType == INSTANCE_MULTIMODE)
                    {
                        WorldPacket msg;
                        msg.Initialize(SMSG_AREA_TRIGGER_MESSAGE);
                        msg << uint32(0) << "Heroic mode is not available for this instance." << uint8(0) << uint8(0);
                        SendPacket(&msg);
                        return;
                    }

                    if(pMapinfo && pMapinfo->type == INSTANCE_RAID && GetPlayer()->InGroup() && GetPlayer()->GetGroup()->GetGroupType() != GROUP_TYPE_RAID)
                    {
                        WorldPacket msg;
                        msg.Initialize(SMSG_AREA_TRIGGER_MESSAGE);
                        msg << uint32(0) << "You need to be in a raid group to be able to enter this instance." << uint8(0) << uint8(0);
                        SendPacket(&msg);
                        return;
                    }
                    if(pMapinfo && pMapinfo->type == INSTANCE_RAID && !GetPlayer()->InGroup())
                    {
                        WorldPacket msg;
                        msg.Initialize(SMSG_AREA_TRIGGER_MESSAGE);
                        msg << uint32(0) << "You need to be in a raid group to be able to enter this instance." << uint8(0) << uint8(0);
                        SendPacket(&msg);
                        return;
                    }

                    if(!GetPlayer()->isAlive())
                    {
                        pCorpse = objmgr.GetCorpseByOwner(GetPlayer()->GetGUIDLow());
                        if(pCorpse)
                        {
                            pMapinfo = sWorld.GetMapInformation(pCorpse->GetMapId());
                            if(pMapinfo)
                            {
                                if(pMapinfo->type != INSTANCE_NULL && pMapinfo->type != INSTANCE_PVP  && pMapinfo->type != INSTANCE_NONRAID && GetPlayer()->GetMapId() != pCorpse->GetMapId() && pCorpse->GetMapId() == pAreaTrigger->Mapid  && !GetPlayer()->InGroup())
                                {
                                    GetPlayer()->ResurrectPlayer();
                                    return;
                                }
                                else if(pMapinfo->type != INSTANCE_NONRAID)
                                {
                                    GetPlayer()->RepopAtGraveyard(GetPlayer()->GetPositionX(),GetPlayer()->GetPositionY(),GetPlayer()->GetPositionZ(), GetPlayer()->GetMapId());
                                    return;
                                }
                            }
                        }
                        else
                        {
                            GetPlayer()->RepopAtGraveyard(GetPlayer()->GetPositionX(),GetPlayer()->GetPositionY(),GetPlayer()->GetPositionZ(),GetPlayer()->GetMapId());
                            return;
                        }
                    }
                    bool result = sWorldCreator.CheckInstanceForObject(static_cast<Object*>(_player), pMapinfo);
                    if(result)
                    {
                        _player->SaveEntryPoint();
                        _player->SafeTeleport(pAreaTrigger->Mapid, 0, LocationVector(pAreaTrigger->x, pAreaTrigger->y, pAreaTrigger->z, pAreaTrigger->o));
                    }
                }
            }break;
        case ATTYPE_QUESTTRIGGER:
            {

            }break;
        case ATTYPE_INN:
            {
                // Inn
                if (!GetPlayer()->m_isResting) GetPlayer()->ApplyPlayerRestState(true);
            }break;
        case ATTYPE_TELEPORT:
            {
                if(GetPlayer()->GetPlayerStatus() != TRANSFER_PENDING) //only ports if player is out of pendings
                {
                    _player->SaveEntryPoint();
                    _player->SafeTeleport(pAreaTrigger->Mapid, 0, LocationVector(pAreaTrigger->x, pAreaTrigger->y, pAreaTrigger->z, pAreaTrigger->o));
                }
            }break;
        case ATTYPE_NULL:
            {
                MapInfo *pMapinfo = NULL;
                pMapinfo = sWorld.GetMapInformation(pAreaTrigger->Mapid);
                if(pMapinfo && pMapinfo->HasFlag(WMI_INSTANCE_XPACK_01) && !HasFlag(ACCOUNT_FLAG_XPACK_01))
                {
                    WorldPacket msg;
                    msg.Initialize(SMSG_BROADCAST_MSG);
                    msg << uint32(3) << "You must have The Burning Crusade Expansion to access this content." << uint8(0);
                    SendPacket(&msg);
                    return;
                }
            }
        default:break;
        }
    }
}

void World::LoadAreaTriggerInformation()
{
    sLog.outString("  Loading Area Triggers...");
    QueryResult *result = NULL;
//    uint32 MaxID = 0;
    AreaTrigger *pAT = NULL;

    result = sDatabase.Query("SELECT * FROM areatriggers");

    if(result==NULL)
    {
        sLog.outString("  Query failed.");
        return;
    }

    do
    {
        Field *fields = result->Fetch();
        pAT = new AreaTrigger;

        pAT->AreaTriggerID = fields[0].GetUInt32();
        pAT->Type = (uint8)fields[1].GetUInt32();
        pAT->Mapid = fields[2].GetUInt32();
        pAT->PendingScreen = fields[3].GetUInt32();
        pAT->Name= fields[4].GetString();
        pAT->x = fields[5].GetFloat();
        pAT->y = fields[6].GetFloat();
        pAT->z = fields[7].GetFloat();
        pAT->o = fields[8].GetFloat();
        if(result->GetFieldCount() > 9) // bur: crash fix for lazy bastards who dont update their db >.>
        {
            pAT->required_honor_rank = fields[9].GetUInt32();
            pAT->required_level = fields[10].GetUInt32();
        }

        AddAreaTrigger(pAT);
    } while( result->NextRow() );
    delete result;
}

AreaTrigger *World::GetAreaTrigger(uint32 id)
{
    AreaTriggerMap::iterator iter=m_AreaTrigger.find(id);
	if(iter!= m_AreaTrigger.end())
		return iter->second;
	else
		return NULL;
}
void World::AddAreaTrigger(AreaTrigger *pArea)
{
    ASSERT( pArea->AreaTriggerID );
    ASSERT( m_AreaTrigger.find(pArea->AreaTriggerID) == m_AreaTrigger.end() );

    m_AreaTrigger[pArea->AreaTriggerID] = pArea;
}

