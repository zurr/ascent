#include "StdAfx.h"
#include "Setup.h"

/************************************************************************/
/* Instance_TheSlavePens.cpp Script		                                    */
/************************************************************************/

// Underbat AI

#define CN_UNDERBAT 17724

#define KNOCKDOWN 20276 // can't find correct aoe knockdown for now =/

class UNDERBATAI : public CreatureAIScript
{
public:
	ADD_CREATURE_FACTORY_FUNCTION(UNDERBATAI);
    UNDERBATAI(Creature* pCreature) : CreatureAIScript(pCreature)
    {
		nrspells = 1;
		m_spellcheck = new bool[nrspells];
		spells = new SP_AI_Spell[nrspells];
		for(int i=0;i<nrspells;i++)
		{
			m_spellcheck[i] = false;
		}

		spells[0].info = sSpellStore.LookupEntry(KNOCKDOWN);
		spells[0].targettype = TARGET_VARIOUS;
		spells[0].instant = true;
		spells[0].perctrigger = 10.0f;
		spells[0].attackstoptimer = 1000;

    }
    
    void OnCombatStart(Unit* mTarget)
    {
        RegisterAIUpdateEvent(_unit->GetUInt32Value(UNIT_FIELD_BASEATTACKTIME));
    }

    void OnCombatStop(Unit *mTarget)
    {
        _unit->GetAIInterface()->setCurrentAgent(AGENT_NULL);
        _unit->GetAIInterface()->SetAIState(STATE_IDLE);
        RemoveAIUpdateEvent();
    }

    void OnDied(Unit * mKiller)
    {
       RemoveAIUpdateEvent();
	   delete[] spells;
	   delete[] m_spellcheck;
	   spells = NULL;
	   m_spellcheck = NULL;
    }

    void AIUpdate()
    {
		float val = sRand.rand(100.0f);
        SpellCast(val);
    }

    void SpellCast(float val)
    {
        if(m_spellcheck && spells && _unit->GetCurrentSpell() == NULL && _unit->GetAIInterface()->GetNextTarget())
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

    bool *m_spellcheck;
    SP_AI_Spell *spells;
	int nrspells;
};

/*****************************/
/*                           */
/*         Boss AIs          */
/*                           */
/*****************************/

// HungarfenAI

#define CN_HUNGARFEN 17770

#define UNDERBOG_MUSHROOM 34588 // No idea if this is right spell, but should be correct (also aditional core support needed!)
#define FOUL_SPORES 31673 //31697 // this one needs additional core support too

class HUNGARFENAI : public CreatureAIScript
{
public:
    ADD_CREATURE_FACTORY_FUNCTION(HUNGARFENAI);
    HUNGARFENAI(Creature* pCreature) : CreatureAIScript(pCreature)
    {
		UNDERBOG_MUSHROOM_Cooldown = 15;
		nrspells = 2;
		m_spellcheck = new bool[nrspells];
		spells = new SP_AI_Spell[nrspells];
		for(int i=0;i<nrspells;i++)
		{
			m_spellcheck[i] = false;
		}
        spells[0].info = sSpellStore.LookupEntry(UNDERBOG_MUSHROOM);
		spells[0].targettype = TARGET_ATTACKING; // should be random location
		spells[0].instant = false;
		spells[0].perctrigger = 0.0f;
		spells[0].attackstoptimer = 1000;

        spells[1].info = sSpellStore.LookupEntry(FOUL_SPORES);
		spells[1].targettype = TARGET_DESTINATION; // target checks needed when it will be fully usable
		spells[1].instant = false;   // needs additional checks as it's complex spell
		spells[1].perctrigger = 0.0f;
		spells[1].attackstoptimer = 1000;

    }
    
    void OnCombatStart(Unit* mTarget)
    {
		UNDERBOG_MUSHROOM_Cooldown = 15;
		RegisterAIUpdateEvent(_unit->GetUInt32Value(UNIT_FIELD_BASEATTACKTIME));
    }

	void OnTargetDied(Unit* mTarget)
    {
    }

    void OnCombatStop(Unit *mTarget)
    {
		UNDERBOG_MUSHROOM_Cooldown = 15;
        _unit->GetAIInterface()->setCurrentAgent(AGENT_NULL);
        _unit->GetAIInterface()->SetAIState(STATE_IDLE);
        RemoveAIUpdateEvent();
    }

