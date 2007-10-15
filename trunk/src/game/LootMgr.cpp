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

#include "StdAfx.h"

initialiseSingleton(LootMgr);

struct loot_tb
{
	uint32 itemid;
	float chance;
};

bool Rand(float chance)
{
	int32 val = sRand.randInt(10000);
	int32 p = int32(chance * 100);
	return p >= val;
}

template <class T>  // works for anything that has the field 'chance' and is stored in plain array
const T& RandomChoice( const T* variant, int count )
{
  float totalChance = 0;
  for( int i = 0; i < count; i++)
    totalChance += variant[i].chance;
  float val = sRand.rand(totalChance);
  for( int i = 0; i < count; i++)
  {
    val -= variant[i].chance;
    if (val <= 0) return variant[i];
  }
  // should not come here, buf if it does, we should return something reasonable
  return variant[count-1];
}

template <class T>  // works for anything that has the field 'chance' and is stored in plain array
T* RandomChoiceVector( vector<pair<T*, float> > & variant )
{
	float totalChance = 0;
	float val;
	vector<pair<T*,float> >::iterator itr;

	if(variant.size() == 0)
		return NULL;

	for(itr = variant.begin(); itr != variant.end(); ++itr)
		totalChance += itr->second;

	val = sRand.rand(totalChance);
	
	for(itr = variant.begin(); itr != variant.end(); ++itr)
	{
		val -= itr->second;
		if (val <= 0) return itr->first;
	}
	// should not come here, buf if it does, we should return something reasonable
	return variant.begin()->first;
}

LootMgr::LootMgr()
{}

void LootMgr::LoadLoot()
{
	//THIS MUST BE CALLED AFTER LOADING OF ITEMS
	LoadLootProp();
	LoadLootTables("creatureloot",&CreatureLoot);
	LoadLootTables("objectloot",&GOLoot);
	LoadLootTables("skinningloot",&SkinningLoot);
	LoadLootTables("fishingloot",&FishingLoot);
	LoadLootTables("itemloot", &ItemLoot);
	LoadLootTables("prospectingloot", &ProspectingLoot);
	LoadLootTables("pickpocketingloot", &PickpocketingLoot);
}

RandomProps * LootMgr::GetRandomProperties(ItemPrototype * proto)
{
	map<uint32,RandomPropertyVector>::iterator itr;

	if(proto->RandomPropId==0)
		return NULL;

    itr = _randomprops.find(proto->RandomPropId);
	if(itr==_randomprops.end())
		return NULL;

	return RandomChoiceVector<RandomProps>(itr->second);
}

ItemRandomSuffixEntry * LootMgr::GetRandomSuffix(ItemPrototype * proto)
{
	map<uint32,RandomSuffixVector>::iterator itr;

	if(proto->RandomSuffixId==0)
		return NULL;

	itr = _randomsuffix.find(proto->RandomSuffixId);
	if(itr==_randomsuffix.end())
		return NULL;

	return RandomChoiceVector<ItemRandomSuffixEntry>(itr->second);
}


void LootMgr::LoadLootProp()
{	
	QueryResult * result = WorldDatabase.Query("SELECT * FROM item_randomprop_groups");
	uint32 id, eid;
	RandomProps * rp;
	ItemRandomSuffixEntry * rs;
	float ch;

	if(result)
	{
		map<uint32, RandomPropertyVector>::iterator itr;
		do 
		{
			id = result->Fetch()[0].GetUInt32();
			eid = result->Fetch()[1].GetUInt32();
			ch = result->Fetch()[2].GetFloat();

			rp = dbcRandomProps.LookupEntryForced(eid);
			if(rp == NULL)
			{
				Log.Error("LoadLootProp", "RandomProp group %u references non-existant randomprop %u.", id, eid);
				continue;
			}

            itr = _randomprops.find(id);
			if(itr == _randomprops.end())
			{
				RandomPropertyVector v;
				v.push_back(make_pair(rp, ch));
				_randomprops.insert(make_pair(id, v));
			}
			else
			{
				itr->second.push_back(make_pair(rp,ch));
			}
		} while(result->NextRow());
		delete result;
	}

	result = WorldDatabase.Query("SELECT * FROM item_randomsuffix_groups");
	if(result)
	{
		map<uint32, RandomSuffixVector>::iterator itr;
		do 
		{
			id = result->Fetch()[0].GetUInt32();
			eid = result->Fetch()[1].GetUInt32();
			ch = result->Fetch()[2].GetFloat();

			rs = dbcItemRandomSuffix.LookupEntryForced(eid);
			if(rs == NULL)
			{
				Log.Error("LoadLootProp", "RandomSuffix group %u references non-existant randomsuffix %u.", id, eid);
				continue;
			}

			itr = _randomsuffix.find(id);
			if(itr == _randomsuffix.end())
			{
				RandomSuffixVector v;
				v.push_back(make_pair(rs, ch));
				_randomsuffix.insert(make_pair(id, v));
			}
			else
			{
				itr->second.push_back(make_pair(rs,ch));
			}
		} while(result->NextRow());
		delete result;
	}   
}

