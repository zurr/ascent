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

#include "TerrainMgr.h"

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
	inline string GetNameString() { return name; }
	inline const char* GetName() { return name.c_str(); }
	inline MapEntry* GetDBCEntry() { return me; }
	void BuildXMLStats(char * m_file);
	inline CellSpawns *GetSpawnsList(uint32 cellx,uint32 celly)
	{
		ASSERT(cellx < _sizeX);
		ASSERT(celly < _sizeY);
		if(spawns[cellx]==NULL) return NULL;

		return spawns[cellx][celly];
	}
	inline CellSpawns * GetSpawnsListAndCreate(uint32 cellx, uint32 celly)
	{
		ASSERT(cellx < _sizeX);
		ASSERT(celly < _sizeY);
		if(spawns[cellx]==NULL)
		{
			for(uint32 x=0;x<_sizeX;++x)
				spawns[cellx] = new CellSpawns*[_sizeY];
		}

		if(spawns[cellx][celly] == 0)
			spawns[cellx][celly] = new CellSpawns;
		return spawns[cellx][celly];
	}

	void LoadSpawns(bool reload);//set to true to make clean up
	uint32 CreatureSpawnCount;
	uint32 GameObjectSpawnCount;

	inline float  GetLandHeight(float x, float y)
	{ 
		if(_terrain)
		{
			return _terrain->GetLandHeight(x, y);
		}
		else
		{
			return 999999.0f;
		}
	}

	inline float  GetWaterHeight(float x, float y) 
	{ 
		if(_terrain)
		{ 
			return _terrain->GetWaterHeight(x, y); 
		}
		else
		{ 
			return 999999.0f; 
		}
	}

	inline uint8  GetWaterType(float x, float y)
	{
		if(_terrain)
		{ 
			return _terrain->GetWaterType(x, y);
		}
		else
		{ 
			return 999999.0f;
		}
	}

	inline uint8  GetWalkableState(float x, float y)
	{
		if(_terrain)
		{ 
			return _terrain->GetWalkableState(x, y);
		}
		else
		{ 
			return 999999.0f; 
		}
	}

	inline uint16 GetAreaID(float x, float y)
	{
		if(_terrain)
		{ 
			return _terrain->GetAreaID(x, y);
		}
		else
		{ 
			return 999999.0f;
		}
	}

	inline void CellGoneActive(uint32 x, uint32 y)
	{ 
		if(_terrain)
		{ 
			_terrain->CellGoneActive(x,y);
		}
	}

	inline void CellGoneIdle(uint32 x,uint32 y)
	{ 
		if(_terrain)
		{ 
			_terrain->CellGoneIdle(x,y);
		}
	}

private:
	InstanceMap	 _instances;	
	MapInfo *	   _mapInfo;
	TerrainMgr*	 _terrain;
	uint32 _mapId;
	string name;
	MapEntry * me;
	Mutex listmutex;
	

	//new stuff
	CellSpawns **spawns[_sizeX];
public:
	CellSpawns staticSpawns;
};

#endif
