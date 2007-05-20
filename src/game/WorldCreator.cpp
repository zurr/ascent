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

//
// WorldCreator.cpp
//

#include "StdAfx.h"

initialiseSingleton( WorldCreator );
//initialiseSingleton( InstanceSavingManagement );

WorldCreator::WorldCreator(TaskList * tl)
{
    // Create all non-instance type maps.
    QueryResult *result;
    m_InstanceHigh = 0;

    result = sDatabase.Query( "SELECT MAX(instanceid) FROM instances" );
    if( result )
    {
        m_InstanceHigh = result->Fetch()[0].GetUInt32()+1;

        delete result;
    }

    for(World::MapInfoMap::iterator itr = sWorld.GetMapInfoBegin(); itr != sWorld.GetMapInfoEnd(); ++itr)
    {
        MapInfo * pInfo = itr->second;
        if(pInfo->type == INSTANCE_NULL)
        {
            // Create it (we're a non-instance).
            // Add a loading task for this map.
            tl->AddTask(new CallbackP1<WorldCreator, uint32>(WorldCreator::getSingletonPtr(), &WorldCreator::CreateMap, pInfo->mapid));
        }
    }
}

WorldCreator::~WorldCreator()
{
    std::map<uint32, Map*>::iterator it;

    for(it = _maps.begin(); it != _maps.end(); it++)
    {
        delete it->second;
    }
}

Map* WorldCreator::_CreateMap(uint32 mapid)
{
    std::map<uint32, Map*>::iterator it;
    it = _maps.find(mapid);
    ASSERT(it == _maps.end());

    Map *newmap = new Map(mapid);
    ASSERT(newmap);

    _maps.insert( std::pair<uint32, Map*>(mapid, newmap) );

    return newmap;
}

Map* WorldCreator::GetMap(uint32 mapid)
{
    std::map<uint32, Map*>::iterator it;
    it = _maps.find(mapid);

    if (it != _maps.end())
        return it->second;
    else
        return _CreateMap(mapid);
}

MapMgr* WorldCreator::GetInstance(uint32 mapid, Object* obj)
{
    // check inactive instances.
    if(obj->GetInstanceID() > 2)
    {
        InactiveInstance * ia = sInstanceSavingManager.GetInactiveInstance(obj->GetInstanceID());
        if(ia != 0)
        {
            //create that inactive instance.
            //extra, it now checks if the instance should expire.
            MapInfo *pMapInfo = sWorld.GetMapInformation(ia->MapId);
            if(pMapInfo)
            {
                if(time(NULL) > (ia->Creation) + (pMapInfo ? pMapInfo->cooldown : 604800))
                {
                    sInstanceSavingManager.RemoveSavedInstance(ia->MapId,ia->InstanceId,true);
                    sInstanceSavingManager.RemoveSavedInstance(ia->InstanceId);
                }
                else
                {
                    MapMgr * dm = 0;
                    CreateInstance(NULL, NULL, ia->MapId, ia->InstanceId, ia->Creation, &dm);
                    obj->SetMapId(ia->MapId);
                    delete ia;
                    return dm;
                }
            }
            else
            {
                MapMgr * dm = 0;
                CreateInstance(NULL, NULL, ia->MapId, ia->InstanceId, ia->Creation, &dm);
                obj->SetMapId(ia->MapId);
                delete ia;
                return dm;
            }
        }
    }

    Map* mp = GetMap(mapid);
    if(!mp) return NULL;
    return mp->GetInstance(obj);
}