LootMgr::~LootMgr()
{
  sLog.outString("  Deleting Loot Tables...");
  for(LootStore::iterator iter=CreatureLoot.begin(); iter != CreatureLoot.end(); ++iter)
  delete [] iter->second.items;

  for(LootStore::iterator iter=FishingLoot.begin(); iter != FishingLoot.end(); ++iter)
  delete [] iter->second.items;

  for(LootStore::iterator iter=SkinningLoot.begin(); iter != SkinningLoot.end(); ++iter)
  delete [] iter->second.items;

  for(LootStore::iterator iter=GOLoot.begin(); iter != GOLoot.end(); ++iter)
  delete [] iter->second.items;

  for(LootStore::iterator iter=ItemLoot.begin(); iter != ItemLoot.end(); ++iter)
	  delete [] iter->second.items;

  for(LootStore::iterator iter=ProspectingLoot.begin(); iter != ProspectingLoot.end(); ++iter)
  delete [] iter->second.items;

 for(LootStore::iterator iter=PickpocketingLoot.begin(); iter != PickpocketingLoot.end(); ++iter)
 delete [] iter->second.items;
}

void LootMgr::LoadLootTables(const char * szTableName,LootStore * LootTable)
{
  /*  DBCFile *dbc = new DBCFile();
	dbc->open("DBC/ItemRandomProperties.dbc");
	_propCount = dbc->getRecordCount();
	delete dbc;*/
	//HM_NAMESPACE::hash_map<uint32, std::vector<loot_tb> > loot_db;
	//HM_NAMESPACE::hash_map<uint32, std::vector<loot_tb> >::iterator itr;
	vector< pair< uint32, vector< tempy > > > db_cache;
	vector< pair< uint32, vector< tempy > > >::iterator itr;
	db_cache.reserve(10000);
	LootStore::iterator tab;
	QueryResult *result =WorldDatabase.Query("SELECT * FROM %s ORDER BY entryid ASC",szTableName);
	if(!result)
	{
		sLog.outError("\rWARNING: Loading loot from table %s failed.", szTableName);
		return;
	}
	uint32 entry_id = 0;
	uint32 last_entry = 0;

	uint32 total =(uint32) result->GetRowCount();
	int pos = 0;
	vector< tempy > ttab;
	tempy t;
	bool d = false;
	do 
	{	 
		Field *fields = result->Fetch();
		entry_id = fields[1].GetUInt32();
		if(entry_id < last_entry)
		{
			sLog.outError("WARNING: Out of order loot table being loaded.\n");
			return;
		}
		if(entry_id != last_entry)
		{
			if(last_entry != 0)
				db_cache.push_back( make_pair( last_entry, ttab) );
			ttab.clear();
		}

		if(result->GetFieldCount() > 4)
		{
			t.itemid = fields[2].GetUInt32();
			t.chance = fields[3].GetFloat();
			t.chance_2 = fields[4].GetFloat();
			t.mincount = fields[5].GetUInt32();
			t.maxcount = fields[6].GetUInt32();
		}
		else
		{
			if(!d)
			{
				Log.Warning("LootMgr", "Loot table %s is using old loot structure (without heroic chances column)!", szTableName);
				Log.Warning("LootMgr", "This will be deprecated soon!");
				d=true;
			}
			t.itemid = fields[2].GetUInt32();
			t.chance = fields[3].GetFloat();
			t.chance_2 = t.chance;
			t.mincount = t.maxcount = 1;
		}
		ttab.push_back( t );
		

		/*loot_tb t;		   
		t.itemid = fields[1].GetUInt32();
		t.chance = fields[2].GetFloat();
		loot_db[fields[0].GetUInt32()].push_back(t);*/

		last_entry = entry_id;
	} while( result->NextRow() );
	//last list was not pushed in
	if(last_entry != 0 && ttab.size())
		db_cache.push_back( make_pair( last_entry, ttab) );
	pos = 0;
	total = db_cache.size();
	ItemPrototype* proto;
	uint32 itemid;

	//for(itr=loot_db.begin();itr!=loot_db.end();++itr)
	for( itr = db_cache.begin(); itr != db_cache.end(); ++itr)
	{
		entry_id = (*itr).first;
		if(LootTable->end()==LootTable->find(entry_id))
		{
			StoreLootList list;
			//list.count = itr->second.size();			
			list.count = (*itr).second.size();
			list.items=new StoreLootItem[list.count];
		
			uint32 ind=0;
			//for(std::vector<loot_tb>::iterator itr2=itr->second.begin();itr2!=itr->second.end();++itr2)
			for(vector< tempy >::iterator itr2 = (*itr).second.begin(); itr2 != (*itr).second.end(); ++itr2)
			{
				//Omit items that are not in db to prevent future bugs
				//uint32 itemid=(*itr2).itemid;
				itemid = itr2->itemid;
				proto=ItemPrototypeStorage.LookupEntry(itemid);
				if(!proto)
				{
					list.items[ind].item.itemproto=NULL;
					sLog.outDetail("WARNING: Loot %u contains item that does not exist in the DB.",entry_id);
				}
				else
				{
					list.items[ind].item.itemproto=proto;
					list.items[ind].item.displayid=proto->DisplayInfoID;
					//list.items[ind].chance=(*itr2).chance;
					list.items[ind].chance= itr2->chance;
					list.items[ind].chance2 = itr2->chance_2;
					list.items[ind].mincount = itr2->mincount;
					list.items[ind].maxcount = itr2->maxcount;

					if(LootTable == &GOLoot)
					{
						if(proto->Class == ITEM_CLASS_QUEST)
						{
							//printf("Quest item \"%s\" allocated to quest ", proto->Name1.c_str());
							sQuestMgr.SetGameObjectLootQuest(itr->first, itemid);
							quest_loot_go[entry_id].insert(proto->ItemId);
						}
					}
				}
				ind++;
			}
			(*LootTable)[entry_id]=list;
		}
	}

	//sLog.outString("  %d loot templates loaded from %s", db_cache.size(), szTableName);
 //   loot_db.clear();
	delete result;
}

