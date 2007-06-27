#include "StdAfx.h"
#include "Setup.h"

/************************************************************************/
/* Raid_TheEye.cpp Script												*/
/************************************************************************/



/****************/
/*    Bosses    */
/****************/

// Void Reaver AI

#define CN_VOID_REAVER 19516

#define POUNDING 34164
#define ARCANE_ORB 34190
#define KNOCK_AWAY 36512 // maybe wrong id (maybe should be: 21737 ? ), but works
#define ENRAGE 36992 // Needs checking (as it can be wrong [or maybe IS wrong])

//TO DO: Add rest of the sounds (no idea to which action they should be used)

class VOIDREAVERAI : public CreatureAIScript
{
public:
    ADD_CREATURE_FACTORY_FUNCTION(VOIDREAVERAI);
	SP_AI_Spell spells[4];
	bool m_spellcheck[4];

    VOIDREAVERAI(Creature* pCreature) : CreatureAIScript(pCreature)
    {
		nrspells = 4;
		for(int i=0;i<nrspells;i++)
		{
			m_spellcheck[i] = false;
		}
        spells[0].info = sSpellStore.LookupEntry(POUNDING);
		spells[0].targettype = TARGET_VARIOUS;
		spells[0].instant = true;
		spells[0].perctrigger = 16.0f;
		spells[0].attackstoptimer = 1000;

		spells[1].info = sSpellStore.LookupEntry(ARCANE_ORB);
		spells[1].targettype = TARGET_VARIOUS;
		spells[1].instant = true;
		spells[1].perctrigger = 9.0f;
		spells[1].attackstoptimer = 2000;

		spells[2].info = sSpellStore.LookupEntry(KNOCK_AWAY);
		spells[2].targettype = TARGET_ATTACKING;
		spells[2].instant = true;
		spells[2].perctrigger = 7.0f;
		spells[2].attackstoptimer = 1000;

		spells[3].info = sSpellStore.LookupEntry(ENRAGE);
		spells[3].targettype = TARGET_SELF;
		spells[3].instant = true;
		spells[3].perctrigger = 0.0f;
		spells[3].attackstoptimer = 1000;
	} 

    void OnCombatStart(Unit* mTarget)
    {
		_unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, "Alert, you are marked for extermination!");
		_unit->PlaySoundToSet(11213);
        RegisterAIUpdateEvent(_unit->GetUInt32Value(UNIT_FIELD_BASEATTACKTIME));
    }

	void OnTargetDied(Unit* mTarget)
    {
		if (_unit->GetHealthPct() > 0)	// Hack to prevent double yelling (OnDied and OnTargetDied when creature is dying)
		{
			int RandomSpeach;
			sRand.randInt(1000);
			RandomSpeach=rand()%3;
			switch (RandomSpeach)
			{
			case 0: 
				_unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, "Extermination, successful.");
				_unit->PlaySoundToSet(11215);
				break;
			case 1:
				_unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, "Imbecile life form, no longer functional.");
				_unit->PlaySoundToSet(11216);
				break;
			case 2:
				_unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, "Threat neutralized.");
				_unit->PlaySoundToSet(11217);
				break;
			}
		}
    }

    void OnCombatStop(Unit *mTarget)
    {
        _unit->GetAIInterface()->setCurrentAgent(AGENT_NULL);
        _unit->GetAIInterface()->SetAIState(STATE_IDLE);
        RemoveAIUpdateEvent();
    }

	void OnDied(Unit * mKiller)
    {
		_unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, "Systems... shutting... down...");
        _unit->PlaySoundToSet(11214);
       RemoveAIUpdateEvent();
    }

    void AIUpdate()
    {
		Timer = Timer + 1;

		if (Timer == 200) // Need correct attackspeed in DB to change it to correct value
		{
			_unit->CastSpell(_unit, spells[3].info, spells[3].instant);
		}

		else
		{
			float val = sRand.rand(100.0f);
			SpellCast(val);
		}
    }

    void SpellCast(float val)
    {
        if(_unit->GetCurrentSpell() == NULL && _unit->GetAIInterface()->GetNextTarget())
        {
			float comulativeperc = 0;
		    Unit *target = NULL;
			for(int i=0;i<nrspells;i++)
			{
				if(!spells[i].perctrigger) continue;
				
				if(m_spellcheck[i])
				{
					target = _unit->GetAIInterface()->GetNextTarget();
					switch(spells[i].targettype)
					{
						case TARGET_SELF:
						case TARGET_VARIOUS:
							_unit->CastSpell(_unit, spells[i].info, spells[i].instant); break;
						case TARGET_ATTACKING:
							_unit->CastSpell(target, spells[i].info, spells[i].instant); break;
						case TARGET_DESTINATION:
							_unit->CastSpellAoF(target->GetPositionX(),target->GetPositionY(),target->GetPositionZ(), spells[i].info, spells[i].instant); break;
					}

					if (spells[i].speech != "")
					{
						_unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, spells[i].speech.c_str());
						_unit->PlaySoundToSet(spells[i].soundid); 
					}

					m_spellcheck[i] = false;
					return;
				}

				if(val > comulativeperc && val <= (comulativeperc + spells[i].perctrigger))
				{
					_unit->setAttackTimer(spells[i].attackstoptimer, false);
					m_spellcheck[i] = true;
				}
				comulativeperc += spells[i].perctrigger;
			}
        }
    }
