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

#ifndef _MAP_SCRIPT_INTERFACE_H
#define _MAP_SCRIPT_INTERFACE_H

/* * Class InstanceScript
   * Instanced class created for each instance of the map, holds all 
   * scriptable exports
*/

/* * Class MapScriptInterface
   * Provides an interface to mapmgr for scripts, to obtain objects,
   * get players, etc.
*/

class GameObject;
class Object;
class Creature;
class Unit;
class Player;

class SERVER_DECL MapScriptInterface
{
public:
	MapScriptInterface(MapMgr & mgr);
	~MapScriptInterface();

	template<class T, uint32 TypeId> T* GetObjectNearestCoords(uint32 Entry, float x, float y, float z = 0.0f)
	{
		MapCell * pCell = mapMgr.GetCell(mapMgr.GetPosX(x), mapMgr.GetPosY(y));
		if(pCell == 0)
			return 0;

		T * ClosestObject = 0;
		float ClosestDist = 999999.0f;
		float CurrentDist = 0;
		ObjectSet::iterator iter = pCell->Begin();
		for(; iter != pCell->End(); ++iter)
		{
			CurrentDist = (*iter)->CalcDistance(x, y, (z != 0.0f ? z : (*iter)->GetPositionZ()));
			if(CurrentDist < ClosestDist && (*iter)->GetTypeId() == TypeId)
			{
				if((Entry && (*iter)->GetEntry() == Entry) || !Entry)
				{
					ClosestDist = CurrentDist;
					ClosestObject = ((T*)(*iter));
				}
			}
		}

		return ClosestObject;
	}

	inline GameObject* GetGameObjectNearestCoords(float x, float y, float z = 0.0f, uint32 Entry = 0)
	{
		return GetObjectNearestCoords<GameObject, TYPEID_GAMEOBJECT>(Entry, x, y, z);
	}

	inline Creature* GetCreatureNearestCoords(float x, float y, float z = 0.0f, uint32 Entry = 0)
	{
		return GetObjectNearestCoords<Creature, TYPEID_UNIT>(Entry, x, y, z);
	}

	inline Player* GetPlayerNearestCoords(float x, float y, float z = 0.0f, uint32 Entry = 0)
	{
		return GetObjectNearestCoords<Player, TYPEID_PLAYER>(Entry, x, y, z);
	}

	uint32 GetPlayerCountInRadius(float x, float y, float z = 0.0f, float radius = 5.0f);
	
	GameObject* SpawnGameObject(uint32 Entry, float cX, float cY, float cZ, float cO, bool AddToWorld, uint32 Misc1, uint32 Misc2);
	Creature* SpawnCreature(uint32 Entry, float cX, float cY, float cZ, float cO, bool AddToWorld, bool tmplate, uint32 Misc1, uint32 Misc2);
	WayPoint * CreateWaypoint();

	void DeleteGameObject(GameObject *ptr);
	void DeleteCreature(Creature* ptr);

private:
	MapMgr & mapMgr;
};

class SERVER_DECL StructFactory : public Singleton<StructFactory>
{
public:
	StructFactory() {}
	WayPoint * CreateWaypoint();
};

#define sStructFactory StructFactory::getSingleton()

class SERVER_DECL InstanceScript
{
public:
	InstanceScript(MapMgr *instance);
	virtual ~InstanceScript() {}

	virtual GameObject * GetObjectForOpenLock(Player *pCaster, Spell* pSpell, SpellEntry* pProto) { return 0; }

	virtual void SetLockOptions(uint32 uEntryId, GameObject* pGameObject) { }
	virtual uint32 GetRespawnTimeForCreature(uint32 uEntryId, Creature *pCreature) { return 240000; }

	virtual void Destroy() {}
	virtual void UpdateEvent() {}

protected:
	MapMgr * _instance;

};

#endif

