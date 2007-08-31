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
// MapCell.cpp
//
#include "StdAfx.h"

MapCell::~MapCell()
{
	RemoveObjects();
}

void MapCell::Init(uint32 x, uint32 y, uint32 mapid, MapMgr *mapmgr)
{
	_mapmgr = mapmgr;
	_active = false;
	_loaded = false;
	_playerCount = 0;
	_x=x;
	_y=y;
	_unloadpending=false;
}

void MapCell::AddObject(Object *obj)
{
	if(obj->IsPlayer())
		++_playerCount;

	_objects.insert(obj);
}

void MapCell::RemoveObject(Object *obj)
{
	if(obj->IsPlayer())
		--_playerCount;

	_objects.erase(obj);
}

void MapCell::SetActivity(bool state)
{	
	if(!_active && state)
	{
		// Move all objects to active set.
		for(ObjectSet::iterator itr = _objects.begin(); itr != _objects.end(); ++itr)
		{
			if(!(*itr)->Active && (*itr)->CanActivate())
				(*itr)->Activate(_mapmgr);
		}

		if(_unloadpending)
			CancelPendingUnload();

	} else if(_active && !state)
	{
		// Move all objects from active set.
		for(ObjectSet::iterator itr = _objects.begin(); itr != _objects.end(); ++itr)
		{
			if((*itr)->Active)
				(*itr)->Deactivate(_mapmgr);
		}

		if(sWorld.map_unload_time && !_unloadpending)
			QueueUnloadPending();
	}

	_active = state; 

}
void MapCell::RemoveObjects()
{
	ObjectSet::iterator itr;
	uint32 count = 0;
	//uint32 ltime = getMSTime();

	//This time it's simpler! We just remove everything :)
	for(itr = _objects.begin(); itr != _objects.end(); )
	{
		count++;

		Object *obj = (*itr);

		itr++;

		if(_unloadpending && UINT32_LOPART(obj->GetGUIDHigh())==HIGHGUID_TRANSPORTER)
			continue;

		if(_unloadpending && obj->GetTypeId()==TYPEID_CORPSE)
			continue;

		if(obj->Active)
			obj->Deactivate(_mapmgr);

		if (obj->IsInWorld())
			obj->RemoveFromWorld();

		if (obj->GetTypeId() == TYPEID_UNIT)
		{
			if(obj->IsPet())
				delete ((Pet*)(obj));
			else
				delete ((Creature*)obj);
		}
		else if (obj->GetTypeId() == TYPEID_GAMEOBJECT)
		{
			if(obj->GetGUIDHigh()==HIGHGUID_TRANSPORTER)
				delete ((Transporter*)obj);
			else
				delete ((GameObject*)obj);
		}
		else if (obj->GetTypeId() == TYPEID_DYNAMICOBJECT)
			delete ((DynamicObject*)obj);
		else if (obj->GetTypeId() == TYPEID_CORPSE)
		{
			delete ((Corpse*)obj);
		}
	}

	_objects.clear();
	_playerCount = 0;
	_loaded = false;
}


void MapCell::LoadObjects(CellSpawns * sp, Instance_Map_InstanceId_Holder * pInstance)
{
	_loaded = true;

	if(sp->CreatureSpawns.size())//got creatures
	{
		for(CreatureSpawnList::iterator i=sp->CreatureSpawns.begin();i!=sp->CreatureSpawns.end();i++)
		{
			if(pInstance && pInstance->FindObject((*i)->id) || pInstance && pInstance->FindObject((*i)->respawnNpcLink))
			{
				continue;
			}
			Creature * c=_mapmgr->CreateCreature();

			c->SetMapId(_mapmgr->GetMapId());
			c->SetInstanceID(_mapmgr->GetInstanceID());

            if(c->Load(*i, _mapmgr->iInstanceMode, _mapmgr->GetMapInfo()))
			{
				if(!c->CanAddToWorld())
					delete c;

				c->PushToWorld(_mapmgr);
			}
			else
				delete c;//missing proto or smth of that kind
		}
	}

	if(sp->GOSpawns.size())//got GOs
	{
		for(GOSpawnList::iterator i=sp->GOSpawns.begin();i!=sp->GOSpawns.end();i++)
		{
			GameObject * go=_mapmgr->CreateGameObject();
			if(go->Load(*i))
			{
				uint32 state = go->GetUInt32Value(GAMEOBJECT_STATE);
				if(pInstance && pInstance->FindObject((*i)->stateNpcLink))
				{
					go->SetUInt32Value(GAMEOBJECT_STATE, (state ? 0 : 1));
				}
			   

				go->PushToWorld(_mapmgr);
			}
			else
				delete go;//missing proto or smth of that kind
		}
	}
}


void MapCell::QueueUnloadPending()
{
	if(_unloadpending)
		return;

	_unloadpending = true;
	//Log.Debug("MapCell", "Queueing pending unload of cell %u %u", _x, _y);
	sEventMgr.AddEvent(_mapmgr, &MapMgr::UnloadCell,(uint32)_x,(uint32)_y,MAKE_CELL_EVENT(_x,_y),sWorld.map_unload_time * 1000,1,0);
}

void MapCell::CancelPendingUnload()
{
	//Log.Debug("MapCell", "Cancelling pending unload of cell %u %u", _x, _y);
	if(!_unloadpending)
		return;

	sEventMgr.RemoveEvents(_mapmgr,MAKE_CELL_EVENT(_x,_y));
}

void MapCell::Unload()
{
	//Log.Debug("MapCell", "Unloading cell %u %u", _x, _y);
	ASSERT(_unloadpending);
	if(_active)
		return;

	RemoveObjects();
	_unloadpending=false;
}
