/****************************************************************************
 *
 * General Object Type File
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

Unit::Unit()
{

	m_attackTimer = 0;
	m_attackTimer_1 = 0;
	m_duelWield = false;

	m_state = 0;
	m_deathState = ALIVE;
	m_currentSpell = NULL;
	m_meleespell = 0;
	m_addDmgOnce = 0;
	m_TotemSlots[0] = NULL;
	m_TotemSlots[1] = NULL;
	m_TotemSlots[2] = NULL;
	m_TotemSlots[3] = NULL;
	m_ObjectSlots[0] = NULL;
	m_ObjectSlots[1] = NULL;
	m_ObjectSlots[2] = NULL;
	m_ObjectSlots[3] = NULL;
	m_silenced = false;
	disarmed   = false;

	// Pet
	m_isPet = false;
	
	//DK:modifiers
	PctRegenModifier = 0;
	for (uint32 x=0;x<4;x++)
	{
		PctPowerRegenModifier[x] = 1;
	}
	m_speedModifier = 0;
	m_mountedspeedModifier=0;
	VampEmbCaster=0;
        VampTchCaster=0;
	for(uint32 x=0;x<27;x++)
	{
		MechanicsDispels[x]=0;
		MechanicsResistancesPCT[x]=0;
	}

	//SM
	SM_CriticalChance=0;
	SM_FDur=0;//flat
	SM_PDur=0;//pct
	SM_FRadius=0;
	SM_FRange=0;
	SM_PCastTime=0;
	SM_FCastTime=0;
	SM_PCriticalDamage=0;
	SM_FDOT=0;
	SM_PDOT=0;
	SM_FEffectBonus=0;
	SM_PEffectBonus=0;
	SM_FDamageBonus=0;
	SM_PDamageBonus=0;
	SM_PDummy=0;
	SM_FDummy=0;
	SM_FResist=0;
	SM_PRange=0;//pct
	SM_PRadius=0;
	SM_PAPBonus=0;
	SM_PCost=0;
	SM_FCost=0;
	SM_FAdditionalTargets=0;
	SM_PJumpReduce=0;
	SM_FSpeedMod=0;
	SM_PNonInterrupt=0;
	SM_FPenalty=0;
	SM_PPenalty=0;
	SM_FCooldownTime = 0;
	SM_PCooldownTime = 0;
	m_pacified = 0;
	m_interruptRegen = 0;
	m_resistChance = 0;
	m_powerRegenPCT = 0;
	RAPvModifier=0;
	APvModifier=0;
	stalkedby=0;

	m_invisible = false;
	m_extraattacks = 0;
	m_stunned = 0;
	m_manashieldamt=0;
	m_rooted = 0;
	m_triggerSpell = 0;
	m_triggerDamage = 0;
	m_canMove = 0;
	m_noInterrupt = 0;
	m_modlanguage = -1;
	
	critterPet = NULL;
	summonPet = NULL;

	m_useAI = false;
	for(uint32 x=0;x<10;x++)
	{
		dispels[x]=0;
		CreatureAttackPowerMod[x] = 0;
		CreatureRangedAttackPowerMod[x] = 0;
	}
	//REMIND:Update these if you make any changes
	CreatureAttackPowerMod[UNIT_TYPE_MISC] = 0;
	CreatureRangedAttackPowerMod[UNIT_TYPE_MISC] = 0;
	CreatureAttackPowerMod[11] = 0;
	CreatureRangedAttackPowerMod[11] = 0;

	m_stealthLevel = 0;
	m_stealthDetectBonus = 0;
	m_stealth = 0;
	m_sleep = 0;
	
	for(uint32 x=0;x<5;x++)
		BaseStats[x]=0;

	m_attackTimer = 0;
	m_H_regenTimer = 2000;
	m_P_regenTimer = 2000;


	//	if(GetTypeId() == TYPEID_PLAYER) //only player for now
	//		CalculateActualArmor();

	m_aiInterface = new AIInterface();
	m_aiInterface->Init(this, AITYPE_AGRO, MOVEMENTTYPE_NONE);

	m_emoteState = 0;
	m_oldEmote = 0;	
	
	BaseDamage[0]=0;
	BaseOffhandDamage[0]=0;
	BaseRangedDamage[0]=0;
	BaseDamage[1]=0;
	BaseOffhandDamage[1]=0;
	BaseRangedDamage[1]=0;

	m_attackTarget = 0;
	m_CombatUpdateTimer = 0;
	for(uint32 x=0;x<7;x++)
	{
		SchoolImmunityList[x] = 0;
		BaseResistance[x] = 0;
		HealDoneMod[x] = 0;
		HealDonePctMod[x] = 0;
		HealTakenMod[x] = 0;
		HealTakenPctMod[x] = 0;
		DamageTakenMod[x] = 0;
		SchoolCastPrevent[x]=0;
		DamageTakenPctMod[x] = 1;
		SpellCritChanceSchool[x] = 0;
		PowerCostMod[x] = 0;
		PowerCostPctMod[x] = 0;
	}
	DamageTakenPctModOnHP = 1;
	RangedDamageTaken = 0;

	for(int i = 0; i < 5; i++)
	{
		m_detectRangeGUID[i] = 0;
		m_detectRangeMOD[i] = 0;
		InvisibilityDetectBonus[i] = 0;
	}
	//REMIND:Update these if you make any changes
	InvisibilityDetectBonus[INVISIBILTY_FLAG_TRAP] = 180;//MaxLevel*3
	InvisibilityDetectBonus[INVISIBILTY_FLAG_GHOSTS] = 0;

	trackStealth = false;
	modAttackTimeIncreasePCT = 0;

	m_threatModifyer = 0;
	m_generatedThreatModifyer = 0;
	memset(m_auras, 0, (MAX_AURAS+MAX_PASSIVE_AURAS)*sizeof(Aura*));
	
	// diminishing return stuff
	memset(m_diminishAuraCount, 0, 16);
	memset(m_diminishCount, 0, 32);
	memset(m_diminishTimer, 0, 32);
	memset(m_auraStackCount, 0, MAX_AURAS);
	m_diminishActive = false;
	dynObj = 0;
	pLastSpell = 0;
	m_flyspeedModifier = 0;
	bInvincible = false;
	m_redirectSpellPackets = 0;
	can_parry = false;
	bProcInUse = false;
	spellcritperc = 0;

	polySpell = 0;
//	fearSpell = 0;
}

Unit::~Unit()
{  
	RemoveAllAuras();

	if(SM_CriticalChance != 0) delete [] SM_CriticalChance ;
	if(SM_FDur != 0) delete [] SM_FDur ;//flat
	if(SM_PDur != 0) delete [] SM_PDur ;//pct
	if(SM_FRadius != 0) delete [] SM_FRadius ;
	if(SM_FRange != 0) delete [] SM_FRange ;
	if(SM_PCastTime != 0) delete [] SM_PCastTime ;
	if(SM_FCastTime != 0) delete [] SM_FCastTime ;
	if(SM_PCriticalDamage != 0) delete [] SM_PCriticalDamage ;
	if(SM_FDOT != 0) delete [] SM_FDOT ;
	if(SM_PDOT != 0) delete [] SM_PDOT ;
	if(SM_PEffectBonus != 0) delete [] SM_PEffectBonus ;
	if(SM_FDamageBonus != 0) delete [] SM_FDamageBonus ;
	if(SM_PDamageBonus != 0) delete [] SM_PDamageBonus ;
	if(SM_PDummy != 0) delete [] SM_PDummy ;
	if(SM_FDummy != 0) delete [] SM_FDummy ;
	if(SM_FResist != 0) delete [] SM_FResist ;
	if(SM_PRange != 0) delete [] SM_PRange ;//pct
	if(SM_PRadius != 0) delete [] SM_PRadius ;
	if(SM_PAPBonus != 0) delete [] SM_PAPBonus ;
	if(SM_PCost != 0) delete [] SM_PCost ;
	if(SM_FCost != 0) delete [] SM_FCost ;
	if(SM_FAdditionalTargets != 0) delete [] SM_FAdditionalTargets ;
	if(SM_PJumpReduce != 0) delete [] SM_PJumpReduce ;
	if(SM_FSpeedMod != 0) delete [] SM_FSpeedMod ;
	if(SM_PNonInterrupt != 0) delete [] SM_PNonInterrupt ;
	if(SM_FPenalty != 0) delete [] SM_FPenalty ;

	delete m_aiInterface;

	/*for(int i = 0; i < 4; i++)
		if (m_ObjectSlots[i])
			delete m_ObjectSlots[i];*/

	if(m_currentSpell)
		m_currentSpell->cancel();
}

void Unit::Update( uint32 p_time )
{
	_UpdateSpells( p_time );

	if(m_attackers.size() == 0 && m_attackTarget == 0)
	{
		RemoveFlag(UNIT_FIELD_FLAGS, U_FIELD_FLAG_ATTACK_ANIMATION);
		if(hasStateFlag(UF_ATTACKING)) clearStateFlag(UF_ATTACKING);
	}
	else
	{
		SetFlag(UNIT_FIELD_FLAGS, U_FIELD_FLAG_ATTACK_ANIMATION);
		if(!hasStateFlag(UF_ATTACKING)) addStateFlag(UF_ATTACKING);
		
	}

	if(p_time >= m_H_regenTimer)
		RegenerateHealth();	
	else
		m_H_regenTimer -= p_time;
	//most of the times the 2 timers will be the same (except on spell casts)
	if(p_time >= m_P_regenTimer)
		RegeneratePower();	
	else
		m_P_regenTimer -= p_time;

	if(!isDead())
	{
		if(m_aiInterface != NULL && m_useAI)
			m_aiInterface->Update(p_time);

		if(m_diminishActive)
		{
			uint32 count = 0;
			for(uint32 x = 0; x < 16; ++x)
			{
				// diminishing return stuff
				if(m_diminishTimer[x] && !m_diminishAuraCount[x])
				{
					if(p_time >= m_diminishTimer[x])
					{
						// resetting after 15 sec
						m_diminishTimer[x] = 0;
						m_diminishCount[x] = 0;
					}
					else
					{
						// reducing, still.
						m_diminishTimer[x] -= p_time;
						++count;
					}
				}
			}
			if(!count)
				m_diminishActive = false;
		}
	}
}

bool Unit::canReachWithAttack(Unit *pVictim)
{
//	float targetreach = pVictim->GetFloatValue(UNIT_FIELD_COMBATREACH);
	float selfreach = GetFloatValue(UNIT_FIELD_COMBATREACH);
	float targetradius = pVictim->GetFloatValue(UNIT_FIELD_BOUNDINGRADIUS);
	float selfradius = GetFloatValue(UNIT_FIELD_BOUNDINGRADIUS);
	float targetscale = pVictim->GetFloatValue(OBJECT_FIELD_SCALE_X);
	float selfscale = GetFloatValue(OBJECT_FIELD_SCALE_X);

	if( GetMapId() != pVictim->GetMapId() )
		return false;
	float distance = sqrt(GetDistanceSq(pVictim));

	float attackreach = (((targetradius*targetscale) + selfreach) + ((pow(selfradius,2)*selfscale)+1.50));
	//formula adjustment for player side.
	if(this->IsPlayer())
	{
		if (attackreach <= 8 && attackreach >= 5 && targetradius >= 1.80f) attackreach = 11; //giant type units
		if (attackreach > 11) attackreach = 11; //distance limited to max 11 yards attack range //max attack distance
		if (attackreach < 5 ) attackreach = 5; //normal units with too small reach.
		//range can not be less than 5 yards - this is normal combat range, SCALE IS NOT SIZE
	}

	return (distance <= attackreach);
}

