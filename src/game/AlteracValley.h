/****************************************************************************
 *
 * Alterac Valley Battleground
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
