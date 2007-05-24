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

#include "StdAfx.h"

initialiseSingleton( InstanceSavingManagement );

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Instance Saving Management
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++
InstanceSavingManagement::InstanceSavingManagement()
{

}

InstanceSavingManagement::~InstanceSavingManagement()
{
	InstanceInfo::const_iterator itr;
	for (itr = mInstanceInfoList.begin();itr != mInstanceInfoList.end(); itr++)
	{
		Instance_Map_Info_Holder *p = itr->second;
		delete p;
	}
	mInstanceInfoList.clear();
}

void InstanceSavingManagement::BuildSavedInstancesForPlayer(Player *pPlayer)
{
	bool result = false;
	bool hasBeenSaved = false;
	WorldPacket data;

	InstanceInfo::const_iterator itr;
	for (itr = mInstanceInfoList.begin();itr != mInstanceInfoList.end(); itr++)
	{
		Instance_Map_Info_Holder *p = itr->second;
		result = p->FindPlayer(pPlayer->GetGUID(), NULL); //find only non grouped results
		if(result)
		{
			data.Initialize(SMSG_INSTANCE_SAVE);
			data << uint32(itr->first);
			pPlayer->GetSession()->SendPacket(&data);

			data.Initialize(SMSG_INSTANCE_RESET_ACTIVATE);
			data << uint32(0x01);
			pPlayer->GetSession()->SendPacket(&data);

			hasBeenSaved = true;
		}
	}

	if(!hasBeenSaved)
	{
		data.Initialize(SMSG_INSTANCE_RESET_ACTIVATE);
		data << uint32(0x00);
		pPlayer->GetSession()->SendPacket(&data);
	}
}

Instance_Map_Info_Holder *InstanceSavingManagement::SaveInstance(MapMgr *pInstance)
{
	ASSERT( pInstance->GetInstanceID() );

	InstanceInfo::iterator itr = mInstanceInfoList.find( pInstance->GetMapId() );
	if(itr == mInstanceInfoList.end())
	{
		Instance_Map_Info_Holder *mapholder;

		mapholder = new Instance_Map_Info_Holder;
		mapholder->SetMapInfo(pInstance->GetMapInfo());
		mapholder->AddInstanceId(pInstance);

		mInstanceInfoList[pInstance->GetMapId()] = mapholder;
		return mapholder;
	}
	else
	{
		Instance_Map_Info_Holder *mapholder = itr->second;
		mapholder->SetMapInfo(pInstance->GetMapInfo());
		mapholder->AddInstanceId(pInstance);
		return mapholder;
	}
	return NULL;
}

void InstanceSavingManagement::SavePlayerToInstance(Player *pPlayer, uint32 mapid)
{
	WorldPacket data;

	InstanceInfo::iterator itr = mInstanceInfoList.find( mapid );
	if(itr != mInstanceInfoList.end())
	{
		bool result = itr->second->FindPlayer(pPlayer->GetGUID(), (pPlayer->InGroup() ? pPlayer->GetGroup()->GetID() : 0));
		if(result)
		{
			return;
		}
		else
		{
			if(pPlayer->GetInstanceID()) //check if player is really on a instanceid
			{
				if(itr->second->GetMapInfo() && itr->second->GetMapInfo()->type == INSTANCE_RAID)
				{
					itr->second->AddPlayer(pPlayer->GetGUID(), pPlayer->GetInstanceID());
					sChatHandler.SystemMessageToPlr(pPlayer,"You are now saved to this instance.");
				}
				else
				{
					itr->second->AddPlayer(pPlayer->GetGUID(), pPlayer->GetInstanceID());
				}

				if(!pPlayer->InGroup())
				{
					data.Initialize(SMSG_INSTANCE_SAVE);
					data << mapid;
					pPlayer->GetSession()->SendPacket(&data);

					data.Initialize(SMSG_INSTANCE_RESET_ACTIVATE);
					data << uint32(0x01);
					pPlayer->GetSession()->SendPacket(&data);
				}
			}
		}
	}
}