void Unit::GiveGroupXP(Unit *pVictim, Player *PlayerInGroup)
{
	if(!PlayerInGroup) 
		return;
	if(!pVictim) 
		return;
	if(!PlayerInGroup->InGroup()) 
		return;
	Group *pGroup = PlayerInGroup->GetGroup();
	uint32 xp;
	if(!pGroup) 
		return;

	//Get Highest Level Player, Calc Xp and give it to each group member
	Player *pHighLvlPlayer = NULL;
	Player *pGroupGuy = NULL;
	  int active_player_count=0;
	Player *active_player_list[MAX_GROUP_SIZE_RAID];//since group is small we can afford to do this ratehr then recheck again the whole active player set
	int total_level=0;
	float xp_mod = 1.0f;

/*	if(pGroup->GetGroupType() == GROUP_TYPE_RAID)
	{   //needs to change
		//Calc XP
		xp = CalculateXpToGive(pVictim, PlayerInGroup);
		xp /= pGroup->MemberCount();

		GroupMembersSet::iterator itr;
		for(uint32 i = 0; i < pGroup->GetSubGroupCount(); i++)
		{
			for(itr = pGroup->GetSubGroup(i)->GetGroupMembersBegin(); itr != pGroup->GetSubGroup(i)->GetGroupMembersEnd(); ++itr)
			{
				if((*itr)->getLevel() < sWorld.LevelCap)
					(*itr)->GiveXP(xp, pVictim->GetGUID(), true);
			}
		}
	}
	else if(pGroup->GetGroupType() == GROUP_TYPE_PARTY) */
	//change on 2007 04 22 by Zack
	//we only take into count players that are near us, on same map
	GroupMembersSet::iterator itr;
	for(uint32 i = 0; i < pGroup->GetSubGroupCount(); i++)
		for(itr = pGroup->GetSubGroup(i)->GetGroupMembersBegin(); itr != pGroup->GetSubGroup(i)->GetGroupMembersEnd(); ++itr)
		{
			pGroupGuy = (*itr);
			if( pGroupGuy && 
				pGroupGuy->isAlive() && 
//				PlayerInGroup->GetInstanceID()==pGroupGuy->GetInstanceID() &&
				pVictim->GetMapMgr() == pGroupGuy->GetMapMgr() && 
				pGroupGuy->GetDistanceSq(pVictim)<100*100
				)
			{
				active_player_list[active_player_count]=pGroupGuy;
				active_player_count++;
				total_level += pGroupGuy->getLevel();
				if(pHighLvlPlayer)
				{
					if(pGroupGuy->getLevel() > pHighLvlPlayer->getLevel())
						pHighLvlPlayer = pGroupGuy;
				}
				else 
					pHighLvlPlayer = pGroupGuy;
			}
		}
	if(active_player_count<1) //killer is always close to the victim. This should never execute
	{
		if(PlayerInGroup == 0) PlayerInGroup = pGroup->GetLeader();
		xp = CalculateXpToGive(pVictim, PlayerInGroup);
		PlayerInGroup->GiveXP(xp, pVictim->GetGUID(), true);
	}
	else
	{
		if( pGroup->GetGroupType() == GROUP_TYPE_PARTY)
		{
			if(active_player_count==3)
				xp_mod=1.1666f;
			else if(active_player_count==4)
				xp_mod=1.3;
			else if(active_player_count==5)
				xp_mod=1.4;
			else xp_mod=1;//in case we have only 2 members ;)
		}
		else if(pGroup->GetGroupType() == GROUP_TYPE_RAID)
			xp_mod=0.5f;
		if(pHighLvlPlayer == 0) pHighLvlPlayer = pGroup->GetLeader();

		xp = CalculateXpToGive(pVictim, pHighLvlPlayer);
		//i'm not sure about this formula is correct or not. Maybe some brackets are wrong placed ?
		for(int i=0;i<active_player_count;i++)
			active_player_list[i]->GiveXP( float2int32(((xp*active_player_list[i]->getLevel()) / total_level)*xp_mod), pVictim->GetGUID(), true );
	}
		/* old code start before 2007 04 22
		GroupMembersSet::iterator itr;
		for(uint32 i = 0; i < pGroup->GetSubGroupCount(); i++)
		{
			for(itr = pGroup->GetSubGroup(i)->GetGroupMembersBegin(); itr != pGroup->GetSubGroup(i)->GetGroupMembersEnd(); ++itr)
			{
				pGroupGuy = (*itr);
				if(pGroupGuy)
				{
					if(pHighLvlPlayer)
					{
						if(pGroupGuy->getLevel() > pHighLvlPlayer->getLevel())
						{
							pHighLvlPlayer = pGroupGuy;
						}
					}
					else
						pHighLvlPlayer = pGroupGuy;
				}
			}
		}

		//Calc XP
		xp = CalculateXpToGive(pVictim, pHighLvlPlayer);
		uint32 giveXP = 0;

		for(uint32 i = 0; i < pGroup->GetSubGroupCount(); i++)
		{
			for(itr = pGroup->GetSubGroup(i)->GetGroupMembersBegin(); itr != pGroup->GetSubGroup(i)->GetGroupMembersEnd(); ++itr)
			{
				pGroupGuy = (*itr);
				giveXP = xp * pGroupGuy->getLevel() / (pHighLvlPlayer->getLevel() + pGroupGuy->getLevel());
				if(pGroupGuy->getLevel() < sWorld.LevelCap)
					pGroupGuy->GiveXP(giveXP, pVictim->GetGUID(), true);
			}
		}
	}*/
}

void Unit::HandleProc(uint32 flag, Unit* victim, SpellEntry* CastingSpell,uint32 dmg)
{
	bool can_delete = !bProcInUse;
	bProcInUse = true;

	std::list<uint32> remove;
	std::list<struct ProcTriggerSpell>::iterator itr,itr2;
	for( itr = m_procSpells.begin();itr != m_procSpells.end();)  // Proc Trigger Spells for Victim
	{
		itr2= itr;
		++itr;
		if(itr2->deleted)
		{
			if(can_delete) m_procSpells.erase(itr2);
			continue;
		}

		uint32 origId = itr2->origId;
		if(CastingSpell)
		{
			if(CastingSpell->Id == itr2->origId || CastingSpell->Id == itr2->spellId)
			{
				//printf("WOULD CRASH HERE ON PROC: CastingId: %u, OrigId: %u, SpellId: %u\n", CastingSpell->Id, itr2->origId, itr2->spellId);
				continue;
			}
		}
		//this requires some specific spell check,not yet implemented
		if(itr2->procFlags & flag)
		{
			uint32 spellId = itr2->spellId;
			if(itr2->procFlags & PROC_ON_CAST_SPECIFIC_SPELL)
			{
				if(!CastingSpell)
					continue;
				
				//this is wrong, dumy is too common to be based on this, we should use spellgroup or something
				SpellEntry *sp=sSpellStore.LookupEntry(spellId);
				if(sp->dummy != CastingSpell->dummy )
					continue;
				else
					if(sp->dummy == 1)
						continue;
			}			

			if(spellId && Rand(itr2->procChance))
			{
				//these are player talents. Fuckem they pull the emu speed down 
				if(IsPlayer())
				{
					//yep, another special case: Nature's grace
					if(spellId==31616 && GetHealthPct()>30)
						continue;
					//warrior mace specialization can trigger only when using maces
					if(spellId==5530)
					{
						Player *pr=static_cast<Player*>(this);
						if(pr->GetItemInterface())
						{
							Item *it;
							it = pr->GetItemInterface()->GetInventoryItem(EQUIPMENT_SLOT_MAINHAND);
							if(it && it->GetProto())
							{
								uint32 reqskill=GetSkillByProto(it->GetProto()->Class,it->GetProto()->SubClass);
								if(!(reqskill==SKILL_MACES || reqskill==SKILL_2H_MACES))
									continue;
							}
						}
					}
					if(spellId==4350)
					{
						Player *pr=static_cast<Player*>(this);
						if(pr->GetItemInterface())
						{
							Item *it;
							it = pr->GetItemInterface()->GetInventoryItem(EQUIPMENT_SLOT_MAINHAND);
							if(it && it->GetProto())
							{
								uint32 reqskill=GetSkillByProto(it->GetProto()->Class,it->GetProto()->SubClass);
								if(!(reqskill==SKILL_SWORDS || reqskill==SKILL_2H_SWORDS))
									continue;
							}
						}
					}
				}
				SpellEntry *spellInfo = sSpellStore.LookupEntry(spellId );
				if(!spellInfo)
				{
					continue;
				}
				Spell *spell = new Spell(this, spellInfo ,true, NULL);
				//Spell *spell = new Spell(this,spellInfo,false,0,true,false);
				if(itr2->spellId==974||itr2->spellId==32593||itr2->spellId==32594) // Earth Shield handler
				{
					spell->pSpellId=itr2->spellId;
					spell->SpellEffectDummy(0);
					delete spell;
					continue;
				}
				SpellCastTargets targets;
				targets.m_unitTarget = victim->GetGUID();
				spell->pSpellId=origId;
				spell->prepare(&targets);
			}//not always we have a spell to cast
			/*if(itr2->procCharges)
			{
				if(origId)
					remove.push_back(origId);
				else 
					m_procSpells.erase(itr2);
			}*/
		}
	}
	std::map<uint32,struct SpellCharge>::iterator iter,iter2;
	iter=m_chargeSpells.begin();
	while(iter!= m_chargeSpells.end())
	{
		iter2=iter++;
		if(iter2->second.count)
		{
			if((iter2->second.ProcFlag&flag))
			{
				//Fixes for spells that dont lose charges when dmg is absorbd
				if(iter2->second.ProcFlag==680&&dmg==0) continue;
				if(CastingSpell)
				{

					SpellCastTime *sd = sCastTime.LookupEntry(CastingSpell->CastingTimeIndex);
					if(!sd) continue; // this shouldnt happen though :P
					switch(iter2->second.spellId)
					{
					case 12043:
						{
							//Presence of Mind and Nature's Swiftness should only get removed
							//when a non-instant and bellow 10 sec. Also must be nature :>
							if(!sd->CastTime||sd->CastTime>10000) continue;
						}break;
					case 16188:
						{
							if(CastingSpell->School!=SCHOOL_NATURE||(!sd->CastTime||sd->CastTime>10000)) continue;
						}break;
					case 16166:
						{
							if(!(CastingSpell->School==SCHOOL_FIRE||CastingSpell->School==SCHOOL_FROST||CastingSpell->School==SCHOOL_NATURE))
								continue;
						}break;

					}
				}
				if(iter2->second.lastproc!=0)
				{
					if(iter2->second.procdiff>3000)
					{
						--(iter2->second.count);
						iter2->second.FromProc=true;
						RemoveAura(iter2->second.spellId);
						iter2->second.FromProc=false;
					}
				}
				else
				{
					--(iter2->second.count);
					iter2->second.FromProc=true;
					this->RemoveAura(iter2->second.spellId);
					iter2->second.FromProc=false;
				}
			}
		}
		if(!iter2->second.count)
		{
			m_chargeSpells.erase(iter2);
		}
	}
	if(can_delete)
		bProcInUse = false;
}

bool Unit::isCasting()
{
	return (m_currentSpell != NULL);
}

void Unit::RegenerateHealth()
{
	m_H_regenTimer = 2000;//set next regen time 

	if (!isAlive())
		return;

	// player regen
	if(this->IsPlayer())
	{
		// These only NOT in combat
		if(!static_cast<Player*>(this)->isInCombat())
		{
			static_cast<Player*>(this)->RegenerateHealth(false);
		}
		else
			static_cast<Player*>(this)->RegenerateHealth(true);
	}
	else
	{
		// Only regen health out of combat
		if(!isInCombat())
			static_cast<Creature*>(this)->RegenerateHealth();
	}
}

void Unit::RegeneratePower()
{
        // This is only 2000 IF the power is not rage
        m_P_regenTimer = 2000;//set next regen time 

	if (!isAlive())
		return;

	// player regen
	if(this->IsPlayer())
	{
		uint32 powertype = GetPowerType();
		switch(powertype)
		{
		case POWER_TYPE_MANA:
			static_cast<Player*>(this)->RegenerateMana();
			break;
		case POWER_TYPE_ENERGY:
			static_cast<Player*>(this)->RegenerateEnergy();
			break;
		}
		
		/*

		There is a problem here for druids.
		Druids when shapeshifted into bear have 2 power with different regen timers
		a) Mana (which regenerates while shapeshifted
		b) Rage

		Mana has a regen timer of 2 seconds
		Rage has a regen timer of 3 seconds

		I think the only viable way of fixing this is to have 2 different timers
		to check each individual power.

		Atm, mana is being regen at 3 seconds while shapeshifted...

		*/


		// druids regen mana when shapeshifted
		if(getClass() == DRUID && powertype != POWER_TYPE_MANA)
			static_cast<Player*>(this)->RegenerateMana();

		// These only NOT in combat
		if(!static_cast<Player*>(this)->isInCombat())
		{
		        // This was already checked above, redundant
			//if(powertype == POWER_TYPE_RAGE && !static_cast<Player*>(this)->isInCombat())//rage is dropped only out of combat

  		        // Rage timer is 3 seconds not 2
		        m_P_regenTimer = 3000;

		        if(powertype == POWER_TYPE_RAGE)
			        static_cast<Player*>(this)->LooseRage();
		}
	}
	else
	{
		uint32 powertype = GetPowerType();
		switch(powertype)
		{
		case POWER_TYPE_MANA:
			static_cast<Creature*>(this)->RegenerateMana();
			break;
		case POWER_TYPE_FOCUS:
			static_cast<Creature*>(this)->RegenerateFocus();
			break;
		}
	}
}

void Unit::CalculateResistanceReduction(Unit *pVictim,dealdamage * dmg)
{
	//	   lvl diff:  0+   1+   2+   3+	4+	5+	6+
	float resistPvE[7]={4.0f,5.0f,6.0f,17.0f,28.0f,39.0f,50.0f};
	//	   lvl diff:  0+   1+   2+   3+	4+	5+	6+
	float resistPvP[7]={4.0f,5.0f,6.0f,13.0f,20.0f,27.0f,34.0f};

	int32 lvldiff = 0;
	float resistchance = 1.0f;
	float miscchance = 0.0f;
	float AverageResistance = 0.0f;
	bool pvp = false;
	
	if(GetTypeId() == TYPEID_PLAYER && pVictim->GetTypeId() == TYPEID_PLAYER) //PvP
	{
		pvp = true;
		miscchance = 7.0f;
	}
	else
		miscchance = 11.0f;

	if((*dmg).damage_type == 0)//physical
	{		
//		double Reduction = double(pVictim->GetResistance(0)) / double(pVictim->GetResistance(0)+400+(85*getLevel()));
		//dmg reduction formula from xinef
		double Reduction = 0;
		if (getLevel() < 60) Reduction = double(pVictim->GetResistance(0)) / double(pVictim->GetResistance(0)+400+(85*getLevel()));
		else if (getLevel() > 59 && getLevel() < 70) Reduction = double(pVictim->GetResistance(0)) / double(pVictim->GetResistance(0)-22167.5+(467.5*getLevel()));
		//
		else Reduction = double(pVictim->GetResistance(0)) / double(pVictim->GetResistance(0)+10557.5);
		if(Reduction > 0.75f) Reduction = 0.75f;
		else if(Reduction < 0) Reduction = 0;
		if(Reduction) dmg[0].resisted_damage = (uint32)(dmg[0].full_damage*Reduction);	  // no multiply by 0
	}
	else
	{
		// applying resistance to other type of damage
		lvldiff = pVictim->getLevel() - getLevel();
	
		if(lvldiff >= 0)
		{
			if(lvldiff < 7) 
				resistchance = pvp ? resistPvP[lvldiff] : resistPvE[lvldiff];
			else
				resistchance = pvp ? resistPvP[6] : resistPvE[6] + miscchance*(lvldiff-6);
			if(IsPlayer())
			{
				float spellHitMod = static_cast<Player*>(this)->GetHitFromSpell();
				if(resistchance <= spellHitMod)
					resistchance = 0;
				else
					resistchance -= spellHitMod;
			}
		}
		resistchance += resistchance*PowerCostPctMod[(*dmg).damage_type];
		if(Rand(resistchance))
		{
			(*dmg).resisted_damage = (*dmg).full_damage;
		}
		else
		{
			AverageResistance = ((float)pVictim->GetResistance( (*dmg).damage_type) / (float)(getLevel() * 5)) * 0.75f;
			  if(AverageResistance > 0.75f)
				AverageResistance = 0.75f;
			if(AverageResistance)
				(*dmg).resisted_damage = (uint32)(((*dmg).full_damage)*AverageResistance);
		 }
	}
	// Checks for random bugs on spells
	if ( (*dmg).full_damage > 10000 && GetTypeId() == TYPEID_PLAYER && ((Player*)this)->GetSession()->GetPermissionCount() == 0) // hits higher then 5.5k must be bugged
	{
		sCheatLog.writefromsession(static_cast<Player*>(this)->GetSession(),"some how caused %u damage using spell %u",(*dmg).full_damage,GetCurrentSpell());
		(*dmg).resisted_damage = (*dmg).full_damage;
		sChatHandler.RedSystemMessage(static_cast<Player*>(this)->GetSession(),"Your actions have been logged, please tell a GM how to re-produce this damage bug or risk getting banned.");
	}
	//sLog.outDebug("calc resistance - damage: %d , dmg type: %d , dmg abs: %d\n",*damage,damage_type,*dmgabs);
}

