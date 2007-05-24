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
// MapCell.h
//

#ifndef __MAP_CELL_H
#define __MAP_CELL_H

class Map;
struct Instance_Map_InstanceId_Holder;

class MapCell
{
	friend class MapMgr;
public:
	MapCell() {};
	~MapCell();

	typedef std::set<Object*> ObjectSet;

	//Init
	void Init(uint32 x, uint32 y, uint32 mapid, MapMgr *mapmgr);

	//Object Managing
	void AddObject(Object *obj); 
	void RemoveObject(Object *obj);
	bool HasObject(Object *obj) { return (_objects.find(obj) != _objects.end()); }
	bool HasPlayers() { return ((_playerCount > 0) ? true : false); }
	inline uint32 GetObjectCount() { return _objects.size(); }
	void RemoveObjects();
	inline ObjectSet::iterator Begin() { return _objects.begin(); }
	inline ObjectSet::iterator End() { return _objects.end(); }

	//State Related
	void SetActivity(bool state);

	inline bool IsActive() { return _active; }
	inline bool IsLoaded() { return _loaded; }

	//Object Loading Managing
	void LoadObjects(CellSpawns * sp, Instance_Map_InstanceId_Holder * pInstance);
	inline uint32 GetPlayerCount() { return _playerCount; }

private:
	ObjectSet _objects;
	bool _active, _loaded;
	uint16 _playerCount;
	MapMgr* _mapmgr;
};

#endif
