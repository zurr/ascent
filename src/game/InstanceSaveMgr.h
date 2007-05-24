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

#ifndef __INSTANCESAVEMGR_H
#define __INSTANCESAVEMGR_H

struct InactiveInstance
{
	uint32 InstanceId;
	uint32 MapId;
	uint32 Creation;
	uint32 GroupSignature;
	uint32 Creator;
	uint32 ExpireTime;
};

//-------------------------------------------------------------------//
//Instance Map InstanceId Holder
//Description: Holds information a saved instanceid
//-------------------------------------------------------------------//
struct Instance_Map_InstanceId_Holder
{
public:
	typedef HM_NAMESPACE::hash_map<uint64, uint64> PlayerList;
	typedef HM_NAMESPACE::hash_map<uint64, uint64> NpcList;
	Instance_Map_InstanceId_Holder();
	~Instance_Map_InstanceId_Holder();

	bool FindPlayer(uint64 guid);
	bool RemovePlayer(uint64 guid);
	bool ClearAllPlayers();
	void AddPlayer(uint64 guid);
	void AddObject(uint32 entry);
	bool FindObject(uint32 entry);
	bool ClearAllObjects();

	uint32 GetGroupSignature() { return mGroupSignature; }
	void SetGroupSignature(uint32 iGroupSignature) { mGroupSignature = iGroupSignature; }
	inline MapInfo *GetMapInfo() { return m_pMapInfo; }
	inline void SetMapInfo(MapInfo *pMapInfo) { m_pMapInfo = pMapInfo; }

	uint32 GetInstanceID() { return m_instanceid; }
	void SetInstanceID(uint32 instanceid) { m_instanceid = instanceid; }

	//timers
	time_t GetCreationTime() { return CreationTime; }
	time_t GetRaidExpireTime() { return ExpireTime; }
	void SetCreationTime(time_t time) { CreationTime = time; }
	void SetRaidExpireTime(time_t time) { ExpireTime = time; }

	//save functions
	void SaveInstanceToDB();
	bool IsSaved;

private:
	PlayerList mPlayerList;
	NpcList mNpcList;
	uint32 mGroupSignature;
	MapInfo *m_pMapInfo;
	time_t CreationTime;
	time_t ExpireTime;
	uint32 m_instanceid;
};


//-------------------------------------------------------------------//
//Instance MapId Info Holder
//Description: Holds information about mapids that were saved.
//-------------------------------------------------------------------//
struct Instance_Map_Info_Holder
{
public:
	typedef HM_NAMESPACE::hash_map<uint32, Instance_Map_InstanceId_Holder*> InstanceIdList;

	Instance_Map_Info_Holder();
	~Instance_Map_Info_Holder();

	bool FindPlayer(uint64 guid, uint32 InstanceID, uint32 iGroupSignature);
	bool FindPlayer(uint64 guid, uint32 iGroupSignature);
	bool IsPlayerSavedToInstanceId(uint64 guid, uint32 instanceid);
	bool RemovePlayer(uint64 guid, uint32 InstanceID);
	bool RemovePlayer(uint64 guid);
	bool RemoveGroup(uint32 iGroupSignature);
	void RemoveInstanceId(uint32 instanceid);

	void AddPlayer(uint64 guid, uint32 InstanceID);
	void AddInstanceId(MapMgr *pInstance);
	void AddInstanceId(InactiveInstance * ia);

	Instance_Map_InstanceId_Holder *GetInstanceId(uint32 instanceid);
	Instance_Map_InstanceId_Holder *getInstanceIdByPlayer(uint64 guid);
	inline MapInfo *GetMapInfo() { return m_pMapInfo; }
	inline void SetMapInfo(MapInfo *pMapInfo) { m_pMapInfo = pMapInfo; }

private:
	InstanceIdList mInstanceIdList;
	MapInfo *m_pMapInfo;
};

//-------------------------------------------------------------------//
//Instance Saving Management class
//Description: Holds information about instances saved.
//-------------------------------------------------------------------//
class InstanceSavingManagement :  public Singleton < InstanceSavingManagement >
{
public:
	InstanceSavingManagement();
	~InstanceSavingManagement();

	typedef HM_NAMESPACE::hash_map<uint32, Instance_Map_Info_Holder*> InstanceInfo;

	Instance_Map_Info_Holder *SaveInstance(MapMgr *pInstance);
	void SavePlayerToInstance(Player *pPlayer, uint32 mapid); 
	void ResetSavedInstancesForPlayer(Player *pPlayer); 
	void RemoveSavedInstance(uint32 mapid, uint32 instanceid, bool bHardReset);
	void DeleteInstanceFromDB(uint32 instanceid);
	void BuildSavedInstancesForPlayer(Player *pPlayer);
	void BuildRaidSavedInstancesForPlayer(Player *pPlayer);
	bool IsPlayerSavedToMap(uint32 mapid, Player *pPlayer);
	bool IsPlayerSavedToInstanceId(uint32 mapid, uint32 instanceid, Player *pPlayer);
	void LoadSavedInstances();
	void RepopulateSavedData(uint32 mapid, uint32 instanceid, const char *cNpc, const char *cPlayer);
	void SaveInstanceIdToDB(uint32 instanceid, uint32 mapid);
	void SaveObjectStateToInstance(Unit *pUnit);
	Instance_Map_InstanceId_Holder *GetInstance(uint32 mapid, uint32 instanceid);
	Instance_Map_InstanceId_Holder *GetRaidInstance(uint32 mapid, Player * pPlayer);

	//INACTIVE INSTANCE FUNCTIONS.
	void SaveInstance(InactiveInstance *ia);
	InactiveInstance * GetInactiveInstance(uint32 instance_id);
	void AddInactiveInstance(InactiveInstance * ia);
	void CreateInactiveInstance(MapMgr * mgr);
	void RemoveSavedInstance(uint32 instance_id);

private:
	InstanceInfo mInstanceInfoList;
	Mutex _busy;
	Mutex inactiveInstancesMutex;
	Mutex instanceInfoListMutex;
	map<uint32, InactiveInstance*> inactiveInstances;

};

#define sInstanceSavingManager InstanceSavingManagement::getSingleton()
#endif