void Unit::Strike(Unit *pVictim, uint32 damage_type, SpellEntry *ability, int32 add_damage, int32 pct_dmg_mod, uint32 exclusive_damage)
{
	if (!pVictim->isAlive() || !isAlive()  || IsStunned() || IsPacified())
	{
		return;
	}
	if(!isInFront(pVictim))
	{
		if(IsPlayer())
		{
			static_cast<Player*>(this)->GetSession()->OutPacket(SMSG_ATTACKSWING_BADFACING);
			return;
		}
	}
	/*if(this->IsPlayer())
	{
		((Player*)this)->CombatModeDelay = COMBAT_DECAY_TIME;
	}
	else if(this->IsPet())
	{
		Player*p = ((Pet*)this)->GetPetOwner();
		if(p)
			p->CombatModeDelay = COMBAT_DECAY_TIME;
	}*/

	dealdamage dmg			  = {0,0,0};
	
	Item * it = NULL;
	
	float dodge				 = 0.0f;
	float block				 = 0.0f;
	float crit				  = 0.0f;
	float parry				 = 0.0f;

	uint32 targetEvent		  = 0;
	uint32 hit_status		   = 0;

	uint32 blocked_damage	   = 0;
	int32  realdamage		   = 0;

	uint32 vstate			   = 1;
	uint32 aproc				= 0;
	uint32 vproc				= 0;
	   
	float hitmodifier		   = 0;
	uint32 self_skill;
	uint32 victim_skill;
	uint32 SubClassSkill		= 0;

	bool backAttack			 = isInBack( pVictim );
	uint32 vskill = 0;
	
	
	if(pVictim->IsPlayer())
	{
		vskill = ((Player*)pVictim)->GetSkillAmt(SKILL_DEFENSE);
		if((damage_type != RANGED) && !backAttack)
		{
			it = ((Player*)pVictim)->GetItemInterface()->GetInventoryItem(EQUIPMENT_SLOT_OFFHAND);
			if(it && it->GetProto()->InventoryType==INVTYPE_SHIELD)
			{
				block = pVictim->GetFloatValue(PLAYER_BLOCK_PERCENTAGE);
			}
			dodge = pVictim->GetFloatValue(PLAYER_DODGE_PERCENTAGE);
			
//			if(((Player*)pVictim)->HasSpell(3127))//only players that have parry skill/spell may parry
//				parry = pVictim->GetFloatValue(PLAYER_PARRY_PERCENTAGE);
			if(pVictim->can_parry)
				parry = pVictim->GetFloatValue(PLAYER_PARRY_PERCENTAGE);
		}
		victim_skill = float2int32(vskill+((Player*)pVictim)->CalcRating(1)); // you compare weapon and defense skills not weapon and weapon
	}
	else
	{
		if(damage_type != RANGED && !backAttack)
			dodge = pVictim->GetUInt32Value(UNIT_FIELD_STAT1) / 14.5;

		victim_skill = pVictim->getLevel() * 5;
	}

	if(this->IsPlayer())
	{	  
		self_skill=0;
		//Finding subclass skill
		Player *pr = ((Player*)this);
		hitmodifier = (uint32)pr->GetHitFromMeleeSpell();  
		
		if(disarmed)
			it = NULL;
		else if(damage_type == MELEE)//melee,
		{
			it = pr->GetItemInterface()->GetInventoryItem(EQUIPMENT_SLOT_MAINHAND);
			hitmodifier+=pr->CalcRating(5);
			self_skill = float2int32(pr->CalcRating(20));
		}
		else if(damage_type == DUALWIELD)//dual wield
		{
			it = pr->GetItemInterface()->GetInventoryItem(EQUIPMENT_SLOT_OFFHAND);
			hitmodifier+=pr->CalcRating(5);
			self_skill = float2int32(pr->CalcRating(21));
		}
		else if(damage_type == RANGED)
		{
			it = pr->GetItemInterface()->GetInventoryItem(EQUIPMENT_SLOT_RANGED);
			hitmodifier+=pr->CalcRating(6);
			self_skill = float2int32(pr->CalcRating(0));
		}

		if(it)
			SubClassSkill=GetSkillByProto(it->GetProto()->Class,it->GetProto()->SubClass);
		else 
			SubClassSkill=SKILL_UNARMED;

		self_skill += pr->GetSkillAmt(SubClassSkill);
		
		crit = GetFloatValue(PLAYER_CRIT_PERCENTAGE);

		// block shit
		if(vskill && block != 0.0f)
		{
			int diff = (int)vskill - (int)self_skill;
			float fdiff = (float)diff * 0.04f;
			block += fdiff;
		}
	}
	else
	{
		self_skill = this->getLevel() * 5;
		crit = (this->getLevel()-pVictim->getLevel()+1)*5+(self_skill-victim_skill)*0.04;//don't ask me ... that's on wowwiki
	}

	if(pVictim->IsPlayer())
	{
		if((damage_type != RANGED))
			crit += static_cast<Player*>(pVictim)->res_M_crit_get();
		else crit += static_cast<Player*>(pVictim)->res_R_crit_get(); //this could be ability but in that case we overwrite the value
	}

//  if we get a negative chance .. we will never use it again
//	if(crit<0) 
//		crit=0;

	float vsk = (self_skill*0.04);
	dodge -= vsk;
	parry -= vsk;
	

	//this is official formula, don't use anything else!
	float hitchance = 95.0 -(victim_skill-self_skill)*0.04 +hitmodifier;
	// grep tweak: previously 95.0f
	
	//(((float)(self_skill+this->getLevel())/(float)(victim_skill+pVictim->getLevel()))*125.0f + hitmodifier );
	
	if(ability && ability->SpellGroupType)
	{
		SM_FFValue(SM_CriticalChance,&crit,ability->SpellGroupType);
		SM_FFValue(SM_FResist,&hitchance,ability->SpellGroupType);
	}
	
	uint32 abs = 0;
	if((!ability) && Rand(100.0f - hitchance)) //Miss
	{
		hit_status |= HITSTATUS_MISS;

		// dirty ai agro fix
		// make mob aggro when u miss it
		// grep: dirty fix for this
		if(pVictim->GetTypeId() == TYPEID_UNIT)
			pVictim->GetAIInterface()->AttackReaction(this, 1, 0);
	}
	else if ((!ability)&&Rand(dodge)) //Dodge
	{
		CALL_SCRIPT_EVENT(pVictim, OnTargetDodged)(this);
		CALL_SCRIPT_EVENT(this, OnDodged)(this);
		targetEvent = 1;
		vstate = DODGE;
		pVictim->Emote(EMOTE_ONESHOT_PARRYUNARMED);			// Animation
		if(pVictim->IsPlayer())
		{
			pVictim->SetFlag(UNIT_FIELD_AURASTATE,AURASTATE_FLAG_DODGE_BLOCK);	//SB@L: Enables spells requiring dodge
		}
	}
	else if ((!ability)&&Rand(parry)) //Parry
	{
		CALL_SCRIPT_EVENT(pVictim, OnTargetParried)(this);
		CALL_SCRIPT_EVENT(this, OnParried)(this);
		targetEvent = 3;
		vstate = PARRY;
		pVictim->Emote(EMOTE_ONESHOT_PARRYUNARMED);			// Animation
		if(pVictim->IsPlayer())
		{
			pVictim->SetFlag(UNIT_FIELD_AURASTATE,AURASTATE_FLAG_PARRY);	//SB@L: Enables spells requiring parry
		}
	}
	else//hit 
	{
		pVictim->RemoveFlag(UNIT_FIELD_AURASTATE,AURASTATE_FLAG_DODGE_BLOCK | AURASTATE_FLAG_PARRY); //SB@L: removes dodge and parry flag after a hit

		hit_status |= HITSTATUS_HITANIMATION;//hit animation on victim

		if(pVictim->SchoolImmunityList[0])
		{
			vstate = IMMUNE;		
		}
		else
		{
			vproc |= PROC_ON_ANY_DAMAGE_VICTIM;			
		
			if(damage_type != RANGED)
			{
				aproc |= PROC_ON_MELEE_ATTACK;
				   vproc |= PROC_ON_MELEE_ATTACK_VICTIM;
			}
			else
			{
				aproc |= PROC_ON_RANGED_ATTACK;
				vproc |= PROC_ON_RANGED_ATTACK_VICTIM;
			}

			if(exclusive_damage)
				dmg.full_damage = exclusive_damage;
			else
			{
				if(damage_type == MELEE && ability)
					dmg.full_damage = CalculateDamage(this, pVictim, damage_type, ability->SpellGroupType);
				else			
					dmg.full_damage = CalculateDamage(this, pVictim, damage_type, 0);
			}

			if(ability && ability->SpellGroupType)
			{	
				SM_FIValue(((Unit*)this)->SM_FDamageBonus,&dmg.full_damage,ability->SpellGroupType);
				SM_PIValue(((Unit*)this)->SM_PDamageBonus,&dmg.full_damage,ability->SpellGroupType);
			}

			if(dmg.full_damage < 0)
				dmg.full_damage = 0;
			else
			{
				dmg.full_damage *= float2int32(pVictim->DamageTakenPctMod[0]); 
				if(pct_dmg_mod)
					dmg.full_damage = (dmg.full_damage*pct_dmg_mod)/100;
			}
			
			dmg.full_damage += pVictim->DamageTakenMod[0]+add_damage;
			if(dmg.damage_type == RANGED)
				dmg.full_damage += pVictim->RangedDamageTaken;
			
			if(dmg.full_damage < 0)
				dmg.full_damage = 0;

			if (Rand(block) && pVictim->GetTypeId() == TYPEID_PLAYER) //Block block can only appear if we have shield!
			{
				Item * shield = ((Player*)pVictim)->GetItemInterface()->GetInventoryItem(EQUIPMENT_SLOT_OFFHAND);
				if(shield)
				{
					targetEvent = 2;
					pVictim->Emote(EMOTE_ONESHOT_PARRYSHIELD);// Animation
					blocked_damage = shield->GetProto()->Block+pVictim->GetUInt32Value(UNIT_FIELD_STAT0)/20;
					if(dmg.full_damage <= blocked_damage)
					{
						CALL_SCRIPT_EVENT(pVictim, OnTargetBlocked)(this, blocked_damage);
						CALL_SCRIPT_EVENT(this, OnBlocked)(pVictim, blocked_damage);
						vstate = BLOCK;			
					}
				}
			}
			else if (Rand(crit)) //Crictical Hit
			{
				hit_status |= HITSTATUS_CRICTICAL;
				int32 dmgbonus = dmg.full_damage;
				if(ability && ability->SpellGroupType)
					SM_FIValue(SM_PCriticalDamage,&dmgbonus,ability->SpellGroupType);
				dmg.full_damage += dmgbonus;
				if(!pVictim->IsPlayer())
				if(IsPlayer())
					dmg.full_damage += float2int32(dmg.full_damage*static_cast<Player*>(this)->IncreaseCricticalByTypePCT[((Creature*)pVictim)->GetCreatureName() ? ((Creature*)pVictim)->GetCreatureName()->Type : 0]);
				
				// burlex: this causes huge damage increases. I'm not sure what it's meant to accompilsh either, so
				//         i'm gonna comment it.

				/*dmg.full_damage = (dmg.full_damage*(100+dmgbonus))/100;*/
				pVictim->Emote(EMOTE_ONESHOT_WOUNDCRITICAL);
				vproc |= PROC_ON_CRIT_HIT_VICTIM;
				aproc |= PROC_ON_CRIT_ATTACK;

				if(this->IsPlayer())
				{
					this->SetFlag(UNIT_FIELD_AURASTATE,AURASTATE_FLAG_CRITICAL);	//SB@L: Enables spells requiring critical strike
				}

				CALL_SCRIPT_EVENT(pVictim, OnTargetCritHit)(this, dmg.full_damage);
				CALL_SCRIPT_EVENT(this, OnCritHit)(pVictim, dmg.full_damage);
			}
			else
			{
				if(this->IsPlayer())
					this->RemoveFlag(UNIT_FIELD_AURASTATE,AURASTATE_FLAG_CRITICAL);	//SB@L: Enables spells requiring critical strike
				//check for crushing hit			
				if(!this->IsPlayer())
				{	
					if(this->getLevel()-pVictim->getLevel() >=3)
					{
						if(Rand((self_skill-victim_skill)*2))
						{
							hit_status |= HITSTATUS_CRUSHINGBLOW;
							// full_damage is a uint32, now, multiplying it by 1.5 is the same thing as multiplying it by 1
							// i'm pretty sure this isn't supposed to be, maybe put full_damage in a tmp double variable, make
							// the math and recast back to uint32 ? os full_damage should be a double ?
							// for now i do the first assumption
							//dmg.full_damage *= 1.5;
							double tmpDmg = (double)dmg.full_damage;
							tmpDmg *= 1.5;
							dmg.full_damage = (uint32)tmpDmg;
						}
					}
				}
				else if(!pVictim->IsPlayer()&&(!ability))	//glancing
				{
					if(damage_type != RANGED)
					{
						if(Rand(pVictim->getLevel()-this->getLevel()-1)*20)
						{
							int32 dif = (victim_skill+10-self_skill)*3;
							if(dif > 0)
							{
								dmg.full_damage -= (dif*dmg.full_damage)/100;
								hit_status |= HITSTATUS_GLANCING;
							}
						}
					}
				}
			}	
			//absorb apply
			uint32 dm = dmg.full_damage;
			abs = pVictim->AbsorbDamage(0,(uint32*)&dm);
		
			if(dmg.full_damage > blocked_damage)
			{
				dmg.full_damage -= blocked_damage;
				uint32 sh = pVictim->ManaShieldAbsorb(dmg.full_damage);
				if(sh)
				{
					dmg.full_damage -= sh;
					if(dmg.full_damage)
						CalculateResistanceReduction(pVictim,&dmg);//armor
					dmg.full_damage += sh;
					dmg.resisted_damage += sh;
				}
				else
					CalculateResistanceReduction(pVictim,&dmg);//armor

				dmg.full_damage += blocked_damage;
			}
			 
			dmg.resisted_damage += abs;
		
			realdamage = dmg.full_damage-dmg.resisted_damage-blocked_damage;
			if(realdamage < 0)
			{
				realdamage = 0;
				vstate = IMMUNE;
				hit_status |= HITSTATUS_ABSORBED;
			}
		}
	}
	//vstate=1-wound,2-dodge,3-parry,4-interrupt,5-block,6-evade,7-immune,8-deflect
	
	// hack fix for stormstirke loop here.
	if(damage_type != DUALWIELD)
    {
	    if( !(ability && ability->NameHash == 0x2535ed19) )
		    this->HandleProc(aproc,pVictim, ability,realdamage);

	    pVictim->HandleProc(vproc,this, ability,realdamage);
	}
	
	if(pVictim->GetTypeId() == TYPEID_UNIT)
	{
		if(pVictim->GetAIInterface() && pVictim->GetAIInterface()->getAIState()== STATE_EVADE)
		{
			vstate = EVADE;
			realdamage = 0;
			dmg.full_damage = 0;
			dmg.resisted_damage = 0;
		}
	}

	if(realdamage > 0 && ability == 0)
	{
		if(IsPlayer() && ((Player*)this)->m_onStrikeSpells.size())
		{
			SpellCastTargets targets;
			targets.m_unitTarget = pVictim->GetGUID();
			targets.m_targetMask = 0x2;
			Spell *cspell;

			// Loop on hit spells, and strike with those.
			for(map< SpellEntry*, pair<uint32, uint32> >::iterator itr = ((Player*)this)->m_onStrikeSpells.begin();
				itr != ((Player*)this)->m_onStrikeSpells.end(); ++itr)
			{
				//Strike(pVictim, 1, (*itr), add_damage, pct_dmg_mod, exclusive_damage);
				if( itr->second.first )
				{
					// We have a *periodic* delayed spell.
					uint32 t = getMSTime();
					if( t > itr->second.second )  // Time expired
					{
						// Set new time
						itr->second.second = t + itr->second.first;
					}

					// Cast.
					cspell = new Spell(this, itr->first, true, NULL);
					cspell->prepare(&targets);
				}
				else
				{
					cspell = new Spell(this, itr->first, true, NULL);
					cspell->prepare(&targets);
				}			
			}
		}

		if(IsPlayer() && ((Player*)this)->m_onStrikeSpellDmg.size())
		{
			map<uint32, OnHitSpell>::iterator itr = ((Player*)this)->m_onStrikeSpellDmg.begin();
			uint32 min_dmg, max_dmg, range, dmg;
			for(; itr != ((Player*)this)->m_onStrikeSpellDmg.end(); ++itr)
			{
				min_dmg = itr->second.mindmg;
				max_dmg = itr->second.maxdmg;
				range = min_dmg - max_dmg;
				dmg = min_dmg;
				if(range) range += sRand.randInt(range);

				SpellNonMeleeDamageLog(pVictim, itr->second.spellid, dmg, true);
			}
		}
	}
	
	// resist damage on godmode
	if(pVictim->GetTypeId() == TYPEID_PLAYER && static_cast<Player*>(pVictim)->GodModeCheat == true)
	{
		dmg.resisted_damage = dmg.full_damage;
	}

	WorldPacket data(SMSG_ATTACKERSTATEUPDATE, 70);
	//0x4--dualwield,0x10 miss,0x20 absorbed,0x80 crit,0x4000 -glancing,0x8000-crushing
	//only for melee!
	
	if(damage_type == DUALWIELD)//dual wield animation
		hit_status |= HITSTATUS_DUALWIELD;

	if(!ability)
	{
		if(dmg.full_damage)
			if(dmg.full_damage == dmg.resisted_damage)
				hit_status |= HITSTATUS_ABSORBED;

		data << (uint32)hit_status;   
		data << GetNewGUID();
		data << pVictim->GetNewGUID();
		
		data << (uint32)realdamage;		 // Realdamage;
		data << (uint8)1;				   // Damage type counter / swing type

		data << (uint32)0;				  // Damage school
		data << (float)dmg.full_damage;	 // Damage float
		data << (uint32)dmg.full_damage;	// Damage amount
		data << (uint32)dmg.resisted_damage;// Damage absorbed
		data << (uint32)0;				  // Damage resisted

		data << (uint32)vstate;			 // new victim state
		data << (int32)0;					// can be 0,1000 or -1
		data << (uint32)0;				  // unknown
		data << (uint32)blocked_damage;	 // Damage amount blocked
		
		data << (uint32) 0;

		SendMessageToSet(&data, this->IsPlayer());
	}
	else
	{
		if(realdamage)//FIXME: add log for miss,block etc for ability and ranged
	//	if(ability)//FIXME:WTF is this
		{
			SendSpellNonMeleeDamageLog(this,pVictim,ability->Id,realdamage,0,dmg.resisted_damage,0,false,blocked_damage,(hit_status & HITSTATUS_CRICTICAL),true);
		}
		//FIXME: add log for miss,block etc for ability and ranged
		//example how it works
		//SendSpellLog(this,pVictim,ability->Id,SPELL_LOG_MISS);
	}
	if(realdamage)
	{
		DealDamage(pVictim, realdamage, 0, targetEvent, 0);

		if (pVictim->GetCurrentSpell())
			pVictim->GetCurrentSpell()->AddTime(0);
	}
	else
	{		
		// have to set attack target here otherwise it wont be set
		// because dealdamage is not called.
		setAttackTarget(pVictim);
	}
	
	if(damage_type != RANGED)
	{
		if(hit_status & HITSTATUS_HITANIMATION)//really hit
		{
			std::list<DamageShield>::iterator i;
			std::list<DamageShield>::iterator i2;
			   for(i = pVictim->m_damageShields.begin();i != pVictim->m_damageShields.end();)	  // Deal Damage to Attacker
			{
				i2 = i++;
				data.Initialize(SMSG_SPELLDAMAGESHIELD);
				data << pVictim->GetGUID();
				data << this->GetGUID();
				data << (*i2).m_damage;
				data << (*i2).m_school;
				SendMessageToSet(&data,true);

				pVictim->DealDamage(this,(*i2).m_damage,0,0,(*i2).m_spellId);
			}
		}
	}

	if(pVictim->IsPlayer())
	{
		static_cast<Player*>(pVictim)->GetItemInterface()->ReduceItemDurability();
		if (!this->IsPlayer())
		{
			Player *pr = ((Player*)pVictim);
			if (Rand(pr->GetSkillUpChance(SKILL_DEFENSE)))
			{
				pr->AdvanceSkillLine(SKILL_DEFENSE);
				pr->UpdateChances();
			}
		}
		else
		{
			 static_cast<Player*>(this)->GetItemInterface()->ReduceItemDurability();
		}
	}
	else
	{
		if (this->IsPlayer())//not pvp
		{
			static_cast<Player*>(this)->GetItemInterface()->ReduceItemDurability();
			Player *pr = ((Player*)this);
			if (Rand(pr->GetSkillUpChance(SubClassSkill)))
			{
				pr->AdvanceSkillLine(SubClassSkill);
				//pr->UpdateChances();
			}
		}
	}

	/* Brandon
	   Rage combat Formulas
	   wowwiki.com
	   2.0.10

	   Rage Gained from dealing damage = ((Damage Dealt) / (Rage Conversion at Your Level) * 7.5 + (Weapon Speed * Factor))/2
	   TODO
	*/

	uint32 val;
	if(IsPlayer())
	{
		if(this->GetPowerType() == POWER_TYPE_RAGE && !ability)
		{
		  // It only regens rage if in combat, don't know why but this is making
		  // the player to regen 1 rage every 3 secs.....
		  // and the formula is wrong also ... TODO
		  if(isInCombat()) {
			val = GetUInt32Value(UNIT_FIELD_POWER2)+(realdamage*20)/getLevel();
			val += (static_cast<Player *>(this)->rageFromDamageDealt*val)/100;
			SetUInt32Value(UNIT_FIELD_POWER2, val>=1000?1000:val);
		  }
		}
	}
		
	RemoveAurasByInterruptFlag(AURA_INTERRUPT_ON_START_ATTACK);

	if(ability && realdamage==0)
	{	
		SendSpellLog(this,pVictim,ability->Id,SPELL_LOG_RESIST);
	}

	while(m_extraattacks > 0)
	{
		m_extraattacks--;
		Strike(pVictim,damage_type,ability,add_damage,pct_dmg_mod,exclusive_damage);
	}
}	

