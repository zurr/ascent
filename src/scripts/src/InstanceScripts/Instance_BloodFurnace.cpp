#include "StdAfx.h"
#include "Setup.h"

/************************************************************************/
/* Instance_BloodFurnace.cpp Script		                                */
/************************************************************************/

/*****************************/
/*                           */
/*         Boss AIs          */
/*                           */
/*****************************/

// Keli'dan the BreakerAI

#define CN_KELIDAN_THE_BREAKER 17377

#define SHADOW_BOLT_VOLLEY 30510 // DBC: 17228; 38840/29924/40070/30510/30354 //CONE_SHADOW_BOLTS 
#define BLAST_WAVE 30600 // DBC: 33775; 39038 or 30600
#define CORRUPTION 30938

class KELIDANTHEBREAKERAI : public CreatureAIScript
{
public:
    ADD_CREATURE_FACTORY_FUNCTION(KELIDANTHEBREAKERAI);
    KELIDANTHEBREAKERAI(Creature* pCreature) : CreatureAIScript(pCreature)
    {
		nrspells = 3;
		m_spellcheck = new bool[nrspells];
		spells = new SP_AI_Spell[nrspells];
		for(int i=0;i<nrspells;i++)
		{
			m_spellcheck[i] = false;
		}
        spells[0].info = sSpellStore.LookupEntry(SHADOW_BOLT_VOLLEY);
		spells[0].targettype = TARGET_DESTINATION;
		spells[0].instant = false;
		spells[0].cooldown = 15;

        spells[1].info = sSpellStore.LookupEntry(BLAST_WAVE);
		spells[1].targettype = TARGET_VARIOUS;
		spells[1].instant = false;
		spells[1].cooldown = 30;
		spells[1].speech = "Closer... Come closer... And burn!";
		spells[1].soundid = 10165;

        spells[2].info = sSpellStore.LookupEntry(CORRUPTION);
		spells[2].targettype = TARGET_ATTACKING;
		spells[2].instant = true;
		spells[2].cooldown = 45;

		_unit->GetAIInterface()->setOutOfCombatRange(200000);
    }
    
    void OnCombatStart(Unit* mTarget)
    {
		CastTime();
		int RandomSpeach;
		sRand.randInt(1000);
		RandomSpeach=rand()%3;
		switch (RandomSpeach)
		{
		case 0:
			_unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, "You mustn't let him loose!");
			_unit->PlaySoundToSet(10166);
			break;
		case 1:
			_unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, "Ignorant whelps!");
			_unit->PlaySoundToSet(10167);
			break;
		case 2:
			_unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, "You fools, he'll kill us all!");
			_unit->PlaySoundToSet(10168);
			break;
		}
		RegisterAIUpdateEvent(_unit->GetUInt32Value(UNIT_FIELD_BASEATTACKTIME));
    }

	void CastTime()
	{
		for(int i=0;i<nrspells;i++)
			spells[i].casttime = spells[i].cooldown;
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
				_unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, "Just as you deserve.");
				_unit->PlaySoundToSet(10169);
				break;
			case 1:
				_unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, "Your friends, will soon be joining you.");
				_unit->PlaySoundToSet(10170);
				break;
			}
		}
    }

    void OnCombatStop(Unit *mTarget)
    {
		CastTime();
        _unit->GetAIInterface()->setCurrentAgent(AGENT_NULL);
        _unit->GetAIInterface()->SetAIState(STATE_IDLE);
        RemoveAIUpdateEvent();
    }

    void OnDied(Unit * mKiller)
    {
		CastTime();
		_unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, "Good luck... You'll need it.");
		_unit->PlaySoundToSet(10171);
       RemoveAIUpdateEvent();
	   delete[] spells;
	   delete[] m_spellcheck;
	   spells = NULL;
	   m_spellcheck = NULL;
    }

    void AIUpdate()
	{
		SpellCast();
    }

	void SpellCast()
	{
        if(m_spellcheck && spells && _unit->GetCurrentSpell() == NULL && _unit->GetAIInterface()->GetNextTarget())
        {
		    Unit *target = NULL;
			for(int i=0;i<nrspells;i++)
			{
				spells[i].casttime--;
				if(spells[i].casttime) continue;
				
				if (!spells[i].casttime && m_spellcheck)
				{
					spells[i].casttime = spells[i].cooldown;					
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

				m_spellcheck[i] = true;
			}
		}
	}

protected:

    bool *m_spellcheck;
    SP_AI_Spell *spells;
	int nrspells;
};

