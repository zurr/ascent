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
    uint32 difficulty;
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

    uint32 GetDifficulty() { return difficulty; }
    void SetDifficulty(uint32 mode) { difficulty = mode; }

	//save functions
	void SaveInstanceToDB();
	bool IsSaved;

private:
	PlayerList mPlayerList;
	NpcList mNpcList;
    Mutex playerListMutex;
    Mutex npcListMutex;
	uint32 mGroupSignature;
	MapInfo *m_pMapInfo;
	time_t CreationTime;
	time_t ExpireTime;
	uint32 m_instanceid;
    uint32 difficulty;
};


//-------------------------------------------------------------------//
//Instance MapId Info Holder
//Description: Holds information about mapids that were saved.
//-------------------------------------------------------------------//
struct Instance_Map_Info_Holder
{
public:
	typedef HM_NAMESPACE::hash_map<uint32, SharedPointer<Instance_Map_InstanceId_Holder> > InstanceIdList;

	Instance_Map_Info_Holder();
	~Instance_Map_Info_Holder();

	bool FindPlayer(uint64 guid, uint32 iGroupSignature, uint32 difficulty);
	bool IsPlayerSavedToInstanceId(uint64 guid, uint32 instanceid);
	bool RemovePlayer(uint64 guid, uint32 InstanceID);
	bool RemovePlayer(uint64 guid);
	bool RemoveGroup(uint32 iGroupSignature);
	void RemoveInstanceId(uint32 instanceid);

	void AddPlayer(uint64 guid, uint32 InstanceID);
	void AddInstanceId(MapMgr *pInstance);
	void AddInstanceId(SharedPointer<InactiveInstance> ia);

	SharedPointer<Instance_Map_InstanceId_Holder> GetInstanceId(uint32 instanceid);
	SharedPointer<Instance_Map_InstanceId_Holder> getInstanceIdByPlayer(uint64 guid, uint32 difficulty, bool iIgnoreDifficulty = false);
	inline MapInfo *GetMapInfo() { return m_pMapInfo; }
	inline void SetMapInfo(MapInfo *pMapInfo) { m_pMapInfo = pMapInfo; }

private:
	InstanceIdList mInstanceIdList;
    Mutex instanceIdListMutex;
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

	typedef HM_NAMESPACE::hash_map<uint32, SharedPointer<Instance_Map_Info_Holder> > InstanceInfo;

	SharedPointer<Instance_Map_Info_Holder> SaveInstance(MapMgr *pInstance);
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
	SharedPointer<Instance_Map_InstanceId_Holder> GetInstance(uint32 mapid, uint32 instanceid);
    SharedPointer<Instance_Map_InstanceId_Holder> GetRaidAndMMInstance(uint32 mapid, Player * pPlayer);

	//INACTIVE INSTANCE FUNCTIONS.
	void SaveInstance(SharedPointer<InactiveInstance> ia);
	SharedPointer<InactiveInstance> GetInactiveInstance(uint32 instance_id);
	void AddInactiveInstance(SharedPointer<InactiveInstance> ia);
	void CreateInactiveInstance(MapMgr * mgr);
	void RemoveSavedInstance(uint32 instance_id);

private:
	InstanceInfo mInstanceInfoList;
	Mutex _busy;
	Mutex inactiveInstancesMutex;
	Mutex instanceInfoListMutex;
    map<uint32, SharedPointer<InactiveInstance> > inactiveInstances;

};

#define sInstanceSavingManager InstanceSavingManagement::getSingleton()
#endif
