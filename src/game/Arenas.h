/*
 * Ascent MMORPG Server
 * Copyright (C) 2005-2007 Ascent Team <http://www.ascentemu.com/>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

class Arena : public CBattleground
{
	set<GameObject*> m_gates;
	bool m_started;
public:
	Arena(MapMgr * mgr, uint32 id, uint32 lgroup, uint32 t, uint32 players_per_side);
	virtual ~Arena();

	bool HookHandleRepop(Player * plr);
	void OnAddPlayer(Player * plr);
	void OnRemovePlayer(Player * plr);
	void OnCreate();
	void HookOnPlayerDeath(Player * plr);
	void HookOnPlayerKill(Player * plr, Unit * pVictim);
	void HookOnHK(Player * plr);
	void UpdatePlayerCounts();
	LocationVector GetStartingCoords(uint32 Team);
	virtual const char * GetName() { return "Arena"; }
	void OnStart();
	bool CanPlayerJoin(Player * plr)
	{
		if(m_started)
			return false;
		else
			return true;
	}

	bool CreateCorpse(Player * plr) { return false; }

	/* dummy stuff */
	void HookOnMount(Player * plr) {}
	void HookFlagDrop(Player * plr, GameObject * obj) {}
	void HookFlagStand(Player * plr, GameObject * obj) {}
	void HookOnAreaTrigger(Player * plr, uint32 id);
};