void Unit::smsg_AttackStop(Unit* pVictim)
{
	if(!pVictim)
		return;

	if(m_attackTarget == pVictim->GetGUID())
		setAttackTarget(NULL);

	WorldPacket data(SMSG_ATTACKSTOP, 24);
	data << GetNewGUID();
	data << pVictim->GetNewGUID();
	data << uint32( 0 );
	SendMessageToSet(&data, IsPlayer() );
}

void Unit::smsg_AttackStop(uint64 victimGuid)
{
	WorldPacket data;
	data.Initialize( SMSG_ATTACKSTOP );
	data << GetNewGUID();
	FastGUIDPack(data, victimGuid);
	data << uint32( 0 );
	SendMessageToSet(&data, IsPlayer());
}

void Unit::smsg_AttackStart(Unit* pVictim)
{
	if(GetTypeId() != TYPEID_PLAYER) 
		return;

	Player* pThis = (Player*) this;

	// Prevent user from ignoring attack speed and stopping and start combat really really fast
	/*if(!isAttackReady())
		setAttackTimer(uint32(0));
	else if(!canReachWithAttack(pVictim))
	{
		setAttackTimer(uint32(500));
		//pThis->GetSession()->OutPacket(SMSG_ATTACKSWING_NOTINRANGE);
	}
	else if(!isInFront(pVictim))
	{
		setAttackTimer(uint32(500));
		//pThis->GetSession()->OutPacket(SMSG_ATTACKSWING_NOTINRANGE);
	}*/

	// Send out ATTACKSTART
	WorldPacket data(SMSG_ATTACKSTART, 16);
	data << GetGUID();
	data << pVictim->GetGUID();
	SendMessageToSet(&data, true);
	Log::getSingleton( ).outDebug( "WORLD: Sent SMSG_ATTACKSTART" );

	// FLAGS changed so other players see attack animation
	//	addUnitFlag(0x00080000);
	//	setUpdateMaskBit(UNIT_FIELD_FLAGS );
	if(pThis->cannibalize)
	{
		sEventMgr.RemoveEvents(pThis, EVENT_CANNIBALIZE);
		pThis->SetUInt32Value(UNIT_NPC_EMOTESTATE, 0);
		pThis->cannibalize = false;
	}
}

void Unit::RemoveBySpecialType(uint32 id, uint64 caster)
{
	for(uint32 x=0;x<MAX_AURAS+MAX_PASSIVE_AURAS;x++)
	{
		if(m_auras[x])
		{
			SpellExtraInfo* aur = SpellExtraStorage.LookupEntry(m_auras[x]->GetSpellId());
			if(aur)
			{
				int type = aur->specialtype;
				if(type == id)
				{
					if(((type == STING) || (type == BLESSING) || (type == CURSE)) && (m_auras[x]->m_casterGuid==caster))
					{
						if (RemoveAura(m_auras[x]->GetSpellId(),caster))
							return;
					}
					else
					{
						if (RemoveAura(m_auras[x]))
							return;
					}
				}
			}
		}
	}
}

