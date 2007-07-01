#include "StdAfx.h"
#include "Setup.h"

/************************************************************************/
/* Instance_TheMechanar.cpp Script										*/
/************************************************************************/

/**************************/
/*                        */
/*       Boss AIs         */
/*                        */
/**************************/

// Gatewatcher Gyro-Kill AI

#define CN_GATEWATCHER_GYRO_KILL 19218

#define SAW_BLADE 35318
#define SHADOW_POWER_GKILL 35322
#define STEAM_OF_MACHINE_FLUID_GKILL 35311

class GatewatcherGyroKillAI : public CreatureAIScript
{
public:
	ADD_CREATURE_FACTORY_FUNCTION(GatewatcherGyroKillAI);
	SP_AI_Spell spells[3];
	bool m_spellcheck[3];

    GatewatcherGyroKillAI(Creature* pCreature) : CreatureAIScript(pCreature)
    {
		nrspells = 3;
		for(int i=0;i<nrspells;i++)
		{
			m_spellcheck[i] = false;
		}

		spells[0].info = sSpellStore.LookupEntry(SAW_BLADE);
		spells[0].targettype = TARGET_ATTACKING;	// to prevent crashes when used VARIOUS
		spells[0].instant = true;
		spells[0].cooldown = -1;
		spells[0].perctrigger = 13.0f;
		spells[0].attackstoptimer = 1000;

		spells[1].info = sSpellStore.LookupEntry(SHADOW_POWER_GKILL);
		spells[1].targettype = TARGET_SELF;
		spells[1].instant = false;
		spells[1].cooldown = -1;
		spells[1].perctrigger = 7.0f;
		spells[1].attackstoptimer = 1000;

		spells[2].info = sSpellStore.LookupEntry(STEAM_OF_MACHINE_FLUID_GKILL);
		spells[2].targettype = TARGET_VARIOUS;	// VARIOUS doesn't work somehow :S (sometimes yes, sometimes no)
		spells[2].instant = true;
		spells[2].cooldown = -1;
		spells[2].perctrigger = 9.0f;
		spells[2].attackstoptimer = 1000;

    }
    
    void OnCombatStart(Unit* mTarget)
    {
		CastTime();
		RegisterAIUpdateEvent(_unit->GetUInt32Value(UNIT_FIELD_BASEATTACKTIME));
		_unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, "I predict a painful death.");
		_unit->PlaySoundToSet(11101);
    }

	void CastTime()
	{
		for(int i=0;i<nrspells;i++)
			spells[i].casttime = spells[i].cooldown;
	}

    void OnCombatStop(Unit *mTarget)
    {
		CastTime();
        _unit->GetAIInterface()->setCurrentAgent(AGENT_NULL);
        _unit->GetAIInterface()->SetAIState(STATE_IDLE);
        RemoveAIUpdateEvent();
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
				_unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, "Your strategy was flat!");	// flat?
				_unit->PlaySoundToSet(11102);
				break;
			case 1:
				_unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, "Yes, the only magical outcome!");	// magical outcome?
				_unit->PlaySoundToSet(11103);
				break;
			}
		}
    }

    void OnDied(Unit * mKiller)
    {
		CastTime();
       RemoveAIUpdateEvent();
		_unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, "An unforseen... contingency.");
		_unit->PlaySoundToSet(11106);
    }

    void AIUpdate()
	{
		float val = sRand.rand(100.0f);
		SpellCast(val);
    }

	void SawBladesSound()
	{
		int RandomSawBlades;
		RandomSawBlades=rand()%30;
		switch (RandomSawBlades)
		{
		case 0:
			_unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, "Measure twice; cut once.");
			_unit->PlaySoundToSet(11104);
			break;
		case 1:
			_unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, "If my devision is correct, you should be quiet dead!");	// needs checks
			_unit->PlaySoundToSet(11105);
			break;
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
				spells[i].casttime--;
				
				if (m_spellcheck[i])
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
					
					if (m_spellcheck[0] == true)
						SawBladesSound();

					if (spells[i].speech != "")
					{
						_unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, spells[i].speech.c_str());
						_unit->PlaySoundToSet(spells[i].soundid); 
					}

					m_spellcheck[i] = false;
					return;
				}

				if ((val > comulativeperc && val <= (comulativeperc + spells[i].perctrigger)) || !spells[i].casttime)
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