void LootMgr::PushLoot(StoreLootList *list,Loot * loot, bool heroic)
{
	uint32 i;
	uint32 count;
	for(uint32 x =0; x<list->count;x++)
	if(list->items[x].item.itemproto)// this check is needed until loot DB is fixed
	{
		float chance = heroic ? list->items[x].chance2 : list->items[x].chance;
		if(chance == 0.0f) continue;

		ItemPrototype *itemproto = list->items[x].item.itemproto;
		if(Rand(chance * sWorld.getRate(RATE_DROP0 + itemproto->Quality)) )//|| itemproto->Class == ITEM_CLASS_QUEST)
		{
			if(list->items[x].mincount == list->items[x].maxcount)
				count = list->items[x].maxcount;
			else
				count = sRand.randInt(list->items[x].maxcount - list->items[x].mincount) + list->items[x].mincount;

			for(i = 0; i < loot->items.size(); ++i)
			{
				//itemid rand match a already placed item, if item is stackable and unique(stack), increment it, otherwise skips
				if((loot->items[i].item.itemproto == list->items[x].item.itemproto) && itemproto->MaxCount && ((loot->items[i].iItemsCount + count) < itemproto->MaxCount))
				{
					if(itemproto->Unique && ((loot->items[i].iItemsCount+count) < itemproto->Unique))
					{
						loot->items[i].iItemsCount += count;
						break;
					}
					else if (!itemproto->Unique)
					{
						loot->items[i].iItemsCount += count;
						break;
					}
				}
			}
			if(i != loot->items.size())
				continue;

			__LootItem itm;
			itm.item =list->items[x].item;
			itm.iItemsCount = count;
			itm.roll = NULL;
			itm.passed = false;
			
			if (itemproto->Quality > 1)
			{
				itm.iRandomProperty=GetRandomProperties(itemproto);
				itm.iRandomSuffix=GetRandomSuffix(itemproto);
			}
			else
			{
				// save some calls :P
				itm.iRandomProperty=NULL;
				itm.iRandomSuffix=NULL;
			}

			loot->items.push_back(itm);
		}	
	}
}