// BroggokAI

#define CN_BROGGOK 17380

#define POISON_BOLT 30917
#define POISON_CLOUD 31259 // DBC: 30916; no idea if correct
#define SLIME_SPRAY 30913

class BROGGOKAI : public CreatureAIScript
{
public:
    ADD_CREATURE_FACTORY_FUNCTION(BROGGOKAI);
    BROGGOKAI(Creature* pCreature) : CreatureAIScript(pCreature)
    {
		nrspells = 3;
		m_spellcheck = new bool[nrspells];
		spells = new SP_AI_Spell[nrspells];
		for(int i=0;i<nrspells;i++)
		{
			m_spellcheck[i] = false;
		}
        spells[0].info = sSpellStore.LookupEntry(POISON_BOLT);
		spells[0].targettype = TARGET_VARIOUS;
		spells[0].instant = true;
		spells[0].cooldown = 15;

        spells[1].info = sSpellStore.LookupEntry(POISON_CLOUD);
		spells[1].targettype = TARGET_DESTINATION;
		spells[1].instant = true;
		spells[1].cooldown = 70;

        spells[2].info = sSpellStore.LookupEntry(SLIME_SPRAY);
		spells[2].targettype = TARGET_VARIOUS;
		spells[2].instant = true;
		spells[2].cooldown = 25;

		_unit->GetAIInterface()->setOutOfCombatRange(200000);
	}

    void OnCombatStart(Unit* mTarget)
    {
		CastTime();
		RegisterAIUpdateEvent(_unit->GetUInt32Value(UNIT_FIELD_BASEATTACKTIME));
    }

	void CastTime()
	{
		for(int i=0;i<nrspells;i++)
			spells[i].casttime = spells[i].cooldown;
	}

	void OnTargetDied(Unit* mTarget)
    {
    }

    void OnCombatStop(Unit *mTarget)
    {
		CastTime();
        _unit->GetAIInterface()->setCurrentAgent(AGENT_NULL);
        _unit->GetAIInterface()->SetAIState(STATE_IDLE);
        RemoveAIUpdateEvent();
    }

    void OnDied(Unit * mKiller)
    {
		CastTime();
       RemoveAIUpdateEvent();
	   delete[] spells;
	   delete[] m_spellcheck;
	   spells = NULL;
	   m_spellcheck = NULL;
    }

    void AIUpdate()
	{
		SpellCast();
    }

	void SpellCast()
	{
        if(m_spellcheck && spells && _unit->GetCurrentSpell() == NULL && _unit->GetAIInterface()->GetNextTarget())
        {
		    Unit *target = NULL;
			for(int i=0;i<nrspells;i++)
			{
				spells[i].casttime--;
				if(spells[i].casttime) continue;
				
				if (!spells[i].casttime && m_spellcheck)
				{
					spells[i].casttime = spells[i].cooldown;					
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

				m_spellcheck[i] = true;
			}
		}
	}

protected:

    bool *m_spellcheck;
    SP_AI_Spell *spells;
	int nrspells;
};

// The MakerAI

#define CN_THE_MAKER 17381

#define DOMINATION 30923 // 36866
#define ACID_SPRAY 38973 // 38973 or 38153	// not sure about casting of this
#define KNOCKDOWN 38576 // 38576 or 37317	// and this spell
#define EXPLODING_BEAKER 30925 // Throw beaker <--- maybe this is it?

class THEMAKERAI : public CreatureAIScript
{
public:
    ADD_CREATURE_FACTORY_FUNCTION(THEMAKERAI);
    THEMAKERAI(Creature* pCreature) : CreatureAIScript(pCreature)
    {
		nrspells = 4;
		m_spellcheck = new bool[nrspells];
		spells = new SP_AI_Spell[nrspells];
		for(int i=0;i<nrspells;i++)
		{
			m_spellcheck[i] = false;
		}
        spells[0].info = sSpellStore.LookupEntry(DOMINATION);
		spells[0].targettype = TARGET_ATTACKING;
		spells[0].instant = true;
		spells[0].cooldown = 50;

        spells[1].info = sSpellStore.LookupEntry(ACID_SPRAY);
		spells[1].targettype = TARGET_VARIOUS; // maybe that?
		spells[1].instant = true;
		spells[1].cooldown = 35;

        spells[2].info = sSpellStore.LookupEntry(KNOCKDOWN);
		spells[2].targettype = TARGET_VARIOUS;
		spells[2].instant = true;
		spells[2].cooldown = 25;

        spells[3].info = sSpellStore.LookupEntry(EXPLODING_BEAKER);
		spells[3].targettype = TARGET_DESTINATION;
		spells[3].instant = true;
		spells[3].cooldown = 15; 

		_unit->GetAIInterface()->setOutOfCombatRange(200000);
    }
    
