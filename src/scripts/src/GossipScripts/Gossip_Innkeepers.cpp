#include "StdAfx.h"
#include "Setup.h"

class SCRIPT_DECL InnkeeperGossip : public GossipScript
{
public:
    ADD_GOSSIP_FACTORY_FUNCTION(InnkeeperGossip);
    void GossipHello(Creature* pCreature, Player* Plr, bool AutoSend);
    void GossipSelectOption(Creature* pCreature, Player* Plr, uint32 Id, uint32 IntId);
    void GossipEnd(Creature* pCreature, Player* Plr);
};

void InnkeeperGossip::GossipHello(Creature* pCreature, Player* Plr, bool AutoSend)
{
    script_debuglog("Gossip-Innkeepers.cpp : Hello by %s", Plr->GetName());
    GossipMenu *Menu;
    uint32 TextID = 820;
    // Hello (First) Menu.
    objmgr.CreateGossipMenuForPlayer(&Menu, pCreature->GetGUID(), TextID, Plr);

    if( pCreature->GetUInt32Value(UNIT_NPC_FLAGS) & UNIT_NPC_FLAG_VENDOR )
        Menu->AddItem( 1, "I would like to browse your goods.", 1 );
    
    Menu->AddItem( 5, "Make this inn your home.", 2 );
    Menu->AddItem( 0, "What can I do at an inn?", 3 );
    if(AutoSend)
        Menu->SendTo(Plr);
}

#define SendQuickMenu(textid) objmgr.CreateGossipMenuForPlayer(&Menu, pCreature->GetGUID(), textid, Plr); \
    Menu->SendTo(Plr);

void InnkeeperGossip::GossipSelectOption(Creature* pCreature, Player* Plr, uint32 Id, uint32 IntId)
{
    script_debuglog("Gossip-Innkeepers.cpp : Select option %u by %s", IntId, Plr->GetName());
    switch(IntId)
    {
    case 1:     // VENDOR
        Plr->GetSession()->SendInventoryList(pCreature);
        break;
    case 2:     // BINDER
        Plr->GetSession()->SendInnkeeperBind(pCreature);
        break;
    case 3:     // WHAT CAN I DO ?
        // Prepare second menu
        GossipMenu *Menu;
        objmgr.CreateGossipMenuForPlayer(&Menu, pCreature->GetGUID(), 1853, Plr);
        Menu->AddItem(5, "Make this inn your home.", 2);
        Menu->SendTo(Plr);
        break;
    }
}

void InnkeeperGossip::GossipEnd(Creature* pCreature, Player* Plr)
{
    GossipScript::GossipEnd(pCreature, Plr);
}

void SetupInnkeepers(ScriptMgr * mgr)
{
    /* Innkeeper List */
    mgr->register_gossip_script(295, &InnkeeperGossip::Create);      // Innkeeper Farley
    mgr->register_gossip_script(1247, &InnkeeperGossip::Create);     // Innkeeper Belm
    mgr->register_gossip_script(1464, &InnkeeperGossip::Create);     // Innkeeper Helbrek
    mgr->register_gossip_script(2352, &InnkeeperGossip::Create);     // Innkeeper Anderson
    mgr->register_gossip_script(2388, &InnkeeperGossip::Create);     // Innkeeper Shay
    mgr->register_gossip_script(3934, &InnkeeperGossip::Create);     // Innkeeper Boorand Plainswind
    mgr->register_gossip_script(5111, &InnkeeperGossip::Create);     // Innkeeper Firebrew
    mgr->register_gossip_script(5688, &InnkeeperGossip::Create);     // Innkeeper Renee
    mgr->register_gossip_script(5814, &InnkeeperGossip::Create);     // Innkeeper Thulbek
    mgr->register_gossip_script(6272, &InnkeeperGossip::Create);     // Innkeeper Janene
    mgr->register_gossip_script(6727, &InnkeeperGossip::Create);     // Innkeeper Brianna
    mgr->register_gossip_script(6734, &InnkeeperGossip::Create);     // Innkeeper Hearthstove
    mgr->register_gossip_script(6735, &InnkeeperGossip::Create);     // Innkeeper Saelienne
    mgr->register_gossip_script(6736, &InnkeeperGossip::Create);     // Innkeeper Keldamyr
    mgr->register_gossip_script(6737, &InnkeeperGossip::Create);     // Innkeeper Shaussiy
    mgr->register_gossip_script(6738, &InnkeeperGossip::Create);     // Innkeeper Kimlya
    mgr->register_gossip_script(6739, &InnkeeperGossip::Create);     // Innkeeper Bates
    mgr->register_gossip_script(6740, &InnkeeperGossip::Create);     // Innkeeper Allison
    mgr->register_gossip_script(6741, &InnkeeperGossip::Create);     // Innkeeper Norman
    mgr->register_gossip_script(6746, &InnkeeperGossip::Create);     // Innkeeper Pala
    mgr->register_gossip_script(6747, &InnkeeperGossip::Create);     // Innkeeper Kauth
    mgr->register_gossip_script(6778, &InnkeeperGossip::Create);     // Melika Isenstrider
    mgr->register_gossip_script(6790, &InnkeeperGossip::Create);     // Innkeeper Trelayne
    mgr->register_gossip_script(6791, &InnkeeperGossip::Create);     // Innkeeper Wiley
    mgr->register_gossip_script(6806, &InnkeeperGossip::Create);     // Tannok Frosthammer
    mgr->register_gossip_script(6807, &InnkeeperGossip::Create);     // Innkeeper Skindle
    mgr->register_gossip_script(6928, &InnkeeperGossip::Create);     // Innkeeper Grosk
    mgr->register_gossip_script(6929, &InnkeeperGossip::Create);     // Innkeeper Gryshka
    mgr->register_gossip_script(6930, &InnkeeperGossip::Create);     // Innkeeper Karakul
    mgr->register_gossip_script(7714, &InnkeeperGossip::Create);     // Innkeeper Byula
    mgr->register_gossip_script(7731, &InnkeeperGossip::Create);     // Innkeeper Jayka
    mgr->register_gossip_script(7733, &InnkeeperGossip::Create);     // Innkeeper Fizzgrimble
    mgr->register_gossip_script(7736, &InnkeeperGossip::Create);     // Innkeeper Shyria
    mgr->register_gossip_script(7737, &InnkeeperGossip::Create);     // Innkeeper Greul
    mgr->register_gossip_script(7744, &InnkeeperGossip::Create);     // Innkeeper Thulfram
    mgr->register_gossip_script(8931, &InnkeeperGossip::Create);     // Innkeeper Heather
    mgr->register_gossip_script(9356, &InnkeeperGossip::Create);     // Innkeeper Shul'kar
    mgr->register_gossip_script(9501, &InnkeeperGossip::Create);     // Innkeeper Adegwa
    mgr->register_gossip_script(11103, &InnkeeperGossip::Create);    // Innkeeper Lyshaerya
    mgr->register_gossip_script(11106, &InnkeeperGossip::Create);    // Innkeeper Sikewa
    mgr->register_gossip_script(11116, &InnkeeperGossip::Create);    // Innkeeper Abeqwa
    mgr->register_gossip_script(11118, &InnkeeperGossip::Create);    // Innkeeper Vizzie
    mgr->register_gossip_script(12196, &InnkeeperGossip::Create);    // Innkeeper Kaylisk
    mgr->register_gossip_script(14731, &InnkeeperGossip::Create);    // Lard
    mgr->register_gossip_script(15174, &InnkeeperGossip::Create);    // Calandrath
}
