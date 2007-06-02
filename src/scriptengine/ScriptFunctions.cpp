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
	return GM_OK;
}

int ScriptEngine_RegisterSpellEvent(gmThread * a_thread)
{
	return GM_OK;
}

int ScriptEngine_RegisterUnitEvent(gmThread * a_thread)
{
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

	GetThisPointer<Player>(a_thread)->SafeTeleport(mapId, 0, posX, posY, posZ, 0);
	return GM_OK;
}

int Player_GetLevel(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(0);
	a_thread->PushInt((int)GetThisPointer<Player>(a_thread)->getLevel());
	return GM_OK;
}

int Player_GetClass(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(0);
	a_thread->PushInt((int)GetThisPointer<Player>(a_thread)->getClass());
	return GM_OK;
}

int Player_GetRace(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(0);
	a_thread->PushInt((int)GetThisPointer<Player>(a_thread)->getRace());
	return GM_OK;
}

int Player_SendAreaTriggerMessage(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(1);
	GM_CHECK_STRING_PARAM(message, 0);

	GetThisPointer<Player>(a_thread)->SendAreaTriggerMessage(message);
	return GM_OK;
}

int Player_BroadcastMessage(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(1);
	GM_CHECK_STRING_PARAM(message, 1);

	GetThisPointer<Player>(a_thread)->BroadcastMessage(message);
	return GM_OK;
}

int Player_GetReputationRank(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(1);
	GM_CHECK_INT_PARAM(faction, 0);

	a_thread->PushInt(GetThisPointer<Player>(a_thread)->GetStandingRank(faction));
    return GM_OK;
}

int Player_GetReputationValue(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(1);
	GM_CHECK_INT_PARAM(faction, 0);

	a_thread->PushInt(GetThisPointer<Player>(a_thread)->GetStanding(faction));
	return GM_OK;
}

int Player_HasFinishedQuest(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(1);
	GM_CHECK_INT_PARAM(questid, 0);

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

int Player_AddItem(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(2);
	GM_CHECK_INT_PARAM(itemid, 0);
	GM_CHECK_INT_PARAM(count, 0);

	Player * pPlayer = GetThisPointer<Player>(a_thread);
	ItemPrototype * proto = objmgr.GetItemPrototype(itemid);
	if(!proto)
		return GM_EXCEPTION;

	if(pPlayer->GetItemInterface()->CanReceiveItem(proto, count))
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
	}

	return GM_OK;
}

int Player_RemoveItem(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(2);
	GM_CHECK_INT_PARAM(itemid, 0);
	GM_CHECK_INT_PARAM(count, 1);

	Player * pPlayer = GetThisPointer<Player>(a_thread);
	pPlayer->GetItemInterface()->RemoveItemAmt(itemid, count);
	return GM_OK;
}

int Player_LearnSpell(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(1);
	GM_CHECK_INT_PARAM(spellid, 0);

	GetThisPointer<Player>(a_thread)->addSpell(spellid);
	return GM_OK;
}

int Player_RemoveSpell(gmThread * a_thread)
{
	GM_CHECK_NUM_PARAMS(1);
	GM_CHECK_INT_PARAM(spellid, 0);

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