MapMgr* WorldCreator::GetInstance(uint32 mapid, uint32 instanceId)
{
    // check inactive instances.
    if(instanceId > 2)
    {
        InactiveInstance * ia = sInstanceSavingManager.GetInactiveInstance(instanceId);
        if(ia != 0)
        {
            // create that inactive instance.
            MapInfo *pMapInfo = sWorld.GetMapInformation(ia->MapId);
            if(pMapInfo)
            {
                if(time(NULL) > (ia->Creation) + (pMapInfo ? pMapInfo->cooldown : 604800))
                {
                    sInstanceSavingManager.RemoveSavedInstance(ia->MapId,ia->InstanceId,true);
                    sInstanceSavingManager.RemoveSavedInstance(ia->InstanceId);
                }
                else
                {
                    MapMgr * dm = 0;
                    CreateInstance(NULL, NULL, ia->MapId, ia->InstanceId, ia->Creation, &dm);
                    delete ia;
                    return dm;
                }
            }
            else
            {
                MapMgr * dm = 0;
                CreateInstance(NULL, NULL, ia->MapId, ia->InstanceId, ia->Creation, &dm);
                delete ia;
                return dm;
            }
        }
    }

    Map* mp = GetMap(mapid);
    if(!mp) return NULL;
    return mp->GetInstance(instanceId);
}

MapMgr* WorldCreator::GetInstance(uint32 instanceId)
{
    // check inactive instances.
    if(instanceId > 2)
    {
        InactiveInstance * ia = sInstanceSavingManager.GetInactiveInstance(instanceId);
        if(ia != 0)
        {
            // create that inactive instance.
            MapInfo *pMapInfo = sWorld.GetMapInformation(ia->MapId);
            if(pMapInfo)
            {
                if(time(NULL) > (ia->Creation) + (pMapInfo ? pMapInfo->cooldown : 604800))
                {
                    sInstanceSavingManager.RemoveSavedInstance(ia->MapId,ia->InstanceId,true);
                    sInstanceSavingManager.RemoveSavedInstance(ia->InstanceId);
                }
                else
                {
                    MapMgr * dm = 0;
                    CreateInstance(NULL, NULL, ia->MapId, ia->InstanceId, ia->Creation, &dm);
                    delete ia;
                    return dm;
                }
            }
            else
            {
                MapMgr * dm = 0;
                CreateInstance(NULL, NULL, ia->MapId, ia->InstanceId, ia->Creation, &dm);
                delete ia;
                return dm;
            }
        }
    }

    std::map<uint32, Map*>::iterator it;
    MapMgr *instance;
    for(it = _maps.begin(); it != _maps.end(); it++)
    {
        instance = it->second->InstanceExists(instanceId);
        if(instance != NULL)
            return instance;

    }
    return NULL;
}

void WorldCreator::CreateBattlegroundInstance(Battleground* m_Battleground)
{
    Map* instance_map = GetMap(m_Battleground->GetMapId());
    // Create a new instance of this battleground.
    ASSERT(instance_map);
    
    uint32 instanceId = GenerateInstanceID();
    m_Battleground->SetInstanceID(instanceId);

    MapMgr * mapMgr = instance_map->CreateMapMgrInstance(instanceId);
    ASSERT(mapMgr);
    mapMgr->m_battleground = m_Battleground;

    m_Battleground->SetMapMgr(mapMgr);
    m_Battleground->SetMap(instance_map);
}

void WorldCreator::DestroyBattlegroundInstance(Battleground* m_Battleground)
{
    Map* instance_map = GetMap(m_Battleground->GetMapId());
    ASSERT(instance_map != NULL);

    // this kills mapmgr
    instance_map->DestroyMapMgrInstance(m_Battleground->GetInstanceID());

    // null out remaining ptr's
    m_Battleground->SetMapMgr(NULL);
    m_Battleground->SetInstanceID(0);
}

void WorldCreator::DestroyBattlegroundInstance(uint32 mapid, uint32 instanceid)
{
    Map* instance_map = GetMap(mapid);
    ASSERT(instance_map != NULL);

    // this kills mapmgr
    instance_map->DestroyMapMgrInstance(instanceid);
}


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Instance Manager
// instance creation and basic management
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++

