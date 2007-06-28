/****************************************************************************
 *
 * Profession Defines
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

uint32 GetGOReqSkill(GameObject * gameObjTarget)  
{
	if(gameObjTarget->GetEntry()==180215)return 300;
	GameObjectInfo*gi= GameObjectNameStorage.LookupEntry(gameObjTarget->GetEntry());
	if(!gi)
	{
		sLog.outError("Unknown go entry=%u",gameObjTarget->GetEntry());
		return 0;
	}

	Lock *lock = sLockStore.LookupEntry( gi->SpellFocus );
	if(!lock) return 0;
	for(uint32 i=0;i<5;i++)
		if(lock->locktype[i] == 2 && lock->minlockskill[i])
		{
			return lock->minlockskill[i];
		}
	return 0;
}

void AddItemFromDisenchant(ItemPrototype *proto,Player*owner)
{
	uint32 count = 0,item = 0;
	uint32 l=proto->ItemLevel;
	if(proto->Quality ==4 && l>=51)
	{
		count=1;
		if (l<61) item=20725;
		else item=22450;
	}
	else
	{
		if(proto->Quality ==2)//green -> dust for armor, essence for weapon
		{
			if(proto->Class==4)//armor->dust
			{
				if(l<=20)item=10940;
				else if(l<=30)item=11083;
				else if(l<=40)item=11137;
				else if(l<=50)item=11176;
				else if(l<=60)item=16204;
				else item=22445;
			}
			else
			{
				if(l<=10)item=10938;
				else if(l<=15)item=10939;
				else if(l<=20)item=10998;
				else if(l<=25)item=11082;
				else if(l<=30)item=11134;
				else if(l<=35)item=11135;
				else if(l<=40)item=11174;
				else if(l<=45)item=11175;
				else if(l<=50)item=16202;
				else if(l<=60)item=16203;
				else if(l<=66) item=22447;
				else item=22446;
			
			}
			count =1+rand()%3; 
		}
		else if(proto->Quality >=3)//blue -> shards
		{
				if(l<=20)item=10978;
				else if(l<=25)item=11084;
				else if(l<=30)item=11138;
				else if(l<=35)item=11139;
				else if(l<=40)item=11177;
				else if(l<=45)item=11178;
				else if(l<=50)item=14343;
				else if(l<=60)item=14344;
				else if(l<=65)item=22448;
				else item=22449;
		
			count = proto->Quality-2+rand()%3;
		}
	}

	Item * it=objmgr.CreateItem(item,owner);
   
	it->SetUInt32Value( ITEM_FIELD_STACK_COUNT, count);
	bool result = owner->GetItemInterface()->AddItemToFreeSlot(it);
	if(!result)
	{
		printf("Error whole trying to add disenchanted items to free slots");
	}
}

/*
Dusts usually are extracted from armors, but also occasionally found from weapons. 

Strange Dust - Disenchanted from level 1 to 20 items [10940]
Soul Dust - Disenchanted from level 21 to 30 items [11083]
Vision Dust - Disenchanted from level 31 to 40 items [11137]
Dream Dust - Disenchanted from level 41 to 50 items [11176]
Illusion Dust - Disenchanted from level 51 to 60 items [16204]


Essences usually are extracted from weapons, but also occasionally found from armors. 

Lesser Magic Essence - Disenchanted from level 1 to 10 items [10938]
Greater Magic Essence - Disenchanted from level 11 to 15 items [10939]
Lesser Astral Essence - Disenchanted from level 16 to 20 items [10998]
Greater Astral Essence - Disenchanted from level 21 to 25 items [11082]
Lesser Mystic Essence - Disenchanted from level 26 to 30 items [11134]
Greater Mystic Essence - Disenchanted from level 31 to 35 items [11135]
Lesser Nether Essence - Disenchanted from level 36 to 40 items [11174]
Greater Nether Essence - Disenchanted from level 41 to 45 items [11175]
Lesser Eternal Essence - Disenchanted from level 46 to 50 items [16202]
Greater Eternal Essence - Disenchanted from level 51 to 60 items [16203]


Shards are normally disenchanted from blue or better items, but have a small chance of coming from green items. 

Small Glimmering Shard - Disenchanted from level 1 to 20 items [10978]
Large Glimmering Shard - Disenchanted from level 21 to 25 items [11084]
Small Glowing Shard - Disenchanted from level 26 to 30 items [11138]
Large Glowing Shard - Disenchanted from level 31 to 35 items [11139]
Small Radiant Shard - Disenchanted from level 36 to 40 items [11177]
Large Radiant Shard - Disenchanted from level 41 to 45 items [11178]
Small Brilliant Shard - Disenchanted from level 46 to 50 items [14343]
Large Brilliant Shard - Disenchanted from level 51 to 60 items [14344]

Nexus Crystals are obtained by disenchanting Epic (purple) items, and sometimes come from Rare (blue) items also. 

Nexus Crystal - Disenchanted from level 51-60 items [20725]
*/