// Gatewatcher Iron-Hand AI

#define CN_GATEWATCHER_IRON_HAND 19710	

#define JACK_HAMMER 35327 // DBC: 35327, 35330
#define HAMMER_PUNCH 35326
#define STREAM_OF_MACHINE_FLUID_IHAND 35311
#define SHADOW_POWER_IHAND 35322

class GatewatcherIronHandAI : public CreatureAIScript
{
public:
	ADD_CREATURE_FACTORY_FUNCTION(GatewatcherIronHandAI);
	SP_AI_Spell spells[4];
	bool m_spellcheck[4];

    GatewatcherIronHandAI(Creature* pCreature) : CreatureAIScript(pCreature)
    {
		nrspells = 4;
		for(int i=0;i<nrspells;i++)
		{
			m_spellcheck[i] = false;
		}

		spells[0].info = sSpellStore.LookupEntry(JACK_HAMMER);
		spells[0].targettype = TARGET_VARIOUS;	// why this is spammed when casted ? :| maybe core bug? :|
		spells[0].instant = false;
		spells[0].cooldown = -1;
		spells[0].perctrigger = 7.0f;
		spells[0].attackstoptimer = 1000;

		spells[1].info = sSpellStore.LookupEntry(HAMMER_PUNCH);
		spells[1].targettype = TARGET_ATTACKING;
		spells[1].instant = true;
		spells[1].cooldown = -1;
		spells[1].perctrigger = 9.0f;
		spells[1].attackstoptimer = 1000;

		spells[2].info = sSpellStore.LookupEntry(STREAM_OF_MACHINE_FLUID_IHAND);
		spells[2].targettype = TARGET_VARIOUS;	// VARIOUS doesn't work somehow (sometimes yes, sometimes no)
		spells[2].instant = true;
		spells[2].cooldown = -1;
		spells[2].perctrigger = 7.0f;
		spells[2].attackstoptimer = 1000;

		spells[3].info = sSpellStore.LookupEntry(SHADOW_POWER_IHAND);
		spells[3].targettype = TARGET_SELF;
		spells[3].instant = false;
		spells[3].cooldown = -1;
		spells[3].perctrigger = 5.0f;
		spells[3].attackstoptimer = 1000;

    }
    
    void OnCombatStart(Unit* mTarget)
    {
		CastTime();
		RegisterAIUpdateEvent(_unit->GetUInt32Value(UNIT_FIELD_BASEATTACKTIME));
		_unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, "You have approximately 5 seconds to live!");	// needs checks!
		_unit->PlaySoundToSet(11109);
    }

	void CastTime()
	{
		for(int i=0;i<nrspells;i++)
			spells[i].casttime = spells[i].cooldown;
	}

    void OnCombatStop(Unit *mTarget)
    {
		CastTime();
        _unit->GetAIInterface()->setCurrentAgent(AGENT_NULL);
        _unit->GetAIInterface()->SetAIState(STATE_IDLE);
        RemoveAIUpdateEvent();
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
				_unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, "Afford gun conclusion!");
				_unit->PlaySoundToSet(11110);
				break;
			case 1:
				_unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, "The processing will continue a schedule!");
				_unit->PlaySoundToSet(11111);
				break;
			}
		}
    }

    void OnDied(Unit * mKiller)
    {
		CastTime();
       RemoveAIUpdateEvent();
		_unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, "My calculations... did not...");	// ??
		_unit->PlaySoundToSet(11114);
    }

    void AIUpdate()
	{
		float val = sRand.rand(100.0f);
		SpellCast(val);
    }

	void HammerSound()
	{
		uint32 RandomHammer;
		RandomHammer=rand()%10;
		switch (RandomHammer)
		{
		case 0:
			{
				_unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, "With the precise <missing_rest_of_text>");	// !!
				_unit->PlaySoundToSet(11112);
			}break;
		case 1:
			{
				_unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, "Low tech yet quiet effective!");	// this one too =/
				_unit->PlaySoundToSet(11113);
			}break;
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
				spells[i].casttime--;
				
				if (m_spellcheck[i])
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

					if (m_spellcheck[1] == true)
					{
						HammerSound();
					}

					if (spells[i].speech != "")
					{
						_unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, spells[i].speech.c_str());
						_unit->PlaySoundToSet(spells[i].soundid); 
					}

					m_spellcheck[i] = false;
					return;
				}

				if ((val > comulativeperc && val <= (comulativeperc + spells[i].perctrigger)) || !spells[i].casttime)
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

