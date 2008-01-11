/*
 * Ascent MMORPG Server
 * Copyright (C) 2005-2007 Ascent Team <http://www.ascentemu.com/>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef WOWSERVER_AIAGENTHEALSUPPORT_H
#define WOWSERVER_AIAGENTHEALSUPPORT_H
#ifdef USE_SPECIFIC_AIAGENTS
class Object;
class Creature;
class Unit;
class Player;
class WorldSession;
class SpellCastTargets;

#define SPELL_EFF_PCT_SCALE_WITH_DIFFICULTY 0.2f	//given as a float value we influence the effectiveness of our spells
#define SPELL_COOLD_PCT_SCALE_WITH_DIFFICULTY 0.2f	//given as a float value we influence the effectiveness of our spells
#define CREATURE_STATS_SCALE_WITH_DIFFICULTY 1

#define HELPER_MOUNT_DISPLAY 14584

#define HealSpellCount 10
#define HealSpellLevels 17

typedef std::map<uint32, uint32> CooldownMap;

class SERVER_DECL AiAgentHealSupport : public AIInterface
{

public:
	void Init(Unit *un, AIType at, MovementType mt, Unit *owner);
	void _UpdateCombat(uint32 p_time);

private:
	SpellEntry*			Get_Best_Heal_Spell(Unit *for_unit);
	bool				Protect_self();
	bool				CheckCanCast(SpellEntry *sp,Unit *target);
	void				SetSpellDuration(SpellEntry *sp);

	SpellEntry			*m_HealSpells[HealSpellLevels][HealSpellCount];	//used to take actions on target
	float				m_HealSpellsEficiency[HealSpellLevels][HealSpellCount];		
	SpellEntry			*m_defend_self;
/*	list<SpellEntry*>	m_HealGroupSpells;	//used to take actions on target
	list<SpellEntry*>	m_AugmentSpells;	//low priority spells	*/
	SpellEntry			*revive_spell;
	CooldownMap			spell_cooldown_map;
	uint32				DifficultyLevel; //spell values should scale up with the level of the support unit 
	SpellEntry			*m_castingSpell;
};

#endif
#endif
