/****************************************************************************
 *
 * Battleground Handling
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

void WorldSession::HandleBattlefieldPortOpcode(WorldPacket &recv_data)
{
	if(!_player->IsInWorld()) return;

	/* Usually the fields in the packet would've been used to check what instance we're porting into, however since we're not
	 * doing "queue multiple battleground types at once" we can just use our cached pointer in the player class. - Burlex
	 */

	if(_player->m_pendingBattleground)
		_player->m_pendingBattleground->PortPlayer(_player);
}

void WorldSession::HandleBattlefieldStatusOpcode(WorldPacket &recv_data)
{
	/* This is done based on whether we are queued, inside, or not in a battleground.
	 */
	if(_player->m_pendingBattleground)		// Ready to port
		BattlegroundManager.SendBattlefieldStatus(_player, 2, _player->m_pendingBattleground->GetType(), _player->m_pendingBattleground->GetId(), 120000);
	else if(_player->m_bg)					// Inside a bg
		BattlegroundManager.SendBattlefieldStatus(_player, 3, _player->m_bg->GetType(), _player->m_bg->GetId(), World::UNIXTIME - _player->m_bg->GetStartTime());
	else									// None
		BattlegroundManager.SendBattlefieldStatus(_player, 0, 0, 0, 0);	
}

void WorldSession::HandleBattlefieldListOpcode(WorldPacket &recv_data)
{
	// TODO implement this
	sLog.outString("Recieved CMSG_BATTLEFIELD_LIST");
}

void WorldSession::SendBattlegroundList(Creature* pCreature, uint32 mapid)
{
	if(!pCreature)
		return;

	/* we should have a bg id selection here. */
	uint32 t = BATTLEGROUND_WARSUNG_GULCH;
	if(pCreature->GetCreatureName())
	{
		if(strstr(pCreature->GetCreatureName()->SubName, "Arena") != NULL)
			t = BATTLEGROUND_ARENA_2V2;
		else if(strstr(pCreature->GetCreatureName()->SubName, "Warsong") != NULL)
			t = BATTLEGROUND_WARSUNG_GULCH;
	}

    BattlegroundManager.HandleBattlegroundListPacket(this, t);
}

void WorldSession::HandleBattleMasterHelloOpcode(WorldPacket &recv_data)
{
	sLog.outString("Recieved CMSG_BATTLEMASTER_HELLO");
}

void WorldSession::HandleLeaveBattlefieldOpcode(WorldPacket &recv_data)
{
	if(_player->m_bg && _player->IsInWorld())
		_player->m_bg->RemovePlayer(_player, false);
}

void WorldSession::HandleAreaSpiritHealerQueryOpcode(WorldPacket &recv_data)
{
	if(!_player->IsInWorld() && _player->m_bg) return;
	uint64 guid;
	recv_data >> guid;
	
	uint32 restime = (_player->m_bg->GetLastResurrect() + 30) - World::UNIXTIME;
	WorldPacket data(SMSG_AREA_SPIRIT_HEALER_TIME, 12);
	data << guid << restime;
	SendPacket(&data);
}

void WorldSession::HandleAreaSpiritHealerQueueOpcode(WorldPacket &recv_data)
{
	if(!_player->IsInWorld() && _player->m_bg) return;
	uint64 guid;
	recv_data >> guid;
	_player->m_bg->QueuePlayerForResurrect(_player);
}

void WorldSession::HandleBattlegroundPlayerPositionsOpcode(WorldPacket &recv_data)
{
	/* This packet doesn't appear to be used anymore... 
	 * - Burlex
	 */
}

void WorldSession::HandleBattleMasterJoinOpcode(WorldPacket &recv_data)
{
	/* are we already in a queue? */
	if(_player->m_bgIsQueued)
		BattlegroundManager.RemovePlayerFromQueues(_player);

	if(_player->IsInWorld())
		BattlegroundManager.HandleBattlegroundJoin(this, recv_data);
}

void WorldSession::HandleArenaJoinOpcode(WorldPacket &recv_data)
{
	/* are we already in a queue? */
	if(_player->m_bgIsQueued)
		BattlegroundManager.RemovePlayerFromQueues(_player);

	uint32 bgtype=0;
	uint64 guid;
	uint8 arenacategory;
    recv_data >> guid >> arenacategory;
	switch(arenacategory)
	{
	case 0:		// 2v2
		bgtype = BATTLEGROUND_ARENA_2V2;
		break;

	case 1:		// 3v3
		bgtype = BATTLEGROUND_ARENA_3V3;
		break;

	case 2:		// 5v5
		bgtype = BATTLEGROUND_ARENA_5V5;
		break;
	}

	if(bgtype != 0)
		BattlegroundManager.HandleArenaJoin(this, bgtype);
}

void WorldSession::HandleInspectHonorStatsOpcode(WorldPacket &recv_data)
{
	/* This belongs in HonorHandler. :P
	 * - Burlex
	 */

	sLog.outString("Recieved MSG_INSPECT_HONOR_STATS");
}

void WorldSession::HandlePVPLogDataOpcode(WorldPacket &recv_data)
{
	if(_player->m_bg)
		_player->m_bg->SendPVPData(_player);
}

