/****************************************************************************
 *
 * Item System
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

#ifndef _CONTAINER_H
#define _CONTAINER_H

class SERVER_DECL Container : public Item
{
public:
	friend class WorldSession;
	Container(uint32 high, uint32 low);
	~Container();

	void Create( uint32 itemid, Player *owner );
	void LoadFromDB( Field*fields);

	bool AddItem(int8 slot, Item *item);
	bool AddItemToFreeSlot(Item *pItem);
	inline Item *GetItem(int8 slot)
	{
		if((uint8)slot < GetProto()->ContainerSlots)
			return m_Slot[slot];
		else
			return 0;
	}
	
	int8 FindFreeSlot();
	bool HasItems();
	
	void SwapItems(int8 SrcSlot,int8 DstSlot);
	Item *SafeRemoveAndRetreiveItemFromSlot(int8 slot, bool destroy); //doesnt destroy item from memory
	bool SafeFullRemoveItemFromSlot(int8 slot); //destroys item fully
   
	void SaveBagToDB(int8 slot, bool first);

protected:
	Item **m_Slot;
	uint32 __fields[CONTAINER_END];
};

#endif
