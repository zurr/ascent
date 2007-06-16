#include "StdAfx.h"
#include "Setup.h"

/************************************************************************/
/* Instance_HellfireRamparts.cpp Script		                            */
/************************************************************************/

/*****************************/
/*                           */
/*         Boss AIs          */
/*                           */
/*****************************/

// Watchkeeper GargolmarAI

#define CN_WATCHKEEPER_GARGOLMAR 17306

#define SURGE 25787 // not sure
#define MORTAL_WOUND 25646 // 25646, 36814, 30641, 31464, 28467, 38770
				  // 38770
class WATCHKEEPERGARGOLMARAI : public CreatureAIScript
{
public:
    ADD_CREATURE_FACTORY_FUNCTION(WATCHKEEPERGARGOLMARAI);
    WATCHKEEPERGARGOLMARAI(Creature* pCreature) : CreatureAIScript(pCreature)
    {
		HEAL_SOUND_LIMITER = 0;
		nrspells = 2;
		m_spellcheck = new bool[nrspells];
		spells = new SP_AI_Spell[nrspells];
		for(int i=0;i<nrspells;i++)
		{
			m_spellcheck[i] = false;
		}
        spells[0].info = sSpellStore.LookupEntry(SURGE);
		spells[0].targettype = TARGET_ATTACKING;
		spells[0].instant = true;
		spells[0].perctrigger = 8.0f;
		spells[0].attackstoptimer = 1000;
		spells[0].speech = "Back off, pup!";
		spells[0].soundid = 10330;

        spells[1].info = sSpellStore.LookupEntry(MORTAL_WOUND);
		spells[1].targettype = TARGET_ATTACKING;
		spells[1].instant = true;
		spells[1].perctrigger = 5.0f;
		spells[1].attackstoptimer = 1000;
    }
    
    void OnCombatStart(Unit* mTarget)
    {
		HEAL_SOUND_LIMITER = 0;
		int RandomSpeach;
		sRand.randInt(1000);
		RandomSpeach=rand()%3;
		switch (RandomSpeach)
		{
		case 0:
			_unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, "What do we have here? ...");
			_unit->PlaySoundToSet(10331);
			break;
		case 1:
			_unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, "Heh... this may hurt a little.");
			_unit->PlaySoundToSet(10332);
			break;
		case 2:
			_unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, "I'm gonna enjoy this!");
			_unit->PlaySoundToSet(10333);
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
				_unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, "Say farewell!");
				_unit->PlaySoundToSet(10334);
				break;
			case 1:
				_unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, "Much too easy!");
				_unit->PlaySoundToSet(10335);
				break;
			}
		}
    }

    void OnCombatStop(Unit *mTarget)
    {
		HEAL_SOUND_LIMITER = 0;
        _unit->GetAIInterface()->setCurrentAgent(AGENT_NULL);
        _unit->GetAIInterface()->SetAIState(STATE_IDLE);
        RemoveAIUpdateEvent();
    }

    void OnDied(Unit * mKiller)
    {
		HEAL_SOUND_LIMITER = 0;
		_unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, "Hah..."); // needs corrections!
		_unit->PlaySoundToSet(10336);
       RemoveAIUpdateEvent();
	   delete[] spells;
	   delete[] m_spellcheck;
	   spells = NULL;
	   m_spellcheck = NULL;
    }

    void AIUpdate()
    {
		if (_unit->GetHealthPct() <= 40 && !HEAL_SOUND_LIMITER)
		{
			_unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, "Heal me! QUICKLY!");
			_unit->PlaySoundToSet(10329);
			HEAL_SOUND_LIMITER = 1;
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

	int HEAL_SOUND_LIMITER;
    bool *m_spellcheck;
    SP_AI_Spell *spells;
	int nrspells;
};

// Omor the UnscarredAI

#define CN_OMOR_THE_UNSCARRED 17308

#define SPELL_REFLECTION 31534 // not sure	// casted since 20% hp
#define SUMMON_FIENDISH_HOUND 30707 // should be ok	// lack of core support
#define SHADOW_WIP 30638
#define TREACHEROUS_AURA 30695
#define BANE_OF_TREACHERY 37566 // it's heroic spell which replaces treacherous aura
#define SHADOW_BOLT 31627 // 31627, 31618, 31627 and many other possibilities
// TO DO: Add sound *Knock1* with spell if needed

