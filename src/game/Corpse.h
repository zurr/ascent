/****************************************************************************
 *
 * Corpses
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
	bool NeedsInRangeSet() { return false; }
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
	void Delink();
private:
	uint32 m_state;
	uint32 _fields[CORPSE_END];
	bool _loadedfromdb;
};

#endif

