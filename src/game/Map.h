/****************************************************************************
 *
 * General Object Type File
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

//
// Map.h
//

#ifndef __MAP_H
#define __MAP_H

class MapMgr;
class TemplateMgr;
struct MapInfo;
typedef std::map<uint32, MapMgr*> InstanceMap;
class TerrainMgr;

struct Formation;

typedef struct
{
	uint32	id;//spawn ID
	uint32	entry;
	float	x;
	float	y;
	float	z;
	float	o;
	Formation* form;
	uint32 movetype;
	uint32 displayid;
	uint32 factionid;
	uint32 flags;
	uint32 bytes;
	uint32 bytes2;
	uint32 emote_state;
	uint32 respawnNpcLink;
}CreatureSpawn;

typedef struct
{
	uint32	id;//spawn ID
	uint32	entry;
	float	x;
	float	y;
	float	z;
	float	o;
	float	o1;
	float	o2;
	float	o3;
	float	facing;
	uint32	flags;
	uint32	state;
	uint32	faction;
//	uint32	level;
	float scale;
	uint32 stateNpcLink;
} GOSpawn;

typedef std::set<CreatureSpawn*> CreatureSpawnList;
typedef std::set<GOSpawn*> GOSpawnList;

typedef struct
{
	CreatureSpawnList CreatureSpawns;
	GOSpawnList GOSpawns;
}CellSpawns;

class SERVER_DECL Map
{
public:
	Map(uint32 mapid);
	~Map();

	inline InstanceMap::iterator GetInstancesBegin() { return _instances.begin(); }
	inline InstanceMap::iterator GetInstancesEnd() { return _instances.end(); }

	MapMgr * CreateMapMgrInstance(uint32 instanceid = 0);
	void DestroyMapMgrInstance(uint32 instanceId);
	MapMgr * GetFirstInstance();	// for main continents

	MapMgr * GetInstance(Object* obj);
	MapMgr * GetInstance(uint32 instanceId);
	MapMgr * GetRawInstance(uint32 instanceid); //this function bypass pending stattes
	MapMgr * GetInstanceByGroup(Group *pGroup, Player *pCaller);
	MapMgr * GetInstanceByCreator(Player *pCreator);
	MapMgr * GetInstanceByGroupInstanceId(uint32 InstanceID, bool Lock);
	MapMgr * InstanceExists(uint32 instanceId);
	inline TerrainMgr * GetTerrainMgr() { return _terrain; }
	inline string GetNameString() { return name; }
	inline const char* GetName() { return name.c_str(); }
	inline MapEntry* GetDBCEntry() { return me; }
	void BuildXMLStats(char * m_file);
	inline CellSpawns *GetSpawnsList(uint32 cellx,uint32 celly)
	{
		ASSERT(cellx < _sizeX);
		ASSERT(celly < _sizeY);

		return spawns[cellx][celly];
	}
	inline CellSpawns * GetSpawnsListAndCreate(uint32 cellx, uint32 celly)
	{
		ASSERT(cellx < _sizeX);
		ASSERT(celly < _sizeY);
		if(spawns[cellx][celly] == 0)
			spawns[cellx][celly] = new CellSpawns;
		return spawns[cellx][celly];
	}

	void LoadSpawns(bool reload);//set to true to make clean up
	uint32 CreatureSpawnCount;
	uint32 GameObjectSpawnCount;

private:
	InstanceMap	 _instances;	
	MapInfo *	   _mapInfo;
	TerrainMgr*	 _terrain;
	uint32 _mapId;
	string name;
	MapEntry * me;
	Mutex listmutex;
	

	//new stuff
	CellSpawns *spawns[_sizeX][_sizeY];

};

#endif
