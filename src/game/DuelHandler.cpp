/****************************************************************************
 *
 * Duel Handlers
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
	_player->DuelingWith->EndDuel(DUEL_WINNER_RETREAT);
}