protected:

	int nrspells;
	int Timer;
};
/*
// High Astromancer Solarian AI


#define CN_HIGH_ASTROMANCER_SOLARIAN 18805
//Phase 1 spells
#define ARCANE_MISSILES 39414 // Should have random targeting
#define WRATH_OF_THE_ASTROMANCER 33045 // Needs random function
#define MARK_OF_SOLARIAN 33023 // not sure... but should be used on random target
//Phase 2 spells
// Just to define portal summoning + summoning creatures + creatures AI
// Add sounds to creature summoning events
//Phase 3 spells
#define VOID_BOLTS 39329 // RANDOM target, but because of lack of feature ATTACKING
#define FEAR 38154 // probably wrong id; maybe one of these are correct: 31970, 31358 (?)

// TO DO: Rewrite it to phase style.

class HIGHASTROMANCERSOLARIANAI : public CreatureAIScript
{
public:
    ADD_CREATURE_FACTORY_FUNCTION(HIGHASTROMANCERSOLARIANAI);
	SP_AI_Spell spells[3];
	bool m_spellcheck[3];

    HIGHASTROMANCERSOLARIANAI(Creature* pCreature) : CreatureAIScript(pCreature)
    {
		m_phase = 1;
		nrspells = 3;
		for(int i=0;i<nrspells;i++)
		{
			m_spellcheck[i] = false;
		}
        spells[0].info = sSpellStore.LookupEntry(ARCANE_MISSILES);
		spells[0].targettype = TARGET_ATTACKING;
		spells[0].instant = true;
		spells[0].perctrigger = 8.0f;
		spells[0].attackstoptimer = 1000;

		spells[1].info = sSpellStore.LookupEntry(WRATH_OF_THE_ASTROMANCER);
		spells[1].targettype = TARGET_ATTACKING;
		spells[1].instant = true;
		spells[1].perctrigger = 15.0f;
		spells[1].attackstoptimer = 2000;

		spells[2].info = sSpellStore.LookupEntry(MARK_OF_SOLARIAN);
		spells[2].targettype = TARGET_ATTACKING;
		spells[2].instant = true;
		spells[2].perctrigger = 10.0f;
		spells[2].attackstoptimer = 1000;
	} 

    void OnCombatStart(Unit* mTarget)
    {
		m_phase = 1;
		_unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, "Whatever, Sindorai!");
		_unit->PlaySoundToSet(11134);
        RegisterAIUpdateEvent(_unit->GetUInt32Value(UNIT_FIELD_BASEATTACKTIME));
    }

	void OnTargetDied(Unit* mTarget)
    {
		if (_unit->GetHealthPct() > 0)	// Hack to prevent double yelling (OnDied and OnTargetDied when creature is dying)
		{
			int RandomSpeach;
			sRand.randInt(1000);
			RandomSpeach=rand()%3;
			switch (RandomSpeach)
			{
			case 0: 
				_unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, "Your soul belongs to the xxxx!");
				_unit->PlaySoundToSet(11136);
				break;
			case 1:
				_unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, "By the blood of the high born!");
				_unit->PlaySoundToSet(11137);
				break;
			case 2:
				_unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, "For the sun well!");
				_unit->PlaySoundToSet(11138);
				break;
			}
		}

    }

    void OnCombatStop(Unit *mTarget)
    {
        _unit->GetAIInterface()->setCurrentAgent(AGENT_NULL);
        _unit->GetAIInterface()->SetAIState(STATE_IDLE);
        RemoveAIUpdateEvent();
    }

	void OnDied(Unit * mKiller)
    {
		_unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, "Died... W00T?!");
        _unit->PlaySoundToSet(11135);
       RemoveAIUpdateEvent();
    }

    void AIUpdate()
    {
		float val = sRand.rand(100.0f);
        SpellCast(val);
		if (_unit->GetHealthPct() > 65)
		if (_unit->GetHealthPct() <= 65)
		if (_unit->GetHealthPct() <= 20)
    }

    void SpellCast(float val)
    {
        if(_unit->GetCurrentSpell() == NULL && _unit->GetAIInterface()->GetNextTarget())
        {
			float comulativeperc = 0;
		    Unit *target = NULL;
			for(int i=0;i<nrspells;i++)
			{
				if(!spells[i].perctrigger) continue;
				
				if(m_spellcheck[i])
				{
					target = _unit->GetAIInterface()->GetNextTarget();
					switch(spells[i].targettype)
					{
						case TARGET_SELF:
						case TARGET_VARIOUS:
							_unit->CastSpell(_unit, spells[i].info, spells[i].instant); break;
						case TARGET_ATTACKING:
							_unit->CastSpell(target, spells[i].info, spells[i].instant); break;
						case TARGET_DESTINATION:
							_unit->CastSpellAoF(target->GetPositionX(),target->GetPositionY(),target->GetPositionZ(), spells[i].info, spells[i].instant); break;
					}

					if (spells[i].speech != "")
					{
						_unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, spells[i].speech.c_str());
						_unit->PlaySoundToSet(spells[i].soundid); 
					}

					m_spellcheck[i] = false;
					return;
				}

				if(val > comulativeperc && val <= (comulativeperc + spells[i].perctrigger))
				{
					_unit->setAttackTimer(spells[i].attackstoptimer, false);
					m_spellcheck[i] = true;
				}
				comulativeperc += spells[i].perctrigger;
			}
        }
    }
protected:

	int nrspells;
};
*/


