// Warrior Dummy Spell Module

#include "StdAfx.h"
#include "Setup.h"

/************************************************************************/
/* Spell Defs                                                           */
/************************************************************************/

bool RockbiterWeapon(uint32 i, Spell* pSpell)
{
    uint32 enchantment_entry = 0;
    switch(pSpell->m_spellInfo->RankNumber)
    {
    case 1:
        enchantment_entry = 3021;
        break;
    case 2:
        enchantment_entry = 3024;
        break;
    case 3:
        enchantment_entry = 3027;
        break;
    case 4:
        enchantment_entry = 3030;
        break;
    case 5:
        enchantment_entry = 3033;
        break;
    case 6:
        enchantment_entry = 3036;
        break;
    case 7:
        enchantment_entry = 3039;
        break;
    case 8:
        enchantment_entry = 3042;
        break;
    }

    if(!enchantment_entry || !pSpell->p_caster)
        return true;

    Item * item = pSpell->p_caster->GetItemInterface()->GetInventoryItem(EQUIPMENT_SLOT_MAINHAND);
    EnchantEntry * enchant = sEnchantStore.LookupEntry(enchantment_entry);
    if(!item || !enchant)
        return true;

    int32 Slot = item->HasEnchantment(enchant->Id);
    if(Slot >= 0)
        item->ModifyEnchantmentTime(Slot, 1800);
    else
    {
        Slot = item->AddEnchantment(enchant, 1800, false, true);   // 5min
        if(Slot < 0) return true;
        item->ApplyEnchantmentBonus(Slot, APPLY);
    }

    script_debuglog("ShamanSpells.cpp :: Rockbiter Weapon Rank %u, enchant %u, slot %u", pSpell->m_spellInfo->RankNumber,
        enchantment_entry, Slot);
    
    return true;
}

void SetupShamanSpells(ScriptMgr * mgr)
{
    mgr->register_dummy_spell(8017, &RockbiterWeapon);
    mgr->register_dummy_spell(8018, &RockbiterWeapon);
    mgr->register_dummy_spell(8019, &RockbiterWeapon);
    mgr->register_dummy_spell(8020, &RockbiterWeapon);
    mgr->register_dummy_spell(8021, &RockbiterWeapon);
    mgr->register_dummy_spell(8022, &RockbiterWeapon);
}
