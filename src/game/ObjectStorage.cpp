/****************************************************************************
 *
 * Object Storage Format Strings / Instantiation
 * Copyright (c) 2007 Burlex, Antrix Team
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

/** Table formats converted to strings
 */
const char * gItemPrototypeFormat						= "uuuusxxxuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuffuffuffuffuffuuuuuuuuuufuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuusuuuuuuuuuuuuuuuuuuuuuuuuuuu";
const char * gCreatureNameFormat						= "ussuuuuuuuffcc";
const char * gGameObjectNameFormat						= "uuusuuuuuuuuuuuuuuuuuuuuuuuu";
const char * gCreatureProtoFormat						= "uuuuufuuffuffuuuuuuuuuuuuuuuuuuffsu";
const char * gAreaTriggerFormat							= "uuuusffffuu";
const char * gItemPageFormat							= "usu";
const char * gNpcTextFormat								= "u";
const char * gQuestFormat								= "u";

/** SQLStorage symbols
 */
SQLStorage<ItemPrototype, ArrayStorageContainer<ItemPrototype> >			ItemPrototypeStorage;
SQLStorage<CreatureInfo, HashMapStorageContainer<CreatureInfo> >			CreatureNameStorage;
SQLStorage<GameObjectInfo, HashMapStorageContainer<GameObjectInfo> >		GameObjectNameStorage;
SQLStorage<CreatureProto, HashMapStorageContainer<CreatureProto> >			CreatureProtoStorage;
SQLStorage<AreaTrigger, HashMapStorageContainer<AreaTrigger> >				AreaTriggerStorage;
SQLStorage<ItemPage, HashMapStorageContainer<ItemPage> >					ItemPageStorage;
SQLStorage<Quest, HashMapStorageContainer<Quest> >							QuestStorage;
SQLStorage<GossipText, HashMapStorageContainer<GossipText> >				NpcTextStoage;

void LoadExtraCreatureProtoStuff()
{
	StorageContainerIterator<CreatureProto> * itr = CreatureProtoStorage.MakeIterator();
	CreatureProto * cn;
	while(!itr->AtEnd())
	{
		cn = itr->Get();
		string auras = itr->Get()->aura_string;
		vector<string> aurs = StrSplit(auras, " ");
		for(vector<string>::iterator it = aurs.begin(); it != aurs.end(); ++it)
		{
			uint32 id = atol((*it).c_str());
			if(id)
				itr->Get()->start_auras.insert( id );
		}

		if(!itr->Get()->Health)
			itr->Get()->Health = 1;

		cn->m_canFlee = cn->m_canRangedAttack = cn->m_canCallForHelp = false;
		cn->m_fleeHealth = 0.0f;
		cn->m_fleeDuration = 0.0f;

        if(!itr->Inc())
			break;
	}

	itr->Destruct();

	// Load AI Agents
	QueryResult * result = sDatabase.Query( "SELECT * FROM ai_agents" );

	if( !result )
		return;

	AI_Spell *sp;
	uint32 entry;

	do
	{
		Field *fields = result->Fetch();
		entry = fields[0].GetUInt32();
		cn = CreatureProtoStorage.LookupEntry(entry);
		if(!cn)
			continue;

		sp = new AI_Spell;
		sp->entryId = fields[0].GetUInt32();
		sp->agent = fields[1].GetUInt16();
		sp->procEvent = fields[2].GetUInt32();
		sp->procChance = fields[3].GetUInt32();
		sp->procCount = fields[4].GetUInt32();
		sp->spellId = fields[5].GetUInt32();
		sp->spellType = fields[6].GetUInt32();;
		sp->spelltargetType = fields[7].GetUInt32();
		sp->spellCooldown = fields[8].GetUInt32();
		sp->floatMisc1 = fields[9].GetFloat();
		sp->Misc2 = fields[10].GetUInt32();
		sp->spellCooldownTimer = 0;
		sp->procCounter = 0;
		sp->minrange = GetMinRange(sSpellRange.LookupEntry(sSpellStore.LookupEntry(sp->spellId)->rangeIndex));
		sp->maxrange = GetMaxRange(sSpellRange.LookupEntry(sSpellStore.LookupEntry(sp->spellId)->rangeIndex));

		if(sp->agent == AGENT_RANGED)
		{
			cn->m_canRangedAttack = true;
			delete sp;
		}
		else if(sp->agent == AGENT_FLEE)
		{
			cn->m_canFlee = true;
			if(sp->floatMisc1)
				cn->m_canFlee = sp->floatMisc1;
			else
				cn->m_fleeHealth = 0.2f;

			if(sp->Misc2)
				cn->m_fleeDuration = sp->Misc2;
			else
				cn->m_fleeDuration = 10000;

			delete sp;
		}
		else if(sp->agent == AGENT_CALLFORHELP)
		{
			cn->m_canCallForHelp = true;
			if(sp->floatMisc1)
				cn->m_callForHelpHealth = 0.2f;
			delete sp;
		}
		else
		{
			cn->spells.push_back(sp);
		}
	} while( result->NextRow() );

	delete result;
}