    void OnDied(Unit * mKiller)
    {
		UNDERBOG_MUSHROOM_Cooldown = 15;
       RemoveAIUpdateEvent();
	   delete[] spells;
	   delete[] m_spellcheck;
	   spells = NULL;
	   m_spellcheck = NULL;
    }

    void AIUpdate()
    {
		UNDERBOG_MUSHROOM_Cooldown--;
		if (!UNDERBOG_MUSHROOM_Cooldown)
		{
			_unit->CastSpell(_unit,spells[0].info, spells[0].instant);
			UNDERBOG_MUSHROOM_Cooldown = 15;
		}
		if(_unit->GetHealthPct() <= 20 && FOUL_SPORES_LIMITER == 0)
		{
			FOUL_SPORES_LIMITER = 1; // Added to prevent situation when health jumps from 21 to 19 % and enrage is never activated
			_unit->CastSpell(_unit, spells[1].info, spells[1].instant);
		}
		else
		{
			float val = sRand.rand(100.0f);
			SpellCast(val);
		}
    }

    void SpellCast(float val)
    {
        if(m_spellcheck && spells && _unit->GetCurrentSpell() == NULL && _unit->GetAIInterface()->GetNextTarget())
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

	int UNDERBOG_MUSHROOM_Cooldown;
	int FOUL_SPORES_LIMITER;
    bool *m_spellcheck;
    SP_AI_Spell *spells;
	int nrspells;
};


// Ghaz'anAI

#define CN_GHAZAN 18105

#define ACID_SPIT 34290
#define TAIL_SWEEP 34267
#define ACID_BREATH 24839 //34268 <-- not sure
#define ENRAGE 34409 // Not sure to id as always in Enrage case: 34409, 34970

class GHAZANAI : public CreatureAIScript
{
public:
    ADD_CREATURE_FACTORY_FUNCTION(GHAZANAI);
    GHAZANAI(Creature* pCreature) : CreatureAIScript(pCreature)
    {
		ENRAGE_LIMITER = 0;
		nrspells = 4;
		m_spellcheck = new bool[nrspells];
		spells = new SP_AI_Spell[nrspells];
		for(int i=0;i<nrspells;i++)
		{
			m_spellcheck[i] = false;

		} 
        spells[0].info = sSpellStore.LookupEntry(ACID_SPIT);
		spells[0].targettype = TARGET_ATTACKING; // needs checks
		spells[0].instant = true;
		spells[0].perctrigger = 12.0f;
		spells[0].attackstoptimer = 1000;

        spells[1].info = sSpellStore.LookupEntry(TAIL_SWEEP);
		spells[1].targettype = TARGET_VARIOUS;
		spells[1].instant = true;
		spells[1].perctrigger = 10.0f;
		spells[1].attackstoptimer = 1000;

        spells[2].info = sSpellStore.LookupEntry(ACID_BREATH);
		spells[2].targettype = TARGET_VARIOUS;
		spells[2].instant = true;
		spells[2].perctrigger = 12.0f;
		spells[2].attackstoptimer = 1000;

        spells[3].info = sSpellStore.LookupEntry(ENRAGE);
		spells[3].targettype = TARGET_SELF;
		spells[3].instant = true;
		spells[3].perctrigger = 0.0f;
		spells[3].attackstoptimer = 1000;
    }
    
    void OnCombatStart(Unit* mTarget)
    {
		ENRAGE_LIMITER = 0;
		RegisterAIUpdateEvent(_unit->GetUInt32Value(UNIT_FIELD_BASEATTACKTIME));
    }

	void OnTargetDied(Unit* mTarget)
    {
    }

    void OnCombatStop(Unit *mTarget)
    {
        _unit->GetAIInterface()->setCurrentAgent(AGENT_NULL);
        _unit->GetAIInterface()->SetAIState(STATE_IDLE);
        RemoveAIUpdateEvent();
    }

    void OnDied(Unit * mKiller)
    {
       RemoveAIUpdateEvent();
	   delete[] spells;
	   delete[] m_spellcheck;
	   spells = NULL;
	   m_spellcheck = NULL;
	   ENRAGE_LIMITER = 0;
    }

