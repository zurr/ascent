/****************************************************************************
 *
 * Item System
 * Copyright (c) 2007 Team Ascent
 *
 * This work is licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 3.0
 * License. To view a copy of this license, visit
 * http://creativecommons.org/licenses/by-nc-sa/3.0/ or send a letter to Creative Commons,
 * 543 Howard Street, 5th Floor, San Francisco, California, 94105, USA.
 *
 * EXCEPT TO THE EXTENT REQUIRED BY APPLICABLE LAW, IN NO EVENT WILL LICENSOR BE LIABLE TO YOU
 * ON ANY LEGAL THEORY FOR ANY SPECIAL, INCIDENTAL, CONSEQUENTIAL, PUNITIVE OR EXEMPLARY DAMAGES
 * ARISING OUT OF THIS LICENSE OR THE USE OF THE WORK, EVEN IF LICENSOR HAS BEEN ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGES.
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