void Unit::AddAura(Aura *aur)
{
	if(m_mapId != 530)
	{
		for(uint32 i = 0; i < 3; ++i)
		{
			// Can't use flying auras in non-outlands.
			if(aur->GetSpellProto()->EffectApplyAuraName[i] == 208 || aur->GetSpellProto()->EffectApplyAuraName[i] == 207)
			{
				delete aur;
				return;
			}
		}
	}

	if(aur->GetSpellProto()->School && SchoolImmunityList[aur->GetSpellProto()->School])
	{
		delete aur;
		return;
	}

	if(!aur->IsPassive())
	{
		//uint32 aurName = aur->GetSpellProto()->Name;
		//uint32 aurRank = aur->GetSpellProto()->Rank;
		uint32 maxStack = aur->GetSpellProto()->maxstack;
		if(aur->GetSpellProto()->procCharges)
			maxStack=aur->GetSpellProto()->procCharges;
		if(IsPlayer() && ((Player*)this)->stack_cheat)
			maxStack = 999;

		SpellEntry * info = aur->GetSpellProto();
		//uint32 flag3 = aur->GetSpellProto()->Flags3;
		AuraCheckResponse acr;
		bool deleteAur = false;
		//check if we already have this aura by this caster -> update duration
		uint32 f=0;
		for(uint32 x=0;x<MAX_AURAS;x++)
		{
			if(m_auras[x])
			{
				if(m_auras[x]->GetSpellProto()->Id != aur->GetSpellId())
				{
					// Check for auras by specific type.
					// Check for auras with the same name and a different rank.
					
					if(info->buffType > 0 && m_auras[x]->GetSpellProto()->buffType & info->buffType && maxStack == 0)
						deleteAur = HasAurasOfBuffType(info->buffType, aur->m_casterGuid);
					else
					{
						acr = AuraCheck(info->NameHash, info->RankNumber, m_auras[x]);
						if(acr.Error == AURA_CHECK_RESULT_HIGHER_BUFF_PRESENT)
							deleteAur = true;
						else if(acr.Error == AURA_CHECK_RESULT_LOWER_BUFF_PRESENT)
						{
							// remove the lower aura
							m_auras[x]->Remove();

							// no more checks on bad ptr
							continue;
						}
					}					   
				}
				else if(m_auras[x]->GetSpellId() == aur->GetSpellId()) // not the best formula to test this I know, but it works until we find a solution
				{
					if(!aur->IsPositive() && m_auras[x]->m_casterGuid != aur->m_casterGuid)
						continue;
					f++;
					//if(maxStack > 1)
					{
						//update duration,the same aura (update the whole stack whenever we cast a new one)
						m_auras[x]->SetDuration(aur->GetDuration());
						sEventMgr.ModifyEventTimeLeft(m_auras[x], EVENT_AURA_REMOVE, aur->GetDuration());
						if(this->IsPlayer())
						{
							WorldPacket data(SMSG_UPDATE_AURA_DURATION, 5);
							data << (uint8)m_auras[x]->m_visualSlot <<(uint32) aur->GetDuration();
							((Player*)this)->GetSession()->SendPacket(&data);
						}
					}
					if(maxStack <= f)
					{
						deleteAur = true;
						break;
					}
				}
			}
		}
		if(deleteAur)
		{
			sEventMgr.RemoveEvents(aur);
			delete aur;
			return;
		}
	}
			
	////////////////////////////////////////////////////////
	if(aur->GetSpellProto()->SpellGroupType && m_objectTypeId == TYPEID_PLAYER)
	{
		int32 speedmod=0;
		SM_FIValue(SM_FSpeedMod,&speedmod,aur->GetSpellProto()->SpellGroupType);
		if(speedmod)
		{
			m_speedModifier += speedmod;
			UpdateSpeed();
		}
	}
	////////////////////////////////////////////////////////

	aur->m_auraSlot=255;
	aur->ApplyModifiers(true);
	
	if(!aur->IsPassive())
	{	
		aur->AddAuraVisual();
		if(aur->m_auraSlot==255)
		{
			//add to invisible slot
			for(uint32 x=MAX_AURAS;x<MAX_AURAS+MAX_PASSIVE_AURAS;x++)
			{
				if(!m_auras[x])
				{
					m_auras[x]=aur;
					aur->m_auraSlot=x;
					break;
				}
			}
			if(aur->m_auraSlot == 255)
			{
				sLog.outError("Aura error in active aura. ");
				// for next loop.. lets kill the fucker
				aur->Remove();
				return;
			} 

			// add visual
			AddAuraVisual(aur->GetSpellId(), 1, aur->IsPositive());
		}
		else
		{
			m_auras[aur->m_auraSlot]=aur;
		}
	}
	else
	{
		for(uint32 x=MAX_AURAS;x<MAX_AURAS+MAX_PASSIVE_AURAS;x++)
		{
			if(!m_auras[x])
			{
				m_auras[x]=aur;
				aur->m_auraSlot=x;
				break;
			}
		}
		if(aur->m_auraSlot==255)
		{
			sLog.outError("Aura error in passive aura. removing. SpellId: %u", aur->GetSpellProto()->Id);
			// for next loop.. lets kill the fucker
			aur->Remove();
			return;
		}
	}

	if(aur->GetDuration() > 0)
		sEventMgr.AddEvent(aur, &Aura::Remove, EVENT_AURA_REMOVE, aur->GetDuration(), 1);

	aur->RelocateEvents();

	// Reaction from enemy AI
	if(GetTypeId() == TYPEID_UNIT && !aur->IsPositive())	  // Creature
	{
		if(isAlive())
		{
			Unit * pCaster = aur->GetUnitCaster();
			if(!pCaster) return;

			addAttacker(pCaster);

			GetAIInterface()->AttackReaction(pCaster, 1, aur->GetSpellId());
		}
	}
}

bool Unit::RemoveAura(Aura *aur)
{
	aur->Remove();
	return true;
}

bool Unit::RemoveAura(uint32 spellId)
{//this can be speed up, if we know passive \pos neg
	for(uint32 x=0;x<MAX_AURAS+MAX_PASSIVE_AURAS;x++)
	{
		if(m_auras[x])
		{
			if(m_auras[x]->GetSpellId()==spellId)
			{
				m_auras[x]->Remove();
				return true;
			}
		}
	}
	return false;
}

bool Unit::RemoveAura(uint32 spellId, uint64 guid)
{   
	for(uint32 x=0;x<MAX_AURAS+MAX_PASSIVE_AURAS;x++)
	{
		if(m_auras[x])
		{
			if(m_auras[x]->GetSpellId()==spellId && m_auras[x]->m_casterGuid == guid)
			{
				m_auras[x]->Remove();
				return true;
			}
		}
	}
	return false;
}

bool Unit::RemoveAuraByNameHash(uint32 namehash)
{
	for(uint32 x=0;x<MAX_AURAS;x++)
	{
		if(m_auras[x])
		{
			if(m_auras[x]->GetSpellProto()->NameHash==namehash)
			{
				m_auras[x]->Remove();
				return true;
			}
		}
	}
	return false;
}

bool Unit::RemoveAuraPosByNameHash(uint32 namehash)
{
	for(uint32 x=0;x<MAX_POSITIVE_AURAS;x++)
	{
		if(m_auras[x])
		{
			if(m_auras[x]->GetSpellProto()->NameHash==namehash)
			{
				m_auras[x]->Remove();
				return true;
			}
		}
	}
	return false;
}

bool Unit::RemoveAuraNegByNameHash(uint32 namehash)
{
	for(uint32 x=MAX_POSITIVE_AURAS;x<MAX_AURAS;x++)
	{
		if(m_auras[x])
		{
			if(m_auras[x]->GetSpellProto()->NameHash==namehash)
			{
				m_auras[x]->Remove();
				return true;
			}
		}
	}
	return false;
}

bool Unit::RemoveAllAuraByNameHash(uint32 namehash)
{
	bool res = false;
	for(uint32 x=0;x<MAX_AURAS;x++)
	{
		if(m_auras[x])
		{
			if(m_auras[x]->GetSpellProto()->NameHash==namehash)
			{
				m_auras[x]->Remove();
				res=true;
			}
		}
	}
	return res;
}

bool Unit::RemoveAllPosAuraByNameHash(uint32 namehash)
{
	bool res = false;
	for(uint32 x=0;x<MAX_POSITIVE_AURAS;x++)
	{
		if(m_auras[x])
		{
			if(m_auras[x]->GetSpellProto()->NameHash==namehash)
			{
				m_auras[x]->Remove();
				res=true;
			}
		}
	}
	return res;
}

bool Unit::RemoveAllNegAuraByNameHash(uint32 namehash)
{
	bool res = false;
	for(uint32 x=MAX_POSITIVE_AURAS;x<MAX_AURAS;x++)
	{
		if(m_auras[x])
		{
			if(m_auras[x]->GetSpellProto()->NameHash==namehash)
			{
				m_auras[x]->Remove();
				res=true;
			}
		}
	}
	return res;
}

void Unit::RemoveNegativeAuras()
{
	for(uint32 x=MAX_POSITIVE_AURAS;x<MAX_AURAS;x++)
	{
		if(m_auras[x])
		{
			if(m_auras[x]->GetSpellId()!=15007)//resurrection sickness
			{
				m_auras[x]->Remove();
			}
		}
	}
}

void Unit::RemoveAllAuras()
{
	for(uint32 x=0;x<MAX_AURAS+MAX_PASSIVE_AURAS;x++)
	{
		if(m_auras[x])
		{
			m_auras[x]->Remove();
		}
	}
}

//ex:to remove morph spells
void Unit::RemoveAllAuraType(uint32 auratype)
{
    for(uint32 x=0;x<MAX_AURAS;x++)
    {
		if(m_auras[x])
		{
			SpellEntry *proto=m_auras[x]->GetSpellProto();
			if(proto->EffectApplyAuraName[0]==auratype || proto->EffectApplyAuraName[1]==auratype || proto->EffectApplyAuraName[2]==auratype)
				RemoveAura(m_auras[x]->GetSpellId());//remove all morph auras containig to this spell (like wolf motph also gives speed)
		}
    }
}

bool Unit::SetAurDuration(uint32 spellId,Unit* caster,uint32 duration)
{
	Aura*aur=FindAura(spellId,caster->GetGUID());
	if(!aur)
		return false;
	aur->SetDuration(duration);
	sEventMgr.ModifyEventTimeLeft(aur, EVENT_AURA_REMOVE, duration);
			
	if(this->IsPlayer())
	{
		WorldPacket data;
		data.SetOpcode(SMSG_UPDATE_AURA_DURATION);
		data << (uint8)(aur)->GetAuraSlot() << duration;
		((Player*)this)->GetSession()->SendPacket(&data);
	}
			
	return true;
}

bool Unit::SetAurDuration(uint32 spellId,uint32 duration)
{
	Aura*aur=FindAura(spellId);

	if(!aur) 
		return false;
	

	aur->SetDuration(duration);
	sEventMgr.ModifyEventTimeLeft(aur, EVENT_AURA_REMOVE, duration);
		
	if(this->IsPlayer())
	{
		WorldPacket data;
		data.SetOpcode(SMSG_UPDATE_AURA_DURATION);
		data << (uint8)(aur)->GetAuraSlot() << duration;
		((Player*)this)->GetSession()->SendPacket(&data);
	}

	return true;
}

Aura* Unit::FindAura(uint32 spellId)
{
	for(uint32 x=0;x<MAX_AURAS+MAX_PASSIVE_AURAS;x++)
	{
		if(m_auras[x])
		{
			if(m_auras[x]->GetSpellId()==spellId)
			{
				return m_auras[x];
			}
		}
	}
	return NULL;
}

Aura* Unit::FindAura(uint32 spellId, uint64 guid)
{
	for(uint32 x=0;x<MAX_AURAS+MAX_PASSIVE_AURAS;x++)
	{
		if(m_auras[x])
		{
			if(m_auras[x]->GetSpellId() == spellId && m_auras[x]->m_casterGuid == guid)
			{
				return m_auras[x];
			}
		}
	}
	return NULL;
}

void Unit::_UpdateSpells( uint32 time )
{
	if(m_currentSpell != NULL)
	{
		m_currentSpell->update(time);
	}
}

void Unit::castSpell( Spell * pSpell )
{
	// check if we have a spell already casting etc
	if(m_currentSpell)
	{
		m_currentSpell->cancel();
	}

	m_currentSpell = pSpell;
	pLastSpell = pSpell->m_spellInfo;
}

void Unit::InterruptSpell()
{
	if(m_currentSpell)
	{
		//ok wtf is this
		//m_currentSpell->SendInterrupted(SPELL_FAILED_INTERRUPTED);
		m_currentSpell->cancel();
	}
}

void Unit::DeMorph()
{
	// hope it solves it :)
	uint32 displayid = this->GetUInt32Value(UNIT_FIELD_NATIVEDISPLAYID);
	this->SetUInt32Value(UNIT_FIELD_DISPLAYID, displayid);
}

void Unit::Emote (EmoteType emote)
{
	if(emote == 0)
		return;

	WorldPacket data(SMSG_EMOTE, 12);
	data << uint32(emote);
	data << this->GetGUID();
	SendMessageToSet (&data, true);
}

void Unit::SendChatMessage(uint8 type, uint32 lang, const char *msg)
{
	uint32 UnitNameLength = 0, MessageLength = 0;
	const char *UnitName = "";
	CreatureInfo *ci;

	ci = CreatureNameStorage.LookupEntry(GetEntry());
	if(!ci)
		return;

	UnitName = ci->Name;
	UnitNameLength = strlen((char*)UnitName) + 1;
	MessageLength = strlen((char*)msg) + 1;

	switch(type)
	{
	case CHAT_MSG_MONSTER_SAY:
		{
			for(Object::InRangeSet::iterator i = GetInRangeSetBegin(); i != GetInRangeSetEnd(); i++)
			{
				if((*i)->GetTypeId() == TYPEID_PLAYER)
				{
					WorldPacket data(SMSG_MESSAGECHAT, 35 + UnitNameLength + MessageLength);
					data << type;
					data << lang;
					data << GetGUID();
					data << uint32(0);			// new in 2.1.0
					data << UnitNameLength;
					data << UnitName;
					data << ((Player*)(*i))->GetGUID();
					data << MessageLength;
					data << msg;
					data << uint8(0x00);

					WorldSession *session = ((Player*)(*i))->GetSession();
					session->SendPacket(&data);
				}
			}
		}break;
	case CHAT_MSG_MONSTER_YELL:
		{
			for(Object::InRangeSet::iterator i = GetInRangeSetBegin(); i != GetInRangeSetEnd(); i++)
			{
				if((*i)->GetTypeId() == TYPEID_PLAYER)
				{
					WorldPacket data(SMSG_MESSAGECHAT, 35 + UnitNameLength + MessageLength);
					data << type;
					data << lang;
					data << GetGUID();
					data << uint32(0);			// new in 2.1.0
					data << UnitNameLength;
					data << UnitName;
					data << ((Player*)(*i))->GetGUID();
					data << MessageLength;
					data << msg;
					data << uint8(0x00);

					WorldSession *session = ((Player*)(*i))->GetSession();
					session->SendPacket(&data);
				}
			}
		}break;
	case CHAT_MSG_MONSTER_EMOTE:
		{
			for(Object::InRangeSet::iterator i = GetInRangeSetBegin(); i != GetInRangeSetEnd(); i++)
			{
				if((*i)->GetTypeId() == TYPEID_PLAYER)
				{
					WorldPacket data(SMSG_MESSAGECHAT, 35 + UnitNameLength + MessageLength);
					data << type;
					data << lang;
					data << GetGUID();
					data << uint32(0);			// new in 2.1.0
					data << UnitNameLength;
					data << UnitName;
					data << ((Player*)(*i))->GetGUID();
					data << MessageLength;
					data << msg;
					data << uint8(0x00);

					WorldSession *session = ((Player*)(*i))->GetSession();
					session->SendPacket(&data);
				}
			}
		}break;
	}
}

