/****************************************************************************
 *
 * Corpses
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

#ifndef WOWSERVER_CORPSE_H
#define WOWSERVER_CORPSE_H

enum CORPSE_STATE
{
	CORPSE_STATE_BODY = 0,
	CORPSE_STATE_BONES = 1,
};
struct CorpseData
{
	uint32 LowGuid;
	uint32 mapid;
	uint64 owner;
	uint32 instancemapid;
	float x;
	float y;
	float z;
	void DeleteFromDB();
};

class SERVER_DECL Corpse : public Object
{
public:
	Corpse( uint32 high, uint32 low );
	~Corpse();

   // void Create();
	void Create (Player *owner, uint32 mapid, float x, float y, float z, float ang );

	void SaveToDB();
	void DeleteFromDB();
	inline void SetCorpseState(uint32 state) { m_state = state; }
	inline uint32 GetCorpseState() { return m_state; }
	void Despawn();
	
	inline void SetLoadedFromDB(bool value) { _loadedfromdb = value; }
	inline bool GetLoadedFromDB(void) { return _loadedfromdb; }
	Loot loot;
	void generateLoot();

	void SpawnBones();
private:
	uint32 m_state;
	uint32 _fields[CORPSE_END];
	bool _loadedfromdb;
};

#endif

