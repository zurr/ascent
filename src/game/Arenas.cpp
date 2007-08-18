/****************************************************************************
 *
 * Arena Battleground
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

#define ARENA_PREPARATION 32727
#define ARENA_WORLD_STATE_A_PLAYER_COUNT 2544
#define ARENA_WORLD_STATE_H_PLAYER_COUNT 2545

Arena::Arena(MapMgr * mgr, uint32 id, uint32 lgroup, uint32 t, uint32 players_per_side) : CBattleground(mgr, id, lgroup, t)
{
	m_started = false;
	m_playerCountPerTeam = players_per_side;
}

Arena::~Arena()
{

}

void Arena::OnAddPlayer(Player * plr)
{
	/* cast arena readyness buff */
	plr->CastSpell(plr, ARENA_PREPARATION, true);
	UpdatePlayerCounts();
}

void Arena::OnRemovePlayer(Player * plr)
{
	/* remove arena readyness buff */
	plr->RemoveAura(ARENA_PREPARATION);
	UpdatePlayerCounts();
}

void Arena::HookOnPlayerKill(Player * plr, Unit * pVictim)
{
	if(pVictim->GetTypeId() != TYPEID_UNIT)
		plr->m_bgScore.KillingBlows++;
}

void Arena::HookOnHK(Player * plr)
{
	plr->m_bgScore.HonorableKills++;
}

void Arena::HookOnPlayerDeath(Player * plr)
{
	UpdatePlayerCounts();
}

void Arena::OnCreate()
{
	/* push gates into world */
	for(set<GameObject*>::iterator itr = m_gates.begin(); itr != m_gates.end(); ++itr)
		(*itr)->PushToWorld(m_mapMgr);

	SetWorldState(0x08D4	,0x0000);
	SetWorldState(0x08D8	,0x0000);
	SetWorldState(0x08D7	,0x0000);
	SetWorldState(0x08D6	,0x0000);
	SetWorldState(0x08D5	,0x0000);
	SetWorldState(0x08D3	,0x0000);
	SetWorldState(0x09F1	,0x0000);
	SetWorldState(0x09F0	,0x0000);
	SetWorldState(0x0C0D	,0x017B);
	SetWorldState(0x09F3	,0x0001);
}

void Arena::OnStart()
{
	/* remove arena readyness buff */
	for(uint32 i = 0; i < 2; ++i) {
		for(set<Player*>::iterator itr = m_players[0].begin(); itr != m_players[1].end(); ++itr) {
			(*itr)->RemoveAura(ARENA_PREPARATION);
		}
	}

	/* open gates */
	for(set<GameObject*>::iterator itr = m_gates.begin(); itr != m_gates.end(); ++itr)
	{
		(*itr)->SetUInt32Value(GAMEOBJECT_FLAGS, 64);
		(*itr)->SetUInt32Value(GAMEOBJECT_STATE, 0);
	}

	m_started = true;
}

void Arena::UpdatePlayerCounts()
{
	uint32 players[2] = {0,0};
	for(uint32 i = 0; i < 2; ++i) {
		for(set<Player*>::iterator itr = m_players[i].begin(); itr != m_players[i].end(); ++itr) {
			if((*itr)->isAlive())
				players[i]++;
		}
	}

	SetWorldState(ARENA_WORLD_STATE_A_PLAYER_COUNT, players[0]);
	SetWorldState(ARENA_WORLD_STATE_H_PLAYER_COUNT, players[1]);
}

LocationVector Arena::GetStartingCoords(uint32 Team)
{
	// 559, 562, 572
	switch(m_mapMgr->GetMapId())
	{
		/* loraedeon */
	case 572:
		/* blades edge arena */
	case 562:
		/* nagrand arena */
	case 559:
		return LocationVector(0,0,0,0);
		break;
	}

	return LocationVector(0,0,0,0);
}

bool Arena::HookHandleRepop(Player * plr)
{
	/* repop in the middle of the arena */
	return true;
}

void Arena::HookOnAreaTrigger(Player * plr, uint32 id)
{

}