// High Astromancer Solarian AI
// First try to make it in phase system. For now not enough luck (as I want to update spell list
// when next phase has just started.

#define CN_HIGH_ASTROMANCER_SOLARIAN 18805
//Phase 1 spells
#define ARCANE_MISSILES 39414 // Should have random targeting
#define WRATH_OF_THE_ASTROMANCER 33045 // Needs random function
#define MARK_OF_SOLARIAN 33023 // not sure... but should be used on random target
//Phase 2 spells
// Just to define portal summoning + summoning creatures + creatures AI
// Add sounds to creature summoning events
//Phase 3 spells
#define VOID_BOLTS 39329 // RANDOM target, but because of lack of feature ATTACKING
#define FEAR 38154 // probably wrong id; maybe one of these are correct: 31970, 31358 (?)

// TO DO: Rewrite it to phase style.

class HIGHASTROMANCERSOLARIANAI : public CreatureAIScript
{
public:
    ADD_CREATURE_FACTORY_FUNCTION(HIGHASTROMANCERSOLARIANAI);
	SP_AI_Spell spells[3];
	bool m_spellcheck[3];

    HIGHASTROMANCERSOLARIANAI(Creature* pCreature) : CreatureAIScript(pCreature)
    {
		//m_phase = 1;
		//if (m_phase == 1)
		if (_unit->GetHealthPct() > 90 || _unit->GetHealthPct() <= 50)
		{
			nrspells = 3;
			for(int i=0;i<nrspells;i++)
			{
				m_spellcheck[i] = false;
			}
			spells[0].info = sSpellStore.LookupEntry(ARCANE_MISSILES);
			spells[0].targettype = TARGET_ATTACKING;
			spells[0].instant = true;
			spells[0].perctrigger = 8.0f;
			spells[0].attackstoptimer = 1000;

			spells[1].info = sSpellStore.LookupEntry(WRATH_OF_THE_ASTROMANCER);
			spells[1].targettype = TARGET_ATTACKING;
			spells[1].instant = true;
			spells[1].perctrigger = 15.0f;
			spells[1].attackstoptimer = 2000;

			spells[2].info = sSpellStore.LookupEntry(MARK_OF_SOLARIAN);
			spells[2].targettype = TARGET_ATTACKING;
			spells[2].instant = true;
			spells[2].perctrigger = 10.0f;
			spells[2].attackstoptimer = 1000;
		}
	
		//if (m_phase == 2)
		if (_unit->GetHealthPct() <= 90 && _unit->GetHealthPct() > 50)
		{
			nrspells = 0;
			for(int i=0;i<nrspells;i++)
			{
				m_spellcheck[i] = false;
			}
		}
/*
		if (m_phase == 3)
		{
			nrspells = 0;
			for(int i=0;i<nrspells;i++)
			{
				m_spellcheck[i] = false;
			}
		}*/
	} 