    void AIUpdate()
    {
		if(_unit->GetHealthPct() <= 20 && ENRAGE_LIMITER == 0)
		{
			ENRAGE_LIMITER = 1; // Added to prevent situation when health jumps from 21 to 19 % and enrage is never activated
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
        if(m_spellcheck && spells && _unit->GetCurrentSpell() == NULL && _unit->GetAIInterface()->GetNextTarget())
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

	int ENRAGE_LIMITER;
    bool *m_spellcheck;
    SP_AI_Spell *spells;
	int nrspells;
};


// Swamplord Musel'ekAI
// Needs additional core support and more work =/
#define CN_SWAMPLORD_MUSELEK 17826

//#define AUTO_SHOT
#define MULTI_SHOT 34879 // 38383, 38383, 
#define AIMED_SHOT 38370
//#define RAPTOR_STRIKE 
#define FREEZING_TRAP 41086 // <-- Ice trap/ couldn't find better spell for now (we can use: 31933, but it has to affect players on xx radius)
#define ECHOING_ROAR 31429 // Special Claw Attk, need to find better id if needed
// No idea for now how to force him to keep the distance and use his abilities
// (Multi, Aimed should be used only when he is quiet far from party)

class SWAMPLORDMUSELEKAI : public CreatureAIScript
{
public:
    ADD_CREATURE_FACTORY_FUNCTION(SWAMPLORDMUSELEKAI);
    SWAMPLORDMUSELEKAI(Creature* pCreature) : CreatureAIScript(pCreature)
    {
		nrspells = 4;
		m_spellcheck = new bool[nrspells];
		spells = new SP_AI_Spell[nrspells];
		for(int i=0;i<nrspells;i++)
		{
			m_spellcheck[i] = false;
		}
        spells[0].info = sSpellStore.LookupEntry(MULTI_SHOT);
		spells[0].targettype = TARGET_ATTACKING; // changed from VARIOUS to prevent crashes
		spells[0].instant = true;
		spells[0].perctrigger = 10.0f;
		spells[0].attackstoptimer = 1000;

        spells[1].info = sSpellStore.LookupEntry(AIMED_SHOT);
		spells[1].targettype = TARGET_ATTACKING;
		spells[1].instant = false;
		spells[1].perctrigger = 12.0f;
		spells[1].attackstoptimer = 1000;

        spells[2].info = sSpellStore.LookupEntry(FREEZING_TRAP);
		spells[2].targettype = TARGET_VARIOUS; // not sure to target type
		spells[2].instant = true;
		spells[2].perctrigger = 7.0f;
		spells[2].attackstoptimer = 1000;

        spells[3].info = sSpellStore.LookupEntry(ECHOING_ROAR);
		spells[3].targettype = TARGET_VARIOUS;
		spells[3].instant = true;
		spells[3].perctrigger = 7.0f;
		spells[3].attackstoptimer = 1000;
		spells[3].speech = "Beast, obey me! Kill them at once!";
		spells[3].soundid = 10383;
    }
    
    void OnCombatStart(Unit* mTarget)
    {
		int RandomSpeach;
		sRand.randInt(1000);
		RandomSpeach=rand()%3;
		switch (RandomSpeach)
		{
		case 0:
			_unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, "We fight to the death!");
			_unit->PlaySoundToSet(10384);
			break;
		case 1:
			_unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, "I will end this quickly!");
			_unit->PlaySoundToSet(10385);
			break;
		case 2:
			_unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, "<missing_text>!");
			_unit->PlaySoundToSet(10386);
			break;
		}
		RegisterAIUpdateEvent(_unit->GetUInt32Value(UNIT_FIELD_BASEATTACKTIME));
    }

	void OnTargetDied(Unit* mTarget)
    {
		if (_unit->GetHealthPct() > 0)	// Hack to prevent double yelling (OnDied and OnTargetDied when creature is dying)
		{
			int RandomSpeach;
			sRand.randInt(1000);
			RandomSpeach=rand()%2;
			switch (RandomSpeach)
			{
			case 0:
				_unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, "<missing_text>!");
				_unit->PlaySoundToSet(10387);
				break;
			case 1:
				_unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, "It is finished!");
				_unit->PlaySoundToSet(10388);
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
		_unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, "Will... done..."); // not sure
		_unit->PlaySoundToSet(10389);
       RemoveAIUpdateEvent();
	   delete[] spells;
	   delete[] m_spellcheck;
	   spells = NULL;
	   m_spellcheck = NULL;
    }

