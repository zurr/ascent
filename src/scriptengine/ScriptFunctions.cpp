/****************************************************************************
 *
 * Script Engine
 * Copyright (c) 2007 Antrix Team
 *
 * This file may be distributed under the terms of the Q Public License
 * as defined by Trolltech ASA of Norway and appearing in the file
 * COPYING included in the packaging of this file.
 *
 * This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 * WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 * GameMonkey Script License
 * Copyright (c) 2003 Auran Development Ltd.
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
 * associated documentation files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do
 * so, subject to the following conditions:
 * The above copyright notice and this permission notice shall be included in all copies or substantial
 * portions of the Software.
 *
 */

#include "StdAfx.h"

int ScriptEngine_RegisterAreaTriggerEvent(gmThread * a_thread)
{
	// Param 1: Areatrigger ID
	// Param 2: Function to execute
	GM_CHECK_NUM_PARAMS(2);
	GM_CHECK_INT_PARAM(entry, 0);
	GM_CHECK_FUNCTION_PARAM(func, 1);
	
	// Get 'this' pointer pointing to scriptengine
	ScriptEngine * eng = GetThisPointer<ScriptEngine>(a_thread);
	eng->AddAreaTriggerEvent(entry, func);
	
	return GM_OK;
}

int ScriptEngine_RegisterGameObjectEvent(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(3);
	GM_CHECK_INT_PARAM(entry, 0);
	GM_CHECK_INT_PARAM(event, 1);
	GM_CHECK_FUNCTION_PARAM(func, 2);

	GetThisPointer<ScriptEngine>(a_thread)->AddGameObjectEvent(entry, event, func);
	return GM_OK;
}

int ScriptEngine_RegisterSpellEvent(gmThread * a_thread)
{
	return GM_OK;
}

int ScriptEngine_RegisterUnitEvent(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(3);
	GM_CHECK_INT_PARAM(entry, 0);
	GM_CHECK_INT_PARAM(event, 1);
	GM_CHECK_FUNCTION_PARAM(func, 2);

	GetThisPointer<ScriptEngine>(a_thread)->AddCreatureEvent(entry, event, func);
	return GM_OK;
}

int ScriptEngine_RegisterPlayerEvent(gmThread * a_thread)
{
	return GM_OK;
}

int ScriptEngine_RegisterQuestEvent(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(3);
	GM_CHECK_INT_PARAM(entry, 0);
	GM_CHECK_INT_PARAM(event, 1);
	GM_CHECK_FUNCTION_PARAM(func, 2);

	GetThisPointer<ScriptEngine>(a_thread)->AddQuestEvent(entry, event, func);
	return GM_OK;
}

// Player Functions
int Player_Teleport(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(4);
	GM_CHECK_INT_PARAM(mapId, 0);
	GM_CHECK_FLOAT_PARAM(posX, 1);
	GM_CHECK_FLOAT_PARAM(posY, 2);
	GM_CHECK_FLOAT_PARAM(posZ, 3);

	if(GetThisPointer<Player>(a_thread)->GetTypeId() != TYPEID_PLAYER)
		return GM_EXCEPTION;

	GetThisPointer<Player>(a_thread)->SafeTeleport(mapId, 0, posX, posY, posZ, 0);
	return GM_OK;
}

int Player_GetLevel(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(0);
	if(GetThisPointer<Player>(a_thread)->GetTypeId() != TYPEID_PLAYER)
		return GM_EXCEPTION;

	a_thread->PushInt((int)GetThisPointer<Player>(a_thread)->getLevel());
	return GM_OK;
}

int Player_GetClass(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(0);
	if(GetThisPointer<Player>(a_thread)->GetTypeId() != TYPEID_PLAYER)
		return GM_EXCEPTION;

	a_thread->PushInt((int)GetThisPointer<Player>(a_thread)->getClass());
	return GM_OK;
}

int Player_GetRace(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(0);
	if(GetThisPointer<Player>(a_thread)->GetTypeId() != TYPEID_PLAYER)
		return GM_EXCEPTION;

	a_thread->PushInt((int)GetThisPointer<Player>(a_thread)->getRace());
	return GM_OK;
}

