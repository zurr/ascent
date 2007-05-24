/****************************************************************************
 *
 * Arena Battleground
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

class Arena : public Battleground
{
public:
	Arena();
	~Arena();

	uint32 m_BGTime;
	uint32 m_MaxScore;

	std::set<GameObject*> m_Gates;

	void HandleBattlegroundAreaTrigger(Player *plr, uint32 TriggerID);
	void HandleBattlegroundEvent(Object *src, Object *dst, uint16 EventID, uint32 params1 = 0, uint32 params2 = 0);
	void SetupBattleground();
	void SpawnBattleground();

	void Remove();

	void Start();
	void EventUpdate(uint32 diff);
};
