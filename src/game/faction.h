/****************************************************************************
 *
 * Faction Checks
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

#ifndef __FACTION_H
#define __FACTION_H

#include "Unit.h"


#include "StdAfx.h"

inline bool isHostile(Object* objA, Object* objB)// B is hostile for A?
{
    if(!objA || !objB)
        return false;
    bool hostile = false;

    if(objB->m_faction == NULL || objA->m_faction == NULL)
        return true;

    if(objA == objB)
        return false;   // can't attack self.. this causes problems with buffs if we dont have it :p

    if(objA->GetTypeId() == TYPEID_CORPSE)
        return false;

    if(objB->GetTypeId() == TYPEID_CORPSE)
        return false;

    uint32 faction = objB->m_faction->Mask;
    uint32 host = objA->m_faction->HostileMask;

    if(faction & host)
    {
        hostile = true;
    }
    // check friend/enemy list
    for(uint32 i = 0; i < 4; i++)
    {
        if(objA->m_faction->EnemyFactions[i] == objB->m_faction->Faction)
        {
            hostile = true;
            break;
        }
        if(objA->m_faction->FriendlyFactions[i] == objB->m_faction->Faction)
        {
            hostile = false;
            break;
        }
    }

    // PvP Flag System Checks
    // We check this after the normal isHostile test, that way if we're
    // on the opposite team we'll already know :p

    if(hostile && 
        objA->IsPlayer() && objB->IsPlayer())
    {
        // Check PvP Flags.
        if(static_cast<Player*>(objB)->IsPvPFlagged())
            return true;
        else
            return false;
    }

    // Reputation System Checks
    if(objA->IsPlayer() && !objB->IsPlayer())       // PvE
    {
        if(objB->m_factionDBC->RepListId >= 0)
            hostile = ((Player*)objA)->IsHostileBasedOnReputation(objB->m_factionDBC);
    }
    
    if(objB->IsPlayer() && !objA->IsPlayer())       // PvE
    {
        if(objA->m_factionDBC->RepListId >= 0)
            hostile = ((Player*)objB)->IsHostileBasedOnReputation(objA->m_factionDBC);
    }

    return hostile;
}

inline bool isAttackable(Object* objA, Object* objB)// A can attack B?
{
    if(!objA || !objB || objB->m_factionDBC == NULL || objA->m_factionDBC == NULL)
        return false;

    if(objB->m_faction == NULL || objA->m_faction == NULL )
        return true;

    if(objA == objB)
        return false;   // can't attack self.. this causes problems with buffs if we dont have it :p

    if(objA->GetTypeId() == TYPEID_CORPSE)
        return false;

    if(objB->GetTypeId() == TYPEID_CORPSE)
        return false;

    if(objA->IsPlayer() && objB->IsPlayer())
    {
        if(
            static_cast<Player *>(objA)->DuelingWith == static_cast<Player *>(objB) && 
            static_cast<Player *>(objA)->GetDuelState() == DUEL_STATE_STARTED
            )
        return true;                    

        if(objA->HasFlag(PLAYER_FLAGS,PLAYER_FLAG_FREE_FOR_ALL_PVP) && objB->HasFlag(PLAYER_FLAGS,PLAYER_FLAG_FREE_FOR_ALL_PVP))
            return true;        // can hurt each other in FFA pvp
    }

    if(objA->m_faction == objB->m_faction)  // same faction can't kill each other unless in ffa pvp/duel
        return false;

    bool attackable = isHostile(objA, objB); // B is attackable if its hostile for A
    /*if((objA->m_faction->HostileMask & 8) && (objB->m_factionDBC->RepListId != 0) && 
        (objB->GetTypeId() != TYPEID_PLAYER) && objB->m_faction->Faction != 31) // B is attackable if its a neutral Creature*/

	// Neutral Creature Check
	if(objA->IsPlayer())
	{
		if(objB->m_factionDBC->RepListId == -1 && objB->m_faction->HostileMask == 0 && objB->m_faction->FriendlyMask == 0)
		{
			attackable = true;
		}
	}
	else if(objB->IsPlayer())
	{
		if(objA->m_factionDBC->RepListId == -1 && objA->m_faction->HostileMask == 0 && objA->m_faction->FriendlyMask == 0)
		{
			attackable = true;
		}
	}

    return attackable;
}



inline bool isFriendly(Object* objA, Object* objB)// B is friendly to A if its not hostile
{
    return !isHostile(objA, objB);
}

inline bool isCombatSupport(Object* objA, Object* objB)// B combat supports A?
{
    if(!objA || !objB)
        return false;

    if(objA->GetTypeId() == TYPEID_CORPSE)
        return false;

    if(objB->GetTypeId() == TYPEID_CORPSE)
        return false;

    if(objB->m_faction == 0 || objA->m_faction == 0)
        return false;

    bool combatSupport = false;

    uint32 fSupport = objB->m_faction->FriendlyMask;
    uint32 myFaction = objA->m_faction->Mask;

    if(myFaction & fSupport)
    {
        combatSupport = true;
    }
    // check friend/enemy list
    for(uint32 i = 0; i < 4; i++)
    {
        if(objB->m_faction->EnemyFactions[i] == objA->m_faction->Faction)
        {
            combatSupport = false;
            break;
        }
        if(objB->m_faction->FriendlyFactions[i] == objA->m_faction->Faction)
        {
            combatSupport = true;
            break;
        }
    }
    return combatSupport;
}
#endif
