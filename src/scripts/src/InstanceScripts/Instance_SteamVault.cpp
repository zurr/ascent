#include "StdAfx.h"
#include "Setup.h"

/************************************************************************/
/* Instance_SteamVault.cpp Script		                                */
/************************************************************************/

// Coilfang Engineer AI

#define CN_COILFANG_ENGINEER 17721

#define BOMB 40332 // AOE
#define NET 38338 // Should stay for 5 or 8 sec?

class COILFANGENGINEERAI : public CreatureAIScript
{
public:
	ADD_CREATURE_FACTORY_FUNCTION(COILFANGENGINEERAI);
    COILFANGENGINEERAI(Creature* pCreature) : CreatureAIScript(pCreature)
    {
		nrspells = 2;
		m_spellcheck = new bool[nrspells];
		spells = new SP_AI_Spell[nrspells];
		for(int i=0;i<nrspells;i++)
		{
			m_spellcheck[i] = false;
		}

		spells[0].info = sSpellStore.LookupEntry(BOMB);
		spells[0].targettype = TARGET_DESTINATION;
		spells[0].instant = false;
		spells[0].perctrigger = 15.0f;
		spells[0].attackstoptimer = 3000;

		spells[1].info = sSpellStore.LookupEntry(NET);
		spells[1].targettype = TARGET_ATTACKING;
		spells[1].instant = true;
		spells[1].perctrigger = 15.0f;
		spells[1].attackstoptimer = 1500; 

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


// Coilfang Oracle AI


#define CN_COILFANG_ORACLE 17803

#define FROST_SHOCK 34353
#define SONIC_BURST 39052 // Should make dmg? or maybe it shouldn't be added here?
#define HEAL 39378 // Hmm... no idea if this is correct id (and how much should heal)
// Also it should heal other nagas like etc.

class COILFANGORACLEAI : public CreatureAIScript
{
public:
	ADD_CREATURE_FACTORY_FUNCTION(COILFANGORACLEAI);
    COILFANGORACLEAI(Creature* pCreature) : CreatureAIScript(pCreature)
    {
		nrspells = 3;
		m_spellcheck = new bool[nrspells];
		spells = new SP_AI_Spell[nrspells];
		for(int i=0;i<nrspells;i++)
		{
			m_spellcheck[i] = false;
		}

		spells[0].info = sSpellStore.LookupEntry(FROST_SHOCK);
		spells[0].targettype = TARGET_ATTACKING;
		spells[0].instant = true;
		spells[0].perctrigger = 15.0f;
		spells[0].attackstoptimer = 1000;

        spells[1].info = sSpellStore.LookupEntry(SONIC_BURST);
		spells[1].targettype = TARGET_VARIOUS;
		spells[1].instant = true;
		spells[1].perctrigger = 18.0f;
		spells[1].attackstoptimer = 2000;

        spells[2].info = sSpellStore.LookupEntry(HEAL);
		spells[2].targettype = TARGET_SELF;
		spells[2].instant = true;
		spells[2].perctrigger = 2.0f;
		spells[2].attackstoptimer = 1000;

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


// Coilfang Warrior AI

#define CN_COILFANG_WARRIOR 17802  // at least, couldn't find any data to compare it with blizz

#define MORTAL_STRIKE 29572

class COILFANGWARRIORAI : public CreatureAIScript
{
public:
	ADD_CREATURE_FACTORY_FUNCTION(COILFANGWARRIORAI);
    COILFANGWARRIORAI(Creature* pCreature) : CreatureAIScript(pCreature)
    {
		nrspells = 1;
		m_spellcheck = new bool[nrspells];
		spells = new SP_AI_Spell[nrspells];
		for(int i=0;i<nrspells;i++)
		{
			m_spellcheck[i] = false;
		}

		spells[0].info = sSpellStore.LookupEntry(MORTAL_STRIKE);
		spells[0].targettype = TARGET_ATTACKING;
		spells[0].instant = false;
		spells[0].perctrigger = 20.0f;
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


// Coilfang Siren AI

#define CN_COILFANG_SIREN 17801

#define AOE_FEAR 30584 // Maybe should be: 33547 or 29321
#define LIGHTNING_BOLT 36152 // 1 target
#define MOONFIRE 20690 // not sure to id

class COILFANGSIRENAI : public CreatureAIScript
{
public:
	ADD_CREATURE_FACTORY_FUNCTION(COILFANGSIRENAI);
    COILFANGSIRENAI(Creature* pCreature) : CreatureAIScript(pCreature)
    {
		nrspells = 3;
		m_spellcheck = new bool[nrspells];
		spells = new SP_AI_Spell[nrspells];
		for(int i=0;i<nrspells;i++)
		{
			m_spellcheck[i] = false;
		}

		spells[0].info = sSpellStore.LookupEntry(AOE_FEAR);
		spells[0].targettype = TARGET_VARIOUS;
		spells[0].instant = true;
		spells[0].perctrigger = 5.0f;
		spells[0].attackstoptimer = 1000;

		spells[1].info = sSpellStore.LookupEntry(LIGHTNING_BOLT);
		spells[1].targettype = TARGET_ATTACKING;
		spells[1].instant = false;
		spells[1].perctrigger = 15.0f;
		spells[1].attackstoptimer = 3000;

		spells[2].info = sSpellStore.LookupEntry(MOONFIRE);
		spells[2].targettype = TARGET_ATTACKING;
		spells[2].instant = true;
		spells[2].perctrigger = 10.0f;
		spells[2].attackstoptimer = 3000;
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



// Bog OverlordAI

#define CN_BOG_OVERLORD 21694

#define FUNGAL_DECAY 32065
#define TRAMPLE 40340 // Should be used when enraged
#define ENRAGE_B 40683 // No idea if this is good spell id (used ENRAGE_B [B = Bog], because of redefining)
// Should be immunite on CC's and slowing down effects.
// Also should detect players in stealth mode.

class BOGOVERLORDAI : public CreatureAIScript
{
public:
	ADD_CREATURE_FACTORY_FUNCTION(BOGOVERLORDAI);
    BOGOVERLORDAI(Creature* pCreature) : CreatureAIScript(pCreature)
    {

		nrspells = 3;
		m_spellcheck = new bool[nrspells];
		spells = new SP_AI_Spell[nrspells];
		for(int i=0;i<nrspells;i++)
		{
			m_spellcheck[i] = false;
		}

		spells[0].info = sSpellStore.LookupEntry(FUNGAL_DECAY);
		spells[0].targettype = TARGET_ATTACKING;
		spells[0].instant = true;
		spells[0].perctrigger = 15.0f;
		spells[0].attackstoptimer = 1000;

		spells[1].info = sSpellStore.LookupEntry(TRAMPLE);
		spells[1].targettype = TARGET_ATTACKING;
		spells[1].instant = true;
		spells[1].perctrigger = 2.0f;
		spells[1].attackstoptimer = 1000;

		spells[2].info = sSpellStore.LookupEntry(ENRAGE_B);
		spells[2].targettype = TARGET_SELF;
		spells[2].instant = false;
		spells[2].perctrigger = 3.0f;
		spells[2].attackstoptimer = 1000;
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

// Coilfang Sorceress

#define CN_COILFANG_SORCERESS 17722

#define FROSTBOLT 39064
#define BLIZZARD 38646 // or maybe: 37263
#define FROST_NOVA 29849 // also can be: 30094 or 32365

class COILFANGSORCERESSAI : public CreatureAIScript
{
public:
    ADD_CREATURE_FACTORY_FUNCTION(COILFANGSORCERESSAI);
    COILFANGSORCERESSAI(Creature* pCreature) : CreatureAIScript(pCreature)
    {
		nrspells = 3;
		m_spellcheck = new bool[nrspells];
		spells = new SP_AI_Spell[nrspells];
		for(int i=0;i<nrspells;i++)
		{
			m_spellcheck[i] = false;
		}
		
        spells[0].info = sSpellStore.LookupEntry(FROSTBOLT);
		spells[0].targettype = TARGET_ATTACKING;
		spells[0].instant = false;
		spells[0].perctrigger = 15.0f;
		spells[0].attackstoptimer = 1000;

        spells[1].info = sSpellStore.LookupEntry(BLIZZARD);
		spells[1].targettype = TARGET_DESTINATION;
		spells[1].instant = true;
		spells[1].perctrigger = 10.0f;
		spells[1].attackstoptimer = 1000;  // Boss should channel this spell and stand in place, but it isn't
    
        spells[2].info = sSpellStore.LookupEntry(FROST_NOVA);
		spells[2].targettype = TARGET_VARIOUS;
		spells[2].instant = true;
		spells[2].perctrigger = 8.0f;
		spells[2].attackstoptimer = 1000; 
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

// Coilfang Leper 

#define CN_COILFANG_LEPER 21338 // Couldn't find source to compare it's spells

#define SHADOW_BOLT 39025

class COILFANGLEPERAI : public CreatureAIScript
{
public:
    ADD_CREATURE_FACTORY_FUNCTION(COILFANGLEPERAI);
    COILFANGLEPERAI(Creature* pCreature) : CreatureAIScript(pCreature)
    {
		nrspells = 1;
		m_spellcheck = new bool[nrspells];
		spells = new SP_AI_Spell[nrspells];
		for(int i=0;i<nrspells;i++)
		{
			m_spellcheck[i] = false;
		}
		
        spells[0].info = sSpellStore.LookupEntry(SHADOW_BOLT);
		spells[0].targettype = TARGET_ATTACKING;
		spells[0].instant = false;
		spells[0].perctrigger = 15.0f;
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

// Coilfang Slavemaster

#define CN_COILFANG_SLAVEMASTER 17805

#define GEYSER 40089 // It don't have to be good spell id [can be: 10987, 37478, 40089, 37427] - no idea why it doesn't knock you out
// Slaves should run after killing slavemaster

class COILFANGSLAVEMASTERAI : public CreatureAIScript
{
public:
    ADD_CREATURE_FACTORY_FUNCTION(COILFANGSLAVEMASTERAI);
    COILFANGSLAVEMASTERAI(Creature* pCreature) : CreatureAIScript(pCreature)
    {
		nrspells = 1;
		m_spellcheck = new bool[nrspells];
		spells = new SP_AI_Spell[nrspells];
		for(int i=0;i<nrspells;i++)
		{
			m_spellcheck[i] = false;
		}
		
        spells[0].info = sSpellStore.LookupEntry(GEYSER);
		spells[0].targettype = TARGET_ATTACKING; // Need to check targettype. (should be random)
		spells[0].instant = true;		
		spells[0].perctrigger = 15.0f;
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

// Coilfang water elemental

#define CN_COILFANG_WATER_ELEMENTAL 17917

#define FROSTBOLT_VOLLEY 36741

class COILFANGWATERELEMENTALAI : public CreatureAIScript
{
public:
    ADD_CREATURE_FACTORY_FUNCTION(COILFANGWATERELEMENTALAI);
    COILFANGWATERELEMENTALAI(Creature* pCreature) : CreatureAIScript(pCreature)
    {
		nrspells = 1;
		m_spellcheck = new bool[nrspells];
		spells = new SP_AI_Spell[nrspells];
		for(int i=0;i<nrspells;i++)
		{
			m_spellcheck[i] = false;
		}
		
        spells[0].info = sSpellStore.LookupEntry(FROSTBOLT_VOLLEY);
		spells[0].targettype = TARGET_ATTACKING;
		spells[0].instant = true;		// Should be false, but doesn't work then
		spells[0].perctrigger = 15.0f;
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

// Coilfang MyrmidonAI

#define CN_COILFANG_MYRMIDON 17800

#define SWEEPING_STRIKES 35429
#define CLEAVE 38260 // no idea about it's id
#define EXECUTE 38959 // should activate only on target with <= 25% hp // not sure about it

class COILFANGMYRMIDONAI : public CreatureAIScript
{
public:
	ADD_CREATURE_FACTORY_FUNCTION(COILFANGMYRMIDONAI);
    COILFANGMYRMIDONAI(Creature* pCreature) : CreatureAIScript(pCreature)
    {
		nrspells = 3;
		m_spellcheck = new bool[nrspells];
		spells = new SP_AI_Spell[nrspells];
		for(int i=0;i<nrspells;i++)
		{
			m_spellcheck[i] = false;
		}

		spells[0].info = sSpellStore.LookupEntry(SWEEPING_STRIKES);
		spells[0].targettype = TARGET_SELF;
		spells[0].instant = true;
		spells[0].perctrigger = 10.0f;
		spells[0].attackstoptimer = 1000;

		spells[1].info = sSpellStore.LookupEntry(CLEAVE);
		spells[1].targettype = TARGET_ATTACKING; // needs target checking
		spells[1].instant = true;
		spells[1].perctrigger = 15.0f;
		spells[1].attackstoptimer = 1000;

		spells[2].info = sSpellStore.LookupEntry(EXECUTE);
		spells[2].targettype = TARGET_ATTACKING; // target checking needed
		spells[2].instant = true;
		spells[2].perctrigger = 10.0f;
		spells[2].attackstoptimer = 1000;
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

// Tidal SurgerAI

#define CN_TIDAL_SURGER 21695

#define KNOCKBACK 30056
// Should has immunite to frost spells (as adds) + should has 
// 3-4 minons (Idk if they should be spawned by script)

class TIDALSURGERAI : public CreatureAIScript
{
public:
	ADD_CREATURE_FACTORY_FUNCTION(TIDALSURGERAI);
    TIDALSURGERAI(Creature* pCreature) : CreatureAIScript(pCreature)
    {
		nrspells = 1;
		m_spellcheck = new bool[nrspells];
		spells = new SP_AI_Spell[nrspells];
		for(int i=0;i<nrspells;i++)
		{
			m_spellcheck[i] = false;
		}

		spells[0].info = sSpellStore.LookupEntry(KNOCKBACK);
		spells[0].targettype = TARGET_ATTACKING; // should be random
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

struct Coords
{
    float x;
    float y;
    float z;
    float o;
};

//Mekgineer Steamrigger

static Coords spawnCoords[] =
{
    { -300.037842, -115.296227, -7.865229, 4.197916 },
    { -330.083008, -121.505997, -7.985120, 5.061450 },
    { -346.530273, -147.167892, -6.703687, 0.010135 }
};

#define CN_MEKGINEER_STEAMRIGGER 17796
#define CN_STEAMRIGGER_MECHANIC 17951

#define SUPER_SHRINK_RAY 31485
#define SAW_BLADE 31486 // Or this: 35318
#define ELECTRIFIED_NET 35107
#define ENRAGE 41447  // No idea if these are good ids

// Must spawn 3 Steamrigger Mechanics when his health is on 75%, 50% and 25%
class MEKGINEERSTEAMRIGGERAI : public CreatureAIScript
{
public:
    ADD_CREATURE_FACTORY_FUNCTION(MEKGINEERSTEAMRIGGERAI);
    MEKGINEERSTEAMRIGGERAI(Creature* pCreature) : CreatureAIScript(pCreature)
    {
		nrspells = 4;
		m_spellcheck = new bool[nrspells];
		spells = new SP_AI_Spell[nrspells];
		for(int i=0;i<nrspells;i++)
		{
			m_spellcheck[i] = false;
		}
        spells[0].info = sSpellStore.LookupEntry(SUPER_SHRINK_RAY);
		spells[0].targettype = TARGET_ATTACKING;
		spells[0].instant = true;
		spells[0].perctrigger = 7.0f;
		spells[0].attackstoptimer = 1000;

		spells[1].info = sSpellStore.LookupEntry(SAW_BLADE);
		spells[1].targettype = TARGET_ATTACKING; // when killed with VARIOUS (because with that caster attacks also himself) server crashes
		spells[1].instant = true;
		spells[1].perctrigger = 15.0f;
		spells[1].attackstoptimer = 1000;

		spells[2].info = sSpellStore.LookupEntry(ELECTRIFIED_NET);
		spells[2].targettype = TARGET_ATTACKING;
		spells[2].instant = true;
		spells[2].perctrigger = 18.0f;
		spells[2].attackstoptimer = 1000;

		spells[3].info = sSpellStore.LookupEntry(ENRAGE);
		spells[3].targettype = TARGET_SELF;
		spells[3].instant = true;
		spells[3].perctrigger = 0.0f;
		spells[3].attackstoptimer = 1000;
		spells[3].soundid = 10374;
		spells[3].speech = "Have fun dyin', cupcake!"; //cupcake?
    }
    
    void OnCombatStart(Unit* mTarget)
    {
		Timer = 0;
		int RandomSpeach;
		sRand.randInt(1000);
		RandomSpeach=rand()%4;
		switch (RandomSpeach)
		{
		case 0:
			_unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, "Your in for a World-O-Hurt !");
			_unit->PlaySoundToSet(10368);
			break;
		case 1:
			_unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, "Eat hot metal! Scumbag!");
			_unit->PlaySoundToSet(10369);
			break;
		case 2:
			_unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, "I'll come over there!"); // really?
			_unit->PlaySoundToSet(10370);
			break;
		case 3:
			_unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, "I'm bringin' the pain!"); // really2?
			_unit->PlaySoundToSet(10371);
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
			RandomSpeach=rand()%3;
			switch (RandomSpeach)
			{
			case 0:
				_unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, "You just got served, Punk!");
				_unit->PlaySoundToSet(10372);
				break;
			case 1:
				_unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, "I own you!");
				_unit->PlaySoundToSet(10373);
				break;
			case 2:
				_unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, "Have fun dying, capcake!"); // capcake?
				_unit->PlaySoundToSet(10374);
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
	   _unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, "Mommy");
       _unit->PlaySoundToSet(10375);
       RemoveAIUpdateEvent();
	   delete[] spells;
	   delete[] m_spellcheck;
	   spells = NULL;
	   m_spellcheck = NULL;
    }

    void AIUpdate()
    {
		Timer = Timer + 1;

		if (Timer == 100)
		{
			_unit->CastSpell(_unit, spells[3].info, spells[3].instant);
		}

		if(_unit->GetHealthPct() == 25 || _unit->GetHealthPct() == 50 || _unit->GetHealthPct() == 75)
        {
            Creature *cre = NULL;
            for(int i = 0; i < 3; i++)
            {
                cre = _unit->GetMapMgr()->GetInterface()->SpawnCreature(CN_STEAMRIGGER_MECHANIC, 
                    spawnCoords[i].x, spawnCoords[i].y,
                    spawnCoords[i].z, spawnCoords[i].o,
                    true, false, _unit->GetUInt32Value(UNIT_FIELD_FACTIONTEMPLATE), 50); //63
                if(cre)
                {
                    //cre->GetAIInterface()->MoveTo(14.161, -177.874, -85.649, 0.23);
                    cre->GetAIInterface()->setOutOfCombatRange(100000);
                }
                cre = _unit->GetMapMgr()->GetInterface()->SpawnCreature(CN_STEAMRIGGER_MECHANIC, 
                    spawnCoords[2-i].x, spawnCoords[2-i].y,
                    spawnCoords[2-i].z, spawnCoords[2-i].o,
                    true, false, _unit->GetUInt32Value(UNIT_FIELD_FACTIONTEMPLATE), 63);
                if(cre)
                {
                    //cre->GetAIInterface()->MoveTo(27.133, -232.030, -84.188, 0.44);
                    cre->GetAIInterface()->setOutOfCombatRange(100000);
                }
            }
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

	int Timer;
    bool *m_spellcheck;
    SP_AI_Spell *spells;
	int nrspells;
};


// Hydromancer ThespiaAI

#define CN_HYDROMANCER_THESPIA 17797

#define ENVELOPING_WINDS 31718 // 1 target
#define LIGHTNING_CLOUD 25033 // various targets on not attacked.
#define LUNG_BURST 31481


class HYDROMANCERTHESPIAAI : public CreatureAIScript
{
public:
    ADD_CREATURE_FACTORY_FUNCTION(HYDROMANCERTHESPIAAI);
    HYDROMANCERTHESPIAAI(Creature* pCreature) : CreatureAIScript(pCreature)
    {
		nrspells = 3;
		m_spellcheck = new bool[nrspells];
		spells = new SP_AI_Spell[nrspells];
		for(int i=0;i<nrspells;i++)
		{
			m_spellcheck[i] = false;

		} 
        spells[0].info = sSpellStore.LookupEntry(ENVELOPING_WINDS);
		spells[0].targettype = TARGET_ATTACKING;
		spells[0].instant = true;
		spells[0].perctrigger = 5.0f;
		spells[0].attackstoptimer = 2500;

		spells[1].info = sSpellStore.LookupEntry(LIGHTNING_CLOUD);
		spells[1].targettype = TARGET_DESTINATION;
		spells[1].instant = true;				
		spells[1].perctrigger = 8.0f;
		spells[1].attackstoptimer = 1500;

		spells[2].info = sSpellStore.LookupEntry(LUNG_BURST);
		spells[2].targettype = TARGET_ATTACKING;
		spells[2].instant = true;
		spells[2].perctrigger = 6.0f;
		spells[2].attackstoptimer = 1000;
    }
    
    void OnCombatStart(Unit* mTarget)
    {
		int RandomSpeach;
		sRand.randInt(1000);
		RandomSpeach=rand()%3;
		switch (RandomSpeach)
		{
		case 0:
			_unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, "The depths will consume you!");
			_unit->PlaySoundToSet(10361);
			break;
		case 1:
			_unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, "Meet your doom, surface dwellers!");
			_unit->PlaySoundToSet(10362);
			break;
		case 2:
			_unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, "You will drown in blood!");
			_unit->PlaySoundToSet(10363);
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
				_unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, "To the depths of oblivion with you!");
				_unit->PlaySoundToSet(10364);
				break;
			case 1:
				_unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, "For my Lady and Master.");
				_unit->PlaySoundToSet(10365);
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
		_unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, "Our matron will be ... The end of you");
        _unit->PlaySoundToSet(10366);
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



// Warlord Kalitresh AI

#define CN_WARLORD_KALITRESH 17798

#define IMPALE 24049 // 1 target Correct spell id? Maybe 34451 or ... correct
#define HEAD_CRACK 16172
#define WARLORDS_RAGE 36453
#define SPELL_REFLECTION 33961
// eventually more spells to add , nop that's it

class WARLORDKALITRESHAI : public CreatureAIScript
{
public:
    ADD_CREATURE_FACTORY_FUNCTION(WARLORDKALITRESHAI);
    WARLORDKALITRESHAI(Creature* pCreature) : CreatureAIScript(pCreature)
    {
		nrspells = 4;
		m_spellcheck = new bool[nrspells];
		spells = new SP_AI_Spell[nrspells];
		for(int i=0;i<nrspells;i++)
		{
			m_spellcheck[i] = false;
		}
        spells[0].info = sSpellStore.LookupEntry(IMPALE);
		spells[0].targettype = TARGET_ATTACKING;
		spells[0].instant = false;
		spells[0].perctrigger = 20.0f;
		spells[0].attackstoptimer = 1000;

		spells[1].info = sSpellStore.LookupEntry(HEAD_CRACK);
		spells[1].targettype = TARGET_ATTACKING;
		spells[1].instant = false;
		spells[1].perctrigger = 10.0f;
		spells[1].attackstoptimer = 2000;

		spells[2].info = sSpellStore.LookupEntry(WARLORDS_RAGE);
		spells[2].targettype = TARGET_SELF;
		spells[2].instant = true;
		spells[2].perctrigger = 2.0f;
		spells[2].attackstoptimer = 1000;
		spells[2].soundid = 10391;
		spells[2].speech = "This is not nearly over!";

		spells[3].info = sSpellStore.LookupEntry(SPELL_REFLECTION);
		spells[3].targettype = TARGET_SELF;
		spells[3].instant = true;
		spells[3].perctrigger = 8.0f;
		spells[3].attackstoptimer = 1000;

	} 

    void OnCombatStart(Unit* mTarget)
    {
		int RandomSpeach;
		sRand.randInt(1000);
		RandomSpeach=rand()%3;
		switch (RandomSpeach)
		{
		case 0: 
			_unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, "Your head will roll!");
			_unit->PlaySoundToSet(10392);
			break;
		case 1:
			_unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, "I despise all of your kind!");
			_unit->PlaySoundToSet(10393);
			break;
		case 2:
			/* Maybe an emote for "strange words"? */
			_unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, "No freaking idea :P");
			_unit->PlaySoundToSet(10394);
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
				_unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, "Squirm , Surface filth!");
				_unit->PlaySoundToSet(10395);
				break;
			case 1:
				_unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, "Hahahahah");
				_unit->PlaySoundToSet(10366);
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
		_unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, "For her exelency , For Vash...");
        _unit->PlaySoundToSet(10397);
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

void SetupSteamVault(ScriptMgr * mgr)
{
    mgr->register_creature_script(CN_COILFANG_ORACLE, &COILFANGORACLEAI::Create);
    mgr->register_creature_script(CN_COILFANG_ENGINEER, &COILFANGENGINEERAI::Create);
    mgr->register_creature_script(CN_COILFANG_WARRIOR, &COILFANGWARRIORAI::Create);
	mgr->register_creature_script(CN_COILFANG_SIREN, &COILFANGSIRENAI::Create);
	mgr->register_creature_script(CN_HYDROMANCER_THESPIA, &HYDROMANCERTHESPIAAI::Create);
	mgr->register_creature_script(CN_WARLORD_KALITRESH, &WARLORDKALITRESHAI::Create);
	mgr->register_creature_script(CN_BOG_OVERLORD, &BOGOVERLORDAI::Create);
	mgr->register_creature_script(CN_COILFANG_WATER_ELEMENTAL, &COILFANGWATERELEMENTALAI::Create);
	mgr->register_creature_script(CN_MEKGINEER_STEAMRIGGER, &MEKGINEERSTEAMRIGGERAI::Create);
	mgr->register_creature_script(CN_COILFANG_SORCERESS, &COILFANGSORCERESSAI::Create);
	mgr->register_creature_script(CN_COILFANG_LEPER, &COILFANGLEPERAI::Create);
	mgr->register_creature_script(CN_COILFANG_SLAVEMASTER, &COILFANGSLAVEMASTERAI::Create);
	mgr->register_creature_script(CN_COILFANG_MYRMIDON, &COILFANGMYRMIDONAI::Create);
	mgr->register_creature_script(CN_TIDAL_SURGER, &TIDALSURGERAI::Create);
}


/* TO DO: Check all spells/creatures and evenatually balance them (if needed!)
   also add spawns correctly (no core support for now and hacky Onyxia way does
   not work (in Onyxia also). Change target type when there will be more core
   support. Also very imporant thing is to force spawns to heal random or target
   you choosed. When that will work, I will add AI for mechanics who should
   heal one of instance bosses.*/