void LootMgr::FillCreatureLoot(Loot * loot,uint32 loot_id, bool heroic)
{
	loot->items.clear ();
	loot->gold =0;
	
	LootStore::iterator tab =CreatureLoot.find(loot_id);
	if( CreatureLoot.end()==tab)return;
	else PushLoot(&tab->second,loot, heroic);
}

void LootMgr::FillGOLoot(Loot * loot,uint32 loot_id, bool heroic)
{
	loot->items.clear ();
	loot->gold =0;

	LootStore::iterator tab =GOLoot.find(loot_id);
	if( GOLoot.end()==tab)return;
	else PushLoot(&tab->second,loot, heroic);
}

void LootMgr::FillPickpocketingLoot(Loot * loot,uint32 loot_id)
{
 loot->items.clear ();
 loot->gold =0;

 LootStore::iterator tab =PickpocketingLoot.find(loot_id);
 if( PickpocketingLoot.end()==tab)return;
 else PushLoot(&tab->second,loot,false);
}

//Puts 1 item always, no random properties
void LootMgr::FillProfessionLoot(LootStore * store,Loot * loot,uint32 loot_id)
{
	loot->items.clear ();
	loot->gold =0;
	
	LootStore::iterator tab =store->find(loot_id);
	if( store->end()==tab)return;
	StoreLootList *list=&(tab->second);
	
	// TODO: fix infinite loop with ' while(true) '
	while(true)
	for(uint32 x =0,pass=0; x<list->count; x++,pass++)
	{
		if(list->items[x].item.itemproto)// this check is needed until loot DB is fixed
		{
//			ItemPrototype *itemproto = ItemPrototypeStorage.LookupEntry(list->items[x].item.itemid);
			if(Rand(list->items[x].chance))// || itemproto->Class == ITEM_CLASS_QUEST)
			{
				__LootItem itm;
				itm.item =list->items[x].item;
				itm.iItemsCount=1;
				itm.passed = false;
				itm.roll = 0;
				/*if(itemproto->MaxCount>1)
				{
					uint32 c=1 + sRand.randInt(itemproto->MaxCount);
					if(Rand(float(100.0/c)))
						itm.iItemsCount=c;
				
				}*/
				
				itm.iRandomProperty=0;
			  
				loot->items.push_back(itm);
				return;
			}	
		}
		
		if(pass>100)
		{
			sLog.outError("WARNING: Loot %u has too low chances",loot_id);
			return;		
		}
	}	
}

bool LootMgr::CanGODrop(uint32 LootId,uint32 itemid)
{
	LootStore::iterator tab =GOLoot.find(LootId);
	if( GOLoot.end()==tab)
	return false;
	StoreLootList *list=&(tab->second);
	for(uint32 x=0;x<list->count;x++)
		if(list->items[x].item.itemproto->ItemId==itemid)
			return true;
	return false;
}

//THIS should be cached 
bool LootMgr::IsPickpocketable(uint32 creatureId)
{
	LootStore::iterator tab =PickpocketingLoot.find(creatureId);
	if( PickpocketingLoot.end()==tab)return false;
	else return true;
}

//THIS should be cached 
bool LootMgr::IsSkinnable(uint32 creatureId)
{
	LootStore::iterator tab =SkinningLoot.find(creatureId);
	if( SkinningLoot.end()==tab)return false;
	else return true;
}

//THIS should be cached 
bool LootMgr::IsFishable(uint32 zoneid)
{
	LootStore::iterator tab =FishingLoot.find(zoneid);
	return tab!=FishingLoot.end();
}

#define NEED 1
#define GREED 2

LootRoll::LootRoll(uint32 timer, uint32 groupcount, uint64 guid, uint32 slotid, uint32 itemid, uint32 itemunk1, uint32 itemunk2, MapMgr * mgr) : EventableObject()
{
	_mgr = mgr;
	sEventMgr.AddEvent(this, &LootRoll::Finalize, EVENT_LOOT_ROLL_FINALIZE, 60000, 1,0);
	_groupcount = groupcount;
	_guid = guid;
	_slotid = slotid;
	_itemid = itemid;
	_itemunk1 = itemunk1;
	_itemunk2 = itemunk2;
	_remaining = groupcount;
	_passedGuid = 0;
}

