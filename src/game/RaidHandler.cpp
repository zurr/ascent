/****************************************************************************
 *
 * General Packet Handler File
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

void WorldSession::HandleConvertGroupToRaidOpcode(WorldPacket & recv_data)
{
    if(!_player->IsInWorld()) return;
    // This is just soooo easy now   
    Group *pGroup = _player->GetGroup();
    if(!pGroup) return;

    if ( pGroup->GetLeader() != _player )   //access denied
    {
        SendPartyCommandResult(_player, 0, "", ERR_PARTY_YOU_ARE_NOT_LEADER);
        return;
    }

    pGroup->ExpandToRaid();
    SendPartyCommandResult(_player, 0, "", ERR_PARTY_NO_ERROR);
}

void WorldSession::HandleGroupChangeSubGroup(WorldPacket & recv_data)
{
    if(!_player->IsInWorld()) return;
    std::string name;
    uint8 subGroup;

    recv_data >> name;
    recv_data >> subGroup;

    Player *plyr = objmgr.GetPlayer(name.c_str());
    if(!plyr)
        return;

    Group *pGroup = _player->GetGroup();
    if(!pGroup)
        return;

    pGroup->MovePlayer(plyr, subGroup);
}

void WorldSession::HandleGroupAssistantLeader(WorldPacket & recv_data)
{
    if(!_player->IsInWorld()) return;
    //80

    std::string name;
    uint8 subGroup;

    recv_data >> name;
    recv_data >> subGroup;

    Player *plyr = objmgr.GetPlayer(name.c_str());
    if(!plyr)
        return;

    Group *pGroup = _player->GetGroup();
    if(!pGroup) return;

    pGroup->SetSubGroupLeader(plyr,subGroup);
}

void WorldSession::HandleRequestRaidInfoOpcode(WorldPacket & recv_data)
{  
    //          SMSG_RAID_INSTANCE_INFO             = 716,  //(0x2CC)    
    sInstanceSavingManager.BuildRaidSavedInstancesForPlayer(_player);
}

void WorldSession::HandleReadyCheckOpcode(WorldPacket& recv_data)
{
    // Seems to be looping. Will re-enable later...

    /*
    Raid *raid = _player->GetCurrentRaid();
    if(!raid) return;
    if(raid->GetRaidLeader()->GetGUID() == _player->GetGUID())
    {
        // Forward the packet onto all raid members. This will cause the box to come up, yes/no.
        WorldPacket *data = new WorldPacket;
        data->Initialize(CMSG_RAID_READYCHECK);
        for(int i=0;i<8;i++)
        {
            RaidGroup *group = raid->GetRaidGroup(i);
            for(std::set<Player*>::iterator it = group->SubGroupMembers.begin(); it!=group->SubGroupMembers.end(); ++it)
                if((*it)->GetGUID() != _player->GetGUID()) (*it)->GetSession()->SendPacket(data);       // dont send to self
        }
        delete data;
    } else {
        uint8 ready = 0;
        if(recv_data.size() >= 1) recv_data >> ready;
        // Build a packet, and send to all (but self, again)

        WorldPacket *data = new WorldPacket;
        data->Initialize(CMSG_RAID_READYCHECK);
        *data << _player->GetGUID() << ready;
        for(int i=0;i<8;i++)
        {
            RaidGroup *group = raid->GetRaidGroup(i);
            for(std::set<Player*>::iterator it = group->SubGroupMembers.begin(); it!=group->SubGroupMembers.end(); ++it)
                if((*it)->GetGUID() != _player->GetGUID()) (*it)->GetSession()->SendPacket(data);       // dont send to self
        }
        delete data;
    }*/
}
