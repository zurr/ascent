/****************************************************************************
 *
 * Combat Start/Stop Handlers
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

void WorldSession::HandleAttackSwingOpcode( WorldPacket & recv_data )
{
	if(!_player->IsInWorld()) return;
	CHECK_PACKET_SIZE(recv_data, 8);
	uint64 guid;
	recv_data >> guid;

	if(!guid)
	{
		// does this mean cancel combat?
		HandleAttackStopOpcode(recv_data);
		return;
	}

	// AttackSwing
	sLog.outDebug( "WORLD: Recvd CMSG_ATTACKSWING Message" );

	if(GetPlayer()->IsPacified() || GetPlayer()->IsStunned())
		return;

//	printf("Got ATTACK SWING: %08X %08X\n", GUID_HIPART(guid), GUID_LOPART(guid));
	Unit *pEnemy = _player->GetMapMgr()->GetUnit(guid);
	//printf("Pointer: %08X\n", pEnemy);

	if(!pEnemy)
	{
		sLog.outDebug("WORLD: "I64FMT" does not exist.", guid);
		return;
	}

	GetPlayer()->smsg_AttackStart(pEnemy);
	GetPlayer()->EventAttackStart();

	// Set PVP Flag.
	if(pEnemy->IsPlayer() && isHostile(_player, pEnemy))
		_player->SetPvPFlag();
}

void WorldSession::HandleAttackStopOpcode( WorldPacket & recv_data )
{
	if(!_player->IsInWorld()) return;
	uint64 guid = GetPlayer()->GetSelection();
	Unit *pEnemy = NULL;

	if(guid)
	{
		pEnemy = _player->GetMapMgr()->GetUnit(guid);
	}
	if(!pEnemy)
	{
		sLog.outError( "WORLD: "I64FMT" doesn't exist.",guid);
		return;
	}

	GetPlayer()->EventAttackStop();
	GetPlayer()->smsg_AttackStop(pEnemy);
}