void ObjectMgr::LoadExtraItemStuff()
{
	StorageContainerIterator<ItemPrototype> * itr = ItemPrototypeStorage.MakeIterator();
	ItemPrototype * pItemPrototype;
	while(!itr->AtEnd())
	{
		pItemPrototype = itr->Get();
		if(pItemPrototype->ItemSet > 0)
		{
			ItemSetContentMap::iterator itr = mItemSets.find(pItemPrototype->ItemSet);
			std::list<ItemPrototype*>* l;
			if(itr == mItemSets.end())
			{
				l = new std::list<ItemPrototype*>;				
				mItemSets.insert( ItemSetContentMap::value_type( pItemPrototype->ItemSet, l) );
			} else {
				l = itr->second;
			}
			l->push_back(pItemPrototype);
		}


		// lowercase name, used for searches
		pItemPrototype->lowercase_name = pItemPrototype->Name;
		for(uint32 j = 0; j < pItemPrototype->lowercase_name.length(); ++j)
			pItemPrototype->lowercase_name[j] = tolower(pItemPrototype->lowercase_name[j]);

        if(!itr->Inc())
			break;
	}

	itr->Destruct();
}

#define make_task(storage, itype, storagetype, tablename, format) tl.AddTask( \
	new CallbackP2< SQLStorage< itype, storagetype< itype > >, const char *, const char *> \
    (&storage, &SQLStorage< itype, storagetype< itype > >::Load, tablename, format) )

void FillTaskList(TaskList & tl)
{
	make_task(ItemPrototypeStorage, ItemPrototype, ArrayStorageContainer, "items", gItemPrototypeFormat);
	make_task(CreatureNameStorage, CreatureInfo, HashMapStorageContainer, "creature_names", gCreatureNameFormat);
	make_task(GameObjectNameStorage, GameObjectInfo, HashMapStorageContainer, "gameobject_names", gGameObjectNameFormat);
	make_task(CreatureProtoStorage, CreatureProto, HashMapStorageContainer, "creature_proto", gCreatureProtoFormat);
	make_task(AreaTriggerStorage, AreaTrigger, HashMapStorageContainer, "areatriggers", gAreaTriggerFormat);
	make_task(ItemPageStorage, ItemPage, HashMapStorageContainer, "itempages", gItemPageFormat);
}

void LoadTest()
{
	ItemPrototypeStorage.Load("items", gItemPrototypeFormat);
	//objmgr.LoadExtraItemStuff();

	CreatureNameStorage.Load("creature_names", gCreatureNameFormat);
	
	CreatureProtoStorage.Load("creature_proto", gCreatureProtoFormat);
	LoadExtraCreatureProtoStuff();

	GameObjectNameStorage.Load("gameobject_names", gGameObjectNameFormat);
}
