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

#include "RStdAfx.h"

/** Table formats converted to strings
 */
const char * gItemPrototypeFormat						= "uuuussssuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuffuffuffuffuffuuuuuuuuuufuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuusuuuuuuuuuuuuuuuuuuuuuuuuuuu";
const char * gCreatureNameFormat						= "ussuuuuuuuffcc";
const char * gGameObjectNameFormat						= "uuusuuuuuuuuuuuuuuuuuuuuuuuu";
const char * gCreatureProtoFormat						= "uuuuufuuffuffuuuuuuuuuuuuuuuuuuffsuu";
const char * gAreaTriggerFormat							= "uuuusffffuu";
const char * gItemPageFormat							= "usu";
const char * gNpcTextFormat								= "ufssuuuuuuufssuuuuuuufssuuuuuuufssuuuuuuufssuuuuuuufssuuuuuuufssuuuuuuufssuuuuuuu";
const char * gQuestFormat								= "uuuuuuuuuuuuuuuuuussssssssssuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuu";
const char * gSpellExtraFormat							= "uuuu";
const char * gGraveyardFormat							= "uffffuuuux";
const char * gTeleportCoordFormat						= "uxufffx";
const char * gPvPAreaFormat								= "ush";
const char * gFishingFormat								= "uuu";
const char * gWorldMapInfoFormat						= "uuuuufffusuuuuu";

/** SQLStorage symbols
 */
SQLStorage<ItemPrototype, ArrayStorageContainer<ItemPrototype> >				ItemPrototypeStorage;
SQLStorage<CreatureInfo, HashMapStorageContainer<CreatureInfo> >				CreatureNameStorage;
SQLStorage<GameObjectInfo, HashMapStorageContainer<GameObjectInfo> >			GameObjectNameStorage;
//SQLStorage<CreatureProto, HashMapStorageContainer<CreatureProto> >				CreatureProtoStorage;
//SQLStorage<AreaTrigger, HashMapStorageContainer<AreaTrigger> >					AreaTriggerStorage;
SQLStorage<ItemPage, HashMapStorageContainer<ItemPage> >						ItemPageStorage;
SQLStorage<Quest, HashMapStorageContainer<Quest> >								QuestStorage;
SQLStorage<GossipText, HashMapStorageContainer<GossipText> >					NpcTextStorage;
//SQLStorage<SpellExtraInfo, HashMapStorageContainer<SpellExtraInfo> >			SpellExtraStorage;
//SQLStorage<GraveyardTeleport, HashMapStorageContainer<GraveyardTeleport> >		GraveyardStorage;
//SQLStorage<TeleportCoords, HashMapStorageContainer<TeleportCoords> >			TeleportCoordStorage;
//SQLStorage<FishingZoneEntry, HashMapStorageContainer<FishingZoneEntry> >		FishingZoneStorage;
SQLStorage<MapInfo, HashMapStorageContainer<MapInfo> >							WorldMapInfoStorage;

void Storage_Load()
{
	ItemPrototypeStorage.Load("items", gItemPrototypeFormat);
	CreatureNameStorage.Load("creature_names", gCreatureNameFormat);
	GameObjectNameStorage.Load("gameobject_names", gGameObjectNameFormat);
	ItemPageStorage.Load("itempages", gItemPageFormat);
	QuestStorage.Load("quests", gQuestFormat);
	WorldMapInfoStorage.Load("worldmap_info", gWorldMapInfoFormat);
}

void Storage_Cleanup()
{
	ItemPrototypeStorage.Cleanup();
	CreatureNameStorage.Cleanup();
	GameObjectNameStorage.Cleanup();
	//CreatureProtoStorage.Cleanup();
	//AreaTriggerStorage.Cleanup();
	ItemPageStorage.Cleanup();
	QuestStorage.Cleanup();
	//SpellExtraStorage.Cleanup();
	//GraveyardStorage.Cleanup();
	//TeleportCoordStorage.Cleanup();
	//FishingZoneStorage.Cleanup();
	NpcTextStorage.Cleanup();
	WorldMapInfoStorage.Cleanup();
}

bool Storage_ReloadTable(const char * TableName)
{
	// bur: mah god this is ugly :P
	if(!stricmp(TableName, "items"))					// Items
		ItemPrototypeStorage.Reload();
/*	else if(!stricmp(TableName, "creature_proto"))		// Creature Proto
		CreatureProtoStorage.Reload();*/
	else if(!stricmp(TableName, "creature_names"))		// Creature Names
		CreatureNameStorage.Reload();
	else if(!stricmp(TableName, "gameobject_names"))	// GO Names
		GameObjectNameStorage.Reload();
/*	else if(!stricmp(TableName, "areatriggers"))		// Areatriggers
		AreaTriggerStorage.Reload();*/
	else if(!stricmp(TableName, "itempages"))			// Item Pages
		ItemPageStorage.Reload();
/*	else if(!stricmp(TableName, "spellextra"))			// Spell Extra Info
		SpellExtraStorage.Reload();*/
	else if(!stricmp(TableName, "quests"))				// Quests
		QuestStorage.Reload();
	else if(!stricmp(TableName, "npc_text"))			// NPC Text Storage
		NpcTextStorage.Reload();
/*	else if(!stricmp(TableName, "fishing"))				// Fishing Zones
		FishingZoneStorage.Reload();
	else if(!stricmp(TableName, "teleport_coords"))		// Teleport coords
		TeleportCoordStorage.Reload();
	else if(!stricmp(TableName, "graveyards"))			// Graveyards
		TeleportCoordStorage.Reload();*/
	else if(!stricmp(TableName, "worldmap_info"))		// WorldMapInfo
		WorldMapInfoStorage.Reload();
	else
		return false;
	return true;
}