bool WorldCreator::CheckInstanceForObject(Object *obj, MapInfo *pMapinfo)
{
    WorldPacket data(4);
    Player *_player = NULL;

    if(obj->GetTypeId() == TYPEID_PLAYER)
    {
        _player = static_cast<Player*>(obj);
    }

    if(pMapinfo && _player)
    {
        switch(pMapinfo->type)
        {
            case INSTANCE_NONRAID:
            case INSTANCE_RAID:
                {
                    //instance creation detection types
                    //case 1, player is inside a group aka not soloing
                    //onoes: leader can be inside a instance already and make a group or its a fresh group, noone inside
                     
                     if(_player->InGroup()) //group created, join leader instance.
                     {
                         Group *pGroup = _player->GetGroup();
                         if(pGroup)
                         {
                             //check if instance already exists(player solo created instance and made group after)
                             MapMgr *pInstance = sWorldCreator.GetInstanceByGroup(pGroup, _player, pMapinfo);
                             if(pInstance)
                             {
                                 //INSTANCE_OK
                                 if(pInstance->IsCombatInProgress())
                                 {
                                    WorldPacket msg;
                                    msg.Initialize(SMSG_AREA_TRIGGER_MESSAGE);
                                    msg << uint32(0) << "Encounter in progress." << uint8(0) << uint8(0);
                                    _player->GetSession()->SendPacket(&msg);
                                    return false;
                                 }
                                 if(pInstance->GetPlayerCount() >= pMapinfo->playerlimit)
                                 {
                                    data.Initialize(SMSG_TRANSFER_ABORTED);
                                    data << uint32(INSTANCE_ABORT_FULL);
                                    _player->GetSession()->SendPacket(&data);
                                    return false;
                                 }
                                 _player->SetInstanceID(pInstance->GetInstanceID());
                             }
                             else
                             {
                                //group leader didnt made any instance yet, create instance for this group.
                                uint32 id = sWorldCreator.CreateInstance(pGroup, pGroup->GetLeader(), pMapinfo->mapid);
                                if(id != NULL)
                                {
                                    //INSTANCE CREATED
                                    _player->SetInstanceID(id);
                                }
                                else
                                {
                                    data.Initialize(SMSG_TRANSFER_ABORTED);
                                    data << uint32(INSTANCE_ABORT_ERROR);
                                    _player->GetSession()->SendPacket(&data);
                                    return false;
                                }
                             }
                         }
                         else
                         {
                             data.Initialize(SMSG_TRANSFER_ABORTED);
                             data << uint32(INSTANCE_ABORT_ERROR);
                             _player->GetSession()->SendPacket(&data);
                             return false;
                         }
                     }
                     else
                     {
                         MapMgr *pInstance = sWorldCreator.GetInstanceByCreator(_player, pMapinfo);
                         if(pInstance)
                         {
                            //INSTANCE_OK
                            if(pInstance->IsCombatInProgress())
                            {
                                WorldPacket msg;
                                msg.Initialize(SMSG_AREA_TRIGGER_MESSAGE);
                                msg << uint32(0) << "Encounter in progress." << uint8(0) << uint8(0);
                                _player->GetSession()->SendPacket(&msg);
                                return false;
                            }
                            if(pInstance->GetPlayerCount() >= pMapinfo->playerlimit)
                            {
                                data.Initialize(SMSG_TRANSFER_ABORTED);
                                data << uint32(INSTANCE_ABORT_FULL);
                                _player->GetSession()->SendPacket(&data);
                                return false;
                            }
                             _player->SetInstanceID(pInstance->GetInstanceID());
                         }
                         else
                         {
                             uint32 id2 = sWorldCreator.CreateInstance(NULL, _player, pMapinfo->mapid);
                             if(id2 != NULL)
                             {
                                _player->SetInstanceID(id2);
                             }
                             else
                             {
                                data.Initialize(SMSG_TRANSFER_ABORTED);
                                data << uint32(INSTANCE_ABORT_ERROR);
                                _player->GetSession()->SendPacket(&data);
                                return false;
                             }
                         }
                     }
                }break;
            case INSTANCE_MULTIMODE:
                {
                    return false;
                }break;
        }
    }
    return true;
}