LootRoll::~LootRoll()
{
	
}

void LootRoll::Finalize()
{
	sEventMgr.RemoveEvents(this);

	// this we will have to finalize with groups types.. for now
	// we'll just assume need before greed. person with highest roll
	// in need gets the item.

	uint32 highest = 0;
	int8 hightype = -1;
	uint64 player = 0;

	WorldPacket data(34);

	/* grab any player */
	Player * gplr = NULL;
	for(std::map<uint64, uint32>::iterator itr = NeedRolls.begin(); itr != NeedRolls.end(); ++itr)
	{
		gplr = _mgr->GetPlayer((uint32)itr->first);
		if(gplr) break;
	}
	
	if(!gplr)
	{
		for(std::map<uint64, uint32>::iterator itr = GreedRolls.begin(); itr != GreedRolls.end(); ++itr)
		{
			gplr = _mgr->GetPlayer((uint32)itr->first);
			if(gplr) break;
		}
	}

	for(std::map<uint64, uint32>::iterator itr = NeedRolls.begin(); itr != NeedRolls.end(); ++itr)
	{
		if(itr->second > highest)
		{
			highest = itr->second;
			player = itr->first;
			hightype = NEED;
		}

		data.Initialize(SMSG_LOOT_ROLL);
		data << _guid << _slotid << itr->first;
		data << _itemid << _itemunk1 << _itemunk2;
		data << uint8(itr->second) << uint8(NEED);
		if(gplr && gplr->GetGroup())
			gplr->GetGroup()->SendPacketToAll(&data);
	}

	for(std::map<uint64, uint32>::iterator itr = GreedRolls.begin(); itr != GreedRolls.end(); ++itr)
	{
		if(!highest && itr->second > highest)
		{
			highest = itr->second;
			player = itr->first;
			hightype = GREED;
		}

		data.Initialize(SMSG_LOOT_ROLL);
		data << _guid << _slotid << itr->first;
		data << _itemid << _itemunk1 << _itemunk2;
		data << uint8(itr->second) << uint8(GREED);
		if(gplr && gplr->GetGroup())
			gplr->GetGroup()->SendPacketToAll(&data);
	}

	Loot * pLoot = 0;
	if(GUID_HIPART(_guid) == HIGHGUID_UNIT)
	{
		Creature * pc = _mgr->GetCreature(_guid);
		if(pc) pLoot = &pc->loot;
	}
	else if(GUID_HIPART(_guid) == HIGHGUID_GAMEOBJECT)
	{
		GameObject * go = _mgr->GetGameObject(_guid);
		if(go) pLoot = &go->loot;
	}

	if(!pLoot)
	{
		delete this;
		return;
	}

	if(_slotid >= pLoot->items.size())
	{
		delete this;
		return;
	}

	pLoot->items.at(_slotid).roll = NULL;

	uint32 itemid = pLoot->items.at(_slotid).item.itemproto->ItemId;
	uint32 amt = pLoot->items.at(_slotid).iItemsCount;
	if(!amt)
	{
		delete this;
		return;
	}

	Player * _player = (player) ? _mgr->GetPlayer(player) : 0;
	if(!player || !_player)
	{
		_player = _mgr->GetPlayer(_passedGuid);
		if(_player)
		{
			/* all passed */
			data.Initialize(SMSG_LOOT_ALL_PASSED);
			data << _guid << _groupcount << _itemid << _itemunk1 << _itemunk2;
			if(_player->InGroup())
				_player->GetGroup()->SendPacketToAll(&data);
			else
				_player->GetSession()->SendPacket(&data);
		}

		/* item can now be looted by anyone :) */
		pLoot->items.at(_slotid).passed = true;
		delete this;
		return;
	}

    pLoot->items.at(_slotid).roll = 0;
	data.Initialize(SMSG_LOOT_ROLL_WON);
	data << _guid << _slotid << _itemid << _itemunk1 << _itemunk2;
	data << _player->GetGUID() << uint8(highest) << uint8(hightype);
	if(_player->InGroup())
		_player->GetGroup()->SendPacketToAll(&data);
	else
		_player->GetSession()->SendPacket(&data);

	ItemPrototype* it = ItemPrototypeStorage.LookupEntry(itemid);

	int8 error;
	if((error = _player->GetItemInterface()->CanReceiveItem(it, 1)))
	{
		_player->GetItemInterface()->BuildInventoryChangeError(NULL, NULL, error);
		return;
	}

	Item * add = _player->GetItemInterface()->FindItemLessMax(itemid, amt, false);

	if (!add)
	{
		SlotResult slotresult = _player->GetItemInterface()->FindFreeInventorySlot(it);
		if(!slotresult.Result)
		{
			_player->GetItemInterface()->BuildInventoryChangeError(NULL, NULL, INV_ERR_INVENTORY_FULL);
			return;
		}

		sLog.outDebug("AutoLootItem MISC");
		Item *item = objmgr.CreateItem( itemid, _player);

		item->SetUInt32Value(ITEM_FIELD_STACK_COUNT,amt);
		if(pLoot->items.at(_slotid).iRandomProperty!=NULL)
		{
			item->SetRandomProperty(pLoot->items.at(_slotid).iRandomProperty->ID);
			item->ApplyRandomProperties(false);
		}
		else if(pLoot->items.at(_slotid).iRandomSuffix != NULL)
		{
			item->SetRandomSuffix(pLoot->items.at(_slotid).iRandomSuffix->id);
			item->ApplyRandomProperties(false);
		}


		_player->GetItemInterface()->SafeAddItem(item,slotresult.ContainerSlot, slotresult.Slot);

		sQuestMgr.OnPlayerItemPickup(_player,item);
	}
	else 
	{	
		add->SetCount(add->GetUInt32Value(ITEM_FIELD_STACK_COUNT) + amt);
		add->m_isDirty = true;
		sQuestMgr.OnPlayerItemPickup(_player,add);
	}

	pLoot->items.at(_slotid).iItemsCount=0;
	// this gets sent to all looters
	data.Initialize(SMSG_LOOT_REMOVED);
	data << uint8(_slotid);
	Player * plr;
	for(LooterSet::iterator itr = pLoot->looters.begin(); itr != pLoot->looters.end(); ++itr)
	{
		if((plr = _player->GetMapMgr()->GetPlayer(*itr)))
			plr->GetSession()->SendPacket(&data);
	}

	WorldPacket idata(45);
	_player->GetSession()->BuildItemPushResult(&idata, _player->GetGUID(), ITEM_PUSH_TYPE_LOOT, amt, itemid, pLoot->items.at(_slotid).iRandomProperty ? pLoot->items.at(_slotid).iRandomProperty->ID : 0);

	if(_player->InGroup())
		_player->GetGroup()->SendPacketToAll(&idata);
	else
		_player->GetSession()->SendPacket(&idata);
	delete this;
}