void Unit::WipeHateList()
{ 
	GetAIInterface()->WipeHateList(); 
}

void Unit::WipeTargetList() 
{ 
	GetAIInterface()->WipeTargetList(); 
}

void Unit::AddInRangeObject(Object* pObj)
{
	if ((pObj->GetTypeId() == TYPEID_UNIT) || (pObj->GetTypeId() == TYPEID_PLAYER))
	{
		if (m_objectTypeId == TYPEID_UNIT && isHostile(this, (Unit*)pObj))
			m_oppFactsInRange.insert(((Unit*)pObj));

		if (GetTypeId()==TYPEID_PLAYER)
		{
			if (((Player*)this)->InGroup())
			{
				if(pObj->GetTypeId() == TYPEID_UNIT)
				{
					if (((Creature*)pObj)->Tagged)
					{
						if(((Player*)this)->GetGroup()->HasMember(pObj->GetMapMgr()->GetPlayer(((Creature*)pObj)->TaggerGuid)))
						{
							uint32 Flags;
							Flags = ((Creature*)pObj)->m_uint32Values[UNIT_DYNAMIC_FLAGS];
							Flags |= U_DYN_FLAG_TAPPED_BY_PLAYER;
							ByteBuffer buf1(500);
							((Creature*)pObj)->BuildFieldUpdatePacket(&buf1, UNIT_DYNAMIC_FLAGS, Flags);
							((Player*)this)->PushUpdateData(&buf1, 1);
						}
					}
				}
			}
		}		
	}

	Object::AddInRangeObject(pObj);
}//427

void Unit::RemoveInRangeObject(Object* pObj)
{
	/*set<Object*>::iterator itr = m_oppFactsInRange.find(pObj);
	if(itr != m_oppFactsInRange.end())
		m_oppFactsInRange.erase(itr);*/
	if(m_objectTypeId == TYPEID_UNIT && pObj->IsUnit())
		m_oppFactsInRange.erase(((Unit*)pObj));

	if(pObj->GetTypeId() == TYPEID_UNIT || pObj->GetTypeId() == TYPEID_PLAYER)
	{
		/*if (m_useAI)*/
		Unit *pUnit = static_cast<Unit*>(pObj);
		GetAIInterface()->CheckTarget(pUnit);

		/* === ATTACKER CHECKS === */

		// if we're being attacked by him, remove from our set.
		removeAttacker(pUnit);

		// check that our target is not him (shouldn't happen!)
		if(m_attackTarget == pUnit->GetGUID())
			setAttackTarget(NULL);

		/* === END ATTACKER CHECKS === */

		if(GetUInt64Value(UNIT_FIELD_CHARM) == pObj->GetGUID())
			if(m_currentSpell)
				m_currentSpell->cancel();
	}

	Object::RemoveInRangeObject(pObj);
}

void Unit::ClearInRangeSet()
{
	Object::ClearInRangeSet();
}

//Events
void Unit::EventAddEmote(EmoteType emote, uint32 time)
{
	m_oldEmote = GetUInt32Value(UNIT_NPC_EMOTESTATE);
	SetUInt32Value(UNIT_NPC_EMOTESTATE,emote);
	sEventMgr.AddEvent(this, &Creature::EmoteExpire, EVENT_UNIT_EMOTE, time, 1);
}

void Unit::EmoteExpire()
{
	SetUInt32Value(UNIT_NPC_EMOTESTATE, m_oldEmote);
	sEventMgr.RemoveEvents(this, EVENT_UNIT_EMOTE);
}

void Unit::RegisterPeriodicChatMessage(uint32 delay, uint32 msgid, std::string message,bool sendnotify)
{
	EventMgr::getSingleton().AddEvent(this, &Unit::DelayedChatMessage, uint32(0), msgid, message, sendnotify, EVENT_UNIT_REPEAT_MSG, delay, 0);
}

void Unit::DelayedChatMessage(uint32 delay, uint32 msgid, std::string message, bool sendnotify)
{
	if(delay == 0)	  // Send instantly
	{
		SendChatMessage(CHAT_MSG_MONSTER_SAY, LANG_UNIVERSAL, message.c_str());
		if(sendnotify && msgid) SendNotifyToNearby(msgid);
	}
	else
	{
		if(delay > 0)
			EventMgr::getSingleton().AddEvent(this, &Unit::DelayedChatMessage, uint32(0), msgid, message, sendnotify, EVENT_UNIT_CHAT_MSG, delay, 1);
		else
			// We shouldn't get here; Repeat this function again, but with a delay of 0 ;)
			DelayedChatMessage(0, msgid, message, sendnotify);
	}
}

void Unit::SendNotifyToNearby(uint32 msgid)
{
	//Zehamster: commented the full code, since it does nothing but wasting cpu cycles
	/*
	InRangeSet::iterator itr = GetInRangeSetBegin();
	for(;itr!=GetInRangeSetEnd();++itr)
	{
		if((*itr)->GetTypeId() == TYPEID_UNIT)
		{
			// Send lua event
			Creature *crt = ((Creature*)(*itr));
			//Log::getSingleton().outDebug("LUA: Sending \"NOTIFY\" Msg %d to mob %s", msgid, crt->GetCreatureName()->Name.c_str());
			//crt->LUA_SendEvent(ON_UNIT_NOTIFYMSG, msgid);
		}
	}
	*/
}

void Unit::SendNotifyToNearbyCreature(uint32 msgid, uint32 entryid)
{
	//Zehamster: commented the full code since it does nothing but wasting cpu cycles
	/*
	InRangeSet::iterator itr = GetInRangeSetBegin();
	for(;itr!=GetInRangeSetEnd();++itr)
	{
		if((*itr)->GetTypeId() == TYPEID_UNIT && (*itr)->GetEntry() == entryid)
		{
			// Send lua event
			Creature *crt = ((Creature*)(*itr));
 //		   Log::getSingleton().outDebug("LUA: Sending \"NOTIFY\" Msg %d to mob %s", msgid, GetCreatureName()->Name.c_str());
			//crt->LUA_SendEvent(ON_UNIT_NOTIFYMSG, msgid);
		}
	}
	*/
}

uint32 Unit::GetResistance(uint32 type)
{
	return GetUInt32Value(UNIT_FIELD_RESISTANCES+type);
}

// grep: note to self.. this should be moved to creature.
/*
void Unit::InitializeEscortQuest(uint32 questid, bool stopatend, bool returnondie)
{
	this->bEscortActive = false;
	this->bHasEscortQuest = true;
	this->bStopAtEndOfWaypoints = stopatend;
	this->m_escortquestid = questid;
	this->m_escortupdatetimer = 0;
	if(this->m_useAI && this->GetAIInterface() != NULL)
		GetAIInterface()->setMoveType(10);				// Quest
}*/

void Unit::MoveToWaypoint(uint32 wp_id)
{
	if(this->m_useAI && this->GetAIInterface() != NULL)
	{
		sLog.outString("WARNING: Cannot find AI Interface for MoveToWaypoint!");
		AIInterface *ai = GetAIInterface();
		WayPoint *wp = ai->getWayPoint(wp_id);
		if(!wp)
			sLog.outString("WARNING: Invalid WP specified in MoveToWaypoint.");

		ai->m_currentWaypoint = wp_id;
		ai->MoveTo(wp->x, wp->y, wp->z, 0);
	}
}

/*
void Unit::StartEscortQuest()
{
	this->bEscortActive = true;
	this->m_escortupdatetimer = 1000;
}

void Unit::PauseEscortQuest()
{
	this->bEscortActive = false;
}

void Unit::EndEscortQuest()
{
	// Return to spawn
	Creature *crt = ((Creature*)this);
	GetAIInterface()->MoveTo(crt->respawn_cord[0], crt->respawn_cord[1], crt->respawn_cord[2], crt->respawn_cord[3]);
	this->bEscortActive = false;
	this->m_escortupdatetimer = 0;
}

void Unit::EscortSetStartWP(uint32 wp)
{
	this->m_escortStartWP = wp;
}

void Unit::EscortSetEndWP(uint32 wp)
{
	this->m_escortEndWP = wp;
}*/

int32 Unit::GetDamageDoneMod(uint32 school)
{
	if(this->IsPlayer())
	   return (int32)(GetUInt32Value(PLAYER_FIELD_MOD_DAMAGE_DONE_POS+school)-GetUInt32Value(PLAYER_FIELD_MOD_DAMAGE_DONE_NEG+school));
	else
	   return ((Creature*)this)->ModDamageDone[school];
}
	
float Unit::GetDamageDonePctMod(uint32 school)
{
   if(this->IsPlayer())
	   return GetFloatValue(PLAYER_FIELD_MOD_DAMAGE_DONE_PCT+school);
	else
	   return ((Creature*)this)->ModDamageDonePct[school];
}

void Unit::CalcDamage()
{
	if(IsPlayer())
		((Player*)this)->CalcDamage();
	else
	{
	float r;
	float delta;
	float mult;
	
	float ap_bonus = GetAP()/14000.0;

		float bonus = ap_bonus*GetUInt32Value(UNIT_FIELD_BASEATTACKTIME);
	
		delta = ((Creature*)this)->ModDamageDone[0];
		mult = ((Creature*)this)->ModDamageDonePct[0];
		r = BaseDamage[0]*mult+delta+bonus;
		SetFloatValue(UNIT_FIELD_MINDAMAGE,r>0?r:0);
		r = BaseDamage[1]*mult+delta+bonus;
		SetFloatValue(UNIT_FIELD_MAXDAMAGE,r>0?r:0);
	
	//	SetFloatValue(UNIT_FIELD_MINRANGEDDAMAGE,BaseRangedDamage[0]*mult+delta);
	//	SetFloatValue(UNIT_FIELD_MAXRANGEDDAMAGE,BaseRangedDamage[1]*mult+delta);
  
	}	
}

//returns absorbed dmg
uint32 Unit::ManaShieldAbsorb(uint32 dmg)
{
	if(!m_manashieldamt)
		return 0;
	//mana shield group->16. the only

	uint32 mana = GetUInt32Value(UNIT_FIELD_POWER1);
	int32 effectbonus = SM_PEffectBonus ? SM_PEffectBonus[16] : 0;

	uint32 potential = (mana*50)/((100+effectbonus));
	if(potential>m_manashieldamt)
		potential = m_manashieldamt;

	if(dmg<potential)
		potential = dmg;

	uint32 cost = (potential*(100+effectbonus))/50;

	SetUInt32Value(UNIT_FIELD_POWER1,mana-cost);
	m_manashieldamt -= potential;
	if(!m_manashieldamt)
		RemoveAura(m_manaShieldId);
	return potential;	
}

void Unit::VampiricEmbrace(uint32 dmg,Unit* tgt)
{
	if(!IsPlayer())
		return;//just in case
	int32 perc = 20;
	SM_FIValue(SM_FDummy,&perc,4);
	uint32 heal = (dmg*perc) / 100;

	WorldPacket data;
	data.SetOpcode(SMSG_HEALSPELL_ON_PLAYER_OBSOLETE);
	
	data << this->GetNewGUID();
	data << this->GetNewGUID();
	data << uint32(15286);  //Vampiric embrace
	data << uint32(heal);   // amt healed
	data << uint8(0);	   //this is crical message
	uint32 ch=this->GetUInt32Value(UNIT_FIELD_HEALTH);
	uint32 mh=this->GetUInt32Value(UNIT_FIELD_MAXHEALTH);
	if(mh!=ch)
	{
		ch += heal;
		if(ch > mh)
			this->SetUInt32Value(UNIT_FIELD_HEALTH, mh);
		else 
			this->SetUInt32Value(UNIT_FIELD_HEALTH, ch);
	
	}
	this->SendMessageToSet(&data, true);

	if(SubGroup* pGroup = static_cast<Player*>(this)->GetSubGroup())
	{
		GroupMembersSet::iterator itr;
		for(itr = pGroup->GetGroupMembersBegin(); itr != pGroup->GetGroupMembersEnd(); ++itr)
		{
			if((*itr) == this)
				continue;
			Player *p = (*itr);
			if(!p->isAlive())
				continue;
			data.clear();
			data << p->GetNewGUID();
			data << this->GetNewGUID();
			data << uint32(15286);//Vampiric embrace
			data << uint32(heal);	// amt healed
			data << uint8(0);	 //this is crical message
			
			ch = p->GetUInt32Value(UNIT_FIELD_HEALTH);
			mh = p->GetUInt32Value(UNIT_FIELD_MAXHEALTH);
			if(mh != ch)
			{
				ch += heal;
				if(ch > mh)
					p->SetUInt32Value(UNIT_FIELD_HEALTH,mh);
				else 
					p->SetUInt32Value(UNIT_FIELD_HEALTH,ch);			
			}
			this->SendMessageToSet(&data, true);
		}
	}

}