// Mechano-Lord Capacitus AI

#define CN_MECHANO_LORD_CAPACITUS 19219	

#define HEAD_CRACK 35161
#define REFLECTIVE_DAMAGE_SHIELD 35159
#define REFLECTIVE_MAGIC_SHIELD 35158
#define SEED_OF_CORRUPTION 37826	// SSS (server side script) (is it really used?)

/*#define NETHER_CHARGE 34303
#define NETHER_CHARGE_PASSIVE 35150 // SSS
#define NETHER_CHARGE_PULSE 35151	// SSS
#define NETHER_CHARGE_TIMER 37670
#define NETHER_DETONATION 35152		// Spell from Timer
// Note: All for bombs :O*/

class MechanoLordCapacitusAI : public CreatureAIScript
{
public:
	ADD_CREATURE_FACTORY_FUNCTION(MechanoLordCapacitusAI);
	SP_AI_Spell spells[4];
	bool m_spellcheck[4];

    MechanoLordCapacitusAI(Creature* pCreature) : CreatureAIScript(pCreature)
    {
		nrspells = 4;
		for(int i=0;i<nrspells;i++)
		{
			m_spellcheck[i] = false;
		}

		spells[0].info = sSpellStore.LookupEntry(HEAD_CRACK);
		spells[0].targettype = TARGET_ATTACKING;
		spells[0].instant = true;
		spells[0].cooldown = -1;
		spells[0].perctrigger = 8.0f;
		spells[0].attackstoptimer = 1000;

		spells[1].info = sSpellStore.LookupEntry(REFLECTIVE_DAMAGE_SHIELD);
		spells[1].targettype = TARGET_SELF;
		spells[1].instant = false;
		spells[1].cooldown = -1;
		spells[1].perctrigger = 7.0f;
		spells[1].attackstoptimer = 1000;
		spells[1].speech = "Think you can hurt me, huh? Think I'm afraid a' you?";
		spells[1].soundid = 11165;

		spells[2].info = sSpellStore.LookupEntry(REFLECTIVE_MAGIC_SHIELD);
		spells[2].targettype = TARGET_SELF;
		spells[2].instant = false;
		spells[2].cooldown = -1;
		spells[2].perctrigger = 7.0f;
		spells[2].attackstoptimer = 1000;
		spells[2].speech = "Go ahead, gimme your best shot. I can take it!";
		spells[2].soundid = 11166;

		spells[3].info = sSpellStore.LookupEntry(SEED_OF_CORRUPTION);	// it won't work anyway
		spells[3].targettype = TARGET_SELF;
		spells[3].instant = true;
		spells[3].cooldown = -1;
		spells[3].perctrigger = 5.0f;
		spells[3].attackstoptimer = 1000;

    }
    
