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

// Spells
#define FROST_SHOCK 25464	// Workaround as this is player spell, but with proly same effect. (lower dmg - 37865, higher dmg - 37332)
#define FLAME_SHOCK 34354	// To small dmg, need to find better one 
#define SHADOW_SHOCK 30138	// SELF IN DESCR. ?_? I think more accurate can be found (but this is quiet good)
#define ARCANE_SHOCK 33175	// I think better can be found, but still it's good
#define CHAIN_LIGHTNING 33643	// Couldn't find more info about this spell?

// Summons
#define SUMMON_SYTH_FIRE_ELEMENTAL 33537	//needs more core support, but those are correct ids!
#define SUMMON_SYTH_FROST_ELEMENTAL 33539
#define SUMMON_SYTH_ARCANE_ELEMENTAL 33538
#define SUMMON_SYTH_SHADOW_ELEMENTAL 33540

class DARKWEAVERSYTHAI : public CreatureAIScript
{
public:
    ADD_CREATURE_FACTORY_FUNCTION(DARKWEAVERSYTHAI);
    DARKWEAVERSYTHAI(Creature* pCreature) : CreatureAIScript(pCreature)
    {
		SUMMON_LIMITER = 1;
		nrspells = 9;
		m_spellcheck = new bool[nrspells];
		spells = new SP_AI_Spell[nrspells];
		for(int i=0;i<nrspells;i++)
		{
			m_spellcheck[i] = false;

		} 
        spells[0].info = sSpellStore.LookupEntry(FROST_SHOCK);
		spells[0].targettype = TARGET_ATTACKING;
		spells[0].instant = true;
		spells[0].perctrigger = 9.0f;
		spells[0].attackstoptimer = 1000;

        spells[1].info = sSpellStore.LookupEntry(FLAME_SHOCK);
		spells[1].targettype = TARGET_ATTACKING;
		spells[1].instant = true;
		spells[1].perctrigger = 9.0f;
		spells[1].attackstoptimer = 1000;

        spells[2].info = sSpellStore.LookupEntry(SHADOW_SHOCK);
		spells[2].targettype = TARGET_ATTACKING;
		spells[2].instant = true;
		spells[2].perctrigger = 9.0f;
		spells[2].attackstoptimer = 1000;

        spells[3].info = sSpellStore.LookupEntry(ARCANE_SHOCK);
		spells[3].targettype = TARGET_ATTACKING;
		spells[3].instant = true;
		spells[3].perctrigger = 9.0f;
		spells[3].attackstoptimer = 1000;

        spells[4].info = sSpellStore.LookupEntry(CHAIN_LIGHTNING);
		spells[4].targettype = TARGET_ATTACKING;
		spells[4].instant = true;
		spells[4].perctrigger = 15.0f;
		spells[4].attackstoptimer = 1000;

        spells[5].info = sSpellStore.LookupEntry(SUMMON_SYTH_FIRE_ELEMENTAL);
		spells[5].targettype = TARGET_SELF;
		spells[5].instant = true;
		spells[5].perctrigger = 0.0f;
		spells[5].attackstoptimer = 1000;

        spells[6].info = sSpellStore.LookupEntry(SUMMON_SYTH_FROST_ELEMENTAL);
		spells[6].targettype = TARGET_SELF;
		spells[6].instant = true;
		spells[6].perctrigger = 0.0f;
		spells[6].attackstoptimer = 1000;

        spells[7].info = sSpellStore.LookupEntry(SUMMON_SYTH_ARCANE_ELEMENTAL);
		spells[7].targettype = TARGET_SELF;
		spells[7].instant = true;
		spells[7].perctrigger = 0.0f;
		spells[7].attackstoptimer = 1000;

        spells[8].info = sSpellStore.LookupEntry(SUMMON_SYTH_SHADOW_ELEMENTAL);
		spells[8].targettype = TARGET_SELF;
		spells[8].instant = true;
		spells[8].perctrigger = 0.0f;
		spells[8].attackstoptimer = 1000;
    }
    
