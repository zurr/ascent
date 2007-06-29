/****************************************************************************
 *
 * Warsong Gulch Battleground
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

class WarsongGulch : public Battleground
{
public:
	WarsongGulch();
	~WarsongGulch();

	/* Scorekeeping */
	uint64 FlagGuids[2];
	uint64 FlagHolders[2];
	bool FlagInbase[2];
	GameObject *pFlags[2];
	uint32 m_BGTime;
	uint32 m_MaxScore;

	GameObject *gbuffs[5];

	std::set<GameObject*> m_Gates;

	void HandleBattlegroundAreaTrigger(Player *plr, uint32 TriggerID);
	void HandleBattlegroundEvent(Object *src, Object *dst, uint16 EventID, uint32 params1 = 0, uint32 params2 = 0);
	void SetupBattleground();
	void SpawnBattleground();
	void SpawnBuff(uint32 typeentry,uint32 bannerslot);

	void Remove();

	void Start();
	void EventUpdate(uint32 diff);

	inline void SetBattlegroundMaxScore(uint32 value){m_MaxScore = value;}
	inline uint32 GetBattlegroundMaxScore(){return m_MaxScore;}

	void SetAllianceScore(uint32 score);
	void SetHordeScore(uint32 score);
	uint32 GetAllianceScore();
	uint32 GetHordeScore();

	void SpawnSpiritGuides();
	bool GetRepopCoordinates(Player * plr, LocationVector & vec);

	void AutoReturnFlag(GameObject * flag, uint32 flagteam);
};
