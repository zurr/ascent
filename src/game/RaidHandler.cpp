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
	//		  SMSG_RAID_INSTANCE_INFO			 = 716,  //(0x2CC)	
	sInstanceSavingManager.BuildRaidSavedInstancesForPlayer(_player);
}

void WorldSession::HandleReadyCheckOpcode(WorldPacket& recv_data)
{
	Group * pGroup  = _player->GetGroup();
	WorldPacket data(CMSG_RAID_READYCHECK, 20);
	uint8 ready;

	if(!pGroup || ! _player->IsInWorld())
		return;

	if(recv_data.size() == 0)
	{
		if(pGroup->GetLeader() == _player)
		{
			/* send packet to group */
			pGroup->SendPacketToAllButOne(&data, _player);
		}
		else
		{
			SendNotification(NOTIFICATION_MESSAGE_NO_PERMISSION);
		}
	}
	else
	{
		if(_player != pGroup->GetLeader())
		{
			recv_data >> ready;
			data << _player->GetGUID();
			data << ready;
			pGroup->GetLeader()->GetSession()->SendPacket(&data);
		}
	}
}

