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

/* * Class InstanceScript
   * Instanced class created for each instance of the map, holds all 
   * scriptable exports
*/

/* * Class MapScriptInterface
   * Provides an interface to mapmgr for scripts, to obtain objects,
   * get players, etc.
*/

#include "StdAfx.h"
createFileSingleton(StructFactory);

MapScriptInterface::MapScriptInterface(MapMgr & mgr) : mapMgr(mgr)
{

}

MapScriptInterface::~MapScriptInterface()
{
    mapMgr.ScriptInterface = 0;
}

uint32 MapScriptInterface::GetPlayerCountInRadius(float x, float y, float z /* = 0.0f */, float radius /* = 5.0f */)
{
    // use a cell radius of 2
    uint32 PlayerCount = 0;
    uint32 cellX = mapMgr.GetPosX(x);
    uint32 cellY = mapMgr.GetPosY(y);

    uint32 endX = cellX < _sizeX ? cellX + 1 : _sizeX;
    uint32 endY = cellY < _sizeY ? cellY + 1 : _sizeY;
    uint32 startX = cellX > 0 ? cellX - 1 : 0;
    uint32 startY = cellY > 0 ? cellY - 1 : 0;
    MapCell * pCell;
    ObjectSet::iterator iter, iter_end;

    for(uint32 cx = startX; cx < endX; ++cx)
    {
        for(uint32 cy = startY; cy < endY; ++cy)
        {
            pCell = mapMgr.GetCell(cx, cy);
            if(pCell == 0 || pCell->GetPlayerCount() == 0)
                continue;

            iter = pCell->Begin();
            iter_end = pCell->End();

            for(; iter != iter_end; ++iter)
            {
                if((*iter)->GetTypeId() == TYPEID_PLAYER &&
                    (*iter)->CalcDistance(x, y, (z == 0.0f ? (*iter)->GetPositionZ() : z)) < radius)
                {
                    ++PlayerCount;
                }
            }
        }
    }

    return PlayerCount;
}

GameObject* MapScriptInterface::SpawnGameObject(uint32 Entry, float cX, float cY, float cZ, float cO, bool AddToWorld, uint32 Misc1, uint32 Misc2)
{
   
    GameObject *pGameObject = mapMgr.CreateGameObject();
	if(!pGameObject->CreateFromProto(Entry, mapMgr.GetMapId(), cX, cY, cZ, cO))
	{
		delete pGameObject;
		return NULL;
	}
    pGameObject->SetInstanceID(mapMgr.GetInstanceID());

    if(AddToWorld)
        pGameObject->PushToWorld(&mapMgr);

    return pGameObject;
}

Creature* MapScriptInterface::SpawnCreature(uint32 Entry, float cX, float cY, float cZ, float cO, bool AddToWorld, bool tmplate, uint32 Misc1, uint32 Misc2)
{
    //TODO: REPLACE WITH PROTOTYPE!
    return 0;
}

void MapScriptInterface::DeleteCreature(Creature* ptr)
{
	delete ptr;
}

void MapScriptInterface::DeleteGameObject(GameObject *ptr)
{
	delete ptr;
}

WayPoint * StructFactory::CreateWaypoint()
{
    return new WayPoint;
}