    void OnCombatStart(Unit* mTarget)
    {
		CastTime();
		RegisterAIUpdateEvent(_unit->GetUInt32Value(UNIT_FIELD_BASEATTACKTIME));
		_unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, "You should split while you can.");
		_unit->PlaySoundToSet(11162);
    }

	void CastTime()
	{
		for(int i=0;i<nrspells;i++)
			spells[i].casttime = spells[i].cooldown;
	}

    void OnCombatStop(Unit *mTarget)
    {
		CastTime();
        _unit->GetAIInterface()->setCurrentAgent(AGENT_NULL);
        _unit->GetAIInterface()->SetAIState(STATE_IDLE);
        RemoveAIUpdateEvent();
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
				_unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, "Can't say I didn't warn you!");	// checks here
				_unit->PlaySoundToSet(11163);
				break;
			case 1:
				_unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, "Damn, I'm good!");
				_unit->PlaySoundToSet(11164);
				break;
			}
		}
    }

    void OnDied(Unit * mKiller)
    {
		CastTime();
       RemoveAIUpdateEvent();
		_unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, "Bully!");
		_unit->PlaySoundToSet(11167);
    }

    void AIUpdate()
	{
		float val = sRand.rand(100.0f);
		SpellCast(val);
    }

	void SpellCast(float val)
	{
        if(_unit->GetCurrentSpell() == NULL && _unit->GetAIInterface()->GetNextTarget())
        {
			float comulativeperc = 0;
		    Unit *target = NULL;
			for(int i=0;i<nrspells;i++)
			{
				spells[i].casttime--;
				
				if (m_spellcheck[i])
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

				if ((val > comulativeperc && val <= (comulativeperc + spells[i].perctrigger)) || !spells[i].casttime)
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

// Nethermancer Sepethrea AI

#define CN_NETHERMANCER_SEPETHREA 19221	

#define SUMMON_RAGIN_FLAMES 35275	// must add despawning after death!
#define FROST_ATTACK 35263
#define ARCANE_BLAST 35314
#define DRAGONS_BREATH 35250
//#define KNOCKBACK 37317	// not sure to this one!

class NethermancerSepethreaAI : public CreatureAIScript
{
public:
	ADD_CREATURE_FACTORY_FUNCTION(NethermancerSepethreaAI);
	SP_AI_Spell spells[4];
	bool m_spellcheck[4];

    NethermancerSepethreaAI(Creature* pCreature) : CreatureAIScript(pCreature)
    {
		SummonTimer = 4;
		nrspells = 4;
		for(int i=0;i<nrspells;i++)
		{
			m_spellcheck[i] = false;
		}

		spells[0].info = sSpellStore.LookupEntry(SUMMON_RAGIN_FLAMES);
		spells[0].targettype = TARGET_SELF;
		spells[0].instant = true;
		spells[0].cooldown = -1;
		spells[0].perctrigger = 0.0f;
		spells[0].attackstoptimer = 1000;

		spells[1].info = sSpellStore.LookupEntry(FROST_ATTACK);
		spells[1].targettype = TARGET_ATTACKING;
		spells[1].instant = true;
		spells[1].cooldown = -1;
		spells[1].perctrigger = 9.0f;
		spells[1].attackstoptimer = 1000;

		spells[2].info = sSpellStore.LookupEntry(ARCANE_BLAST);
		spells[2].targettype = TARGET_ATTACKING;
		spells[2].instant = true;
		spells[2].cooldown = -1;
		spells[2].perctrigger = 3.0f;
		spells[2].attackstoptimer = 1000;

		spells[3].info = sSpellStore.LookupEntry(DRAGONS_BREATH);
		spells[3].targettype = TARGET_VARIOUS;	// doesn't afffect when VARIOUS? WTF? :|  Sometimes works, sometimes not? :|
		spells[3].instant = true;
		spells[3].cooldown = -1;
		spells[3].perctrigger = 8.0f;
		spells[3].attackstoptimer = 1000;
/*
		spells[4].info = sSpellStore.LookupEntry(KNOCKBAC);
		spells[4].targettype = TARGET_ATTACKING;
		spells[4].instant = true;
		spells[4].cooldown = -1;
		spells[4].perctrigger = 2.0f;
		spells[4].attackstoptimer = 1000;
*/
    }
    
    void OnCombatStart(Unit* mTarget)
    {
		SummonTimer = 4;
		CastTime();
		RegisterAIUpdateEvent(_unit->GetUInt32Value(UNIT_FIELD_BASEATTACKTIME));
		//_unit->CastSpell(_unit, spells[0].info, spells[0].instant);
		_unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, "Don't value your life very much, do you?");
		_unit->PlaySoundToSet(11186);
    }

	void CastTime()
	{
		for(int i=0;i<nrspells;i++)
			spells[i].casttime = spells[i].cooldown;
	}

    void OnCombatStop(Unit *mTarget)
    {
		SummonTimer = 4;
		CastTime();
        _unit->GetAIInterface()->setCurrentAgent(AGENT_NULL);
        _unit->GetAIInterface()->SetAIState(STATE_IDLE);
        RemoveAIUpdateEvent();
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
				_unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, "And don't come back!");	// checks here
				_unit->PlaySoundToSet(11187);
				break;
			case 1:
				_unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, "<missing_text>");	// ? ? ! !
				_unit->PlaySoundToSet(11188);
				break;
			}
		}
    }

    void OnDied(Unit * mKiller)
    {
		SummonTimer = 4;
		CastTime();
       RemoveAIUpdateEvent();
		_unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, "Anu... bala belore...alon.");
		_unit->PlaySoundToSet(11192);
    }

    void AIUpdate()
	{
		SummonTimer--;

		if (!SummonTimer)
		{
			_unit->CastSpell(_unit, spells[0].info, spells[0].instant);
			_unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, "I am not alone!");	// checks!
			_unit->PlaySoundToSet(11191);
		}

		float val = sRand.rand(100.0f);
		SpellCast(val);
    }

	void DragonsBreathSound()
	{
		uint32 RandomDragonsBreath;
		RandomDragonsBreath=rand()%10;
		switch (RandomDragonsBreath)
		{
		case 0:
			{
				_unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, "Think you can take the heat?");
				_unit->PlaySoundToSet(11189);
			}break;
		case 1:
			{
				_unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, "Anar'endal dracon!");
				_unit->PlaySoundToSet(11190);
			}break;
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
				spells[i].casttime--;
				
				if (m_spellcheck[i])
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

					if (m_spellcheck[3] == true)
						DragonsBreathSound();

					if (spells[i].speech != "")
					{
						_unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, spells[i].speech.c_str());
						_unit->PlaySoundToSet(spells[i].soundid); 
					}

					m_spellcheck[i] = false;
					return;
				}

				if ((val > comulativeperc && val <= (comulativeperc + spells[i].perctrigger)) || !spells[i].casttime)
				{
					_unit->setAttackTimer(spells[i].attackstoptimer, false);
					m_spellcheck[i] = true;
				}
				comulativeperc += spells[i].perctrigger;
			}
		}
	}