int Player_SendAreaTriggerMessage(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(1);
	GM_CHECK_STRING_PARAM(message, 0);

	if(GetThisPointer<Player>(a_thread)->GetTypeId() != TYPEID_PLAYER)
		return GM_EXCEPTION;

	GetThisPointer<Player>(a_thread)->SendAreaTriggerMessage(message);
	return GM_OK;
}

int Player_BroadcastMessage(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(1);
	GM_CHECK_STRING_PARAM(message, 1);

	if(GetThisPointer<Player>(a_thread)->GetTypeId() != TYPEID_PLAYER)
		return GM_EXCEPTION;

	GetThisPointer<Player>(a_thread)->BroadcastMessage(message);
	return GM_OK;
}

int Player_GetReputationRank(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(1);
	GM_CHECK_INT_PARAM(faction, 0);
	if(GetThisPointer<Player>(a_thread)->GetTypeId() != TYPEID_PLAYER)
		return GM_EXCEPTION;

	a_thread->PushInt(GetThisPointer<Player>(a_thread)->GetStandingRank(faction));
    return GM_OK;
}

int Player_GetReputationValue(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(1);
	GM_CHECK_INT_PARAM(faction, 0);
	if(GetThisPointer<Player>(a_thread)->GetTypeId() != TYPEID_PLAYER)
		return GM_EXCEPTION;

	a_thread->PushInt(GetThisPointer<Player>(a_thread)->GetStanding(faction));
	return GM_OK;
}

int Player_HasFinishedQuest(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(1);
	GM_CHECK_INT_PARAM(questid, 0);
	if(GetThisPointer<Player>(a_thread)->GetTypeId() != TYPEID_PLAYER)
		return GM_EXCEPTION;

	Player *p = GetThisPointer<Player>(a_thread);
	if(p->HasFinishedQuest(questid))
		a_thread->PushInt(1);
	else
		a_thread->PushInt(0);

	return GM_OK;
}

int Player_IsGroupLeader(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(0);
	if(GetThisPointer<Player>(a_thread)->GetTypeId() != TYPEID_PLAYER)
		return GM_EXCEPTION;

	Player * p = GetThisPointer<Player>(a_thread);
	if(p->InGroup() && p->IsGroupLeader())
		a_thread->PushInt(1);
	else
		a_thread->PushInt(0);

	return GM_OK;
}

/* Areatrigger events */
int AreaTrigger_GetEntry(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(0);
	a_thread->PushInt(GetThisPointer<AreaTrigger>(a_thread)->AreaTriggerID);
	return GM_OK;
}


/* Unit events */
int Unit_Despawn(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(2);
	GM_CHECK_INT_PARAM(delay, 0);
	GM_CHECK_INT_PARAM(respawntime, 1);

	Unit * pUnit = GetThisPointer<Unit>(a_thread);
	if(pUnit->GetTypeId() != TYPEID_UNIT)
		return GM_EXCEPTION;

	Creature * pCreature = ((Creature*)pUnit);
	pCreature->Despawn(delay, respawntime);

	return GM_OK;
}

int Unit_Emote(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(1);
	GM_CHECK_INT_PARAM(emote, 0);

	Unit * pUnit = GetThisPointer<Unit>(a_thread);
	pUnit->Emote((EmoteType)emote);
	return GM_OK;
}

int Unit_SendChatMessage(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(1);
	GM_CHECK_STRING_PARAM(msg, 0);

	Unit * pUnit = GetThisPointer<Unit>(a_thread);
	pUnit->SendChatMessage(CHAT_MSG_MONSTER_SAY, LANG_UNIVERSAL, msg);
	return GM_OK;
}

