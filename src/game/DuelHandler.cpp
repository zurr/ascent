/****************************************************************************
 *
 * Duel Handlers
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

void WorldSession::HandleDuelAccepted(WorldPacket & recv_data)
{
	if(!_player->IsInWorld()) return;
	if(_player->DuelingWith == 0)
		return;

	_player->m_duelStatus = DUEL_STATUS_INBOUNDS;
	_player->DuelingWith->m_duelStatus = DUEL_STATUS_INBOUNDS;
	_player->m_duelState = DUEL_STATE_STARTED;
	_player->DuelingWith->m_duelState = DUEL_STATE_STARTED;

	WorldPacket data(SMSG_DUEL_COUNTDOWN, 4);
	data << uint32(3000);
	SendPacket(&data);
	_player->DuelingWith->m_session->SendPacket(&data);

	_player->m_duelCountdownTimer = 3000;
	sEventMgr.AddEvent(_player, &Player::DuelCountdown, EVENT_PLAYER_DUEL_COUNTDOWN, 1000, 3);
}

void WorldSession::HandleDuelCancelled(WorldPacket & recv_data)
{
	if(_player->DuelingWith == 0)
		return;
	if (_player->m_duelState == DUEL_STATE_STARTED)
	{
	 _player->DuelingWith->EndDuel(DUEL_WINNER_KNOCKOUT);
    return;
	}
	WorldPacket data(SMSG_DUEL_COMPLETE, 1);
	data << uint8(0);
	SendPacket(&data);
	_player->DuelingWith->m_session->SendPacket(&data);
	GameObject *arbiter = _player->GetMapMgr() ? _player->GetMapMgr()->GetGameObject(_player->GetUInt32Value(PLAYER_DUEL_ARBITER)) : 0;
	if(arbiter)
	{
			arbiter->RemoveFromWorld();
			delete arbiter;
 		}
	_player->SetUInt64Value(PLAYER_DUEL_ARBITER, 0);
	_player->DuelingWith->SetUInt64Value(PLAYER_DUEL_ARBITER, 0);
	_player->SetUInt32Value(PLAYER_DUEL_TEAM, 0);
	_player->DuelingWith->SetUInt32Value(PLAYER_DUEL_TEAM, 0);
	_player->DuelingWith->DuelingWith = NULL;
	_player->DuelingWith = NULL;

	}