    void OnCombatStart(Unit* mTarget)
    {
		m_phase = 1;
		_unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, "Whatever, Sindorai!");
		_unit->PlaySoundToSet(11134);
        RegisterAIUpdateEvent(_unit->GetUInt32Value(UNIT_FIELD_BASEATTACKTIME));
    }

	void OnTargetDied(Unit* mTarget)
    {
		if (_unit->GetHealthPct() > 0)	// Hack to prevent double yelling (OnDied and OnTargetDied when creature is dying)
		{
			int RandomSpeach;
			sRand.randInt(1000);
			RandomSpeach=rand()%3;
			switch (RandomSpeach)
			{
			case 0: 
				_unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, "Your soul belongs to the xxxx!");
				_unit->PlaySoundToSet(11136);
				break;
			case 1:
				_unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, "By the blood of the high born!");
				_unit->PlaySoundToSet(11137);
				break;
			case 2:
				_unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, "For the sun well!");
				_unit->PlaySoundToSet(11138);
				break;
			}
		}
    }

    void OnCombatStop(Unit *mTarget)
    {
        _unit->GetAIInterface()->setCurrentAgent(AGENT_NULL);
        _unit->GetAIInterface()->SetAIState(STATE_IDLE);
        RemoveAIUpdateEvent();
    }

	void OnDied(Unit * mKiller)
    {
		m_phase = 1;
		_unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, "Died... W00T?!");
        _unit->PlaySoundToSet(11135);
       RemoveAIUpdateEvent();
    }

    void AIUpdate()
    {/*
		if (_unit->GetHealthPct() <= 90 && _unit->GetHealthPct() > 50)
		{
			PhaseTwo();
		}
		
		if (_unit->GetHealthPct() > 90 && _unit->GetHealthPct() < 50)
		{
			PhaseOne();
		}

		float val = sRand.rand(100.0f);
		SpellCast(val);*/
		/*
		if(_unit->GetHealthPct() <= 95)
		{
			m_phase = 2;
			float val = sRand.rand(100.0f);
			SpellCast(val);
		}

		if(_unit->GetHealthPct() <= 50 || _unit->GetHealthPct() >95)
		{
			m_phase = 1;
			float val = sRand.rand(100.0f);
			SpellCast(val);
		}*/
		float val = sRand.rand(100.0f);
		SpellCast(val);
    }

	void PhaseOne()
	{
		nrspells = 3;
		for(int i=0;i<nrspells;i++)
		{
			m_spellcheck[i] = false;
		}
		spells[0].info = sSpellStore.LookupEntry(ARCANE_MISSILES);
		spells[0].targettype = TARGET_ATTACKING;
		spells[0].instant = true;
		spells[0].perctrigger = 8.0f;
		spells[0].attackstoptimer = 1000;

		spells[1].info = sSpellStore.LookupEntry(WRATH_OF_THE_ASTROMANCER);
		spells[1].targettype = TARGET_ATTACKING;
		spells[1].instant = true;
		spells[1].perctrigger = 15.0f;
		spells[1].attackstoptimer = 2000;

		spells[2].info = sSpellStore.LookupEntry(MARK_OF_SOLARIAN);
		spells[2].targettype = TARGET_ATTACKING;
		spells[2].instant = true;
		spells[2].perctrigger = 10.0f;
		spells[2].attackstoptimer = 1000;
	}

	void PhaseTwo()
	{
		nrspells = 0;
		for(int i=0;i<nrspells;i++)
		{
			m_spellcheck[i] = false;
		}
	}


    void SpellCast(float val)
    {
        if(_unit->GetCurrentSpell() == NULL && _unit->GetAIInterface()->GetNextTarget())
        {
			float comulativeperc = 0;
		    Unit *target = NULL;
			for(int i=0;i<nrspells;i++)
			{
				if(!spells[i].perctrigger) continue;
				
				if(m_spellcheck[i])
				{
					target = _unit->GetAIInterface()->GetNextTarget();
					switch(spells[i].targettype)
					{
						case TARGET_SELF:
						case TARGET_VARIOUS:
							_unit->CastSpell(_unit, spells[i].info, spells[i].instant); break;
						case TARGET_ATTACKING:
							_unit->CastSpell(target, spells[i].info, spells[i].instant); break;
						case TARGET_DESTINATION:
							_unit->CastSpellAoF(target->GetPositionX(),target->GetPositionY(),target->GetPositionZ(), spells[i].info, spells[i].instant); break;
					}

					if (spells[i].speech != "")
					{
						_unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, spells[i].speech.c_str());
						_unit->PlaySoundToSet(spells[i].soundid); 
					}

					m_spellcheck[i] = false;
					return;
				}

				if(val > comulativeperc && val <= (comulativeperc + spells[i].perctrigger))
				{
					_unit->setAttackTimer(spells[i].attackstoptimer, false);
					m_spellcheck[i] = true;
				}
				comulativeperc += spells[i].perctrigger;
			}
        }
    }
protected:

	int nrspells;
	int m_phase;
};


void SetupTheEye(ScriptMgr * mgr)
{
	mgr->register_creature_script(CN_VOID_REAVER, &VOIDREAVERAI::Create);
	mgr->register_creature_script(CN_HIGH_ASTROMANCER_SOLARIAN, &HIGHASTROMANCERSOLARIANAI::Create);
}