void InstanceSavingManagement::ResetSavedInstancesForPlayer(Player *pPlayer)
{
	bool result;
	WorldPacket data;

	InstanceInfo::const_iterator itr;
	for (itr = mInstanceInfoList.begin();itr != mInstanceInfoList.end(); itr++)
	{
		Instance_Map_Info_Holder *p = itr->second;
		if(p->GetMapInfo()->type == INSTANCE_RAID)
			continue;

		if(pPlayer->InGroup())
			result = p->RemoveGroup(pPlayer->GetGroup()->GetID());
		else
			result = p->RemovePlayer(pPlayer->GetGUID());
		
		if(result)
		{
			data.Initialize(SMSG_RESET_INSTANCE);
			data << itr->first;
			pPlayer->GetSession()->SendPacket(&data);
		}
	}
}

void InstanceSavingManagement::RemoveSavedInstance(uint32 mapid, uint32 instanceid, bool bHardReset)
{
	//my concept: while the real instance data is deleted from worldcreator,  the player saving remain until the real
	//reset comes from MapMgr wich should be a "ghost" map by then without players going in or out.
	//why is this? simple, due to players be abling to reset their instance saving. this method is more safe to avoid conflits
	//or worse :P bugs.
	//Guard guard(_busy);

	ASSERT( mapid );
	ASSERT( instanceid );

	InstanceInfo::iterator itr = mInstanceInfoList.find( mapid );
	if(itr != mInstanceInfoList.end())
	{
		Instance_Map_Info_Holder *p1 = itr->second;
		MapInfo * pMapInfo = sWorld.GetMapInformation(mapid);
		if(bHardReset && pMapInfo && pMapInfo->type == INSTANCE_RAID)
		{
			p1->RemoveInstanceId(instanceid);
			DeleteInstanceFromDB(instanceid);
		}
		else if(!bHardReset && pMapInfo && pMapInfo->type != INSTANCE_RAID )
		{
			p1->RemoveInstanceId(instanceid);
		}
	}
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Description: Detects if a player is saved or not inside a specific mapid
// Note: This can be only used for Instances that have a lockout system like heroic and raid instances.
//	   Otherwise this function doesnt know if it should search for group or solo instances.
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool InstanceSavingManagement::IsPlayerSavedToMap(uint32 mapid, Player *pPlayer)
{
	InstanceInfo::iterator itr = mInstanceInfoList.find( mapid );
  
	if(itr != mInstanceInfoList.end())
	{
		Instance_Map_Info_Holder *p = itr->second;
		if(p->GetMapInfo() && p->GetMapInfo()->type == INSTANCE_RAID)
		{
			bool result = p->FindPlayer(pPlayer->GetGUID(), NULL);
			if(result) return true;
		}
	}
	return false;
}
bool InstanceSavingManagement::IsPlayerSavedToInstanceId(uint32 mapid, uint32 instanceid, Player *pPlayer)
{
	InstanceInfo::iterator itr = mInstanceInfoList.find( mapid );
  
	if(itr != mInstanceInfoList.end())
	{
		Instance_Map_Info_Holder *p = itr->second;
		if(p->GetMapInfo() && p->GetMapInfo()->type == INSTANCE_RAID)
		{
			bool result = p->IsPlayerSavedToInstanceId(pPlayer->GetGUID(), instanceid);
			if(result) return true;
		}
	}
	return false;
}

void InstanceSavingManagement::LoadSavedInstances()
{
	QueryResult *result = NULL;

	result = sDatabase.Query("SELECT * FROM instances");

	if(result==NULL)
	{
		sLog.outString("  Query failed.");
		return;
	}

	do
	{
		Field *fields = result->Fetch();
		
		// check for instance expiry
		if(time(NULL) >= fields[5].GetUInt32())
		{
			sDatabase.Execute("DELETE FROM instances WHERE instanceid = %u", fields[0].GetUInt32());
			continue;
		}

		// Inactive instance data saving system added
		InactiveInstance * ia = new InactiveInstance;
		ia->InstanceId = fields[0].GetUInt32();
		ia->MapId = fields[1].GetUInt32();
		ia->Creation = fields[4].GetUInt32();
		ia->ExpireTime = fields[5].GetUInt32();
		sInstanceSavingManager.AddInactiveInstance(ia);
		sInstanceSavingManager.SaveInstance(ia);
		sInstanceSavingManager.RepopulateSavedData(fields[1].GetUInt32(), fields[0].GetUInt32(), fields[2].GetString(), fields[3].GetString());

	} while( result->NextRow() );
	delete result;
}

void InstanceSavingManagement::RepopulateSavedData(uint32 mapid, uint32 instanceid, const char *cNpc, const char *cPlayer)
{
	 std::string ndata = cPlayer;
	 std::string::size_type last_pos = 0, pos = 0;
	 uint64 val = 0;
	 uint32 val2 = 0;

	InstanceInfo::iterator itr = mInstanceInfoList.find( mapid );
	if(itr != mInstanceInfoList.end())
	{
		Instance_Map_Info_Holder *p = itr->second;
		Instance_Map_InstanceId_Holder *pi = p->GetInstanceId(instanceid);
		if(pi)
		{
			pi->ClearAllPlayers();
			do 
			{
				pos = ndata.find(" ", last_pos);
				if(pos == std::string::npos)
				{
					break;
				}

				sscanf(ndata.substr(last_pos, (pos-last_pos)).c_str(), I64FMT, &val);
				last_pos = pos+1;
				if(val)
					pi->AddPlayer(val);

			} while(pos != std::string::npos);

			last_pos = 0, pos = 0, val = 0;
			ndata = cNpc;

			pi->ClearAllObjects();
			do 
			{
				pos = ndata.find(" ", last_pos);
				if(pos == std::string::npos)
				{
					break;
				}

				val2 = atol(ndata.substr(last_pos, (pos-last_pos)).c_str());
				last_pos = pos+1;
				if(val2)
					pi->AddObject(val2);

			} while(pos != std::string::npos);

		}
	}
}

void InstanceSavingManagement::SaveInstanceIdToDB(uint32 instanceid, uint32 mapid)
{
	InstanceInfo::iterator itr = mInstanceInfoList.find( mapid );
	if(itr != mInstanceInfoList.end())
	{
		Instance_Map_Info_Holder *p = itr->second;
		Instance_Map_InstanceId_Holder *pi = p->GetInstanceId(instanceid);
		if(pi)
		{
			pi->SaveInstanceToDB();
		}
	}
}
void InstanceSavingManagement::SaveObjectStateToInstance(Unit *pUnit)
{
	InstanceInfo::iterator itr = mInstanceInfoList.find( pUnit->GetMapId() );
	if(itr != mInstanceInfoList.end())
	{
		Instance_Map_Info_Holder *p = itr->second;
		Instance_Map_InstanceId_Holder *pi = p->GetInstanceId(pUnit->GetInstanceID());
		if(pi)
		{
			pi->AddObject(static_cast<Creature*>(pUnit)->GetSQL_id());
		}
	}
}

Instance_Map_InstanceId_Holder *InstanceSavingManagement::GetInstance(uint32 mapid, uint32 instanceid)
{
	InstanceInfo::iterator itr = mInstanceInfoList.find( mapid );
	if(itr != mInstanceInfoList.end())
	{
		Instance_Map_Info_Holder *p = itr->second;
		Instance_Map_InstanceId_Holder *pi = p->GetInstanceId(instanceid);
		if(pi)
		{
			return pi;
		}
	}
	return NULL;
}

Instance_Map_InstanceId_Holder *InstanceSavingManagement::GetRaidInstance(uint32 mapid, Player * pPlayer)
{
	InstanceInfo::iterator itr = mInstanceInfoList.find( mapid );
	if(itr != mInstanceInfoList.end())
	{
		Instance_Map_Info_Holder *p = itr->second;
		if(p->GetMapInfo() && p->GetMapInfo()->type == INSTANCE_RAID)
		{
			Instance_Map_InstanceId_Holder *pi = p->getInstanceIdByPlayer(pPlayer->GetGUID());
			if(pi)
			{
				return pi;
			}
		}
	}
	return NULL;
}

void InstanceSavingManagement::BuildRaidSavedInstancesForPlayer(Player *pPlayer)
{
	/*hmm packet structure for this one?
	counter, mapid time instanceid
	*/
	WorldPacket data;
	uint32 counter = 0;

	data.Initialize(SMSG_RAID_INSTANCE_INFO);

	InstanceInfo::const_iterator itr;
	for (itr = mInstanceInfoList.begin();itr != mInstanceInfoList.end(); itr++)
	{
		Instance_Map_Info_Holder *p = itr->second;
		if(p->GetMapInfo() && p->GetMapInfo()->type == INSTANCE_RAID)
		{
			Instance_Map_InstanceId_Holder *pi = p->getInstanceIdByPlayer(pPlayer->GetGUID());
			if(pi)
			{
				data << uint32(0x00);
				data << (p->GetMapInfo() ? p->GetMapInfo()->mapid : 0);
				data << (uint32)(pi->GetRaidExpireTime() - time(NULL));
				data << pi->GetInstanceID();

				counter++;
			}
		}
	}
	data.wpos(0);
	data << counter;

	pPlayer->GetSession()->SendPacket(&data);
}

void InstanceSavingManagement::DeleteInstanceFromDB(uint32 instanceid)
{
	std::stringstream ss;
	ss << "DELETE FROM instances WHERE instanceid = '";
	ss << instanceid << "'";
	sDatabase.Execute( ss.str().c_str() );

}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Instance Management - MapID InstanceId list holder
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Instance_Map_Info_Holder::Instance_Map_Info_Holder()
{
	m_pMapInfo = NULL;

}
Instance_Map_Info_Holder::~Instance_Map_Info_Holder()
{
	//clear all instanceids inside this mapid
	InstanceIdList::const_iterator itr;
	for (itr = mInstanceIdList.begin();itr != mInstanceIdList.end(); itr++)
	{
		 Instance_Map_InstanceId_Holder *p = itr->second;
		 delete p;
	}
	mInstanceIdList.clear();
}

void Instance_Map_Info_Holder::RemoveInstanceId(uint32 instanceid)
{
	InstanceIdList::iterator itr = mInstanceIdList.find( instanceid );
	if(itr != mInstanceIdList.end())
	{
		delete itr->second;
		mInstanceIdList.erase(itr);
	}
}

Instance_Map_InstanceId_Holder *Instance_Map_Info_Holder::GetInstanceId(uint32 instanceid)
{
	InstanceIdList::iterator itr = mInstanceIdList.find( instanceid );
	if(itr != mInstanceIdList.end())
	{
		return itr->second;
	}
	return NULL;
}

void Instance_Map_Info_Holder::AddInstanceId(MapMgr *pInstance)
{
	ASSERT( pInstance->GetInstanceID() );

	InstanceIdList::iterator itr = mInstanceIdList.find( pInstance->GetInstanceID() );
	if(itr == mInstanceIdList.end())
	{
		Instance_Map_InstanceId_Holder *pIdList;
		pIdList = new Instance_Map_InstanceId_Holder;
		pIdList->SetMapInfo(pInstance->GetMapInfo());
		pIdList->SetCreationTime(pInstance->CreationTime);
		pIdList->SetRaidExpireTime(pInstance->RaidExpireTime);
		pIdList->SetInstanceID(pInstance->GetInstanceID());

		pIdList->SetGroupSignature(pInstance->GetGroupSignature()); //allows multiple instanceids per mapid

		mInstanceIdList[pInstance->GetInstanceID()] = pIdList;
	}
}

void Instance_Map_Info_Holder::AddInstanceId(InactiveInstance * ia)
{
	ASSERT( ia->InstanceId );

	InstanceIdList::iterator itr = mInstanceIdList.find( ia->InstanceId );
	if(itr == mInstanceIdList.end())
	{
		Instance_Map_InstanceId_Holder *pIdList;
		pIdList = new Instance_Map_InstanceId_Holder;

		MapInfo *pMapInfo = sWorld.GetMapInformation(ia->MapId);
		pIdList->SetMapInfo(pMapInfo);
		pIdList->SetCreationTime(ia->Creator);
		pIdList->SetRaidExpireTime(ia->ExpireTime);
		pIdList->SetInstanceID(ia->InstanceId);

		mInstanceIdList[ia->InstanceId] = pIdList;
	}
}

void Instance_Map_Info_Holder::AddPlayer(uint64 guid, uint32 InstanceID)
{
	ASSERT( InstanceID );

	InstanceIdList::iterator itr = mInstanceIdList.find( InstanceID );
	if(itr != mInstanceIdList.end())
	{
		 Instance_Map_InstanceId_Holder *p = itr->second;
		 p->AddPlayer(guid);
	}
}

bool Instance_Map_Info_Holder::RemovePlayer(uint64 guid, uint32 InstanceID)
{
	ASSERT( InstanceID );
	bool result = false;

	InstanceIdList::iterator itr = mInstanceIdList.find( InstanceID );
	if(itr != mInstanceIdList.end())
	{
		Instance_Map_InstanceId_Holder *p = itr->second;
		 result = p->RemovePlayer(guid);
		 return result;
	}
	return false;
}

bool Instance_Map_Info_Holder::RemovePlayer(uint64 guid)
{
	bool result = false;
	bool result2 = false;

	InstanceIdList::iterator itr, itr2;
	for (itr = mInstanceIdList.begin();itr != mInstanceIdList.end();)
	{
		 itr2 = itr;
		 ++itr;
		 Instance_Map_InstanceId_Holder *p = itr2->second;
		 if(!p->GetGroupSignature() && GetMapInfo()->type != INSTANCE_RAID) //only resets solo instances
		 {
			 MapMgr * mapMgr = sWorldCreator.GetInstanceByGroupInstanceId(itr2->first, GetMapInfo()->mapid, true);
			 if(mapMgr)
			 {
				 // dont reset an instnace with players inside.. dunno how they got in there anyway :P
				 if(mapMgr->HasPlayers())
				 {
					 Player * pPlayer = objmgr.GetPlayer(guid);
					 if(pPlayer) 
						 sChatHandler.SystemMessageToPlr(pPlayer,"You are trying to reset a instance when there are still players inside");
					 return false;
				 }
			 }
			 result = p->RemovePlayer(guid);
			 if(result)
			 {
				 sWorldCreator.DeleteInstance(itr2->first, m_pMapInfo->mapid);
				 delete p;
				 sLog.outDebug("Removing instance from the itr\n");
				 mInstanceIdList.erase(itr2);
				 result2 = true;
			 }
		 }
	}
	return result2;
}
bool Instance_Map_Info_Holder::RemoveGroup(uint32 iGroupSignature)
{
	bool result = false;
	bool result2 = false;

	InstanceIdList::iterator itr, itr2;
	for (itr = mInstanceIdList.begin();itr != mInstanceIdList.end();)
	{
		 itr2 = itr;
		 ++itr;
		Instance_Map_InstanceId_Holder *p = itr2->second;
		 if(p->GetGroupSignature() && p->GetGroupSignature() == iGroupSignature) //only resets this group instance ids
		 {
			 MapMgr * mapMgr = sWorldCreator.GetInstanceByGroupInstanceId(itr2->first, GetMapInfo()->mapid, true);
			 if(mapMgr)
			 {
				 // dont reset an instnace with players inside.. dunno how they got in there anyway :P
				 if(mapMgr->HasPlayers())
				 {
					Group *pGroup =  objmgr.GetGroupById(iGroupSignature);
					if(pGroup)
					{
						Player * pPlayer = pGroup->GetLeader();
						if(pPlayer) 
							sChatHandler.SystemMessageToPlr(pPlayer,"You are trying to reset a instance when there are still players inside");
					}
					 return false;
				 }
			 }
			 result = p->ClearAllPlayers();
			 if(result)
			 {
				 sWorldCreator.DeleteInstance(itr2->first, m_pMapInfo->mapid);
				 delete p;
				 sLog.outDebug("Removing group instance from the itr\n");
				 mInstanceIdList.erase(itr2);
				 result2 = true;
			 }
		 }
	}
	return result2;
}

bool Instance_Map_Info_Holder::FindPlayer(uint64 guid, uint32 iGroupSignature)
{
	InstanceIdList::const_iterator itr;
	for (itr = mInstanceIdList.begin();itr != mInstanceIdList.end(); itr++)
	{
		Instance_Map_InstanceId_Holder *p = itr->second;
		// group matches, returns false and breaks loop if it fails
		// cant exist 2 groups with same id  so no point in continue loop
		// raid groups are ignored for this check.
		if(iGroupSignature && this->GetMapInfo()->type != INSTANCE_RAID)
		{
			if(p->GetGroupSignature() == iGroupSignature)
			{
				bool result = p->FindPlayer(guid);
				if(result) { return true; }

				return false;
			}
		}
		else
		{
			bool result = p->FindPlayer(guid);
			if(result) { return true; }
		}
	}
	return false;
}

bool Instance_Map_Info_Holder::IsPlayerSavedToInstanceId(uint64 guid, uint32 instanceid)
{
	InstanceIdList::iterator itr = mInstanceIdList.find( instanceid );
	if(itr != mInstanceIdList.end())
	{
		Instance_Map_InstanceId_Holder *p = itr->second;
		if(p->GetInstanceID() == instanceid)
		{
			bool result = p->FindPlayer(guid);
			if(result) { return true; }
		}
	}
	return false;
}

Instance_Map_InstanceId_Holder *Instance_Map_Info_Holder::getInstanceIdByPlayer(uint64 guid)
{
	InstanceIdList::const_iterator itr;
	for (itr = mInstanceIdList.begin();itr != mInstanceIdList.end(); itr++)
	{
		Instance_Map_InstanceId_Holder *p = itr->second;
		if(p->FindPlayer(guid))
		{
			return p;
		}
	}
	return NULL;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Instance Management - InstanceId player list holder
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Instance_Map_InstanceId_Holder::Instance_Map_InstanceId_Holder()
{
	m_pMapInfo = NULL;
	CreationTime = 0;
	ExpireTime = 0;
	IsSaved = false;
	m_instanceid = 0;
}

Instance_Map_InstanceId_Holder::~Instance_Map_InstanceId_Holder()
{
	mPlayerList.clear();
	mNpcList.clear();
}

bool Instance_Map_InstanceId_Holder::FindPlayer(uint64 guid)
{
	PlayerList::iterator itr = mPlayerList.find( guid );
	if(itr == mPlayerList.end())
	{
		return false;
	}
	else
	{
		return true;
	}
}
bool Instance_Map_InstanceId_Holder::ClearAllPlayers()
{
	mPlayerList.clear();
	return true;
}

bool Instance_Map_InstanceId_Holder::ClearAllObjects()
{
	mNpcList.clear();
	return true;
}

void Instance_Map_InstanceId_Holder::AddPlayer(uint64 guid)
{
	mPlayerList[guid] = guid;
}

void Instance_Map_InstanceId_Holder::AddObject(uint32 entry)
{
	mNpcList[entry] = entry;
}

bool Instance_Map_InstanceId_Holder::FindObject(uint32 entry)
{
	NpcList::iterator itr = mNpcList.find( entry );
	if(itr == mNpcList.end())
	{
		return false;
	}
	else
	{
		return true;
	}
}

bool Instance_Map_InstanceId_Holder::RemovePlayer(uint64 guid)
{
	PlayerList::iterator itr = mPlayerList.find( guid );
	if(itr == mPlayerList.end())
	{
		//error
		return false;
	}
	else
	{
		mPlayerList.erase(itr);
		return true;
	}
}

void Instance_Map_InstanceId_Holder::SaveInstanceToDB()
{
	std::stringstream ss;
	PlayerList::const_iterator itr;

	ss << "REPLACE INTO instances VALUES (";
	ss << m_instanceid << ", ";
	ss << GetMapInfo()->mapid << ", '";

	for (itr = mNpcList.begin();itr != mNpcList.end(); itr++)
	{
	ss << itr->first << " ";

	}
	ss << "', '";

	for (itr = mPlayerList.begin();itr != mPlayerList.end(); itr++)
	{
	ss << itr->first << " ";

	}
	ss << "', ";
	ss << CreationTime << ", ";
	ss << ExpireTime << ");";
	sDatabase.Execute( ss.str().c_str() );
}

void InstanceSavingManagement::AddInactiveInstance(InactiveInstance * ia)
{
	inactiveInstancesMutex.Acquire();
	inactiveInstances[ia->InstanceId] = ia;
	inactiveInstancesMutex.Release();
}

void InstanceSavingManagement::RemoveSavedInstance(uint32 instance_id)
{
	map<uint32, InactiveInstance*>::iterator itr = inactiveInstances.find(instance_id);
	if(itr == inactiveInstances.end()) return;

	delete itr->second;
	inactiveInstances.erase(itr);
}

void InstanceSavingManagement::CreateInactiveInstance(MapMgr * mgr)
{
	// called on mapmgr expire..
	InactiveInstance * ia = new InactiveInstance;
	ia->Creation = mgr->CreationTime;
	ia->InstanceId = mgr->GetInstanceID();
	ia->MapId = mgr->GetMapId();
	ia->Creator = mgr->GetCreator();
	ia->GroupSignature = mgr->GetGroupSignature();

	AddInactiveInstance(ia);
}

InactiveInstance * InstanceSavingManagement::GetInactiveInstance(uint32 instance_id)
{
	inactiveInstancesMutex.Acquire();
	map<uint32, InactiveInstance*>::iterator itr = inactiveInstances.find(instance_id);
	if(itr == inactiveInstances.end())
	{
		inactiveInstancesMutex.Release();
		return 0;
	}

	InactiveInstance * ia = itr->second;
	inactiveInstances.erase(itr);
	inactiveInstancesMutex.Release();
	return ia;
}

void InstanceSavingManagement::SaveInstance(InactiveInstance *ia)
{
	ASSERT( ia->InstanceId );

	inactiveInstancesMutex.Acquire();
	InstanceInfo::iterator itr = mInstanceInfoList.find( ia->MapId );
	if(itr == mInstanceInfoList.end())
	{
		Instance_Map_Info_Holder *mapholder;

		mapholder = new Instance_Map_Info_Holder;
		MapInfo *pMapInfo = sWorld.GetMapInformation(ia->MapId);
		mapholder->SetMapInfo(pMapInfo);
		mapholder->AddInstanceId(ia);

		mInstanceInfoList[ia->MapId] = mapholder;
	}
	else
	{
		Instance_Map_Info_Holder *mapholder = itr->second;
		MapInfo *pMapInfo = sWorld.GetMapInformation(ia->MapId);
		mapholder->SetMapInfo(pMapInfo);
		mapholder->AddInstanceId(ia);
	}
	inactiveInstancesMutex.Release();
}
