#include "StdAfx.h"
#include "Setup.h"

/************************************************************************/
/* Instance_Naxxramas.cpp Script                                    */
/************************************************************************/

// ---- Spider Wing ----

// Carrion Spinner AI

#define CN_CARRION_SPINNER 15975

#define POISON_BOLT 26052
#define WEB_WRAP 28618 // TODO: PULL EFFECT *FUN*

class CarrionSpinnerAI : public CreatureAIScript
{
public:
	ADD_CREATURE_FACTORY_FUNCTION(CarrionSpinnerAI);
    CarrionSpinnerAI(Creature* pCreature) : CreatureAIScript(pCreature)
    {
		// -- Number of spells to add --
		nrspells = 2;

		// --- Initialization ---
		m_spellcheck = new bool[nrspells];
		spells = new SP_AI_Spell[nrspells];
		for(int i=0;i<nrspells;i++)
		{
			m_spellcheck[i] = false;
		}
		// ----------------------
		
		// Create basic info for spells here, and play with it later , fill always the info, targettype and if is instant or not! 
        spells[0].info = sSpellStore.LookupEntry(POISON_BOLT);
		spells[0].targettype = TARGET_ATTACKING;
		spells[0].instant = false;
		spells[0].perctrigger = 15.0f;
		spells[0].attackstoptimer = 1000; // 1sec

		spells[1].info = sSpellStore.LookupEntry(WEB_WRAP);
		spells[1].targettype = TARGET_ATTACKING;
		spells[1].instant = false;
		spells[1].perctrigger = 4.0f;
		spells[1].attackstoptimer = 2000; // 2sec
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


// Dread Creeper AI

#define CN_DREAD_CREEPER 15974

#define VEIL_OF_SHADOW 28440

class DreadCreeperAI : public CreatureAIScript
{
public:
	ADD_CREATURE_FACTORY_FUNCTION(DreadCreeperAI);
    DreadCreeperAI(Creature* pCreature) : CreatureAIScript(pCreature)
    {
		// -- Number of spells to add --
		nrspells = 1;

		// --- Initialization ---
		m_spellcheck = new bool[nrspells];
		spells = new SP_AI_Spell[nrspells];
		for(int i=0;i<nrspells;i++)
		{
			m_spellcheck[i] = false;
		}
		// ----------------------
		
		// Create basic info for spells here, and play with it later , fill always the info, targettype and if is instant or not! 
        spells[0].info = sSpellStore.LookupEntry(VEIL_OF_SHADOW);
		spells[0].targettype = TARGET_VARIOUS;
		spells[0].instant = false;
		spells[0].perctrigger = 10.0f;
		spells[0].attackstoptimer = 1000; // 1sec

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



// Naxxramas Cultist AI

#define CN_NAXX_CULTIST 15980

#define KNOCKBACK 10689

class NaxxCultistAI : public CreatureAIScript
{
public:
	ADD_CREATURE_FACTORY_FUNCTION(NaxxCultistAI);
    NaxxCultistAI(Creature* pCreature) : CreatureAIScript(pCreature)
    {
		// -- Number of spells to add --
		nrspells = 1;

		// --- Initialization ---
		m_spellcheck = new bool[nrspells];
		spells = new SP_AI_Spell[nrspells];
		for(int i=0;i<nrspells;i++)
		{
			m_spellcheck[i] = false;
		}
		// ----------------------
		
		// Create basic info for spells here, and play with it later , fill always the info, targettype and if is instant or not! 
        spells[0].info = sSpellStore.LookupEntry(KNOCKBACK);
		spells[0].targettype = TARGET_ATTACKING;
		spells[0].instant = false;
		spells[0].perctrigger = 10.0f;
		spells[0].attackstoptimer = 1000; // 1sec

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



// Necro Stalker AI - Venom Stalker as same ai

#define CN_NECRO_STALKER 16453
#define VENOM_STALKER 15976

#define POISON_CHARGE 28431

class NecroStalkerAI : public CreatureAIScript
{
public:
	ADD_CREATURE_FACTORY_FUNCTION(NecroStalkerAI);
    NecroStalkerAI(Creature* pCreature) : CreatureAIScript(pCreature)
    {
		// -- Number of spells to add --
		nrspells = 1;

		// --- Initialization ---
		m_spellcheck = new bool[nrspells];
		spells = new SP_AI_Spell[nrspells];
		for(int i=0;i<nrspells;i++)
		{
			m_spellcheck[i] = false;
		}
		// ----------------------
		
		// Create basic info for spells here, and play with it later , fill always the info, targettype and if is instant or not! 
        spells[0].info = sSpellStore.LookupEntry(POISON_CHARGE);
		spells[0].targettype = TARGET_ATTACKING;
		spells[0].instant = false;
		spells[0].perctrigger = 10.0f;
		spells[0].attackstoptimer = 1000; // 1sec

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

// Tomb Horror AI

#define CN_TOMB_HORROR 15979

#define SPIKE_VOLLEY 28615

class TombHorrorAI : public CreatureAIScript
{
public:
	ADD_CREATURE_FACTORY_FUNCTION(TombHorrorAI);
    TombHorrorAI(Creature* pCreature) : CreatureAIScript(pCreature)
    {
		// -- Number of spells to add --
		nrspells = 1;

		// --- Initialization ---
		m_spellcheck = new bool[nrspells];
		spells = new SP_AI_Spell[nrspells];
		for(int i=0;i<nrspells;i++)
		{
			m_spellcheck[i] = false;
		}
		// ----------------------
		
		// Create basic info for spells here, and play with it later , fill always the info, targettype and if is instant or not! 
        spells[0].info = sSpellStore.LookupEntry(SPIKE_VOLLEY);
		spells[0].targettype = TARGET_VARIOUS;
		spells[0].instant = false;
		spells[0].perctrigger = 10.0f;
		spells[0].attackstoptimer = 1000; // 1sec

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



// Necropolis Acolyte AI

#define CN_NECRO_ACOLYTE 16368

#define SHADOWBOLT_VOLLEY 27831
#define ARCANE_EXPLOSION 26643

class NecroAcolyteAI : public CreatureAIScript
{
public:
	ADD_CREATURE_FACTORY_FUNCTION(NecroAcolyteAI);
    NecroAcolyteAI(Creature* pCreature) : CreatureAIScript(pCreature)
    {
		// -- Number of spells to add --
		nrspells = 2;

		// --- Initialization ---
		m_spellcheck = new bool[nrspells];
		spells = new SP_AI_Spell[nrspells];
		for(int i=0;i<nrspells;i++)
		{
			m_spellcheck[i] = false;
		}
		// ----------------------
		
		// Create basic info for spells here, and play with it later , fill always the info, targettype and if is instant or not! 
        spells[0].info = sSpellStore.LookupEntry(SHADOWBOLT_VOLLEY);
		spells[0].targettype = TARGET_VARIOUS;
		spells[0].instant = true;
		spells[0].perctrigger = 10.0f;
		spells[0].attackstoptimer = 4000; // 1sec

		spells[1].info = sSpellStore.LookupEntry(ARCANE_EXPLOSION);
		spells[1].targettype = TARGET_VARIOUS;
		spells[1].instant = true;
		spells[1].perctrigger = 10.0f;
		spells[1].attackstoptimer = 1000; // 1sec

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



// Maexxna AI *BOSS*
#define CN_MAEXXNA 15952

#define WEB_WRAP_MAEXX 28619 // 1 target
#define WEB_SPRAY 29484 // various
#define POISON_SHOCK 28741 // various
#define NECROTIC_POISON 28776 // 1 target


class MaexxnaAI : public CreatureAIScript
{
public:
	ADD_CREATURE_FACTORY_FUNCTION(MaexxnaAI);
    MaexxnaAI(Creature* pCreature) : CreatureAIScript(pCreature)
    {
		// -- Number of spells to add --
		nrspells = 4;

		// --- Initialization ---
		m_spellcheck = new bool[nrspells];
		spells = new SP_AI_Spell[nrspells];
		for(int i=0;i<nrspells;i++)
		{
			m_spellcheck[i] = false;
		}
		// ----------------------
		
		// Create basic info for spells here, and play with it later , fill always the info, targettype and if is instant or not! 
        spells[0].info = sSpellStore.LookupEntry(WEB_WRAP_MAEXX);
		spells[0].targettype = TARGET_ATTACKING;
		spells[0].instant = false;
		spells[0].perctrigger = 3.0f;
		spells[0].attackstoptimer = 4000; // 1sec

		spells[1].info = sSpellStore.LookupEntry(WEB_SPRAY);
		spells[1].targettype = TARGET_VARIOUS;
		spells[1].instant = false;
		spells[1].perctrigger = 2.0f;
		spells[1].attackstoptimer = 1000; // 1sec

		spells[2].info = sSpellStore.LookupEntry(POISON_SHOCK);
		spells[2].targettype = TARGET_VARIOUS;
		spells[2].instant = false;
		spells[2].perctrigger = 5.0f;
		spells[2].attackstoptimer = 1000; // 1sec

		spells[3].info = sSpellStore.LookupEntry(NECROTIC_POISON);
		spells[3].targettype = TARGET_ATTACKING;
		spells[3].instant = false;
		spells[3].perctrigger = 10.0f;
		spells[3].attackstoptimer = 1000; // 1sec

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

// Grand Widow Faerlina AI

// 8794,'A_FAER_NAXX_AGGRO01'
// 8795,'A_FAER_NAXX_AGGRO02'
// 8796,'A_FAER_NAXX_AGGRO03'
// 8797,'A_FAER_NAXX_AGGRO04'
// 8798,'A_FAER_NAXX_DEATH'
// 8799,'A_FAER_NAXX_GREET'
// 8800,'A_FAER_NAXX_SLAY01'
// 8801,'A_FAER_NAXX_SLAY02'

#define CN_WIDOW_FAERLINA 15953

#define POISON_BOLT_VOLLEY 25991 // various
#define RAIN_OF_FIRE 38635 // dest

class WidowFaerlinaAI : public CreatureAIScript
{
public:
	ADD_CREATURE_FACTORY_FUNCTION(WidowFaerlinaAI);
    WidowFaerlinaAI(Creature* pCreature) : CreatureAIScript(pCreature)
    {
		// -- Number of spells to add --
		nrspells = 2;

		// --- Initialization ---
		m_spellcheck = new bool[nrspells];
		spells = new SP_AI_Spell[nrspells];
		for(int i=0;i<nrspells;i++)
		{
			m_spellcheck[i] = false;
		}
		// ----------------------
		
		// Create basic info for spells here, and play with it later , fill always the info, targettype and if is instant or not! 
        spells[0].info = sSpellStore.LookupEntry(POISON_BOLT_VOLLEY);
		spells[0].targettype = TARGET_VARIOUS;
		spells[0].instant = false;
		spells[0].perctrigger = 10.0f;
		spells[0].attackstoptimer = 1000; // 1sec

		spells[1].info = sSpellStore.LookupEntry(RAIN_OF_FIRE);
		spells[1].targettype = TARGET_DESTINATION;
		spells[1].instant = false;
		spells[1].perctrigger = 5.0f;
		spells[1].attackstoptimer = 7000; // 1sec

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

// Anub'Rekhan
#define CN_ANUB 15956

// 8785,'A_ANU_NAXX_AGGRO01'
// 8786,'A_ANU_NAXX_AGGRO02'
// 8787,'A_ANU_NAXX_AGGRO03'
// 8788,'A_ANU_NAXX_GREET'
// 8789,'A_ANU_NAXX_SLAY'
// 8790,'A_ANU_NAXX_TAUNT01'
// 8791,'A_ANU_NAXX_TAUNT02'
// 8792,'A_ANU_NAXX_TAUNT03'
// 8793,'A_ANU_NAXX_TAUNT04'

#define IMPALE 28783 // various, wicked spell :P
#define LOCUST_SWARM 28785 // self

class AnubAI : public CreatureAIScript
{
public:
	ADD_CREATURE_FACTORY_FUNCTION(AnubAI);
    AnubAI(Creature* pCreature) : CreatureAIScript(pCreature)
    {
		// -- Number of spells to add --
		nrspells = 2;

		// --- Initialization ---
		m_spellcheck = new bool[nrspells];
		spells = new SP_AI_Spell[nrspells];
		for(int i=0;i<nrspells;i++)
		{
			m_spellcheck[i] = false;
		}
		// ----------------------

		pAnubDoor = _unit->GetMapMgr()->GetInterface()->GetObjectNearestCoords<GameObject, TYPEID_GAMEOBJECT>(
            181195, 3143.96f, -3572.22f, 287.097f);

		if(pAnubDoor)
			m_gatestate = pAnubDoor->GetUInt32Value(GAMEOBJECT_STATE);
		else 
			m_gatestate = 0;
		
		// Create basic info for spells here, and play with it later , fill always the info, targettype and if is instant or not! 
        spells[0].info = sSpellStore.LookupEntry(IMPALE);
		spells[0].targettype = TARGET_VARIOUS;
		spells[0].instant = false;
		spells[0].perctrigger = 5.0f;
		spells[0].attackstoptimer = 3000; // 1sec

		spells[1].info = sSpellStore.LookupEntry(LOCUST_SWARM);
		spells[1].targettype = TARGET_SELF;
		spells[1].instant = false;
		spells[1].perctrigger = 2.0f;
		spells[1].attackstoptimer = 1000; // 1sec

    }
    
    void OnCombatStart(Unit* mTarget)
    {
		// TODO: check right sounds...
        RegisterAIUpdateEvent(_unit->GetUInt32Value(UNIT_FIELD_BASEATTACKTIME));
		if(Rand(25.0f))
		{
			_unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, "Just a little taste...");
			//_unit->PlaySoundToSet(8785);
		}
		else if(Rand(25.0f))
		{
			_unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, "There is no way out.");
			//_unit->PlaySoundToSet(8786);
		}
		else
		{
			_unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, "Yes, run! It makes the blood pump faster!");
			//_unit->PlaySoundToSet(8787);
		}

			
    }

    void OnCombatStop(Unit *mTarget)
    {
        _unit->GetAIInterface()->setCurrentAgent(AGENT_NULL);
        _unit->GetAIInterface()->SetAIState(STATE_IDLE);
        RemoveAIUpdateEvent();
    }

	void OnTargetDied(Unit* mTarget)
    { 
        _unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, "Shhh... it will all be over soon.");
        _unit->PlaySoundToSet(8789);
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
		if(pAnubDoor && m_gatestate && pAnubDoor->GetUInt32Value(GAMEOBJECT_STATE) == 0)
		{
			m_gatestate = 0;
			_unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, "Ahh... welcome to my parlor.");
			_unit->PlaySoundToSet(8788); // GREET
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
	uint32 m_gatestate;
	GameObject *pAnubDoor;
};



void SetupNaxxramas(ScriptMgr * mgr)
{
	// ---- Spider Wing ----
    mgr->register_creature_script(CN_CARRION_SPINNER, &CarrionSpinnerAI::Create);
	mgr->register_creature_script(CN_DREAD_CREEPER, &DreadCreeperAI::Create);
    mgr->register_creature_script(CN_NAXX_CULTIST, &NaxxCultistAI::Create);
	mgr->register_creature_script(CN_NECRO_STALKER, &NecroStalkerAI::Create);
	mgr->register_creature_script(VENOM_STALKER, &NecroStalkerAI::Create);
	mgr->register_creature_script(CN_TOMB_HORROR, &TombHorrorAI::Create);
    mgr->register_creature_script(CN_NECRO_ACOLYTE, &NecroAcolyteAI::Create);
	mgr->register_creature_script(CN_MAEXXNA, &MaexxnaAI::Create);
	mgr->register_creature_script(CN_WIDOW_FAERLINA, &WidowFaerlinaAI::Create);
	mgr->register_creature_script(CN_ANUB, &AnubAI::Create);
	/*mgr->register_creature_script(CN_VAELASTRASZ, &VaelastraszAI::Create);
	mgr->register_gossip_script(CN_VAELASTRASZ, &VaelastraszGossip::Create);*/
}