uint32 WorldCreator::CreateInstance(Group *pGroup, Player *pPlayer, uint32 mapid, uint32 instanceid, uint32 creation, MapMgr ** destptr)
{
    if(pGroup == NULL && pPlayer == NULL && instanceid == 0)
    {
        return INSTANCE_ABORT_ERROR;
    }

    Map* instance_map = GetMap(mapid);
    ASSERT(instance_map != NULL);

    MapMgr * pInstance = instance_map->CreateMapMgrInstance(instanceid);
    ASSERT(pInstance);
    if(creation)
    {
        pInstance->SetNewExpireTime(creation);
    }
    if(pGroup)
    {
        pInstance->SetGroupSignature(pGroup->GetID());
    }
    if(pPlayer)
    {
        pInstance->SetCreator(pPlayer);
    }

    Instance_Map_Info_Holder *pMapHolder = sInstanceSavingManager.SaveInstance(pInstance); //first time instance saving holder
    if(destptr)
        *destptr = pInstance;

    return pInstance->GetInstanceID();
}

MapMgr *WorldCreator::GetInstanceByGroup(Group *pGroup, Player *pPlayer, MapInfo *pMapInfo)
{
    MapMgr * mgr = GetMap(pMapInfo->mapid)->GetInstanceByGroup(pGroup, pPlayer);
    if(mgr)
    {
        return mgr;
    }
    else
    {
        bool result =  sInstanceSavingManager.IsPlayerSavedToMap(pMapInfo->mapid, pPlayer);
        if(result)
        {
            bool result = sInstanceSavingManager.IsPlayerSavedToMap(pMapInfo->mapid, pPlayer);
            if(result)
            {
                Instance_Map_InstanceId_Holder *p = sInstanceSavingManager.GetRaidInstance(pMapInfo->mapid, pPlayer);
                if(p)
                {
                    InactiveInstance * ia = sInstanceSavingManager.GetInactiveInstance(p->GetInstanceID());
                    if(ia != 0)
                    {
                        //create that inactive instance.
                        //extra, it now checks if the instance should expire.
                        MapInfo *pMapInfo = sWorld.GetMapInformation(ia->MapId);
                        if(pMapInfo)
                        {
                            if(time(NULL) > (ia->Creation) + (pMapInfo ? pMapInfo->cooldown : 604800))
                            {
                                sInstanceSavingManager.RemoveSavedInstance(ia->MapId,ia->InstanceId,true);
                                sInstanceSavingManager.RemoveSavedInstance(ia->InstanceId);
                            }
                            else
                            {
                                MapMgr * dm = 0;
                                CreateInstance(NULL, NULL, ia->MapId, ia->InstanceId, ia->Creation, &dm);
                                delete ia;
                                return dm;
                            }
                        }
                        else
                        {
                            MapMgr * dm = 0;
                            CreateInstance(NULL, NULL, ia->MapId, ia->InstanceId, ia->Creation, &dm);
                            delete ia;
                            return dm;
                        }
                    }

                }
                
            }
        }
    }
    return NULL;
}

MapMgr *WorldCreator::GetInstanceByCreator(Player *pCreator, MapInfo *pMapInfo)
{
    MapMgr *mgr = GetMap(pMapInfo->mapid)->GetInstanceByCreator(pCreator);
    if(mgr)
    {
        return mgr;
    }
    else
    {
        bool result = sInstanceSavingManager.IsPlayerSavedToMap(pMapInfo->mapid, pCreator);
        if(result)
        {
            Instance_Map_InstanceId_Holder *p = sInstanceSavingManager.GetRaidInstance(pMapInfo->mapid, pCreator);
            if(p)
            {
                InactiveInstance * ia = sInstanceSavingManager.GetInactiveInstance(p->GetInstanceID());
                if(ia != 0)
                {
                    //create that inactive instance.
                    //extra, it now checks if the instance should expire.
                    MapInfo *pMapInfo = sWorld.GetMapInformation(ia->MapId);
                    if(pMapInfo)
                    {
                        if(time(NULL) > (ia->Creation) + (pMapInfo ? pMapInfo->cooldown : 604800))
                        {
                            sInstanceSavingManager.RemoveSavedInstance(ia->MapId,ia->InstanceId,true);
                            sInstanceSavingManager.RemoveSavedInstance(ia->InstanceId);
                        }
                        else
                        {
                            MapMgr * dm = 0;
                            CreateInstance(NULL, NULL, ia->MapId, ia->InstanceId, ia->Creation, &dm);
                            delete ia;
                            return dm;
                        }
                    }
                    else
                    {
                        MapMgr * dm = 0;
                        CreateInstance(NULL, NULL, ia->MapId, ia->InstanceId, ia->Creation, &dm);
                        delete ia;
                        return dm;
                    }
                }

            }
            
        }
    }
    return NULL;
}