int Unit_CastSpell(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(1);
	GM_CHECK_INT_PARAM(spellid, 0);

	Unit * pUnit = GetThisPointer<Unit>(a_thread);
	pUnit->CastSpell(pUnit, sSpellStore.LookupEntry(spellid), true);
	return GM_OK;
}
int Unit_SetStandState(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(1);
	GM_CHECK_INT_PARAM(stateid, 0);

	Unit * pUnit = GetThisPointer<Unit>(a_thread);
	pUnit->SetStandState(stateid);

	return GM_OK;
}
int Player_AddItem(gmThread * a_thread)
{
	bool push = false;
	if(a_thread->GetNumParams() == 2)
	{
		// Assume to send an item push
		push = true;
	}
	else if(a_thread->GetNumParams() == 3)
	{
		// Choice for push
		GM_CHECK_INT_PARAM(push_, 2);
		push = (push_ > 0) ? true : false;
	}
	else
        GM_CHECK_NUM_PARAMS(2);

	GM_CHECK_INT_PARAM(itemid, 0);
	GM_CHECK_INT_PARAM(count, 1);

	Player * pPlayer = GetThisPointer<Player>(a_thread);
	if(pPlayer->GetTypeId() != TYPEID_PLAYER)
		return GM_EXCEPTION;

	ItemPrototype * proto = ItemPrototypeStorage.LookupEntry(itemid);
	if(!proto)
		return GM_EXCEPTION;

	if(pPlayer->GetItemInterface()->CanReceiveItem(proto, count) == 0)
	{
		int acount;
		int bcount;
		if(proto->MaxCount && proto->MaxCount <= count)
		{
			acount = 1;
			bcount = count;
		}
		else
		{
			acount = count;
			bcount = 0;
		}

		for(int i = 0; i < acount; ++i)
		{
			Item * pItem = objmgr.CreateItem(itemid, pPlayer);
			if(!pPlayer->GetItemInterface()->AddItemToFreeSlot(pItem))
			{
				delete pItem;
				break;
			}
		}

		if(push)
		{
			WorldPacket data(50);
			pPlayer->GetSession()->BuildItemPushResult(&data, pPlayer->GetGUID(), ITEM_PUSH_TYPE_RECEIVE, count, itemid, 0);
			pPlayer->GetSession()->SendPacket(&data);
		}
	}

	return GM_OK;
}

int Player_RemoveItem(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(2);
	GM_CHECK_INT_PARAM(itemid, 0);
	GM_CHECK_INT_PARAM(count, 1);

	Player * pPlayer = GetThisPointer<Player>(a_thread);
	if(pPlayer->GetTypeId() != TYPEID_PLAYER)
		return GM_EXCEPTION;

	pPlayer->GetItemInterface()->RemoveItemAmt(itemid, count);
	return GM_OK;
}

int Player_LearnSpell(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(1);
	GM_CHECK_INT_PARAM(spellid, 0);

	if(GetThisPointer<Player>(a_thread)->GetTypeId() != TYPEID_PLAYER)
		return GM_EXCEPTION;

	GetThisPointer<Player>(a_thread)->addSpell(spellid);
	return GM_OK;
}

int Player_RemoveSpell(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(1);
	GM_CHECK_INT_PARAM(spellid, 0);
	if(GetThisPointer<Player>(a_thread)->GetTypeId() != TYPEID_PLAYER)
		return GM_EXCEPTION;

	GetThisPointer<Player>(a_thread)->removeSpell(spellid, false, false, 0);
	return GM_OK;
}

int Unit_CastSpellOnTarget(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(2);
	GM_CHECK_INT_PARAM(spellid, 0);
	
	Unit * pTarget = (Unit*)a_thread->ParamUser_NoCheckTypeOrParam(1);
	Unit * pThis = GetThisPointer<Unit>(a_thread);
	
	SpellEntry * pSpellEntry = sSpellStore.LookupEntry(spellid);

	pThis->CastSpell(pTarget, pSpellEntry, true);
	return GM_OK;
}

int Unit_TimedEmote(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(2);
	GM_CHECK_INT_PARAM(emoteid, 0);
	GM_CHECK_INT_PARAM(timer, 1);

	Unit * pThis = GetThisPointer<Unit>(a_thread);
	if(timer)
		pThis->EventAddEmote((EmoteType)emoteid, timer);
	else
		pThis->SetUInt32Value(UNIT_NPC_EMOTESTATE, emoteid);

	return GM_OK;
}