    void AIUpdate()
    {
		float val = sRand.rand(100.0f);
		SpellCast(val);
    }

    void SpellCast(float val)
    {
        if(m_spellcheck && spells && _unit->GetCurrentSpell() == NULL && _unit->GetAIInterface()->GetNextTarget())
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

    bool *m_spellcheck;
    SP_AI_Spell *spells;
	int nrspells;
};


// The Black StalkerAI

#define CN_THE_BLACK_STALKER 17882

#define CHAIN_LIGHTNING 39066 // 28167, 39066
#define LEVITATE 31704 // Not sure to id
#define STATIC_CHARGE 31715
#define SUMMON_SPORE_STRIDER 38755 // spawning adds only on Heroic! lack of core support =/

class THEBLACKSTALKERAI : public CreatureAIScript
{
public:
    ADD_CREATURE_FACTORY_FUNCTION(THEBLACKSTALKERAI);
    THEBLACKSTALKERAI(Creature* pCreature) : CreatureAIScript(pCreature)
    {
		nrspells = 4;
		m_spellcheck = new bool[nrspells];
		spells = new SP_AI_Spell[nrspells];
		for(int i=0;i<nrspells;i++)
		{
			m_spellcheck[i] = false;
		}
        spells[0].info = sSpellStore.LookupEntry(CHAIN_LIGHTNING);
		spells[0].targettype = TARGET_ATTACKING;
		spells[0].instant = true;
		spells[0].perctrigger = 7.0f;
		spells[0].attackstoptimer = 1000;

        spells[1].info = sSpellStore.LookupEntry(LEVITATE);
		spells[1].targettype = TARGET_ATTACKING;
		spells[1].instant = true;
		spells[1].perctrigger = 9.0f;
		spells[1].attackstoptimer = 1000;

        spells[2].info = sSpellStore.LookupEntry(STATIC_CHARGE);
		spells[2].targettype = TARGET_ATTACKING; //?
		spells[2].instant = true;
		spells[2].perctrigger = 15.0f;
		spells[2].attackstoptimer = 1000;

        spells[3].info = sSpellStore.LookupEntry(SUMMON_SPORE_STRIDER);
		spells[3].targettype = TARGET_SELF;
		spells[3].instant = true;
		spells[3].perctrigger = 8.0f;
		spells[3].attackstoptimer = 1000;
    }
    
    void OnCombatStart(Unit* mTarget)
    {
		RegisterAIUpdateEvent(_unit->GetUInt32Value(UNIT_FIELD_BASEATTACKTIME));
    }

	void OnTargetDied(Unit* mTarget)
    {
    }

    void OnCombatStop(Unit *mTarget)
    {
        _unit->GetAIInterface()->setCurrentAgent(AGENT_NULL);
        _unit->GetAIInterface()->SetAIState(STATE_IDLE);
        RemoveAIUpdateEvent();
    }

    void OnDied(Unit * mKiller)
    {
       RemoveAIUpdateEvent();
	   delete[] spells;
	   delete[] m_spellcheck;
	   spells = NULL;
	   m_spellcheck = NULL;
    }

    void AIUpdate()
    {
		float val = sRand.rand(100.0f);
		SpellCast(val);
    }

    void SpellCast(float val)
    {
        if(m_spellcheck && spells && _unit->GetCurrentSpell() == NULL && _unit->GetAIInterface()->GetNextTarget())
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

    bool *m_spellcheck;
    SP_AI_Spell *spells;
	int nrspells;
};


void SetupTheUnderbog(ScriptMgr * mgr)
{
    mgr->register_creature_script(CN_UNDERBAT, &UNDERBATAI::Create);
	mgr->register_creature_script(CN_HUNGARFEN, &HUNGARFENAI::Create);
	mgr->register_creature_script(CN_GHAZAN, &GHAZANAI::Create);
	mgr->register_creature_script(CN_SWAMPLORD_MUSELEK, &SWAMPLORDMUSELEKAI::Create);
	mgr->register_creature_script(CN_THE_BLACK_STALKER, &THEBLACKSTALKERAI::Create);
}