void AddItemFromProspecting(ItemPrototype *proto,Player*owner)
{
	uint32 count = 0,item = 0;
	uint32 l=proto->ItemLevel;

	switch (proto->ItemId)
	{
		case 2771: //thin ore
		{
			if(Rand(100))
			{
				uint32 addme_entry=24188; //tin powder = 100%
				uint32 addme_count=1+rand() % 2;
				Item * it=objmgr.CreateItem(item,owner);  
				if(it)
				{
					it->SetUInt32Value( ITEM_FIELD_STACK_COUNT, count);
					bool result = owner->GetItemInterface()->AddItemToFreeSlot(it);
					if(!result)
					{
						owner->GetItemInterface()->BuildInventoryChangeError(0, 0, INV_ERR_INVENTORY_FULL);
						return;
					}
				}
			}
			
			if(Rand(38.7))
			{
				uint32 addme_entry=1206; //mos agate
				uint32 addme_count=1+rand() % 2;
				Item * it=objmgr.CreateItem(item,owner);  
				if(it)
				{
					it->SetUInt32Value( ITEM_FIELD_STACK_COUNT, count);
					bool result = owner->GetItemInterface()->AddItemToFreeSlot(it);
					if(!result)
					{
						owner->GetItemInterface()->BuildInventoryChangeError(0, 0, INV_ERR_INVENTORY_FULL);
						return;
					}
				}
			}
			if(Rand(37.6))
			{
				uint32 addme_entry=1705; //lesser moonstone
				uint32 addme_count=1+rand() % 2;
				Item * it=objmgr.CreateItem(item,owner);  
				if(it)
				{
					it->SetUInt32Value( ITEM_FIELD_STACK_COUNT, count);
					bool result = owner->GetItemInterface()->AddItemToFreeSlot(it);
					if(!result)
					{
						owner->GetItemInterface()->BuildInventoryChangeError(0, 0, INV_ERR_INVENTORY_FULL);
						return;
					}
				}
			}
			if(Rand(36.8))
			{
				uint32 addme_entry=1210; //shadowgem
				uint32 addme_count=1+rand() % 2;
				Item * it=objmgr.CreateItem(item,owner);  
				if(it)
				{
					it->SetUInt32Value( ITEM_FIELD_STACK_COUNT, count);
					bool result = owner->GetItemInterface()->AddItemToFreeSlot(it);
					if(!result)
					{
						owner->GetItemInterface()->BuildInventoryChangeError(0, 0, INV_ERR_INVENTORY_FULL);
						return;
					}
				}
			}
			if(Rand(3.5))
			{
				uint32 addme_entry=1529; //jade
				uint32 addme_count=1+rand() % 2;
				Item * it=objmgr.CreateItem(item,owner);  
				if(it)
				{
					it->SetUInt32Value( ITEM_FIELD_STACK_COUNT, count);
					bool result = owner->GetItemInterface()->AddItemToFreeSlot(it);
					if(!result)
					{
						owner->GetItemInterface()->BuildInventoryChangeError(0, 0, INV_ERR_INVENTORY_FULL);
						return;
					}
				}
			}
			if(Rand(3.3))
			{
				uint32 addme_entry=3864; //citrin
				uint32 addme_count=1+rand() % 2;
				Item * it=objmgr.CreateItem(item,owner);  
				if(it)
				{
					it->SetUInt32Value( ITEM_FIELD_STACK_COUNT, count);
					bool result = owner->GetItemInterface()->AddItemToFreeSlot(it);
					if(!result)
					{
						owner->GetItemInterface()->BuildInventoryChangeError(0, 0, INV_ERR_INVENTORY_FULL);
						return;
					}
				}
			}
			if(Rand(3.1))
			{
				uint32 addme_entry=7909; //aquamarine
				uint32 addme_count=1+rand() % 2;
				Item * it=objmgr.CreateItem(item,owner);  
				if(it)
				{
					it->SetUInt32Value( ITEM_FIELD_STACK_COUNT, count);
					bool result = owner->GetItemInterface()->AddItemToFreeSlot(it);
					if(!result)
					{
						owner->GetItemInterface()->BuildInventoryChangeError(0, 0, INV_ERR_INVENTORY_FULL);
						return;
					}
				}
			}
		}break;
		case 10620: //thorium ore
		{
		}break;
	}
}