    void OnCombatStart(Unit* mTarget)
    {
		SUMMON_LIMITER = 1;
		int RandomSpeach;
		sRand.randInt(1000);
		RandomSpeach=rand()%3;
		switch (RandomSpeach)
		{
		case 0:
			_unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, "Time to... make my move!"); // needs corrections
			_unit->PlaySoundToSet(10503);
			break;
		case 1:
			_unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, "Nice pets, yes!"); // corrections needed!
			_unit->PlaySoundToSet(10504);
			break;
		case 2:
			_unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, "Nice pets have... weapons, not so... nice!");
			_unit->PlaySoundToSet(10505);
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
				_unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, "<missing_text>!"); // need to add it
				_unit->PlaySoundToSet(10506);
				break;
			case 1:
				_unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, "Be free!"); // corrections needed!!
				_unit->PlaySoundToSet(10507);
				break;
			}
		}
    }

    void OnCombatStop(Unit *mTarget)
    {
		SUMMON_LIMITER = 1;
        _unit->GetAIInterface()->setCurrentAgent(AGENT_NULL);
        _unit->GetAIInterface()->SetAIState(STATE_IDLE);
        RemoveAIUpdateEvent();
    }

    void OnDied(Unit * mKiller)
    {
		SUMMON_LIMITER = 1;
		_unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, "No more life, no more pain!"); // It's talking so <censored>
		_unit->PlaySoundToSet(10508);
       RemoveAIUpdateEvent();
	   delete[] spells;
	   delete[] m_spellcheck;
	   spells = NULL;
	   m_spellcheck = NULL;
    }

    void AIUpdate()
    {
		if ((_unit->GetHealthPct() <= 75 && SUMMON_LIMITER == 1) || (_unit->GetHealthPct() <= 50 && SUMMON_LIMITER == 2) || (_unit->GetHealthPct() <= 25 && SUMMON_LIMITER == 3))
		{
			SummonElementalWave();
		}
		else
		{
			float val = sRand.rand(100.0f);
			SpellCast(val);
		}
	}

	void SummonElementalWave()
	{
		_unit->CastSpell(_unit, spells[5].info, spells[5].instant);
		_unit->CastSpell(_unit, spells[6].info, spells[6].instant);
		_unit->CastSpell(_unit, spells[7].info, spells[7].instant);
		_unit->CastSpell(_unit, spells[8].info, spells[8].instant);
		_unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, "I have pets... of my own"); // It's talking so <doublecensored> -.-'
		_unit->PlaySoundToSet(10502);
		SUMMON_LIMITER += 1;
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

	uint32 SUMMON_LIMITER;
    bool *m_spellcheck;
    SP_AI_Spell *spells;
	int nrspells;
};


// Talon King IkissAI

#define CN_TALON_KING_IKISS 18473

#define ARCANE_VOLLEY 37078 // maybe should be: 37078 or other
#define ARCANE_EXPLOSION 38197 // bit too high dmg, but should work nearly in the same way
#define BLINK 38642 // Should be to random character, also can be: 38642, 29883, 38932, 36718, // doesn't work, because of lack features in core
#define POLYMORPH 38245 // worth to try also: 38245, 38896
#define MANA_SHIELD 38151 // also: 35064, 38151

class TALONKINGIKISSAI : public CreatureAIScript
{
public:
    ADD_CREATURE_FACTORY_FUNCTION(TALONKINGIKISSAI);
    TALONKINGIKISSAI(Creature* pCreature) : CreatureAIScript(pCreature)
    {
		nrspells = 5;
		m_spellcheck = new bool[nrspells];
		spells = new SP_AI_Spell[nrspells];
		for(int i=0;i<nrspells;i++)
		{
			m_spellcheck[i] = false;
		}
        
		spells[0].info = sSpellStore.LookupEntry(ARCANE_VOLLEY);
		spells[0].targettype = TARGET_VARIOUS;
		spells[0].instant = false;
		spells[0].perctrigger = 15.0f;
		spells[0].attackstoptimer = 1000;

		spells[1].info = sSpellStore.LookupEntry(ARCANE_EXPLOSION);
		spells[1].targettype = TARGET_VARIOUS;
		spells[1].instant = false;
		spells[1].perctrigger = 8.0f;
		spells[1].attackstoptimer = 1000;

		spells[2].info = sSpellStore.LookupEntry(BLINK);
		spells[2].targettype = TARGET_ATTACKING; // SELF?
		spells[2].instant = true;
		spells[2].perctrigger = 5.0f;
		spells[2].attackstoptimer = 1000;

		spells[3].info = sSpellStore.LookupEntry(POLYMORPH);
		spells[3].targettype = TARGET_ATTACKING;
		spells[3].instant = false;
		spells[3].perctrigger = 7.0f;
		spells[3].attackstoptimer = 1000;

		spells[4].info = sSpellStore.LookupEntry(MANA_SHIELD);
		spells[4].targettype = TARGET_SELF;
		spells[4].instant = true;
		spells[4].perctrigger = 6.0f;
		spells[4].attackstoptimer = 1000;

	} 

    void OnCombatStart(Unit* mTarget)
    {
		int RandomSpeach;
		sRand.randInt(1000);
		RandomSpeach=rand()%3;
		switch (RandomSpeach)
		{
		case 0:
			_unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, "You make war on Ikiss!"); // needs corrections
			_unit->PlaySoundToSet(10554);
			break;
		case 1:
			_unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, "Ikiss caught you pretty... sliced you, yes!"); // corrections needed!
			_unit->PlaySoundToSet(10555);
			break;
		case 2:
			_unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, "No escape for... for you!");
			_unit->PlaySoundToSet(10556);
			break;
		}
        RegisterAIUpdateEvent(_unit->GetUInt32Value(UNIT_FIELD_BASEATTACKTIME));
    }

	void OnTargetDied(Unit* mTarget) // left to keep it easy to add needed data.
    {
		if (_unit->GetHealthPct() > 0)	// Hack to prevent double yelling (OnDied and OnTargetDied when creature is dying)
		{
			int RandomSpeach;
			sRand.randInt(1000);
			RandomSpeach=rand()%2;
			switch (RandomSpeach)
			{
			case 0:
				_unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, "You'll die! Stay away from trinkets!"); // needs corrections
				_unit->PlaySoundToSet(10558);
				break;
			case 1:
				_unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, "<strange_noises>"); // corrections needed!
				_unit->PlaySoundToSet(10559);
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
		_unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, "Ikiss will not... die!");
		_unit->PlaySoundToSet(10560);
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
