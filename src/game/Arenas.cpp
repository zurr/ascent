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
	plr->m_deathVision = true;
	plr->CastSpell(plr, ARENA_PREPARATION, true);
	UpdatePlayerCounts();
}

void Arena::OnRemovePlayer(Player * plr)
{
	/* remove arena readyness buff */
	plr->m_deathVision = false;
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
	GameObject * obj;
	switch(m_mapMgr->GetMapId())
	{
		/* loraedeon */
	case 572: {
		obj = SpawnGameObject(185917, 572, 1278.647705, 1730.556641, 31.605574, 1.684245, 32, 1375, 1.0f);
		obj->SetUInt32Value(GAMEOBJECT_STATE, 1);
		obj->SetFloatValue(GAMEOBJECT_ROTATION_02, 0.746058f);
		obj->SetFloatValue(GAMEOBJECT_ROTATION_03, 0.665881f);
		m_gates.insert(obj);

		obj = SpawnGameObject(185918, 572, 1293.560791, 1601.937988, 31.605574, -1.457349, 32, 1375, 1.0f);
		obj->SetUInt32Value(GAMEOBJECT_STATE, 1);
		obj->SetFloatValue(GAMEOBJECT_ROTATION_02, -0.665881f);
		obj->SetFloatValue(GAMEOBJECT_ROTATION_03, 0.746058f);
		m_gates.insert(obj);

			  }break;

		/* blades edge arena */
	case 562: {
		obj = SpawnGameObject(183972, 562, 6177.707520, 227.348145, 3.604374, -2.260201, 32, 1375, 1.0f);
		obj->SetUInt32Value(GAMEOBJECT_STATE, 1);
		obj->SetFloatValue(GAMEOBJECT_ROTATION_02, 0.90445f);
		obj->SetFloatValue(GAMEOBJECT_ROTATION_03, -0.426569f);
		obj->PushToWorld(m_mapMgr);

		obj = SpawnGameObject(183973, 562, 6189.546387, 241.709854, 3.101481, 0.881392, 32, 1375, 1.0f);
		obj->SetUInt32Value(GAMEOBJECT_STATE, 1);
		obj->SetFloatValue(GAMEOBJECT_ROTATION_02, 0.426569f);
		obj->SetFloatValue(GAMEOBJECT_ROTATION_03, 0.904455f);
		m_gates.insert(obj);

		obj = SpawnGameObject(183970, 562, 6299.115723, 296.549438, 3.308032, 0.881392, 32, 1375, 1.0f);
		obj->SetUInt32Value(GAMEOBJECT_STATE, 1);
		obj->SetFloatValue(GAMEOBJECT_ROTATION_02, 0.426569f);
		obj->SetFloatValue(GAMEOBJECT_ROTATION_03, 0.904455f);
		obj->PushToWorld(m_mapMgr);

		obj = SpawnGameObject(183971, 562, 6287.276855, 282.187714, 3.810925, -2.260201, 32, 1375, 1.0f);
		obj->SetUInt32Value(GAMEOBJECT_STATE, 1);
		obj->SetFloatValue(GAMEOBJECT_ROTATION_02, 0.904455f);
		obj->SetFloatValue(GAMEOBJECT_ROTATION_03, -0.426569f);
		m_gates.insert(obj);
			  }break;

		/* nagrand arena */
	case 559: {
		obj = SpawnGameObject(183979, 559, 4090.064453, 2858.437744, 10.236313, 0.492805, 32, 1375, 1.0f);
		obj->SetUInt32Value(GAMEOBJECT_STATE, 1);
		obj->SetFloatValue(GAMEOBJECT_ROTATION_02, 0.243916f);
		obj->SetFloatValue(GAMEOBJECT_ROTATION_03, 0.969796f);
		obj->PushToWorld(m_mapMgr);

		obj = SpawnGameObject(183980, 559, 4081.178955, 2874.970459, 12.391714, 0.492805, 32, 1375, 1.0f);
		obj->SetUInt32Value(GAMEOBJECT_STATE, 1);
		obj->SetFloatValue(GAMEOBJECT_ROTATION_02, 0.243916f);
		obj->SetFloatValue(GAMEOBJECT_ROTATION_03, 0.969796f);
		m_gates.insert(obj);

		obj = SpawnGameObject(183977, 559, 4023.709473, 2981.776611, 10.701169, -2.648788, 32, 1375, 1.0f);
		obj->SetUInt32Value(GAMEOBJECT_STATE, 1);
		obj->SetFloatValue(GAMEOBJECT_ROTATION_02, 0.969796f);
		obj->SetFloatValue(GAMEOBJECT_ROTATION_03, -0.243916f);
		obj->PushToWorld(m_mapMgr);

		obj = SpawnGameObject(183978, 559, 4031.854248, 2966.833496, 12.646200, -2.648788, 32, 1375, 1.0f);
		obj->SetUInt32Value(GAMEOBJECT_STATE, 1);
		obj->SetFloatValue(GAMEOBJECT_ROTATION_02, 0.969796f);
		obj->SetFloatValue(GAMEOBJECT_ROTATION_03, -0.243916f);
		m_gates.insert(obj);

			  }break;
	}

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
		for(set<Player*>::iterator itr = m_players[i].begin(); itr != m_players[i].end(); ++itr) {
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
	if(m_ended)
		return;

	uint32 players[2] = {0,0};
	for(uint32 i = 0; i < 2; ++i) {
		for(set<Player*>::iterator itr = m_players[i].begin(); itr != m_players[i].end(); ++itr) {
			if((*itr)->isAlive())
				players[(*itr)->GetTeam()]++;
		}
	}

	SetWorldState(ARENA_WORLD_STATE_A_PLAYER_COUNT, players[0]);
	SetWorldState(ARENA_WORLD_STATE_H_PLAYER_COUNT, players[1]);

	if(!m_started)
		return;

	if(players[1] == 0)
		m_winningteam = 1;
	else if(players[0] == 0)
		m_winningteam = 0;
	else
		return;

	m_ended = true;
	m_nextPvPUpdateTime = 0;
	UpdatePvPData();
	PlaySoundToAll(m_winningteam ? SOUND_ALLIANCEWINS : SOUND_HORDEWINS);

				sEventMgr.RemoveEvents(this, EVENT_BATTLEGROUND_CLOSE);
			sEventMgr.AddEvent(((CBattleground*)this), &CBattleground::Close, EVENT_BATTLEGROUND_CLOSE, 120000, 1,0);
}

LocationVector Arena::GetStartingCoords(uint32 Team)
{
	// 559, 562, 572
	/*
	A start
	H start
	Repop
	572 1295.322388 1585.953369 31.605387
	572 1277.105103 1743.956177 31.603209
	572 1286.112061 1668.334961 39.289127

	562 6184.806641 236.643463 5.037095
	562 6292.032227 287.570343 5.003577
	562 6241.171875 261.067322 0.891833

	559 4085.861328 2866.750488 12.417445
	559 4027.004883 2976.964844 11.600499
	559 4057.042725 2918.686523 13.051933
	*/
	switch(m_mapMgr->GetMapId())
	{
		/* loraedeon */
	case 572:
		{
			if(Team)
				return LocationVector(1277.105103, 1743.956177, 31.603209);
			else
				return LocationVector(1295.322388, 1585.953369, 31.605387);
		}break;

		/* blades edge arena */
	case 562:
		{
			if(Team)
				return LocationVector(6292.032227, 287.570343, 5.003577);
			else
				return LocationVector(6184.806641, 236.643463, 5.037095);
		}break;

		/* nagrand arena */
	case 559:
		{
			if(Team)
				return LocationVector(4027.004883, 2976.964844, 11.600499);
			else
				return LocationVector(4085.861328, 2866.750488, 12.417445);
		}break;
	}

	return LocationVector(0,0,0,0);
}

bool Arena::HookHandleRepop(Player * plr)
{
	// 559, 562, 572
	/*
	A start
	H start
	Repop
	572 1295.322388 1585.953369 31.605387
	572 1277.105103 1743.956177 31.603209
	572 1286.112061 1668.334961 39.289127

	562 6184.806641 236.643463 5.037095
	562 6292.032227 287.570343 5.003577
	562 6241.171875 261.067322 0.891833

	559 4085.861328 2866.750488 12.417445
	559 4027.004883 2976.964844 11.600499
	559 4057.042725 2918.686523 13.051933
	*/
	LocationVector dest(0,0,0,0);
	switch(m_mapMgr->GetMapId())
	{
		/* loraedeon */
	case 572: {
			dest.ChangeCoords(1286.112061, 1668.334961, 39.289127);
		}break;

		/* blades edge arena */
	case 562: {
			dest.ChangeCoords(6241.171875, 261.067322, 0.891833);
		}break;

		/* nagrand arena */
	case 559: {
			dest.ChangeCoords(4057.042725, 2918.686523, 13.051933);
		}break;
	}

	plr->SafeTeleport(m_mapMgr->GetMapId(), m_mapMgr->GetInstanceID(), dest);
	return true;
}

void Arena::HookOnAreaTrigger(Player * plr, uint32 id)
{

}
