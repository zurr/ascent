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