int Unit_RegisterTimer(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(3);
	GM_CHECK_INT_PARAM(delay, 0);
	GM_CHECK_FUNCTION_PARAM(func, 1);
	GM_CHECK_INT_PARAM(repeats, 2);

	Unit * pThis = GetThisPointer<Unit>(a_thread);
	if(pThis->GetTypeId() != TYPEID_UNIT)
		return GM_EXCEPTION;

	sEventMgr.AddEvent(((Creature*)pThis), &Creature::TriggerScriptEvent, ((void*)func), EVENT_SCRIPT_UPDATE_EVENT, delay, repeats);
	return GM_OK;
}

int Unit_DeregisterTimer(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(0);

	Unit * pThis = GetThisPointer<Unit>(a_thread);
	if(pThis->GetTypeId() != TYPEID_UNIT)
		return GM_EXCEPTION;

	sEventMgr.RemoveEvents(((Creature*)pThis), EVENT_SCRIPT_UPDATE_EVENT);
	return GM_OK;
}

int Unit_SpawnMonster(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(4);
	GM_CHECK_INT_PARAM(entry, 0);
	GM_CHECK_FLOAT_PARAM(posX, 1);
	GM_CHECK_FLOAT_PARAM(posY, 2);
	GM_CHECK_FLOAT_PARAM(posZ, 3);

	Unit * pThis = GetThisPointer<Unit>(a_thread);
	CreatureProto * p = CreatureProtoStorage.LookupEntry(entry);
	if(!p)
		return GM_EXCEPTION;

	Creature * pCreature = pThis->GetMapMgr()->CreateCreature();
	pCreature->spawnid = 0;
	pCreature->m_spawn = 0;
	pCreature->Load(p, posX, posY, posZ);
	pCreature->SetMapId(pThis->GetMapId());
	pCreature->SetInstanceID(pThis->GetInstanceID());
	pCreature->PushToWorld(pThis->GetMapMgr());

	return GM_OK;
}
int GameObject_Despawn(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(1);
	GM_CHECK_INT_PARAM(timer, 0);

	GameObject * pThis = GetThisPointer<GameObject>(a_thread);
	pThis->Despawn(timer);
	return GM_OK;
}

int GM_RAND(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(1);
	GM_CHECK_FLOAT_OR_INT_PARAM(chance, 0);
	if(Rand(chance))
		a_thread->PushInt(1);
	else
		a_thread->PushInt(0);
	return GM_OK;
}

int Quest_GetID(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(0);
	Quest * pThis = GetThisPointer<Quest>(a_thread);
	a_thread->PushInt(pThis->id);
	return GM_OK;
}

int GM_RegisterEvent(gmThread * a_thread)
{
	Object * pThis = GetThisPointer<Object>(a_thread);
	if(a_thread->GetNumParams() < 2)
		return GM_EXCEPTION;

	if(a_thread->ParamType(0) != GM_INT || a_thread->ParamType(1) != GM_FUNCTION)
		return GM_EXCEPTION;

	uint32 argc = a_thread->GetNumParams() - 2;
	uint32 * argv = argc ? new uint32[argc] : 0;
	uint32 * argt = argc ? new uint32[argc] : 0;
	
	/* see if any of the arguments are our user types -> if so we need to convert the pointers. */
	for(uint32 i = 0; i < argc; ++i)
	{
		void * pointer = 0;
		uint32 t;
		for(list<gmType>::iterator itr = ScriptSystem->m_allowedTypes.begin(); itr != ScriptSystem->m_allowedTypes.end(); ++itr)
		{
			pointer = a_thread->Param(2+i).GetUserSafe((*itr));
			if(pointer)
			{
				t = *itr;
				break;
			}
		}

		if(pointer)
		{
			argt[i] = uint32( (t << 16) | uint32(GM_USER) );
			argv[i] = (uint32)pointer;
		}
		else
		{
			argt[i] = 0 | uint32( ( uint32(a_thread->ParamType(2+i)) << 16) );
			argv[i] = *(uint32*)&a_thread->Param(2+i).m_value;
		}
	}

	/* set up the event :) */
	sEventMgr.AddEvent(pThis, &Object::GMScriptEvent, (void*)a_thread->ParamRef(1), argc, argv, argt, EVENT_GMSCRIPT_EVENT, a_thread->ParamInt(0), 1);
	return GM_OK;
}