void LootRoll::PlayerRolled(Player *player, uint8 choice)
{
	if(NeedRolls.find(player->GetGUID()) != NeedRolls.end() || GreedRolls.find(player->GetGUID()) != GreedRolls.end())
		return; // dont allow cheaters

	std::map<uint64, uint32>* rmap = 0;
	
	if(choice == NEED) {
		rmap = &NeedRolls;
	} else if(choice == GREED) {
		rmap = &GreedRolls;
	}

	int roll = sRand.randInt(99)+1;
	// create packet
	WorldPacket data(34);
	data.SetOpcode(SMSG_LOOT_ROLL);
	data << _guid << _slotid << player->GetGUID();
	data << _itemid << _itemunk1 << _itemunk2;

	if(rmap)
	{
		rmap->insert ( std::make_pair(player->GetGUID(), roll) );
		if(choice == GREED)
			data << uint8(0xF9) << uint8(0x00);
		else
			data << uint8(0xC1) << uint8(0x00);
	}
	else
	{
		if(!_passedGuid)
			_passedGuid = player->GetGUID();

		data << uint8(128) << uint8(128);
	}

	data << uint8(roll) << uint8(choice);
	
	if(player->InGroup())
		player->GetGroup()->SendPacketToAll(&data);
	else
		player->GetSession()->SendPacket(&data);
	
	// check for early completion
	if(!--_remaining)
	{
		// kill event early
		//sEventMgr.RemoveEvents(this);
		Finalize();
	}
}

void LootMgr::FillItemLoot(Loot *loot, uint32 loot_id)
{
	loot->items.clear ();
	loot->gold =0;

	LootStore::iterator tab =ItemLoot.find(loot_id);
	if( ItemLoot.end()==tab)return;
	else PushLoot(&tab->second,loot,false);
}

int32 LootRoll::event_GetInstanceID()
{
	return _mgr->GetInstanceID();
}
