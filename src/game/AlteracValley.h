/****************************************************************************
 *
 * Alterac Valley Battleground
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

#define MAXOFBANNERS 14

class AlteracValley : public Battleground
{
public:
	AlteracValley();
	~AlteracValley();

	uint32 m_BGTime;
	
	GameObject *gcbanner[MAXOFBANNERS];

	std::set<GameObject*> m_Gates;

	void HandleBanner(Player *p_caster,GameObject *go,uint32 spellid);
	void HandleBattlegroundAreaTrigger(Player *plr, uint32 TriggerID);
	void HandleBattlegroundEvent(Object *src, Object *dst, uint16 EventID, uint32 params1 = 0, uint32 params2 = 0);
	void SetupBattleground();
	void SpawnBattleground();
	
	void Remove();

	void Start();
	void EventUpdate(uint32 diff);
	void EventCaptureBase(Player *src,uint32 bannerslot);
};
