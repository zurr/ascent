#include "StdAfx.h"
#include "Setup.h"

/************************************************************************/
/* Spell Defs                                                           */
/************************************************************************/

bool Charge(uint32 i, Spell* pSpell)
{
    uint32 rage_to_gen;
    switch(pSpell->m_spellInfo->Id)
    {
    case 100:   // Charge Rank 1
        rage_to_gen = 90;
        break;

    case 6178:  // Charge Rank 2
        rage_to_gen = 120;
        break;

    default:    // Charge Rank 3 +
        rage_to_gen = 150;
        break;
    }

    // Add the rage to the caster
    if(!pSpell->u_caster)
        return true;

    uint32 val = pSpell->u_caster->GetUInt32Value(UNIT_FIELD_POWER2);
    uint32 max = pSpell->u_caster->GetUInt32Value(UNIT_FIELD_MAXPOWER2);
    val += rage_to_gen;
    if(val > max)
        val = max;
    
    pSpell->u_caster->SetUInt32Value(UNIT_FIELD_POWER2, val);
  //  script_debuglog("WarriorSpells.cpp :: Charge generate %u rage on "I64FMT, rage_to_gen, pSpell->u_caster->GetGUID());
    return true;
}

bool Execute(uint32 i, Spell* pSpell)
{
    uint32 uSpellId = pSpell->m_spellInfo->Id;
    uint32 base_dmg = pSpell->damage;
    /*
    Attempt to finish off a wounded foe, causing 125 damage and converting each extra point
    of rage into 3 additional damage.  Only usable on enemies that have less than 20% health.
    */

    Unit * target = pSpell->GetUnitTarget();
    if(!target || !pSpell->u_caster) return true;

    // "Only usable on enemies that have less than 20% health."
    if(target->GetHealthPct() > 20)
    {
        // send failed
        pSpell->SendCastResult(SPELL_FAILED_BAD_TARGETS);
        return true;
    }

    // get the caster's rage points, and convert them
    // formula is 3 damage * spell rank * rage points
    uint32 add_damage = (3 * pSpell->m_spellInfo->RankNumber);
    add_damage *= pSpell->u_caster->GetUInt32Value(UNIT_FIELD_POWER2) / 10;   // rage is *10 always
    
    // send spell damage log
    pSpell->u_caster->SpellNonMeleeDamageLog(target, uSpellId, base_dmg + add_damage, false);

    // zero rage
    pSpell->u_caster->SetUInt32Value(UNIT_FIELD_POWER2, 0);
    return true;
}

/* Module info */
void SetupWarriorSpells(ScriptMgr * mgr)
{
    /**** Charge ****/
    mgr->register_dummy_spell(100, &Charge);      // Rank 1
    mgr->register_dummy_spell(6178, &Charge);     // Rank 2
    mgr->register_dummy_spell(11578, &Charge);    // Rank 3

    /**** Execute ****/
    mgr->register_dummy_spell(5308, &Execute);     // Rank 1
    mgr->register_dummy_spell(20660, &Execute);    // Rank 2
    mgr->register_dummy_spell(20661, &Execute);    // Rank 3
    mgr->register_dummy_spell(20662, &Execute);    // Rank 4
    mgr->register_dummy_spell(25234, &Execute);    // Rank 5
    mgr->register_dummy_spell(25236, &Execute);    // Rank 6
}