// based of vampiric embrace code
void Unit::VampiricTouch(uint32 dmg,Unit* tgt)
{
        if(!IsPlayer() || this->getClass() == WARRIOR || this->getClass() == ROGUE)
                return;//just in case
        
        int32 perc = 5;
        SM_FIValue(SM_FDummy,&perc,4); // need fixing if required
        uint32 man = (dmg*perc) / 100;
        
        uint32 cm=this->GetUInt32Value(UNIT_FIELD_POWER1);
        uint32 mm=this->GetUInt32Value(UNIT_FIELD_MAXPOWER1);
        
        if(mm!=cm)
        {
                cm += man;
                if(cm > mm)
                        this->SetUInt32Value(UNIT_FIELD_POWER1, mm);
                else 
                        this->SetUInt32Value(UNIT_FIELD_POWER1, cm);
        }
    
        if(SubGroup* pGroup = static_cast<Player*>(this)->GetSubGroup())
        {
                GroupMembersSet::iterator itr;
                for(itr = pGroup->GetGroupMembersBegin(); itr != pGroup->GetGroupMembersEnd(); ++itr)
                {
                        if((*itr) == this)
                                continue;
                        Player *p = (*itr);
                        if(!p->isAlive() || this->getClass()==WARRIOR || this->getClass() == ROGUE)
                                continue;
                        
                        uint32 cm=p->GetUInt32Value(UNIT_FIELD_POWER1);
                        uint32 mm=p->GetUInt32Value(UNIT_FIELD_MAXPOWER1);
            
                        if(mm!=cm)
                        {
                                cm += man;
                                if(cm > mm)
                                        this->SetUInt32Value(UNIT_FIELD_POWER1, mm);
                                else 
                                        this->SetUInt32Value(UNIT_FIELD_POWER1, cm);
                        }
                }
        }
}

// grep: Remove any AA spells that aren't owned by this player.
//		 Otherwise, if he logs out and in and joins another group,
//		 he'll apply it to them.
/*
void Unit::RemoveAllAreaAuras()
{
	AuraList::iterator itr,it1;
	for(itr = m_auras.begin();itr!=m_auras.end();)
	{
		it1 = itr++;
		if(((*it1)->m_spellProto->Effect[0] == SPELL_EFFECT_APPLY_AREA_AURA ||
			(*it1)->m_spellProto->Effect[1] == SPELL_EFFECT_APPLY_AREA_AURA ||
			(*it1)->m_spellProto->Effect[2] == SPELL_EFFECT_APPLY_AREA_AURA) && (*it1)->GetCaster() != this)
		{
			(*it1)->Remove();
			m_auras.erase(it1);
		} 
	}
}
*/
uint32 Unit::AbsorbDamage(uint32 School,uint32 * dmg)
{
	SchoolAbsorb::iterator i,j;
	uint32 abs=0;
	for(i=Absorbs[School].begin();i!=Absorbs[School].end();)
	{
		if (*dmg >= (*i)->amt)//remove this absorb
		{
			*dmg -= (*i)->amt;
			abs += (*i)->amt;
			j = i++;
			// protect iterator
			while(i != Absorbs[School].end() && (*i)->spellid == (*j)->spellid)
				continue;

			this->RemoveAura((*j)->spellid); //,(*j)->caster);
			if(!*dmg)//absorbed all dmg
				break;		
		}
		else //absorb full dmg
		{
			abs += *dmg;
			(*i)->amt -= *dmg;
			*dmg=0;
			break;
		}
	}
	
	return abs;
}

bool Unit::setDetectRangeMod(uint64 guid, int32 amount)
{
	int next_free_slot = -1;
	for(int i = 0; i < 5; i++)
	{
		if(m_detectRangeGUID[i] == 0 && next_free_slot == -1)
		{
			next_free_slot = i;
		}
		if(m_detectRangeGUID[i] == guid)
		{
			m_detectRangeMOD[i] = amount;
			return true;
		}
	}
	if(next_free_slot != -1)
	{
		m_detectRangeGUID[next_free_slot] = guid;
		m_detectRangeMOD[next_free_slot] = amount;
		return true;
	}
	return false;
}

void Unit::unsetDetectRangeMod(uint64 guid)
{
	for(int i = 0; i < 5; i++)
	{
		if(m_detectRangeGUID[i] == guid)
		{
			m_detectRangeGUID[i] = 0;
			m_detectRangeMOD[i] = 0;
		}
	}
}

int32 Unit::getDetectRangeMod(uint64 guid)
{
	for(int i = 0; i < 5; i++)
	{
		if(m_detectRangeGUID[i] == guid)
		{
			return m_detectRangeMOD[i];
		}
	}
	return 0;
}

void Unit::SetStandState(uint8 standstate)
{
	SetByte(UNIT_FIELD_BYTES_1,0,standstate);
	if(standstate == STANDSTATE_STAND)//standup
		RemoveAurasByInterruptFlag(AURA_INTERRUPT_ON_STAND_UP);

	if(m_objectTypeId == TYPEID_PLAYER)
		((Player*)this)->GetSession()->OutPacket(SMSG_STANDSTATE_CHANGE_ACK, 1, &standstate);
}

void Unit::RemoveAurasByInterruptFlag(uint32 flag)
{
	Aura * a;
	for(uint32 x=0;x<MAX_AURAS;x++)
	{
		a = m_auras[x];
		if(a == NULL)
			continue;

		if(a->m_spellProto->AuraInterruptFlags & flag)
		{
			a->Remove();
			m_auras[x] = NULL;
		}
	}
}

bool Unit::HasAura(uint32 visualid)
{
	for(uint32 x=0;x<MAX_AURAS+MAX_PASSIVE_AURAS;x++)
	if(m_auras[x] && m_auras[x]->GetSpellProto()->SpellVisual==visualid)
	{
		return true;
	}

	return false;
}

void Unit::DropAurasOnDeath()
{
	for(uint32 x=0;x<MAX_AURAS;x++)
	if(m_auras[x] && m_auras[x]->GetSpellProto()->Attributes!=671088640)//flask
	{
		m_auras[x]->Remove();
	}
}

void Unit::UpdateSpeed(bool delay /* = false */)
{
	if(!HasFlag( UNIT_FIELD_FLAGS , U_FIELD_FLAG_MOUNT_SIT ))
	{
		m_runSpeed = PLAYER_NORMAL_RUN_SPEED*(1.0f + ((float)m_speedModifier)/100.0f);
		m_flySpeed = PLAYER_NORMAL_FLIGHT_SPEED*(1.0f + ((float)m_speedModifier)/100.0f);
	}
	else
	{
		m_runSpeed = PLAYER_NORMAL_RUN_SPEED*(1.0f + ((float)m_mountedspeedModifier)/100.0f);
		m_flySpeed = PLAYER_NORMAL_FLIGHT_SPEED*(1.0f + ((float)m_flyspeedModifier)/100.0f);
	}
 
	if(IsPlayer())
	{
		if(delay)
			static_cast<Player*>(this)->resend_speed = delay;
		else
		{
			static_cast<Player*>(this)->SetPlayerSpeed(RUN, m_runSpeed);
			static_cast<Player*>(this)->SetPlayerSpeed(FLY, m_flySpeed);
		}
	}
}

bool Unit::HasActiveAura(uint32 spellid)
{
	for(uint32 x=0;x<MAX_AURAS;x++)
	if(m_auras[x] && m_auras[x]->GetSpellId()==spellid)
	{
		return true;
	}

	return false;
}

bool Unit::HasActiveAura(uint32 spellid,uint64 guid)
{
	for(uint32 x=0;x<MAX_AURAS;x++)
	if(m_auras[x] && m_auras[x]->GetSpellId()==spellid && m_auras[x]->m_casterGuid==guid)
	{
		return true;
	}

	return false;
}

void Unit::addAttacker(Unit *pUnit)
{
	if(!pUnit->isAlive() || !isAlive())
		return;

	// this shouldn't happen
	ASSERT(pUnit != NULL);

	// being attacked by this unit
	if(m_attackers.count(pUnit->GetGUID()) == 0)
		m_attackers.insert(pUnit->GetGUID());
}

void Unit::removeAttacker(Unit *pUnit)
{
	// this should't happen
	ASSERT(pUnit != NULL);

	// crashfix at shutdown
//	if(m_attackers.size() == 0) return;

	// no longer being attacked by unit
	m_attackers.erase(pUnit->GetGUID());
}

void Unit::setAttackTarget(Unit* pUnit)
{
	if(!isAlive())
		pUnit = 0;

	if(pUnit == NULL)
	{
		// check if we have a current target
		// if so, remove from their attacker set
		if(m_attackTarget)
		{
			Unit *pTarget = GetMapMgr()->GetUnit(m_attackTarget);
			if(pTarget)
				pTarget->removeAttacker(this);
		}

		// now, clear ourselves and abort
		m_attackTarget = 0;
		return;		
	}

	// can't attack self
	if(pUnit == this)
		return;

	// already done -> skip
	if(m_attackTarget == pUnit->GetGUID())
		return;

	// remove from our old target, if we have one
	// this will stop attackers getting "lost" and people
	// "stuck" in combat.

	if(m_attackTarget != 0)
	{
		Unit *pTarget = GetMapMgr()->GetUnit(m_attackTarget);
		if(pTarget)
			pTarget->removeAttacker(this);
	}

	// add to victim's attacker set
	pUnit->addAttacker(this);

	// set our value
	m_attackTarget = pUnit->GetGUID();
}

void Unit::clearAttackers(bool bFromOther)
{
	// this will _usually_ be called on death.
	// so, we will remove ourselves from our target's set,
	// and clear our target set

	// clear our target
	if(bFromOther)
		setAttackTarget(NULL);

	// skip this shit if we don't have any attackers
	if(m_attackers.size() == 0)
		return;
	
	if(bFromOther && IsInWorld())
	{
		// push our existing attackers into a temp vector
		// because they will erase from our attacker set when they have their target
		// reset, we need to have another place to store them
		std::vector<Unit*> tmp_vec;
		tmp_vec.reserve(m_attackers.size());
		Unit *pUnit;

		for(AttackerSet::iterator itr = m_attackers.begin(); itr != m_attackers.end(); ++itr)
		{
			pUnit = GetMapMgr()->GetUnit(*itr);
			if(pUnit)
				tmp_vec.push_back(pUnit);
		}

		// clear any attackers from us
		for(std::vector<Unit*>::iterator itr = tmp_vec.begin(); itr != tmp_vec.end(); ++itr)
		{
			pUnit = (*itr);
			if(pUnit->getAttackTarget() == GetGUID())
				pUnit->setAttackTarget(NULL);
		}
	}

	// wipe our attacker set (we shouldnt have any unknowns)
	m_attackers.clear();
}

void Unit::EventSummonPetExpire()
{
	if(summonPet)
	{
		if(summonPet->GetEntry() == 7915)//Goblin Bomb
		{
			SpellEntry *spInfo = sSpellStore.LookupEntry(13259);
			if(!spInfo)
				return;

			Spell*sp=new Spell(summonPet,spInfo,true,NULL);
			SpellCastTargets tgt;
			tgt.m_unitTarget=summonPet->GetGUID();
			sp->prepare(&tgt);
		}
		else
		{
			summonPet->m_BeingRemoved = true;
			summonPet->RemoveFromWorld(false);
			delete summonPet;
			summonPet = NULL;
		}
	}
	sEventMgr.RemoveEvents(this, EVENT_SUMMON_PET_EXPIRE);
}

void Unit::CastSpell(Unit* Target, SpellEntry* Sp, bool triggered)
{
	SpellCastTargets targets(Target->GetGUID());
	Spell *newSpell = new Spell(this, Sp, triggered, 0);
	newSpell->prepare(&targets);
}

void Unit::CastSpell(Unit* Target, uint32 SpellID, bool triggered)
{
	SpellEntry * ent = sSpellStore.LookupEntry(SpellID);
	if(ent == 0) return;

	CastSpell(Target, ent, triggered);
}

void Unit::CastSpell(uint64 targetGuid, SpellEntry* Sp, bool triggered)
{
	SpellCastTargets targets(targetGuid);
	Spell *newSpell = new Spell(this, Sp, triggered, 0);
	newSpell->prepare(&targets);
}

void Unit::CastSpell(uint64 targetGuid, uint32 SpellID, bool triggered)
{
	SpellEntry * ent = sSpellStore.LookupEntry(SpellID);
	if(ent == 0) return;

	CastSpell(targetGuid, ent, triggered);
}
void Unit::CastSpellAoF(float x,float y,float z,SpellEntry* Sp, bool triggered)
{
	SpellCastTargets targets;
	targets.m_destX = x;
	targets.m_destY = y;
	targets.m_destZ = z;
	targets.m_targetMask=TARGET_FLAG_DEST_LOCATION;
	Spell *newSpell = new Spell(this, Sp, triggered, 0);
	newSpell->prepare(&targets);
}

void Unit::PlaySoundToSet(uint32 sound_entry)
{
	WorldPacket data(SMSG_PLAY_SOUND, 4);
	data << sound_entry;
	SendMessageToSet(&data, true);
}

void Unit::PlaySpellVisual(uint64 target, uint32 spellVisual)
{
	WorldPacket data(SMSG_PLAY_SPELL_VISUAL, 12);
	data << target << spellVisual;
	SendMessageToSet(&data, true);
}

void Unit::Root()
{
	if(m_objectTypeId == TYPEID_PLAYER)
	{
		static_cast<Player*>(this)->SetMovement(MOVE_ROOT, 1);
	}
	else
	{
		m_aiInterface->m_canMove = false;
	}
}

void Unit::Unroot()
{
	if(m_objectTypeId == TYPEID_PLAYER)
	{
		static_cast<Player*>(this)->SetMovement(MOVE_UNROOT, 5);
	}
	else
	{
		m_aiInterface->m_canMove = true;
	}
}

