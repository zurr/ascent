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
// WorldCreator.h
//

#ifndef __WORLDCREATOR_H
#define __WORLDCREATOR_H

enum INSTANCE_TYPE
{
	INSTANCE_NULL,
	INSTANCE_RAID,
	INSTANCE_NONRAID,
	INSTANCE_PVP,
	INSTANCE_MULTIMODE,
};

enum INSTANCE_MODE
{
    MODE_NORMAL = 0,
    MODE_HEROIC = 1,
    MODE_EPIC   = 2,
};

enum INSTANCE_ABORT_ERROR
{
	INSTANCE_ABORT_ERROR	 = 0x00,
	INSTANCE_ABORT_FULL	  = 0x01,
	INSTANCE_ABORT_NOT_FOUND = 0x02,
	INSTANCE_ABORT_TOO_MANY  = 0x03,
	INSTANCE_ABORT_ENCOUNTER = 0x04,
};

#define INSTANCE_OK  INSTANCE_ABORT_TOO_MANY + 0x01

class Map;
class MapMgr;

class Object;
class Group;
class Player;
class MapUpdaterThread;
class Battleground;

// first = mapid
// first = instance id
typedef std::map<uint32, std::map<uint32, std::pair<Map*, Battleground*> > > BattlegroundInstanceMap;

class SERVER_DECL WorldCreator :  public Singleton < WorldCreator >
{
public:
	WorldCreator(TaskList * tl);	
	~WorldCreator();

	Map* GetMap(uint32 mapid);
	MapMgr * GetInstance(uint32 mapid, Object* obj);
	MapMgr * GetInstance(uint32 mapid, uint32 instanceId);
	MapMgr * GetInstance(uint32 instanceId);
	MapMgr * GetInstanceByGroupInstanceId(uint32 InstanceID, uint32 mapid, bool Lock);
	MapMgr * GetInstanceByGroup(Group *pGroup, Player *pPlayer, MapInfo *pMapInfo);
	MapMgr * GetInstanceByCreator(Player *pCreator, MapInfo *pMapInfo);

	//Normal instance management
	uint32 CreateInstance(Group *pGroup, Player *pPlayer, uint32 mapid, uint32 instanceid = 0, uint32 creation = 0, MapMgr ** destptr = 0, uint32 difficulty = 0);

    void DeleteInstance(uint32 instanceid, uint32 mapid);
	bool CheckInstanceForObject(Object *obj, MapInfo *pMapinfo);

	uint32 GenerateInstanceID();
	void InstanceSoftReset(MapMgr *mMapMgr);
	void InstanceHardReset(MapMgr *mMapMgr);

	void CreateBattlegroundInstance(Battleground* m_Battleground);
	void DestroyBattlegroundInstance(Battleground* m_Battleground);
	void DestroyBattlegroundInstance(uint32 mapid, uint32 instanceid); //only to be used after bg class is destroyed.
	void BuildXMLStats(char * m_file);
	void CreateMap(uint32 mapid) { _CreateMap(mapid); }

private:
	Map* _CreateMap(uint32 mapid);
	std::map<uint32, Map*> _maps;

	uint32 m_InstanceHigh;

	//thread locks
	Mutex _busy;
};

#define sWorldCreator WorldCreator::getSingleton()
#define sInstanceSavingManager InstanceSavingManagement::getSingleton()

#endif
