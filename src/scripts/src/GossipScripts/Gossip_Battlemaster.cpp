#include "StdAfx.h"
#include "Setup.h"

class SCRIPT_DECL WarsongGulchBattlemaster : public GossipScript
{
public:
    ADD_GOSSIP_FACTORY_FUNCTION(WarsongGulchBattlemaster);
    void GossipHello(Creature * pCreature, Player * plr, bool AutoSend)
    {
        script_debuglog("Gossip-Battlemaster.cpp : WSG Hello by %s", plr->GetName());

        GossipMenu *Menu;
        uint32 Team = plr->GetTeam();
        if(Team > 1) Team = 1;
        
        // Check if the player can be entered into the bg or not.
        if(plr->getLevel() < 10)
        {
            uint32 FactMessages[2] = { 7599, 7688 };

            // Send "you cannot enter" message.
            objmgr.CreateGossipMenuForPlayer(&Menu, pCreature->GetGUID(), FactMessages[Team], plr);

            if(Rand(30.0f))    // 30% chance to send message
            {
                char message[200];
                sprintf(message, "Sorry, %s, you are not skilled enough to enter the hazards of Warsong Gulch. Come back when you have trained some more.", plr->GetName());
                pCreature->SendChatMessage(CHAT_MSG_MONSTER_SAY, LANG_UNIVERSAL, message);
            }
        }
        else
        {
            uint32 FactMessages[2] = { 7689, 7705 }; // need to find the second one

            // Send "you cannot enter" message.
            objmgr.CreateGossipMenuForPlayer(&Menu, pCreature->GetGUID(), FactMessages[Team], plr);
            Menu->AddItem( 0, "I would like to enter the battleground.", 1);
        }
        
        if(AutoSend)
            Menu->SendTo(plr);
    }

    void GossipSelectOption(Creature * pCreature, Player * plr, uint32 Id, uint32 IntId)
    {
        // Check the player is capable.
        if(plr->getLevel() < 10)
        {
            // We shouldn't actually get here.. it means they've cheated with packets or
            // are using a bot :p
            pCreature->SendChatMessage(CHAT_MSG_MONSTER_SAY, LANG_UNIVERSAL, "You cannot enter the battleground." );
        }

        // Send battleground list.
        plr->GetSession()->SendBattlegroundList(pCreature, 2);  // WSG = 2
    }

    void Destroy()
    {
        delete this;
    }
};

class SCRIPT_DECL ArathiBasinBattlemaster : public GossipScript
{
public:
    ADD_GOSSIP_FACTORY_FUNCTION(ArathiBasinBattlemaster);

    void GossipHello(Creature * pCreature, Player * plr, bool AutoSend)
    {
        script_debuglog("Gossip-Battlemaster.cpp : AB Hello by %s", plr->GetName());

        GossipMenu *Menu;
        uint32 Team = plr->GetTeam();
        if(Team > 1) Team = 1;

        // Check if the player can be entered into the bg or not.
        if(plr->getLevel() < 20)
        {
            uint32 FactMessages[2] = { 7700, 7667 };

            // Send "you cannot enter" message.
            objmgr.CreateGossipMenuForPlayer(&Menu, pCreature->GetGUID(), FactMessages[Team], plr);

            if(Rand(30.0f))    // 30% chance to send message
            {
                char message[200];
                sprintf(message, "You cannot help our struggle yet, %s. Once you are stronger come back and join the fight!", plr->GetName());
                pCreature->SendChatMessage(CHAT_MSG_MONSTER_SAY, LANG_UNIVERSAL, message);
            }
        }
        else
        {
            uint32 FactMessages[2] = { 7700, 7555 }; // need to find the second one

            // Send "you cannot enter" message.
            objmgr.CreateGossipMenuForPlayer(&Menu, pCreature->GetGUID(), FactMessages[Team], plr);
            Menu->AddItem( 0, "I would like to enter the battleground.", 1);
        }

        if(AutoSend)
            Menu->SendTo(plr);
    }