void Object::GMScriptEvent(void * function, uint32 argc, uint32 * argv, uint32 * argt)
{
	ScriptSystem->GetLock().Acquire();

	gmFunctionObject * func = (gmFunctionObject*)function;
	ASSERT(func->GetType() == GM_FUNCTION);
	
	/* strange if we had a function without any arguments, that means no 'this' pointer :P *shrugs* */
	ScriptSystem->m_userObjectCounter = argc;
	/*if(argc)
	{
		ASSERT((uint16)argt != 0);			// is a user pointer
		ScriptSystem->SetVariable(0, (void*)argv[0], (gmType)(argt[0] >> 16));
	}*/

	gmCall call;
	if(!call.BeginFunction(ScriptSystem->GetMachine(), func, /*argc ? ScriptSystem->m_variables[0] : */gmVariable::s_null, false))
	{
		printf("Could not execute delayed function.");
		ScriptSystem->DumpErrors();
	}
	else
	{
        /* the rest of the arguments */
		if(argc)
		{
			for(uint32 i = 0; i < argc; ++i)
			{
				if((uint16)argt[i] != 0)		// user pointer
				{
					ScriptSystem->SetVariable(i, (void*)argv[i], (gmType)(argt[i] >> 16));
					call.AddParam(ScriptSystem->m_variables[i]);
				}
				else		// normal param
				{
					ScriptSystem->m_variables[i].m_type = UINT32_HIPART(argt[i]);
					*(uint32*)&ScriptSystem->m_variables[i].m_value = argv[i];

					// push it!
					call.AddParam(ScriptSystem->m_variables[i]);
				}
			}
		}

		/* fly away! */
		call.End();
		ScriptSystem->DumpErrors();
	}

	ScriptSystem->GetLock().Release();
	delete [] argt;
	delete [] argv;
}


// Escort Quest System
int Unit_CreateCustomWaypointMap(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(0);

	Creature * pCreature = GetThisPointer<Creature>(a_thread);
	if(pCreature->GetTypeId() != TYPEID_UNIT)
		return GM_EXCEPTION;

	if(pCreature->m_custom_waypoint_map)
	{
		for(WayPointMap::iterator itr = pCreature->m_custom_waypoint_map->begin(); itr != pCreature->m_custom_waypoint_map->end(); ++itr)
			delete itr->second;
		delete pCreature->m_custom_waypoint_map;
	}

	pCreature->m_custom_waypoint_map = new WayPointMap;
	pCreature->GetAIInterface()->SetWaypointMap(pCreature->m_custom_waypoint_map);
	return GM_OK;
}

int Unit_CreateWaypoint(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(7);
	GM_CHECK_FLOAT_PARAM(x, 0);
	GM_CHECK_FLOAT_PARAM(y, 1);
	GM_CHECK_FLOAT_PARAM(z, 2);
	GM_CHECK_FLOAT_PARAM(o, 3);
	GM_CHECK_INT_PARAM(waittime, 4);
	GM_CHECK_INT_PARAM(flags, 5);
	GM_CHECK_INT_PARAM(modelid, 6);

	Creature * pCreature = GetThisPointer<Creature>(a_thread);
	if(pCreature->GetTypeId() != TYPEID_UNIT)
		return GM_EXCEPTION;

	if(!pCreature->m_custom_waypoint_map)
	{
		pCreature->m_custom_waypoint_map = new WayPointMap;
		pCreature->GetAIInterface()->SetWaypointMap(pCreature->m_custom_waypoint_map);
	}

	if(!modelid)
		modelid = pCreature->GetEntry();

	WayPoint * wp = new WayPoint;
	wp->id = pCreature->m_custom_waypoint_map->size() + 1;
	wp->x = x;
	wp->y = y;
	wp->z = z;
	wp->o = o;
	wp->flags = flags;
	wp->backwardskinid = modelid;
	wp->forwardskinid = modelid;
	wp->backwardemoteid = wp->forwardemoteid = 0;
	wp->backwardemoteoneshot = wp->forwardemoteoneshot = false;
	wp->waittime = waittime;

	pCreature->m_custom_waypoint_map->insert( make_pair( wp->id, wp ) );
	return GM_OK;
}