void Unit::RemoveAurasByBuffType(uint32 buff_type, uint64 guid)
{
	uint64 sguid = buff_type >= SPELL_TYPE_BLESSING ? guid : 0;

	for(uint32 x=0;x<MAX_AURAS;x++)
	{
		if(m_auras[x] && m_auras[x]->GetSpellProto()->buffType & buff_type)
			if(!sguid || (sguid && m_auras[x]->m_casterGuid == sguid))
				m_auras[x]->Remove();
	}
}

bool Unit::HasAurasOfBuffType(uint32 buff_type, uint64 guid)
{
	uint64 sguid = buff_type >= SPELL_TYPE_BLESSING ? guid : 0;

	for(uint32 x=0;x<MAX_AURAS;x++)
	{
		if(m_auras[x] && m_auras[x]->GetSpellProto()->buffType & buff_type)
			if(!sguid || (sguid && m_auras[x]->m_casterGuid == sguid))
				return true;
	}

	return false;
}

AuraCheckResponse Unit::AuraCheck(uint32 name_hash, uint32 rank)
{
	AuraCheckResponse resp;

	// no error for now
	resp.Error = AURA_CHECK_RESULT_NONE;
	resp.Misc  = 0;

	// look for spells with same namehash
	for(uint32 x=0;x<MAX_AURAS;x++)
	{
		if(m_auras[x] && m_auras[x]->GetSpellProto()->NameHash == name_hash)
		{
			// we've got an aura with the same name as the one we're trying to apply
			resp.Misc = m_auras[x]->GetSpellProto()->Id;

			// compare the rank to our applying spell
			if(m_auras[x]->GetSpellProto()->RankNumber > rank)
				resp.Error = AURA_CHECK_RESULT_HIGHER_BUFF_PRESENT;
			else
				resp.Error = AURA_CHECK_RESULT_LOWER_BUFF_PRESENT;

			// we found something, save some loops and exit
			break;
		}
	}

	// return it back to our caller
	return resp;
}

AuraCheckResponse Unit::AuraCheck(uint32 name_hash, uint32 rank, Aura* aur)
{
	AuraCheckResponse resp;

	// no error for now
	resp.Error = AURA_CHECK_RESULT_NONE;
	resp.Misc  = 0;

	// look for spells with same namehash
	if(aur->GetSpellProto()->NameHash == name_hash)
	{
		// we've got an aura with the same name as the one we're trying to apply
		resp.Misc = aur->GetSpellProto()->Id;

		// compare the rank to our applying spell
		if(aur->GetSpellProto()->RankNumber > rank)
			resp.Error = AURA_CHECK_RESULT_HIGHER_BUFF_PRESENT;
		else
			resp.Error = AURA_CHECK_RESULT_LOWER_BUFF_PRESENT;
	}

	// return it back to our caller
	return resp;
}

void Unit::AddToWorld()
{
	Object::AddToWorld();
}

void Unit::OnPushToWorld()
{
	for(uint32 x = 0; x < MAX_AURAS+MAX_PASSIVE_AURAS; ++x)
	{
		if(m_auras[x] != 0)
			m_auras[x]->RelocateEvents();
	}
}

void Unit::RemoveFromWorld()
{
	if(dynObj != 0)
		dynObj->Remove();

	for(uint32 i = 0; i < 4; ++i)
	{
		if(m_ObjectSlots[i] != 0)
		{
			GameObject * obj = m_mapMgr->GetGameObject(m_ObjectSlots[i]);
			if(obj)
				obj->ExpireAndDelete();

			m_ObjectSlots[i] = 0;
		}
	}

	Object::RemoveFromWorld();


	for(uint32 x = 0; x < MAX_AURAS+MAX_PASSIVE_AURAS; ++x)
	{
		if(m_auras[x] != 0)
			m_auras[x]->RelocateEvents();
	}
	m_aiInterface->WipeReferences();
}

void Unit::RemoveAurasByInterruptFlagButSkip(uint32 flag, uint32 skip)
{
	Aura * a;
	for(uint32 x=0;x<MAX_AURAS;x++)
	{
		a = m_auras[x];
		if(a == 0)
			continue;

		if(a->m_spellProto->AuraInterruptFlags & flag && a->m_spellProto->Id != skip)
		{
			// Fix for instant cast spell removing presence of mind and shit
			if(flag==AURA_INTERRUPT_ON_CAST_SPELL&&a->m_spellProto->procCharges) continue;
			a->Remove();
		}
	}
}

bool Unit::HasAurasWithNameHash(uint32 name_hash)
{
	for(uint32 x = 0; x < MAX_AURAS; ++x)
	{
		if(m_auras[x] && m_auras[x]->GetSpellProto()->NameHash == name_hash)
			return true;
	}

	return false;
}

uint32 Unit::AddAuraVisual(uint32 spellid, uint32 count, bool positive)
{
	int32 free = -1;
	uint32 start = positive ? 0 : MAX_POSITIVE_AURAS;
	uint32 end_  = positive ? MAX_POSITIVE_AURAS : MAX_AURAS;

	for(uint32 x = start; x < end_; ++x)
	{
		if(free == -1 && m_uint32Values[UNIT_FIELD_AURA+x] == 0)
			free = x;

		if(m_uint32Values[UNIT_FIELD_AURA+x] == spellid)
		{
			// Increase count of this aura.
			ModAuraStackCount(x, count);
			return free;
		}
	}

	if(free == -1) return 0xFF;

	uint8 flagslot = free >> 3;
	uint32 value = GetUInt32Value((uint16)(UNIT_FIELD_AURAFLAGS + flagslot));

	uint8 aurapos = (free & 7) << 2;
	uint32 setflag = AFLAG_SET;
	if(positive)
		setflag = 0xD;

	uint32 value1 = (uint32)setflag << aurapos;
	value |= value1;

	SetUInt32Value((uint16)(UNIT_FIELD_AURAFLAGS + flagslot), value);
	SetUInt32Value(UNIT_FIELD_AURA + free, spellid);
	ModAuraStackCount(free, 1);
	SetAuraSlotLevel(free, positive);
	
	return free;
}

void Unit::SetAuraSlotLevel(uint32 slot, bool positive)
{
	uint32 index = slot / 4;
	uint32 val = m_uint32Values[UNIT_FIELD_AURALEVELS + index];
	uint32 bit = (slot % 4) * 8;
	val &= ~(0xFF << bit);
	if(positive)
		val |= (0x11 << bit);

	SetUInt32Value(UNIT_FIELD_AURALEVELS + index, val);
}

void Unit::RemoveAuraVisual(uint32 spellid, uint32 count)
{
	for(uint32 x = 0; x < MAX_AURAS; ++x)
	{
		if(m_uint32Values[UNIT_FIELD_AURA+x] == spellid)
		{
			// Decrease count of this aura.
			int test = ModAuraStackCount(x, -(int32)count);
			if(test == 0)
			{
				// Aura has been removed completely.
				uint8 flagslot = x >> 3;
				uint32 value = GetUInt32Value((uint16)(UNIT_FIELD_AURAFLAGS + flagslot));
				uint8 aurapos = (x & 7) << 2;
				uint32 value1 = ~( (uint32)0xF << aurapos );
				value &= value1;
				SetUInt32Value(UNIT_FIELD_AURAFLAGS + flagslot,value);
				SetUInt32Value(UNIT_FIELD_AURA + x, 0);
				SetAuraSlotLevel(x, false);
			}			
		}
	}
}

uint32 Unit::ModAuraStackCount(uint32 slot, int32 count)
{
	uint32 index = (slot / 4);
	uint32 byte  = (slot % 4);
	uint32 val   = m_uint32Values[UNIT_FIELD_AURAAPPLICATIONS+index];

	// shouldn't happen
	uint32 ac;

	if(count < 0 && m_auraStackCount[slot] < abs(count))
	{
		m_auraStackCount[slot] = ac = 0;
	}
	else
	{
		m_auraStackCount[slot] += count;
		ac = (m_auraStackCount[slot] > 0) ? m_auraStackCount[slot] - 1 : 0;
	}

	val &= ~(0xFF << byte * 8);
	val |= (ac << byte * 8);

	SetUInt32Value(UNIT_FIELD_AURAAPPLICATIONS+index, val);
	return m_auraStackCount[slot];
}

void Unit::RemoveAurasOfSchool(uint32 School, bool Positive)
{
	for(uint32 x = 0; x < MAX_AURAS; ++x)
	{
		if(m_auras[x] && m_auras[x]->GetSpellProto()->School == School && (!m_auras[x]->IsPositive() || Positive))
			m_auras[x]->Remove();
	}
}

void Unit::EnableFlight(bool delay /* = false */)
{
	if(!delay || m_objectTypeId != TYPEID_PLAYER)
	{
		WorldPacket data(SMSG_MOVE_SET_FLY, 13);
		data << GetNewGUID();
		data << uint32(2);
		SendMessageToSet(&data, true);
	}
	else
	{
		WorldPacket * data = new WorldPacket(SMSG_MOVE_SET_FLY, 13);
		*data << GetNewGUID();
		*data << uint32(2);
		SendMessageToSet(data, false);
		static_cast<Player*>(this)->delayedPackets.add(data);
	}
}

void Unit::DisableFlight(bool delay /* = false */)
{
	if(!delay || m_objectTypeId != TYPEID_PLAYER)
	{
		WorldPacket data(SMSG_MOVE_SET_UNFLY, 13);
		data << GetNewGUID();
		data << uint32(5);
		SendMessageToSet(&data, true);
	}
	else
	{
		WorldPacket * data = new WorldPacket(SMSG_MOVE_SET_UNFLY, 13);
		*data << GetNewGUID();
		*data << uint32(5);
		SendMessageToSet(data, false);
		static_cast<Player*>(this)->delayedPackets.add(data);
	}
}

bool Unit::IsDazed()
{
	for(uint32 x = 0; x < MAX_AURAS; ++x)
	{
		if(m_auras[x])
		{
			if(m_auras[x]->GetSpellProto()->MechanicsType == MECHANIC_ENSNARED)
			return true;
			for(uint32 y=0;y<3;y++)
			if(m_auras[x]->GetSpellProto()->EffectMechanic[y]==MECHANIC_ENSNARED)
				return true;
		}
	}

	return false;

}

void Unit::UpdateVisibility()
{
	ByteBuffer buf(2500);
	uint32 count;
	bool can_see;
	bool is_visible;
	Player *pl;
	Object * pObj;
	Player * plr;

	if(m_objectTypeId == TYPEID_PLAYER)
	{
		plr = ((Player*)this);
		for (Object::InRangeSet::iterator itr = m_objectsInRange.begin(); itr != m_objectsInRange.end();)
		{
			pObj = (*itr);
			++itr;

			can_see = plr->CanSee(pObj);
			is_visible = plr->IsVisible(pObj);
			if(can_see)
			{
				if(!is_visible)
				{
					buf.clear();
					count = pObj->BuildCreateUpdateBlockForPlayer( &buf, plr );
					plr->PushUpdateData(&buf, count);
					plr->AddVisibleObject(pObj);
				}
			}
			else
			{
				if(is_visible)
				{
					pObj->DestroyForPlayer(plr);
					plr->RemoveVisibleObject(pObj);
				}
			}

			if (pObj->GetTypeId() == TYPEID_PLAYER)
			{
				pl = ((Player*)pObj);
				can_see = pl->CanSee(plr);
				is_visible = pl->IsVisible(plr);
				if(can_see)
				{
					if(!is_visible)
					{
						buf.clear();
						count = plr->BuildCreateUpdateBlockForPlayer( &buf, pl );
						pl->PushUpdateData(&buf, count);
						pl->AddVisibleObject(plr);
					}
				}
				else
				{
					if(is_visible)
					{
						plr->DestroyForPlayer(pl);
						pl->RemoveVisibleObject(plr);
					}
				}
			}
		}
	}
	else			// For units we can save a lot of work
	{
		for(set<Player*>::iterator itr = GetInRangePlayerSetBegin(); itr != GetInRangePlayerSetEnd(); ++itr)
		{
			can_see = (*itr)->CanSee(this);
			is_visible = (*itr)->IsVisible(this);
			if(!can_see)
			{
				if(is_visible)
				{
					DestroyForPlayer(*itr);
					(*itr)->RemoveVisibleObject(this);
				}
			}
			else
			{
				if(!is_visible)
				{
					buf.clear();
					count = BuildCreateUpdateBlockForPlayer(&buf, *itr);
					(*itr)->PushUpdateData(&buf, count);
					(*itr)->AddVisibleObject(this);
				}
			}
		}
	}
}

void Unit::RemoveSoloAura(uint32 type)
{
	switch(type)
	{
		case 1:// Polymorph
		{
			if(!polySpell) return;
			if(HasActiveAura(polySpell))
				RemoveAura(polySpell);
		}break;
/*		case 2:// Sap
		{
			if(!sapSpell) return;
			if(HasActiveAura(sapSpell))
				RemoveAura(sapSpell);
		}break;
		case 3:// Fear (Warlock)
		{
			if(!fearSpell) return;
			if(HasActiveAura(fearSpell))
				RemoveAura(fearSpell);
		}break;*/
		default:
			{
			sLog.outDebug("Warning: we are trying to remove a soloauratype that has no handle");
			}break;
	}
}

void Unit::UpdateOppFactionSet()
{
	m_oppFactsInRange.clear();

	for(Object::InRangeSet::iterator i = GetInRangeSetBegin(); i != GetInRangeSetEnd(); ++i)
	{
		if (((*i)->GetTypeId() == TYPEID_UNIT) || ((*i)->GetTypeId() == TYPEID_PLAYER))
		{
			if(isHostile(this, (*i)))
			{
				if(m_objectTypeId == TYPEID_UNIT)
					m_oppFactsInRange.insert(((Unit*)*i));

				if((*i)->GetTypeId() == TYPEID_UNIT)
					((Unit*)*i)->m_oppFactsInRange.insert(this);
			}
			else
			{
				if((*i)->GetTypeId() == TYPEID_UNIT)
					((Unit*)*i)->m_oppFactsInRange.erase(this);
			}
		}
	}
}