    void GossipSelectOption(Creature * pCreature, Player * plr, uint32 Id, uint32 IntId)
    {
        // Check the player is capable.
        if(plr->getLevel() < 20)
        {
            // We shouldn't actually get here.. it means they've cheated with packets or
            // are using a bot :p
            pCreature->SendChatMessage(CHAT_MSG_MONSTER_SAY, LANG_UNIVERSAL, "You cannot enter the battleground." );
        }

        // Send battleground list.
        plr->GetSession()->SendBattlegroundList(pCreature, 3);  // WSG = 3
    }

    void Destroy()
    {
        delete this;
    }
};

class SCRIPT_DECL AlteracValleyBattlemaster : public GossipScript
{
public:
    ADD_GOSSIP_FACTORY_FUNCTION(AlteracValleyBattlemaster);
    void GossipHello(Creature * pCreature, Player * plr, bool AutoSend)
    {
        script_debuglog("Gossip-Battlemaster.cpp : AV Hello by %s", plr->GetName());

        GossipMenu *Menu;
        uint32 Team = plr->GetTeam();
        if(Team > 1) Team = 1;

        // Check if the player can be entered into the bg or not.
        if(plr->getLevel() < 60)
        {
            uint32 FactMessages[2] = { 7658, 7658 };

            // Send "you cannot enter" message.
            objmgr.CreateGossipMenuForPlayer(&Menu, pCreature->GetGUID(), FactMessages[Team], plr);

            if(Rand(30.0f))    // 30% chance to send message
            {
                char message[200];
                sprintf(message, "Only our best warriors can enter Alterac Valley, %s. Come back when you are more experienced.", plr->GetName());
                pCreature->SendChatMessage(CHAT_MSG_MONSTER_SAY, LANG_UNIVERSAL, message);
            }
        }
        else
        {
            uint32 FactMessages[2] = { 7658, 7659 }; // need to find the second one

            // Send "you cannot enter" message.
            objmgr.CreateGossipMenuForPlayer(&Menu, pCreature->GetGUID(), FactMessages[Team], plr);
            Menu->AddItem( 0, "I would like to enter the battleground.", 1);
        }

        if(AutoSend)
            Menu->SendTo(plr);
    }

    void GossipSelectOption(Creature * pCreature, Player * plr, uint32 Id, uint32 IntId)
    {
        // Check the player is capable.
        if(plr->getLevel() < 60)
        {
            // We shouldn't actually get here.. it means they've cheated with packets or
            // are using a bot :p
            pCreature->SendChatMessage(CHAT_MSG_MONSTER_SAY, LANG_UNIVERSAL, "You cannot enter the battleground." );
        }

        // Send battleground list.
        plr->GetSession()->SendBattlegroundList(pCreature, 1);  // AV = 2
    }

    void Destroy()
    {
        delete this;
    }
};

void SetupBattlemaster(ScriptMgr * mgr)
{
    /* Battlemaster List */
    mgr->register_gossip_script(15102, &WarsongGulchBattlemaster::Create);
    mgr->register_gossip_script(14981, &WarsongGulchBattlemaster::Create);
    mgr->register_gossip_script(14982, &WarsongGulchBattlemaster::Create);
    mgr->register_gossip_script(2302, &WarsongGulchBattlemaster::Create);
    mgr->register_gossip_script(10360, &WarsongGulchBattlemaster::Create);
    mgr->register_gossip_script(3890, &WarsongGulchBattlemaster::Create);
    mgr->register_gossip_script(15008, &ArathiBasinBattlemaster::Create);
    mgr->register_gossip_script(857, &ArathiBasinBattlemaster::Create);
    mgr->register_gossip_script(907, &ArathiBasinBattlemaster::Create);
    mgr->register_gossip_script(12198, &ArathiBasinBattlemaster::Create);
    mgr->register_gossip_script(14990, &ArathiBasinBattlemaster::Create);
    mgr->register_gossip_script(15006, &ArathiBasinBattlemaster::Create);
    mgr->register_gossip_script(7410, &AlteracValleyBattlemaster::Create);
    mgr->register_gossip_script(12197, &AlteracValleyBattlemaster::Create);
    mgr->register_gossip_script(5118, &AlteracValleyBattlemaster::Create);
    mgr->register_gossip_script(15106, &AlteracValleyBattlemaster::Create);
    mgr->register_gossip_script(7247, &AlteracValleyBattlemaster::Create);
    mgr->register_gossip_script(14942, &AlteracValleyBattlemaster::Create);
}