protected:

	int SummonTimer;
	int nrspells;
};

// Pathaleon the Calculator AI

#define CN_PATHALEON_THE_CALCULATOR 19220	

#define MANA_TRAP 36021 // I am not sure to any of those ids =(
#define DOMINATION 36866
#define SILENCE 38491
#define SUMMON_NETHER_WRAITH1 35285	// not the best way, but blizzlike :) (but they don't work for now =()
#define SUMMON_NETHER_WRAITH2 35286
#define SUMMON_NETHER_WRAITH3 35287
#define SUMMON_NETHER_WRAITH4 35288
// hmm... he switches weapons and there is sound for it, but I must know when he does that, how it looks like etc.
// before adding weapon switching =/	(Sound: 11199; speech: "I prefer to be hands-on...";)

class PathaleonTheCalculatorAI : public CreatureAIScript
{
public:
	ADD_CREATURE_FACTORY_FUNCTION(PathaleonTheCalculatorAI);
	SP_AI_Spell spells[7];
	bool m_spellcheck[7];

    PathaleonTheCalculatorAI(Creature* pCreature) : CreatureAIScript(pCreature)
    {
		SummonTimer=rand()%16+30;	// 30 - 45 sec
		nrspells = 7;
		for(int i=0;i<nrspells;i++)
		{
			m_spellcheck[i] = false;
		}

		spells[0].info = sSpellStore.LookupEntry(MANA_TRAP);
		spells[0].targettype = TARGET_ATTACKING;
		spells[0].instant = true;
		spells[0].cooldown = -1;
		spells[0].perctrigger = 8.0f;
		spells[0].attackstoptimer = 1000;

		spells[1].info = sSpellStore.LookupEntry(DOMINATION);
		spells[1].targettype = TARGET_ATTACKING;
		spells[1].instant = true;
		spells[1].cooldown = -1;
		spells[1].perctrigger = 4.0f;
		spells[1].attackstoptimer = 1000;

		spells[2].info = sSpellStore.LookupEntry(SILENCE);
		spells[2].targettype = TARGET_VARIOUS;
		spells[2].instant = true;
		spells[2].cooldown = -1;
		spells[2].perctrigger = 6.0f;
		spells[2].attackstoptimer = 1000;

		spells[3].info = sSpellStore.LookupEntry(SUMMON_NETHER_WRAITH1);
		spells[3].targettype = TARGET_SELF;	// hmm
		spells[3].instant = true;
		spells[3].cooldown = -1;
		spells[3].perctrigger = 0.0f;
		spells[3].attackstoptimer = 1000;

		spells[4].info = sSpellStore.LookupEntry(SUMMON_NETHER_WRAITH2);
		spells[4].targettype = TARGET_SELF;	// hmm
		spells[4].instant = true;
		spells[4].cooldown = -1;
		spells[4].perctrigger = 0.0f;
		spells[4].attackstoptimer = 1000;

		spells[5].info = sSpellStore.LookupEntry(SUMMON_NETHER_WRAITH3);
		spells[5].targettype = TARGET_SELF;	// hmm
		spells[5].instant = true;
		spells[5].cooldown = -1;
		spells[5].perctrigger = 0.0f;
		spells[5].attackstoptimer = 1000;

		spells[6].info = sSpellStore.LookupEntry(SUMMON_NETHER_WRAITH4);
		spells[6].targettype = TARGET_SELF;	// hmm
		spells[6].instant = true;
		spells[6].cooldown = -1;
		spells[6].perctrigger = 0.0f;
		spells[6].attackstoptimer = 1000;
    }
    