uint32 WorldCreator::GenerateInstanceID()
{
    m_InstanceHigh++;
    return m_InstanceHigh;
}

void WorldCreator::DeleteInstance(uint32 instanceid, uint32 mapid)
{
    _busy.Acquire();
    MapMgr * mapMgr = NULL;
    mapMgr = GetMap(mapid)->GetRawInstance(instanceid);

    if(mapMgr)
    {
        if(!mapMgr->HasPlayers())
        {
            GetMap(mapid)->DestroyMapMgrInstance(instanceid);
            sLog.outDebug("Instance %i on map %i deleted with success\n", instanceid, mapid);
        }
    }
    else
    {
        sLog.outDebug("Instance %i failed to be deleted, reason: Doesnt Exist\n", instanceid);
    }
    _busy.Release();
}

void WorldCreator::InstanceSoftReset(MapMgr *mMapMgr)
{
    uint32 mapid = mMapMgr->GetMapId();
    uint32 instanceid = mMapMgr->GetInstanceID();
    //delete public instance WoWInstance to avoid "rats".
    DeleteInstance(mMapMgr->GetInstanceID(), mapid);
    //delete instance save data
    sInstanceSavingManager.RemoveSavedInstance(mapid, instanceid, false);
}

void WorldCreator::InstanceHardReset(MapMgr *mMapMgr)
{
    uint32 mapid = mMapMgr->GetMapId();
    uint32 instanceid = mMapMgr->GetInstanceID();
    //delete public instance WoWInstance to avoid "rats".
    DeleteInstance(mMapMgr->GetInstanceID(), mapid);
    //delete instance save data
    sInstanceSavingManager.RemoveSavedInstance(mapid, instanceid, true);
}

MapMgr * WorldCreator::GetInstanceByGroupInstanceId(uint32 InstanceID, uint32 mapid, bool Lock)
{
    if(InstanceID > 2)
    {
        InactiveInstance * ia = sInstanceSavingManager.GetInactiveInstance(InstanceID);
        if(ia != 0)
        {
            // create that inactive instance.
            MapInfo *pMapInfo = sWorld.GetMapInformation(ia->MapId);
            if(pMapInfo)
            {
                if(time(NULL) > (ia->Creation) + (pMapInfo ? pMapInfo->cooldown : 604800))
                {
                    sInstanceSavingManager.RemoveSavedInstance(ia->MapId,ia->InstanceId,true);
                    sInstanceSavingManager.RemoveSavedInstance(ia->InstanceId);
                }
                else
                {
                    MapMgr * dm = 0;
                    CreateInstance(NULL, NULL, ia->MapId, ia->InstanceId, ia->Creation, &dm);
                    delete ia;
                    return dm;
                }
            }
            else
            {
                MapMgr * dm = 0;
                CreateInstance(NULL, NULL, ia->MapId, ia->InstanceId, ia->Creation, &dm);
                delete ia;
                return dm;
            }
        }
    }

    return GetMap(mapid)->GetInstanceByGroupInstanceId(InstanceID, Lock);
}

void WorldCreator::BuildXMLStats(char * m_file)
{
    _busy.Acquire();
    map<uint32, Map*>::iterator itr = _maps.begin();
    for(; itr != _maps.end(); ++itr)
    {
        itr->second->BuildXMLStats(m_file);
    }
    _busy.Release();
}