    void OnCombatStart(Unit* mTarget)
    {
		CastTime();
		int RandomSpeach;
		sRand.randInt(1000);
		RandomSpeach=rand()%3;
		switch (RandomSpeach)
		{
		case 0:
			_unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, "My work must not be interrupted.");
			_unit->PlaySoundToSet(10286);
			break;
		case 1:
			_unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, "Perhaps I can find a use for you.");
			_unit->PlaySoundToSet(10287);
			break;
		case 2:
			_unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, "Anger... Hate... These are tools I can use.");
			_unit->PlaySoundToSet(10288);
			break;
		}
		RegisterAIUpdateEvent(_unit->GetUInt32Value(UNIT_FIELD_BASEATTACKTIME));
    }

	void CastTime()
	{
		for(int i=0;i<nrspells;i++)
			spells[i].casttime = spells[i].cooldown;
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
				_unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, "Let's see what I can make of you.");
				_unit->PlaySoundToSet(10289);
				break;
			case 1:
				_unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, "It is pointless to resist.");
				_unit->PlaySoundToSet(10290);
				break;
			}
		}
    }

    void OnCombatStop(Unit *mTarget)
    {
		CastTime();
        _unit->GetAIInterface()->setCurrentAgent(AGENT_NULL);
        _unit->GetAIInterface()->SetAIState(STATE_IDLE);
        RemoveAIUpdateEvent();
    }

    void OnDied(Unit * mKiller)
    {
		CastTime();
		_unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, "Stay away from... me.");
		_unit->PlaySoundToSet(10291);
       RemoveAIUpdateEvent();
	   delete[] spells;
	   delete[] m_spellcheck;
	   spells = NULL;
	   m_spellcheck = NULL;
    }

    void AIUpdate()
	{
		SpellCast();
    }

	void SpellCast()
	{
        if(m_spellcheck && spells && _unit->GetCurrentSpell() == NULL && _unit->GetAIInterface()->GetNextTarget())
        {
		    Unit *target = NULL;
			for(int i=0;i<nrspells;i++)
			{
				spells[i].casttime--;
				if(spells[i].casttime) continue;
				
				if (!spells[i].casttime && m_spellcheck)
				{
					spells[i].casttime = spells[i].cooldown;					
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

				m_spellcheck[i] = true;
			}
		}
	}

protected:

    bool *m_spellcheck;
    SP_AI_Spell *spells;
	int nrspells;
};

void SetupBloodFurnace(ScriptMgr * mgr)
{
	mgr->register_creature_script(CN_KELIDAN_THE_BREAKER, &KELIDANTHEBREAKERAI::Create);
	mgr->register_creature_script(CN_BROGGOK, &BROGGOKAI::Create);
	mgr->register_creature_script(CN_THE_MAKER, &THEMAKERAI::Create);
}

// Lack of info to make this instance with trash mobs...