    void OnCombatStart(Unit* mTarget)
    {
		SummonTimer=rand()%16+30;
		CastTime();
		RegisterAIUpdateEvent(_unit->GetUInt32Value(UNIT_FIELD_BASEATTACKTIME));
		_unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, "We are on a strict timetable. You will not interfere!");
		_unit->PlaySoundToSet(11193);
    }

	void CastTime()
	{
		for(int i=0;i<nrspells;i++)
			spells[i].casttime = spells[i].cooldown;
	}

    void OnCombatStop(Unit *mTarget)
    {
		SummonTimer=rand()%16+30;
		CastTime();
        _unit->GetAIInterface()->setCurrentAgent(AGENT_NULL);
        _unit->GetAIInterface()->SetAIState(STATE_IDLE);
        RemoveAIUpdateEvent();
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
				_unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, "A minor inconvenience.");
				_unit->PlaySoundToSet(11194);
				break;
			case 1:
				_unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, "Looks like you lose.");
				_unit->PlaySoundToSet(11195);
				break;
			}
		}
    }

    void OnDied(Unit * mKiller)
    {
		SummonTimer=rand()%16+30;
		CastTime();
       RemoveAIUpdateEvent();
		_unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, "The project will... continue.");
		_unit->PlaySoundToSet(11200);
    }

    void AIUpdate()
	{
		SummonTimer--;

		if (!SummonTimer)
		{
			_unit->CastSpell(_unit, spells[3].info, spells[3].instant);
			_unit->CastSpell(_unit, spells[4].info, spells[4].instant);
			_unit->CastSpell(_unit, spells[5].info, spells[5].instant);
			_unit->CastSpell(_unit, spells[6].info, spells[6].instant);
			SummonTimer=rand()%16+30;	// 30 - 45
			_unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, "Time to supplement my work force.");
			_unit->PlaySoundToSet(11196);;
		}

		float val = sRand.rand(100.0f);
		SpellCast(val);
    }

	void DominationSound()
	{
		int RandomDomination;
		RandomDomination=rand()%2;
		switch (RandomDomination)
		{
		case 0:
			_unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, "I'm looking for a team player...");
			_unit->PlaySoundToSet(11197);
			break;
		case 1:
			_unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, "You work for me now!");
			_unit->PlaySoundToSet(11198);
			break;
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
				spells[i].casttime--;
				
				if (m_spellcheck[i])
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

					if (m_spellcheck[1] == true)
					{
						DominationSound();
					}

					if (spells[i].speech != "")
					{
						_unit->SendChatMessage(CHAT_MSG_MONSTER_YELL, LANG_UNIVERSAL, spells[i].speech.c_str());
						_unit->PlaySoundToSet(spells[i].soundid); 
					}

					m_spellcheck[i] = false;
					return;
				}

				if ((val > comulativeperc && val <= (comulativeperc + spells[i].perctrigger)) || !spells[i].casttime)
				{
					_unit->setAttackTimer(spells[i].attackstoptimer, false);
					m_spellcheck[i] = true;
				}
				comulativeperc += spells[i].perctrigger;
			}
		}
	}

protected:

	uint32 SummonTimer;
	int nrspells;
};

void SetupTheMechanar(ScriptMgr * mgr)
{
	mgr->register_creature_script(CN_GATEWATCHER_GYRO_KILL, &GatewatcherGyroKillAI::Create);
	mgr->register_creature_script(CN_GATEWATCHER_IRON_HAND, &GatewatcherIronHandAI::Create);
	mgr->register_creature_script(CN_MECHANO_LORD_CAPACITUS, &MechanoLordCapacitusAI::Create);
	mgr->register_creature_script(CN_NETHERMANCER_SEPETHREA, &NethermancerSepethreaAI::Create);
	mgr->register_creature_script(CN_PATHALEON_THE_CALCULATOR, &PathaleonTheCalculatorAI::Create);
}
