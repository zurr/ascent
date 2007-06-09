#include "StdAfx.h"
#include "Setup.h"

/************************************************************************/
/* Instance_GruulsLair.cpp Script										*/
/************************************************************************/


/*****************************/
/*                           */
/*         Boss AIs          */
/*                           */
/*****************************/

// Shirrak the GruulsAI

#define CN_GRUUL_THE_DRAGONKILLER 19044

#define GROWTH            36300 
#define CAVE_IN           36240 
#define GROUND_SLAM       33525
#define SHATTER           33671	// Should be used only when Stoned
#define HURTFUL_STRIKE    33813 
#define REVERBERATION     36297 
#define STONED			  33652 
#define GRONN_LORDS_GRASP 33572 // Should be used only after Ground Slam
// % chances changed to let boss use normal attks too
// Note: Maybe we should add additional spell description option to
// define next spells.

class GruulsTheDragonkillerAI : public CreatureAIScript
{
public:
    ADD_CREATURE_FACTORY_FUNCTION(GruulsTheDragonkillerAI);
    GruulsTheDragonkillerAI(Creature* pCreature) : CreatureAIScript(pCreature)
    {
		GrowthCooldown = 30;
		nrspells = 8;
		m_spellcheck = new bool[nrspells];
		spells = new SP_AI_Spell[nrspells];
		for(int i=0;i<nrspells;i++)
		{
			m_spellcheck[i] = false;

		} 
		spells[0].info = sSpellStore.LookupEntry(GROWTH);
		spells[0].targettype = TARGET_SELF;
		spells[0].instant = true;
		spells[0].perctrigger = 0.0f;
		spells[0].attackstoptimer = 1000;

		spells[1].info = sSpellStore.LookupEntry(CAVE_IN);
		spells[1].targettype = TARGET_DESTINATION;
		spells[1].instant = true;
		spells[1].perctrigger = 10.0f;
		spells[1].attackstoptimer = 1000;

		spells[2].info = sSpellStore.LookupEntry(HURTFUL_STRIKE);
		spells[2].targettype = TARGET_ATTACKING; // Should attk party member with second the highest aggro in melee range
		spells[2].instant = true;
		spells[2].perctrigger = 3.0f;
		spells[2].attackstoptimer = 1000;

		spells[3].info = sSpellStore.LookupEntry(REVERBERATION);
		spells[3].targettype = TARGET_VARIOUS;
		spells[3].instant = true;
		spells[3].perctrigger = 6.0f;
		spells[3].attackstoptimer = 1000;

		spells[4].info = sSpellStore.LookupEntry(GROUND_SLAM);
		spells[4].targettype = TARGET_DESTINATION;
		spells[4].instant = false;
		spells[4].perctrigger = 7.0f;
		spells[4].attackstoptimer = 2000;

		spells[5].info = sSpellStore.LookupEntry(SHATTER);
		spells[5].targettype = TARGET_VARIOUS;
		spells[5].instant = true;
		spells[5].perctrigger = 6.0f;
		spells[5].attackstoptimer = 1000;

		spells[6].info = sSpellStore.LookupEntry(STONED);
		spells[6].targettype = TARGET_SELF;
		spells[6].instant = true;
		spells[6].perctrigger = 7.0f;
		spells[6].attackstoptimer = 1000;

		spells[7].info = sSpellStore.LookupEntry(GRONN_LORDS_GRASP);
		spells[7].targettype = TARGET_SELF; // <-- not sure to that (description tells it works like that, but should it really be casted on boss?)
		spells[7].instant = true;
		spells[7].perctrigger = 6.0f;
		spells[7].attackstoptimer = 1000;

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
        GrowthCooldown = 30;
        RemoveAIUpdateEvent();
    }

    void OnDied(Unit * mKiller)
    {
       RemoveAIUpdateEvent();
       GrowthCooldown = 30;
	   delete[] spells;
	   delete[] m_spellcheck;
	   spells = NULL;
	   m_spellcheck = NULL;
    }

    void AIUpdate()
    {
		GrowthCooldown--;
		if(!GrowthCooldown)//_unit->getAttackTarget())
        {
		_unit->CastSpell(_unit, spells[0].info, spells[0].instant);
		GrowthCooldown=30;
		}
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

	uint32 GrowthCooldown;
    bool *m_spellcheck;
    SP_AI_Spell *spells;
	int nrspells;
};
void SetupGruulsLair(ScriptMgr * mgr)
{
    mgr->register_creature_script(CN_GRUUL_THE_DRAGONKILLER, &GruulsTheDragonkillerAI::Create);
}