int Unit_SpawnWithoutWorld(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(4);
	GM_CHECK_INT_PARAM(entry, 0);
	GM_CHECK_FLOAT_PARAM(posX, 1);
	GM_CHECK_FLOAT_PARAM(posY, 2);
	GM_CHECK_FLOAT_PARAM(posZ, 3);

	Unit * pThis = GetThisPointer<Unit>(a_thread);
	CreatureProto * p = CreatureProtoStorage.LookupEntry(entry);
	if(!p)
		return GM_EXCEPTION;

	Creature * pCreature = pThis->GetMapMgr()->CreateCreature();
	pCreature->spawnid = 0;
	pCreature->m_spawn = 0;
	pCreature->Load(p, posX, posY, posZ);
	pCreature->SetMapId(pThis->GetMapId());
	pCreature->SetInstanceID(pThis->GetInstanceID());
	
	ScriptSystem->m_userObjects[ScriptSystem->m_userObjectCounter]->m_user = (void*)pCreature;
	ScriptSystem->m_userObjects[ScriptSystem->m_userObjectCounter]->m_userType = ScriptSystem->m_unitType;
	a_thread->PushUser(ScriptSystem->m_userObjects[ScriptSystem->m_userObjectCounter]);
	ScriptSystem->m_userObjectCounter++;
	return GM_OK;
}

int Unit_AddToWorld(gmThread * a_thread)
{
	/* we need to provide a mapmgr to "leech" from. */
	GM_CHECK_NUM_PARAMS(1);
	/*if(a_thread->ParamType(0) != ScriptSystem->m_unitType)
	{
		a_thread->GetMachine()->GetLog().LogEntry("expecting param %d as user type Unit", 0);
		return GM_EXCEPTION;
	}
	
	Object * leech = (Object*)a_thread->ParamUser_NoCheckTypeOrParam(0);*/
	GM_CHECK_USER_PARAM(Object*, ScriptSystem->m_unitType, leech, 0);
	
	Unit * pThis = GetThisPointer<Unit>(a_thread);
	if(!pThis->IsInWorld())
	{
		pThis->PushToWorld(leech->GetMapMgr());
	}

	return GM_OK;
}

int Unit_MoveToWaypoint(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(1);
	GM_CHECK_INT_PARAM(wp, 0);
	Unit * pThis = GetThisPointer<Unit>(a_thread);
	if(pThis->GetTypeId() == TYPEID_UNIT)
		pThis->MoveToWaypoint(wp);

	return GM_OK;
}

int Unit_Delete(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(0);
	Unit * pThis = GetThisPointer<Unit>(a_thread);
	if(pThis->GetTypeId() == TYPEID_UNIT)
		((Creature*)pThis)->SafeDelete();

	return GM_OK;
}

int Unit_SetCombatCapable(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(1);
	GM_CHECK_INT_PARAM(capable, 0);
	Unit * pThis = GetThisPointer<Unit>(a_thread);
	if(pThis->GetTypeId() == TYPEID_UNIT)
		pThis->GetAIInterface()->disable_melee = (capable > 0) ? true : false;

	return GM_OK;
}

int Unit_HaltMovement(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(1);
	GM_CHECK_INT_PARAM(timer, 0);
	Unit * pThis = GetThisPointer<Unit>(a_thread);
	if(pThis->GetTypeId() == TYPEID_UNIT)
		pThis->GetAIInterface()->StopMovement(timer);

	return GM_OK;
}

/*int Player_GetSelectedCreature(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(1);
	GM_CHECK_INT_PARAM(show_message, 0);

	Player * pThis = GetThisPointer<Player>(a_thread);
	Creature * pCreature = sChatHandler.getSelectedCreature(pThis->GetSession(), show_message);
	if(ScriptSystem->m_userObjectCounter == 9)
		return GM_EXCEPTION;

	gmUserObject * obj = ScriptSystem->m_userObjects[ScriptSystem->m_userObjectCounter++];
	obj->m_userType = ScriptSystem->m_unitType;
	obj->m_user = pCreature;

	a_thread->PushUser(obj);
}
*/