class OMORTHEUNSCARREDAI : public CreatureAIScript
{
public:
    ADD_CREATURE_FACTORY_FUNCTION(OMORTHEUNSCARREDAI);
    OMORTHEUNSCARREDAI(Creature* pCreature) : CreatureAIScript(pCreature)
    {
		SPELL_REFLECTIONCooldown = 0; // not sure if this should be casted one or after some time
		nrspells = 6;
		m_spellcheck = new bool[nrspells];
		spells = new SP_AI_Spell[nrspells];
		for(int i=0;i<nrspells;i++)
		{
			m_spellcheck[i] = false;
		}
        spells[0].info = sSpellStore.LookupEntry(SPELL_REFLECTION);
		spells[0].targettype = TARGET_SELF;
		spells[0].instant = true;
		spells[0].perctrigger = 0.0f;
		spells[0].attackstoptimer = 1000;

        spells[1].info = sSpellStore.LookupEntry(SUMMON_FIENDISH_HOUND);
		spells[1].targettype = TARGET_SELF;
		spells[1].instant = false;
		spells[1].perctrigger = 3.0f;	// maybe I will change it to cooldown
		spells[1].attackstoptimer = 1000;
		spells[1].speech = "<missing_text>";
		spells[1].soundid = 10277;

        spells[2].info = sSpellStore.LookupEntry(SHADOW_WIP);
		spells[2].targettype = TARGET_ATTACKING;	// should be random target
		spells[2].instant = true;
		spells[2].perctrigger = 5.0f;
		spells[2].attackstoptimer = 1000;

        spells[3].info = sSpellStore.LookupEntry(TREACHEROUS_AURA);
		spells[3].targettype = TARGET_ATTACKING;
		spells[3].instant = false;
		spells[3].perctrigger = 6.0f;
		spells[3].attackstoptimer = 1000;

        spells[4].info = sSpellStore.LookupEntry(BANE_OF_TREACHERY);
		spells[4].targettype = TARGET_ATTACKING;
		spells[4].instant = false;
		spells[4].perctrigger = 0.0f;
		spells[4].attackstoptimer = 1000;

        spells[5].info = sSpellStore.LookupEntry(SHADOW_BOLT);
		spells[5].targettype = TARGET_ATTACKING;
		spells[5].instant = false;
		spells[5].perctrigger = 12.0f;
		spells[5].attackstoptimer = 1000;
    }
    
    void OnCombatStart(Unit* mTarget)
    {
		SPELL_REFLECTIONCooldown = 0;
		_unit->CastSpell(_unit, spells[1].info, spells[1].instant);
		RegisterAIUpdateEvent(_unit->GetUInt32Value(UNIT_FIELD_BASEATTACKTIME));
		int RandomSpeach;
		sRand.randInt(1000);
		RandomSpeach=rand()%3;
		switch (RandomSpeach)
		{
		case 0:
			_unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, "I will not be... defeated!");
			_unit->PlaySoundToSet(10279);
			break;
		case 1:
			_unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, "You dare stand against me?");	// ofc they need corrections ;)
			_unit->PlaySoundToSet(10280);
			break;
		case 2:
			_unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, "Your incidents will be your death!");
			_unit->PlaySoundToSet(10281);
			break;
		}
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
				_unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, "Die weakling!");
				_unit->PlaySoundToSet(10282);
				break;
			case 1:
				_unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, "I am victorious!");
				_unit->PlaySoundToSet(10283);
				break;
			}
		}
    }

    void OnCombatStop(Unit *mTarget)
    {
		SPELL_REFLECTIONCooldown = 0;
        _unit->GetAIInterface()->setCurrentAgent(AGENT_NULL);
        _unit->GetAIInterface()->SetAIState(STATE_IDLE);
        RemoveAIUpdateEvent();
    }

    void OnDied(Unit * mKiller)
    {
		_unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, "It is... not... over...");
		_unit->PlaySoundToSet(10284);
		SPELL_REFLECTIONCooldown = 0;
       RemoveAIUpdateEvent();
	   delete[] spells;
	   delete[] m_spellcheck;
	   spells = NULL;
	   m_spellcheck = NULL;
    }

    void AIUpdate()
    {
		SPELL_REFLECTIONCooldown--;
		if (_unit->GetHealthPct() <= 20 && SPELL_REFLECTIONCooldown <= 0)
		{
			_unit->CastSpell(_unit, spells[0].info, spells[0].instant);
			SPELL_REFLECTIONCooldown = 30;
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

	int SPELL_REFLECTIONCooldown;
    bool *m_spellcheck;
    SP_AI_Spell *spells;
	int nrspells;
};

void SetupHellfireRamparts(ScriptMgr * mgr)
{
	mgr->register_creature_script(CN_WATCHKEEPER_GARGOLMAR, &WATCHKEEPERGARGOLMARAI::Create);
	mgr->register_creature_script(CN_OMOR_THE_UNSCARRED, &OMORTHEUNSCARREDAI::Create);
}
