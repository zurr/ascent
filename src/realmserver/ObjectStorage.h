/****************************************************************************
 *
 * General Object Type File
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

#ifndef OBJECT_STORAGE_H
#define OBJECT_STORAGE_H

extern SQLStorage<ItemPrototype, ArrayStorageContainer<ItemPrototype> >				ItemPrototypeStorage;
extern SQLStorage<CreatureInfo, HashMapStorageContainer<CreatureInfo> >				CreatureNameStorage;
extern SQLStorage<GameObjectInfo, HashMapStorageContainer<GameObjectInfo> >			GameObjectNameStorage;
//extern SQLStorage<CreatureProto, HashMapStorageContainer<CreatureProto> >			CreatureProtoStorage;
//extern SQLStorage<AreaTrigger, HashMapStorageContainer<AreaTrigger> >				AreaTriggerStorage;
extern SQLStorage<ItemPage, HashMapStorageContainer<ItemPage> >						ItemPageStorage;
extern SQLStorage<Quest, HashMapStorageContainer<Quest> >							QuestStorage;
extern SQLStorage<GossipText, HashMapStorageContainer<GossipText> >					NpcTextStorage;
//extern SQLStorage<SpellExtraInfo, HashMapStorageContainer<SpellExtraInfo> >			SpellExtraStorage;
//extern SQLStorage<GraveyardTeleport, HashMapStorageContainer<GraveyardTeleport> >	GraveyardStorage;
//extern SQLStorage<TeleportCoords, HashMapStorageContainer<TeleportCoords> >			TeleportCoordStorage;
//extern SQLStorage<FishingZoneEntry, HashMapStorageContainer<FishingZoneEntry> >		FishingZoneStorage;
extern SQLStorage<MapInfo, HashMapStorageContainer<MapInfo> >						WorldMapInfoStorage;

/*void Storage_FillTaskList(TaskList & tl);*/
void Storage_Cleanup();
void Storage_Load();
bool Storage_ReloadTable(const char * TableName);

#endif


