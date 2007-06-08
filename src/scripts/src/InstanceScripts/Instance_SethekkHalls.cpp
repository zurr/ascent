#include "StdAfx.h"
#include "Setup.h"

/************************************************************************/
/* Instance_SethekkHalls.cpp Script		                            */
/************************************************************************/


/*****************************/
/*                           */
/*         Boss AIs          */
/*                           */
/*****************************/

// Darkweaver SythAI

#define CN_DARKWEAVER_SYTH 18472

#define FROST_SHOCK
#define FLAME_SHOCK
#define SHADOW_SHOCK
#define ARCANE_SHOCK
#define CHAIN_LIGHTNING

// Summons
/*
#define // Fire Elemental
#define // Frost Elemental
#define // Arcane Elemental
#define // Shadow Elemental
*/

class DARKWEAVERSYTHAI : public CreatureAIScript
{
public:
    ADD_CREATURE_FACTORY_FUNCTION(DARKWEAVERSYTHAI);
    DARKWEAVERSYTHAI(Creature* pCreature) : CreatureAIScript(pCreature)
    {
		nrspells = 0;
		m_spellcheck = new bool[nrspells];
		spells = new SP_AI_Spell[nrspells];
		for(int i=0;i<nrspells;i++)
		{
			m_spellcheck[i] = false;

		} 
        /*spells[0].info = sSpellStore.LookupEntry();
		spells[0].targettype = TARGET_SELF;
		spells[0].instant = true;
		spells[0].perctrigger = 0.0f;
		spells[0].attackstoptimer = 1000;*/

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


// Talon King IkissAI

#define CN_TALON_KING_IKISS 18473

#define ARCANE_VOLLEY
#define ARCANE_EXPLOSION
#define BLINK // To random character
#define POLYMORPH

class TALONKINGIKISSAI : public CreatureAIScript
{
public:
    ADD_CREATURE_FACTORY_FUNCTION(TALONKINGIKISSAI);
    TALONKINGIKISSAI(Creature* pCreature) : CreatureAIScript(pCreature)
    {
		nrspells = 0;
		m_spellcheck = new bool[nrspells];
		spells = new SP_AI_Spell[nrspells];
		for(int i=0;i<nrspells;i++)
		{
			m_spellcheck[i] = false;
		}
        /*
		spells[0].info = sSpellStore.LookupEntry();
		spells[0].targettype = TARGET_SELF;
		spells[0].instant = true;
		spells[0].perctrigger = 0.0f;
		spells[0].attackstoptimer = 1000;
		*/

	} 

    void OnCombatStart(Unit* mTarget)
    {
        RegisterAIUpdateEvent(_unit->GetUInt32Value(UNIT_FIELD_BASEATTACKTIME));
    }

	void OnTargetDied(Unit* mTarget) // left to keep it easy to add needed data.
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

// AnzuAI

void SetupSethekkHalls(ScriptMgr * mgr)
{
    mgr->register_creature_script(CN_DARKWEAVER_SYTH, &DARKWEAVERSYTHAI::Create);
    mgr->register_creature_script(CN_TALON_KING_IKISS, &TALONKINGIKISSAI::Create);
	//mgr->register_creature_script(CN_ANZU, &ANZUAI::Create);
}
