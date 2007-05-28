#include "StdAfx.h"
#include "Setup.h"

/************************************************************************/
/* GENERAL GUARD SCRIPT                                                 */
/************************************************************************/

// Covers *all* guard types, scripting their texts to guide players around.
// Enable this define to make all gossip texts have a "back" / "I was looking
// for somethinge else" button added.

#define BACK_BUTTON

#ifdef BACK_BUTTON

// Make code neater with this define.
#define SendQuickMenu(textid) objmgr.CreateGossipMenuForPlayer(&Menu, pCreature->GetGUID(), textid, Plr); \
    Menu->SendTo(Plr);

#else

// Make code neater with this define.
#define SendQuickMenu(textid) objmgr.CreateGossipMenuForPlayer(&Menu, pCreature->GetGUID(), textid, Plr); \
    Menu->AddItem(0, "I was looking for something else.", 0); \
    Menu->SendTo(Plr);

#endif

/************************************************************************/
/* Stormwind CITY Guards                                                */
/************************************************************************/

class SCRIPT_DECL StormwindGuard : public GossipScript
{
public:
    ADD_GOSSIP_FACTORY_FUNCTION(StormwindGuard);
    void GossipHello(Creature * pCreature, Player * Plr, bool AutoSend)
    {
        GossipMenu *Menu;
        objmgr.CreateGossipMenuForPlayer(&Menu, pCreature->GetGUID(), 2593, Plr);

        Menu->AddItem(0, "Auction House", 1);
        Menu->AddItem(0, "Bank of Stormwind", 2);
        Menu->AddItem(0, "Deeprun Tram", 3);
        Menu->AddItem(0, "The Inn", 4);
        Menu->AddItem(0, "Gryphon Master", 5);    
        Menu->AddItem(0, "Guild Master", 6);
        Menu->AddItem(0, "Mailbox", 7);
        Menu->AddItem(0, "Stable Master", 8);
        Menu->AddItem(0, "Weapons Trainer", 9);
        Menu->AddItem(0, "Officers' Lounge", 10);
        Menu->AddItem(0, "Battlemaster", 11);
        Menu->AddItem(0, "Class Trainer", 12);
        Menu->AddItem(0, "Profession Trainer", 13);

        if(AutoSend)
            Menu->SendTo(Plr);
    }

    void GossipSelectOption(Creature* pCreature, Player* Plr, uint32 Id, uint32 IntId)
    {
        GossipMenu * Menu;
        switch(IntId)
        {
        case 0:     // Return to start
            GossipHello(pCreature, Plr, true);
            break;

            //////////////////////
            // Main menu handlers
            /////

        case 1:     // Auction House
            SendQuickMenu(3834);
            Plr->Gossip_SendPOI(-8811.46, 667.46, 6, 6, 0, "Stormwind Auction House");
            break;

        case 2:     // Bank of Stormwind
            SendQuickMenu(764);
            Plr->Gossip_SendPOI(-8916.87, 622.87, 6, 6, 0, "Stormwind Bank");
            break;

        case 3:     // Deeprun Tram
            SendQuickMenu(3813);
            Plr->Gossip_SendPOI(-8378.88, 554.23, 6, 6, 0, "The Deeprun Tram");
            break;

        case 4:     // The Inn
            SendQuickMenu(3860);
            Plr->Gossip_SendPOI(-8869.0, 675.4, 6, 6, 0, "The Gilded Rose");
            break;

        case 5:     // Gryphon Master
            SendQuickMenu(879);
            Plr->Gossip_SendPOI(-8837.0, 493.5, 6, 6, 0, "Stormwind Gryphon Master");
            break;

        case 6:     // Guild Master
            SendQuickMenu(882);
            Plr->Gossip_SendPOI(-8894.0, 611.2, 6, 6, 0, "Stormwind Vistor`s Center");
            break;

        case 7:     // Mailbox
            SendQuickMenu(3518);
            Plr->Gossip_SendPOI(-8876.48, 649.18, 6, 6, 0, "Stormwind Mailbox");
            break;

        case 8:     // Stable Master
            SendQuickMenu(5984);
            Plr->Gossip_SendPOI(-8433.0, 554.7, 6, 6, 0, "Jenova Stoneshield");
            break;

        case 9:     // Weapons Master
            SendQuickMenu(4516);
            Plr->Gossip_SendPOI(-8797.0, 612.8, 6, 6, 0, "Woo Ping");
            break;

        case 10:    // Officers' Lounge
            SendQuickMenu(7047);
            Plr->Gossip_SendPOI(-8759.92, 399.69, 6, 6, 0, "Champions` Hall");
            break;

        case 11:    // Battlemaster
            {
			SendQuickMenu(7499);
            Plr->Gossip_SendPOI(-8393.62, 274.21, 6, 6, 0, "Battlemasters Stormwind");
            }break;

        case 12:    // Class Trainers
            {
                objmgr.CreateGossipMenuForPlayer(&Menu, pCreature->GetGUID(), 4264, Plr);
				Menu->AddItem( 0, "Druid"      , 14);
                Menu->AddItem( 0, "Hunter"     , 15);
                Menu->AddItem( 0, "Mage"       , 16);
                Menu->AddItem( 0, "Paladin"    , 17);
                Menu->AddItem( 0, "Priest"     , 18);
                Menu->AddItem( 0, "Rogue"      , 19);
                Menu->AddItem( 0, "Shaman"     , 20);
                Menu->AddItem( 0, "Warlock"    , 21);
				Menu->AddItem( 0, "Warrior"    , 22);
                Menu->SendTo(Plr);
            }break;

        case 13:    // Profession Trainers
            {
                objmgr.CreateGossipMenuForPlayer(&Menu, pCreature->GetGUID(), 4264, Plr);
                Menu->AddItem( 0, "Alchemy"           , 23);
                Menu->AddItem( 0, "Blacksmithing"     , 24);
                Menu->AddItem( 0, "Cooking"           , 25);
                Menu->AddItem( 0, "Enchanting"        , 26);
                Menu->AddItem( 0, "Engineering"       , 27);
                Menu->AddItem( 0, "First Aid"         , 28);
                Menu->AddItem( 0, "Fishing"           , 29);
                Menu->AddItem( 0, "Herbalism"         , 30);
                Menu->AddItem( 0, "Leatherworking"    , 31);
                Menu->AddItem( 0, "Mining"            , 32);
                Menu->AddItem( 0, "Skinning"          , 33);
                Menu->AddItem( 0, "Tailoring"         , 34);
                Menu->SendTo(Plr);
            }break;

            ////////////////
            // Class trainer submenu
            ////////

        case 14: //Druid
            {
                Plr->Gossip_SendPOI(-8751.0, 1124.5, 6, 6, 0, "The Park");
                SendQuickMenu(902);
            }break;

        case 15: //Hunter
            {
                Plr->Gossip_SendPOI(-8413.0, 541.5, 6, 6, 0, "Hunter Lodge");
                SendQuickMenu(905);
            }break;

        case 16: //Mage
            {
                Plr->Gossip_SendPOI(-9012.0, 867.6, 6, 6, 0, "Wizard`s Sanctum");
                SendQuickMenu(899);
            }break;

        case 17: //Paladin
            {
                Plr->Gossip_SendPOI(-8577.0, 881.7, 6, 6, 0, "Cathedral Of Light");
                SendQuickMenu(904);
            }break;

        case 18: //Priest
            {
                Plr->Gossip_SendPOI(-8512.0, 862.4, 6, 6, 0, "Cathedral Of Light");
                SendQuickMenu(903);
            }break;

        case 19: //Rogue
            {
                Plr->Gossip_SendPOI(-8753.0, 367.8, 6, 6, 0, "Stormwind - Rogue House");
                SendQuickMenu(900);
            }break;

        case 20: //Shaman
            {
                Plr->Gossip_SendPOI(-8624.54, 402.61, 6, 6, 0, "Farseer Umbrua");
                SendQuickMenu(10106);
            }break;

		case 21: //Warlock
            {
                Plr->Gossip_SendPOI(-8948.91, 998.35, 6, 6, 0, "The Slaughtered Lamb");
                SendQuickMenu(906);
            }break;

        case 22: //Warrior
            {
                Plr->Gossip_SendPOI(-8624.54, 402.61, 6, 6, 0, "Stormwind Barracks");
                SendQuickMenu(901);
            }break;

        case 23: //Alchemy
            {
                Plr->Gossip_SendPOI(-8988.0, 759.60, 6, 6, 0, "Alchemy Needs");
                SendQuickMenu(919);
            }break;

        case 24: //Blacksmithing
            {
                Plr->Gossip_SendPOI(-8424.0, 616.9, 6, 6, 0, "Therum Deepforge");
                SendQuickMenu(920);
            }break;

        case 25: //Cooking
            {
                Plr->Gossip_SendPOI(-8611.0, 364.6, 6, 6, 0, "Pig and Whistle Tavern");
                SendQuickMenu(921);
            }break;

        case 26: //Enchanting
            {
                Plr->Gossip_SendPOI(-8858.0, 803.7, 6, 6, 0, "Lucan Cordell");
                SendQuickMenu(941);
            }break;

        case 27: //Engineering
            {
                Plr->Gossip_SendPOI(-8347.0, 644.1, 6, 6, 0, "Lilliam Sparkspindle");
                SendQuickMenu(922);
            }break;

        case 28: //First Aid
            {
                Plr->Gossip_SendPOI(-8513.0, 801.8, 6, 6, 0, "Shaina Fuller");
                SendQuickMenu(923);
            }break;

        case 29: //Fishing
            {
                Plr->Gossip_SendPOI(-8803.0, 767.5, 6, 6, 0, "Arnold Leland");
                SendQuickMenu(940);
            }break;

        case 30: //Herbalism
            {
                Plr->Gossip_SendPOI(-8967.0, 779.5, 6, 6, 0, "Alchemy Needs");
                SendQuickMenu(924);
            }break;

        case 31: //Leatherworking
            {
                Plr->Gossip_SendPOI(-8726.0, 477.4, 6, 6, 0, "The Protective Hide");
                SendQuickMenu(925);
            }break;

        case 32: //Mining
            {
                Plr->Gossip_SendPOI(-8434.0, 692.8, 6, 6, 0, "Gelman Stonehand");
                SendQuickMenu(927);
            }break;

        case 33: //Skinning
            {
                Plr->Gossip_SendPOI(-8716.0, 469.4, 6, 6, 0, "The Protective Hide");
                SendQuickMenu(928);
            }break;

        case 34: //Tailoring
            {
                Plr->Gossip_SendPOI(-8938.0, 800.7, 6, 6, 0, "Duncan`s Textiles");
                SendQuickMenu(929);
            }break;
        }
    }
};

class SCRIPT_DECL DarnassusGuard : public GossipScript
{
public:
    ADD_GOSSIP_FACTORY_FUNCTION(DarnassusGuard);
    void GossipHello(Creature * pCreature, Player * Plr, bool AutoSend)
    {
        GossipMenu *Menu;
        objmgr.CreateGossipMenuForPlayer(&Menu, pCreature->GetGUID(), 3016, Plr);

        Menu->AddItem( 0, "Auction House"      , 1);
        Menu->AddItem( 0, "The Bank"           , 2);
        Menu->AddItem( 0, "Hippogryph Master"  , 3);
        Menu->AddItem( 0, "Guild Master"       , 4);
        Menu->AddItem( 0, "The Inn"            , 5);
        Menu->AddItem( 0, "Mailbox"            , 6);
        Menu->AddItem( 0, "Stable Master"      , 7);
        Menu->AddItem( 0, "Weapons Trainer"    , 8);
        Menu->AddItem( 0, "Battlemaster"       , 9);
        Menu->AddItem( 0, "Class Trainer"      , 10);
	 	Menu->AddItem( 0, "Profession Trainer" , 11);

        if(AutoSend)
            Menu->SendTo(Plr);
    }

    void GossipSelectOption(Creature* pCreature, Player* Plr, uint32 Id, uint32 IntId)
    {
        GossipMenu * Menu;
        switch(IntId)
        {
        case 0:     // Return to start
            GossipHello(pCreature, Plr, true);
            break;

            //////////////////////
            // Main menu handlers (Most/All 'borrowed' from scriptdev)
            /////

        case 1:     // Auction House
			SendQuickMenu(3833);
			Plr->Gossip_SendPOI(9861.23, 2334.55, 6, 6, 0, "Darnassus Auction House");
			break;
			
		case 2:		// The Bank
			SendQuickMenu(3017);
			Plr->Gossip_SendPOI(9938.45, 2512.35, 6, 6, 0, "Darnassus Bank");
			break;
		
		case 3:		// Hippogryph Master
			SendQuickMenu(3018);
			Plr->Gossip_SendPOI(9945.65, 2618.94, 6, 6, 0, "Rut'theran Village");
			break;
			
		case 4:		// Guild Master
			SendQuickMenu(3019);
			Plr->Gossip_SendPOI(10076.40, 2199.59, 6, 6, 0, "Darnassus Guild Master");
			break;
			
		case 5:		// The Inn
			SendQuickMenu(3020);
			Plr->Gossip_SendPOI(10133.29, 2222.52, 6, 6, 0, "Darnassus Inn");
			break;
			
		case 6:		// Mailbox
			SendQuickMenu(3021);
			Plr->Gossip_SendPOI(9942.17, 2495.48, 6, 6, 0, "Darnassus Mailbox");
			break;
		
		case 7:		// Stable Master
			SendQuickMenu(5980);
			Plr->Gossip_SendPOI(10167.20, 2522.66, 6, 6, 0, "Alassin");
			break;
			
		case 8:		// Weapons Trainer
			SendQuickMenu(4517);
			Plr->Gossip_SendPOI(9907.11, 2329.70, 6, 6, 0, "Ilyenia Moonfire");
			break;
			
		case 9:    // Battlemaster
            {
				SendQuickMenu(7519);
				Plr->Gossip_SendPOI(9981.9, 2325.9, 6, 6, 0, "Battlemasters Darnassus");
            }break;
			
		case 10:    // Class Trainer
            {
                objmgr.CreateGossipMenuForPlayer(&Menu, pCreature->GetGUID(), 4264, Plr);
                Menu->AddItem( 0, "Druid"       , 12);
                Menu->AddItem( 0, "Hunter"      , 13);
                Menu->AddItem( 0, "Priest"      , 14);
                Menu->AddItem( 0, "Rogue"       , 15);
                Menu->AddItem( 0, "Warrior"     , 16);
                Menu->SendTo(Plr);
            }break;

        case 11:    // Profession Trainer
            {
                objmgr.CreateGossipMenuForPlayer(&Menu, pCreature->GetGUID(), 4273, Plr);
                Menu->AddItem( 0, "Alchemy"           , 17);
                Menu->AddItem( 0, "Cooking"           , 18);
                Menu->AddItem( 0, "Enchanting"        , 19);
                Menu->AddItem( 0, "First Aid"         , 20);
                Menu->AddItem( 0, "Fishing"           , 21);
                Menu->AddItem( 0, "Herbalism"         , 22);
                Menu->AddItem( 0, "Leatherworking"    , 23);
                Menu->AddItem( 0, "Skinning"          , 24);
                Menu->AddItem( 0, "Tailoring"         , 25);
                Menu->SendTo(Plr);
            }break;
		
		case 12:	// Druid
            {
                Plr->Gossip_SendPOI(10186, 2570.46, 6, 6, 0, "Darnassus Druid Trainer");
                SendQuickMenu(3024);
            }break;

		case 13:	// Hunter
            {
                Plr->Gossip_SendPOI(10177.29, 2511.10, 6, 6, 0, "Darnassus Hunter Trainer");
                SendQuickMenu(3023);
            }break;

        case 14:	// Priest
            {
                Plr->Gossip_SendPOI(9659.12, 2524.88, 6, 6, 0, "Temple of the Moon");
                SendQuickMenu(3025);
            }break;
	
        case 15:	// Rogue
            {
                Plr->Gossip_SendPOI(10122, 2599.12, 6, 6, 0, "Darnassus Rogue Trainer");
                SendQuickMenu(3026);
            }break;

        case 16:	// Warrior
            {
                Plr->Gossip_SendPOI(9951.91, 2280.38, 6, 6, 0, "Warrior's Terrace");
                SendQuickMenu(3033);
            }break;
			
		case 17: //Alchemy
            {
                Plr->Gossip_SendPOI(10075.90, 2356.76, 6, 6, 0, "Darnassus Alchemy Trainer");
                SendQuickMenu(3035);
            }break;

        case 18: //Cooking
            {
                Plr->Gossip_SendPOI(10088.59, 2419.21, 6, 6, 0, "Darnassus Cooking Trainer");
                SendQuickMenu(3036);
            }break;
			
        case 19: //Enchanting
            {
                Plr->Gossip_SendPOI(10146.09, 2313.42, 6, 6, 0, "Darnassus Enchanting Trainer");
                SendQuickMenu(3337);
            }break;

        case 20: //First Aid
            {
                Plr->Gossip_SendPOI(10150.09, 2390.43, 6, 6, 0, "Darnassus First Aid Trainer");
                SendQuickMenu(3037);
            }break;

        case 21: //Fishing
            {
                Plr->Gossip_SendPOI(9836.20, 2432.17, 6, 6, 0, "Darnassus Fishing Trainer");
                SendQuickMenu(3038);
            }break;

        case 22: //Herbalism
            {
                Plr->Gossip_SendPOI(9757.17, 2430.16, 6, 6, 0, "Darnassus Herbalism Trainer");
                SendQuickMenu(3039);
            }break;

        case 23: //Leatherworking
            {
                Plr->Gossip_SendPOI(10086.59, 2255.77, 6, 6, 0, "Darnassus Leatherworking Trainer");
                SendQuickMenu(3040);
            }break;

        case 24: //Skinning
            {
                Plr->Gossip_SendPOI(10081.40, 2257.18, 6, 6, 0, "Darnassus Skinning Trainer");
                SendQuickMenu(3042);
            }break;

        case 25: //Tailoring
            {
                Plr->Gossip_SendPOI(10079.70, 2268.19, 6, 6, 0, "Darnassus Tailor");
                SendQuickMenu(3044);
            }break;


		}
	}
    void GossipEnd(Creature* pCreature, Player* Plr)
    {

    }
};

class SCRIPT_DECL GoldshireGuard : public GossipScript
{
public:
    ADD_GOSSIP_FACTORY_FUNCTION(GoldshireGuard);
    void GossipHello(Creature * pCreature, Player * Plr, bool AutoSend)
    {
        GossipMenu *Menu;
        objmgr.CreateGossipMenuForPlayer(&Menu, pCreature->GetGUID(), 2593, Plr);
		Menu->AddItem( 0, "Bank"                 , 1);
		Menu->AddItem( 0, "Gryphon Master"       , 2);
		Menu->AddItem( 0, "Guild Master"         , 3);
		Menu->AddItem( 0, "Inn"                  , 4);
		Menu->AddItem( 0, "Stable Master"        , 5);
		Menu->AddItem( 0, "Class Trainer"        , 6);
		Menu->AddItem( 0, "Profession Trainer"   , 7);
        if(AutoSend)
            Menu->SendTo(Plr);
    }

	void GossipSelectOption(Creature* pCreature, Player* Plr, uint32 Id, uint32 IntId)
    {
        GossipMenu * Menu;
        switch(IntId)
        {	
	
		case 0:     // Return to start
			GossipHello(pCreature, Plr, true);
			break;

            //////////////////////
            // Main menu handlers
            /////

		case 1:     //Bank
			SendQuickMenu(4260);
			break;

		case 2:     //Gryphon Master
	        SendQuickMenu(4261);
		    break;

		case 3:     //Guild Master
			SendQuickMenu(4262);
		    break;
	
	   case 4:     //Inn
	       SendQuickMenu(4263);
	       Plr->Gossip_SendPOI(-9459.34, 42.08, 99, 6, 0, "Lion's Pride Inn");
	      break;

	   case 5:     //Stable Master
	       SendQuickMenu(5983);
	       Plr->Gossip_SendPOI(-9466.62, 45.87, 99, 6, 0, "Erma");
	        break;

	   case 6:     //Class Trainer
		   objmgr.CreateGossipMenuForPlayer(&Menu, pCreature->GetGUID(), 4264, Plr);

	       Menu->AddItem( 0, "Druid",                8);
	       Menu->AddItem( 0, "Hunter",               9);
	       Menu->AddItem( 0, "Mage",                 10);
	       Menu->AddItem( 0, "Paladin",              11);
	       Menu->AddItem( 0, "Priest",               12);
	       Menu->AddItem( 0, "Rogue",                13);
		   Menu->AddItem( 0, "Shaman",               14);
	       Menu->AddItem( 0, "Warlock",              15);
		   Menu->AddItem( 0, "Warrior",              16);

			Menu->SendTo(Plr);

		case 7:		//Profession Trainer
			{
			objmgr.CreateGossipMenuForPlayer(&Menu, pCreature->GetGUID(), 4273, Plr);

			Menu->AddItem( 0, "Alchemy"              ,17);
			Menu->AddItem( 0, "Blacksmithing"        ,18);
			Menu->AddItem( 0, "Cooking"              ,19);
			Menu->AddItem( 0, "Enchanting"           ,20);
			Menu->AddItem( 0, "Engineering"          ,21);
			Menu->AddItem( 0, "First Aid"            ,22);
			Menu->AddItem( 0, "Fishing"              ,23);
			Menu->AddItem( 0, "Herbalism"            ,24);
			Menu->AddItem( 0, "Leatherworking"       ,25);
			Menu->AddItem( 0, "Mining"               ,26);
			Menu->AddItem( 0, "Skinning"             ,27);
			Menu->AddItem( 0, "Tailoring"            ,28);
			Menu->SendTo(Plr);
			}break;

		case 8: //Druid
            {
                SendQuickMenu(4265);
            }break;

		case 9: //Hunter
            {
                SendQuickMenu(4266);
            }break;

		case 10: //Mage
            {
                Plr->Gossip_SendPOI(-9471.12, 33.44, 6, 6, 0, "Zaldimar Wefhellt");
                SendQuickMenu(4268);
            }break;

		case 11: //Paladin
            {
                Plr->Gossip_SendPOI(-9469, 108.05, 6, 6, 0, "Brother Wilhelm");
                SendQuickMenu(4269);
            }break;

		case 12: //Priest
            {
                Plr->Gossip_SendPOI(-9461.07, 32.6, 6, 6, 0, "Priestess Josetta");
                SendQuickMenu(4267);
            }break;

		case 13: //Rogue
            {
                Plr->Gossip_SendPOI(-9465.13, 13.29, 6, 6, 0, "Keryn Sylvius");
                SendQuickMenu(4270);
            }break;

		case 14: //Shaman <-- TO DO!
            {
                Plr->Gossip_SendPOI(1781, 53, 6, 6, 0, "Undercity Mage Trainers");
                SendQuickMenu(3513);
            }break;

		case 15: //Warlock
            {
                Plr->Gossip_SendPOI(-9473.21, -4.08, 6, 6, 0, "Maximillian Crowe");
                SendQuickMenu(4272);
            }break;

		case 16: //Warrior
            {
                Plr->Gossip_SendPOI(-9461.82, 109.50, 6, 6, 0, "Lyria Du Lac");
                SendQuickMenu(4271);
            }break;

		case 17: //Alchemy
            {
                Plr->Gossip_SendPOI(-9057.04, 153.63, 6, 6, 0, "Alchemist Mallory");
                SendQuickMenu(4274);
            }break;

		case 18: //Blacksmithing
            {
                Plr->Gossip_SendPOI(-9456.58, 87.90, 6, 6, 0, "Smith Argus");
                SendQuickMenu(4275);
            }break;

		case 19: //Cooking
            {
                Plr->Gossip_SendPOI(-9467.54, -3.16, 6, 6, 0, "Tomas");
                SendQuickMenu(4276);
            }break;

		case 20: //Enchanting
            {
                SendQuickMenu(4277);
            }break;

		case 21: //Engineering
            {
                SendQuickMenu(4278);
            }break;

		case 22: //First Aid
            {
                Plr->Gossip_SendPOI(-9456.82, 30.49, 6, 6, 0, "Michelle Belle");
                SendQuickMenu(4279);
            }break;

		case 23: //Fishing
            {
                Plr->Gossip_SendPOI(-9386.54, -118.73, 6, 6, 0, "Lee Brown");
                SendQuickMenu(4280);
            }break;

		case 24: //Herbalism
            {
                Plr->Gossip_SendPOI(-9060.70, 149.23, 6, 6, 0, "Herbalist Pomeroy");
                SendQuickMenu(4281);        
            }break;

		case 25: //Leatherworking
            {
                Plr->Gossip_SendPOI(-9376.12, -75.23, 6, 6, 0, "Adele Fielder");
                SendQuickMenu(4282);
            }break;

		case 26: //Mining
            {
                SendQuickMenu(4283);
            }break;

		case 27: //Skinning
            {
                Plr->Gossip_SendPOI(-9536.91, -1212.76, 6, 6, 0, "Helene Peltskinner");
                SendQuickMenu(4284);
            }break;

		case 28: //Tailoring
            {
                Plr->Gossip_SendPOI(-9376.12, -75.23, 6, 6, 0, "Eldrin");
                SendQuickMenu(4285);
            }break;
		}
    }   
};

class SCRIPT_DECL UndercityGuard : public GossipScript
{
public:
    ADD_GOSSIP_FACTORY_FUNCTION(UndercityGuard);
    void GossipHello(Creature * pCreature, Player * Plr, bool AutoSend)
    {
        GossipMenu *Menu;
        objmgr.CreateGossipMenuForPlayer(&Menu, pCreature->GetGUID(), 3543, Plr);
		
		Menu->AddItem(0, "The bank", 1);
		Menu->AddItem(0, "The bat handler", 2);
		Menu->AddItem(0, "The guild master", 3);
		Menu->AddItem(0, "The inn", 4);
		Menu->AddItem(0, "The mailbox", 5);
		Menu->AddItem(0, "The auction house", 6);
		Menu->AddItem(0, "The zeppelin master", 7);
		Menu->AddItem(0, "The weapon master", 8);
		Menu->AddItem(0, "The stable master", 9);
		Menu->AddItem(0, "The battlemaster", 10);
		Menu->AddItem(0, "A class trainer", 11);
		Menu->AddItem(0, "A profession trainer", 12);
        if(AutoSend)
            Menu->SendTo(Plr);
    }

    void GossipSelectOption(Creature* pCreature, Player* Plr, uint32 Id, uint32 IntId)
    {
        GossipMenu * Menu;
        switch(IntId)
        {
        case 0:     // Return to start
            GossipHello(pCreature, Plr, true);
            break;

            //////////////////////
            // Main menu handlers
            /////
			
		case 1:     // The bank
            SendQuickMenu(3514);
            Plr->Gossip_SendPOI(1595.64, 232.45, 6, 6, 0, "Undercity Bank");
            break;

        case 2:     // The bat handler
            SendQuickMenu(3515);
            Plr->Gossip_SendPOI(1565.9, 271.43, 6, 6, 0, "Undercity Bat Handler");
            break;

        case 3:     // The guild master
            SendQuickMenu(3516);
            Plr->Gossip_SendPOI(1594.17, 205.57, 6, 6, 0, "Undercity Guild Master");
            break;

        case 4:     // The inn
            SendQuickMenu(3517);
            Plr->Gossip_SendPOI(1639.43, 220.99, 6, 6, 0, "Undercity Inn");
            break;

        case 5:     // The mailbox
            SendQuickMenu(3518);
            Plr->Gossip_SendPOI(1632.68, 219.4, 6, 6, 0, "Undercity Mailbox");
            break;

        case 6:     // The auction house
            SendQuickMenu(3520);
            Plr->Gossip_SendPOI(1647.9, 258.49, 6, 6, 0, "Undercity Auction House");
            break;

        case 7:     // The zeppelin master
            SendQuickMenu(3519);
            Plr->Gossip_SendPOI(2059, 274.86, 6, 6, 0, "Undercity Zeppelin");
            break;

        case 8:     // The weapon master
            SendQuickMenu(4521);
            Plr->Gossip_SendPOI(1670.31, 324.66, 6, 6, 0, "Archibald");
            break;

        case 9:     // The stable master
            SendQuickMenu(5979);
            Plr->Gossip_SendPOI(1634.18, 226.76, 6, 6, 0, "Anya Maulray");
            break;

        case 10:    // The battlemaster
            {
				SendQuickMenu(7527);
				Plr->Gossip_SendPOI(1300.33, 350.92, 6, 6, 0, "Battlemasters Undercity");
            }break;

        case 11:    // A class trainer
            {
                objmgr.CreateGossipMenuForPlayer(&Menu, pCreature->GetGUID(), 3542, Plr);
                Menu->AddItem( 0, "Mage"		 , 13);
                Menu->AddItem( 0, "Paladin"      , 14);
                Menu->AddItem( 0, "Priest"       , 15);
                Menu->AddItem( 0, "Rogue"        , 16);
                Menu->AddItem( 0, "Warlock"      , 17);
                Menu->AddItem( 0, "Warrior"      , 18);
                Menu->SendTo(Plr);
            }break;

        case 12:    // A profession trainer
            {
                objmgr.CreateGossipMenuForPlayer(&Menu, pCreature->GetGUID(), 3541, Plr);
                Menu->AddItem( 0, "Alchemy"           , 19);
                Menu->AddItem( 0, "Blacksmithing"     , 20);
                Menu->AddItem( 0, "Cooking"           , 21);
                Menu->AddItem( 0, "Enchanting"        , 22);
                Menu->AddItem( 0, "Engineering"       , 23);
                Menu->AddItem( 0, "First Aid"         , 24);
                Menu->AddItem( 0, "Fishing"           , 25);
                Menu->AddItem( 0, "Herbalism"         , 26);
                Menu->AddItem( 0, "Leatherworking"    , 27);
                Menu->AddItem( 0, "Mining"            , 28);
                Menu->AddItem( 0, "Skinning"          , 29);
                Menu->AddItem( 0, "Tailoring"         , 30);
                Menu->SendTo(Plr);
            }break;

            ////////////////
            // Class trainer submenu
            ////////

        case 13: //Mage
            {
                Plr->Gossip_SendPOI(1781, 53, 6, 6, 0, "Undercity Mage Trainers");
                SendQuickMenu(3513);
            }break;

        case 14: //Paladin
            {
                Plr->Gossip_SendPOI(1298.98, 316.51, 6, 6, 0, "Champion Cyssa Downrose");
                SendQuickMenu(3521);
            }break;

        case 15: //Priest
            {
                Plr->Gossip_SendPOI(1758.33, 401.5, 6, 6, 0, "Undercity Priest Trainers");
                SendQuickMenu(3521);
            }break;

        case 16: //Rogue
            {
                Plr->Gossip_SendPOI(1418.56, 65, 6, 6, 0, "Undercity Rogue Trainers");
                SendQuickMenu(3526);
            }break;

        case 17: //Warlock
            {
                Plr->Gossip_SendPOI(1780.92, 53.16, 6, 6, 0, "Undercity Warlock Trainers");
                SendQuickMenu(3526);
            }break;

        case 18: //Warrior
            {
                Plr->Gossip_SendPOI(1775.59, 418.19, 6, 6, 0, "Undercity Warrior Trainers");
                SendQuickMenu(3527);
            }break;

        case 19: //Alchemy
            {
                Plr->Gossip_SendPOI(1419.82, 417.19, 6, 6, 0, "The Apothecarium");
                SendQuickMenu(3528);
            }break;

        case 20: //Blacksmithing
            {
                Plr->Gossip_SendPOI(1696, 285, 6, 6, 0, "Undercity Blacksmithing Trainer");
                SendQuickMenu(3529);
            }break;

        case 21: //Cooking
            {
                Plr->Gossip_SendPOI(1596.34, 274.68, 6, 6, 0, "Undercity Cooking Trainer");
                SendQuickMenu(3530);
            }break;

        case 22: //Enchanting
            {
                Plr->Gossip_SendPOI(1488.54, 280.19, 6, 6, 0, "Undercity Enchanting Trainer");
                SendQuickMenu(3531);
            }break;

        case 23: //Engineering
            {
                Plr->Gossip_SendPOI(1408.58, 143.43, 6, 6, 0, "Undercity Engineering Trainer");
                SendQuickMenu(3532);
            }break;

        case 24: //First Aid
            {
                Plr->Gossip_SendPOI(1519.65, 167.19, 6, 6, 0, "Undercity First Aid Trainer");
                SendQuickMenu(3533);
            }break;

        case 25: //Fishing
            {
                Plr->Gossip_SendPOI(1679.9, 89, 6, 6, 0, "Undercity Fishing Trainer");
                SendQuickMenu(3534);
            }break;

        case 26: //Herbalism
            {
                Plr->Gossip_SendPOI(1558, 349.36, 6, 6, 0, "Undercity Herbalism Trainer");
                SendQuickMenu(3535);
            }break;

        case 27: //Leatherworking
            {
                Plr->Gossip_SendPOI(1498.76, 196.43, 6, 6, 0, "Undercity Leatherworking Trainer");
                SendQuickMenu(3536);
            }break;

        case 28: //Mining
            {
                Plr->Gossip_SendPOI(1642.88, 335.58, 6, 6, 0, "Undercity Mining Trainer");
                SendQuickMenu(3537);
            }break;

        case 29: //Skinning
            {
                Plr->Gossip_SendPOI(1498.6, 196.46, 6, 6, 0, "Undercity Skinning Trainer");
                SendQuickMenu(3538);
            }break;

        case 30: //Tailoring
            {
                Plr->Gossip_SendPOI(1689.55, 193, 6, 6, 0, "Undercity Tailoring Trainer");
                SendQuickMenu(3539);
            }break;
        }
    }
};

class SCRIPT_DECL DolanaarGuard : public GossipScript
{
public:
    ADD_GOSSIP_FACTORY_FUNCTION(DolanaarGuard);
    void GossipHello(Creature * pCreature, Player * Plr, bool AutoSend)
    {
        GossipMenu *Menu;
        objmgr.CreateGossipMenuForPlayer(&Menu, pCreature->GetGUID(), 4316, Plr);

        Menu->AddItem(0, "The Bank", 1);
		Menu->AddItem(0, "Rut'Theran Ferry", 2);
		Menu->AddItem(0, "The Guild Master", 3);
		Menu->AddItem(0, "The Inn", 4);
		Menu->AddItem(0, "Stable Master", 5);
		Menu->AddItem(0, "Class Trainer", 6);
		Menu->AddItem(0, "Profession Trainer", 7);

        if(AutoSend)
            Menu->SendTo(Plr);
    }
	
    void GossipSelectOption(Creature* pCreature, Player* Plr, uint32 Id, uint32 IntId)
    {
        GossipMenu * Menu;
        switch(IntId)
        {
        case 0:     // Return to start
            GossipHello(pCreature, Plr, true);
            break;

            //////////////////////
            // Main menu handlers
            /////

        case 1:     // The Bank
            SendQuickMenu(4317);
            break;
		
		case 2:		// Rut'theran erry 
			SendQuickMenu(4318);
			break;
		
		case 3:		// The Guild Master
			SendQuickMenu(4319);
			break;
		
		case 4:		// The Inn
			Plr->Gossip_SendPOI(9821.49, 960.13, 6, 6, 0, "Dolanaar Inn");
			SendQuickMenu(4320);
			
		case 5:		// Stable Master
			Plr->Gossip_SendPOI(9808.37, 931.1, 6, 6, 0, "Seriadne");
			SendQuickMenu(5982);


        case 6:    // Class Trainers
            {
                objmgr.CreateGossipMenuForPlayer(&Menu, pCreature->GetGUID(), 4264, Plr);
                Menu->AddItem( 0, "Druid"       , 8);
                Menu->AddItem( 0, "Hunter"      , 9);
                Menu->AddItem( 0, "Priest"     , 10);
                Menu->AddItem( 0, "Rogue"      , 11);
                Menu->AddItem( 0, "Warrior"    , 12);
                Menu->SendTo(Plr);
            }break;

        case 7:    // Profession Trainers
            {
                objmgr.CreateGossipMenuForPlayer(&Menu, pCreature->GetGUID(), 4273, Plr);
                Menu->AddItem( 0, "Alchemy"           , 13);
                Menu->AddItem( 0, "Cooking"           , 14);
                Menu->AddItem( 0, "Enchanting"        , 15);
                Menu->AddItem( 0, "First Aid"         , 16);
                Menu->AddItem( 0, "Fishing"           , 17);
                Menu->AddItem( 0, "Herbalism"         , 18);
                Menu->AddItem( 0, "Leatherworking"    , 19);
                Menu->AddItem( 0, "Skinning"          , 20);
                Menu->AddItem( 0, "Tailoring"         , 21);
                Menu->SendTo(Plr);
            }break;

            ////////////////
            // Class trainer submenu
            ////////

        case 8: //Druid
            {
                Plr->Gossip_SendPOI(9741.58, 963.7, 6, 6, 0, "Kal");
                SendQuickMenu(4323);
            }break;
		
		case 9: // Hunter
			{
				Plr->Gossip_SendPOI(9815.12, 926.28, 6, 6, 0, "Dazalar");
				SendQuickMenu(4324);
			}break;
		
		case 10: // Priest
			{
				Plr->Gossip_SendPOI(9906.16, 986.63, 6, 6, 0, "Laurna Morninglight");
				SendQuickMenu(4325);
			}break;

		case 11: // Rogue
			{
				Plr->Gossip_SendPOI(9789, 942.86, 6, 6, 0, "Jannok Breezesong");
				SendQuickMenu(4327);
			}break;
			
		case 12: // Warrior
			{
				Plr->Gossip_SendPOI(9821.96, 950.61, 6, 6, 0, "Kyra Windblade");
				SendQuickMenu(4327);
			}break;

        case 13: // Alchemy
            {
                Plr->Gossip_SendPOI(9767.59, 878.81, 6, 6, 0, "Cyndra Kindwhisper");
                SendQuickMenu(4329);
            }break;
		
		case 14: // Cooking
			{
				Plr->Gossip_SendPOI(9751.19, 906.13, 6, 6, 0, "Zarrin");
				SendQuickMenu(4330);
			}break;
		
		case 15: // Enchanting
			{
				Plr->Gossip_SendPOI(10677.59, 1946.56, 6, 6, 0, "Alanna Raveneye");
				SendQuickMenu(4331);
			}break;
			
		case 16: // First Aid
			{
				Plr->Gossip_SendPOI(9903.12, 999, 6, 6, 0, "Byancie");
				SendQuickMenu(4332);
			}break;
			
		case 17: // Fishing
			{
				SendQuickMenu(4333);
			}break;
			
		case 18: // Herbalism
			{
				Plr->Gossip_SendPOI(9773.78, 875.88, 6, 6, 0, "Malorne Bladeleaf");
				SendQuickMenu(4334);
			}break;
			
		case 19: // Leatherworking
			{
				Plr->Gossip_SendPOI(10152.59, 1681.46, 6, 6, 0, "Nadyia Maneweaver");
				SendQuickMenu(4335);
			}break;
			
		case 20: // Skinning
			{
				Plr->Gossip_SendPOI(10135.59, 1673.18, 6, 6, 0, "Radnaal Maneweaver");
				SendQuickMenu(4336);
			}break;
			
		case 21: // Tailoring
			{
				SendQuickMenu(4337);
			}break;
        }
    }
};

class SCRIPT_DECL SilvermoonGuard : public GossipScript
{
public:
    ADD_GOSSIP_FACTORY_FUNCTION(SilvermoonGuard);
    void GossipHello(Creature * pCreature, Player * Plr, bool AutoSend)
    {
        GossipMenu *Menu;
        objmgr.CreateGossipMenuForPlayer(&Menu, pCreature->GetGUID(), 9316, Plr);

        Menu->AddItem(0, "Auction House", 1);
        Menu->AddItem(0, "Bank", 2);
        Menu->AddItem(0, "Gulid Master", 3);
        Menu->AddItem(0, "The Inn", 4);
        Menu->AddItem(0, "Mailbox", 5);    
        Menu->AddItem(0, "Stable Master", 6);
        Menu->AddItem(0, "Weapon Trainer", 7);
        Menu->AddItem(0, "Wind Master", 8);
        Menu->AddItem(0, "Battlemaster", 9);
        Menu->AddItem(0, "Class Trainer", 10);
        Menu->AddItem(0, "Profession Trainer", 11);

        if(AutoSend)
            Menu->SendTo(Plr);
    }

    void GossipSelectOption(Creature* pCreature, Player* Plr, uint32 Id, uint32 IntId)
    {
        GossipMenu * Menu;
        switch(IntId)
        {
        case 0:     // Return to start
            GossipHello(pCreature, Plr, true);
            break;

            //////////////////////
            // Main menu handlers
            /////

        case 1:     // Auction House
            SendQuickMenu(9316);
            Plr->Gossip_SendPOI(9672.73, -7528.61, 6, 6, 0, "Silvermoon Auction House");
            break;

        case 2:     // Bank
            SendQuickMenu(9322);
            Plr->Gossip_SendPOI(9808.4, -7488.16, 6, 6, 0, "Silvermoon Bank");
            break;

        case 3:     // Guild Master
            SendQuickMenu(9324);
            Plr->Gossip_SendPOI(9474.97, -7345.21, 6, 6, 0, "Silvermoon Guild Master");
            break;

        case 4:     // Inn
            SendQuickMenu(9325);
            Plr->Gossip_SendPOI(9683.82, -7365.19, 6, 6, 0, "Silvermoon Inn");
            break;

        case 5:     // Mailbox
            SendQuickMenu(9316);
            Plr->Gossip_SendPOI(9658.33, -7492.17, 6, 6, 0, "Silvermoon Mailbox");
            break;

        case 6:     // Stable Master
            SendQuickMenu(9316);
            Plr->Gossip_SendPOI(9904.95, -7404.31, 6, 6, 0, "Shalenn");
            break;

        case 7:     // Weapon Trainer
            SendQuickMenu(9316);
            Plr->Gossip_SendPOI(9841.17, -7505.13, 6, 6, 0, "Ileda");
            break;

        case 8:     // Wind Master
            SendQuickMenu(9316);
            Plr->Gossip_SendPOI(9378.45, -7163.94, 6, 6, 0, "Silvermoon Wind Master");
            break;

	    case 9:     // Battlemasters
			objmgr.CreateGossipMenuForPlayer(&Menu, pCreature->GetGUID(), 9316, Plr);
            Menu->AddItem(0, "Alterac Valley"  , 12);
            Menu->AddItem(0, "Arathi Basin"    , 13);
			Menu->AddItem(0, "Arena"           , 14);
			Menu->AddItem(0, "Eye of the Storm", 15);
            Menu->AddItem(0, "Warsong Gulch"   , 16);
            Menu->SendTo(Plr);
            break;

        case 10:    // Class Trainers
            {
                objmgr.CreateGossipMenuForPlayer(&Menu, pCreature->GetGUID(), 9316, Plr);
                Menu->AddItem( 0, "Druid"       , 17);
                Menu->AddItem( 0, "Hunter"      , 18);
                Menu->AddItem( 0, "Mage"        , 19);
                Menu->AddItem( 0, "Paladin"     , 20);
                Menu->AddItem( 0, "Priest"      , 21);
                Menu->AddItem( 0, "Rogue"       , 22);
                Menu->AddItem( 0, "Warlock"     , 23);
                Menu->SendTo(Plr);
            }break;

        case 11:    // Profession Trainers
            {
                objmgr.CreateGossipMenuForPlayer(&Menu, pCreature->GetGUID(), 9338, Plr);
                Menu->AddItem( 0, "Alchemy"           , 24);
                Menu->AddItem( 0, "Blacksmithing"     , 25);
                Menu->AddItem( 0, "Cooking"           , 26);
                Menu->AddItem( 0, "Enchanting"        , 27);
                Menu->AddItem( 0, "Engineering"       , 28);
                Menu->AddItem( 0, "First Aid"         , 29);
                Menu->AddItem( 0, "Fishing"           , 30);
				Menu->AddItem( 0, "Jewelcrafting"     , 31);
                Menu->AddItem( 0, "Herbalism"         , 32);
                Menu->AddItem( 0, "Leatherworking"    , 33);
                Menu->AddItem( 0, "Mining"            , 34);
                Menu->AddItem( 0, "Skinning"          , 35);
                Menu->AddItem( 0, "Tailoring"         , 36);
                Menu->SendTo(Plr);
            }break;

            //////////////////
            // Battlemaster submenu
            ////////

        case 12://AV
            {
                Plr->Gossip_SendPOI(9850.49, -7572.26, 6, 6, 0, "Gurak");
                SendQuickMenu(9316);
            }break;

        case 13://AB
            {
                Plr->Gossip_SendPOI(9857.18, -7564.36, 6, 6, 0, "Karen Wentworth");
                SendQuickMenu(9316);
            }break;

        case 14://A
            {
                Plr->Gossip_SendPOI(9850.6, -7559.25, 6, 6, 0, "Bipp Glizzitor");
                SendQuickMenu(9316);
            }break;

        case 15://EoS
            {
                Plr->Gossip_SendPOI(9857.18, -7564.36, 6, 6, 0, "Karen Wentworth");
                SendQuickMenu(9316);
            }break;

        case 16://WSG
            {
                Plr->Gossip_SendPOI(9845.45, -7562.58, 6, 6, 0, "Krukk");
                SendQuickMenu(1);
            }break;

            ////////////////
            // Class trainer submenu
            ////////

        case 17: //Druid
            {
                Plr->Gossip_SendPOI(9700.55, -7262.57, 6, 6, 0, "Silvermoon Druid Trainer");
                SendQuickMenu(9316);
            }break;

        case 18: //Hunter
            {
                Plr->Gossip_SendPOI(9927.48, -7426.14, 6, 6, 0, "Silvermoon Hunter Trainer");
                SendQuickMenu(9316);
            }break;

        case 19: //Mage
            {
                Plr->Gossip_SendPOI(9995.07, -7118.17, 6, 6, 0, "Silvermoon Mage Trainer");
                SendQuickMenu(9316);
            }break;

        case 20: //Paladin
            {
                Plr->Gossip_SendPOI(9850.22, -7516.93, 6, 6, 0, "Silvermoon Paladin Trainer");
                SendQuickMenu(9316);
            }break;

        case 21: //Priest
            {
                Plr->Gossip_SendPOI(9926.79, -7066.66, 6, 6, 0, "Silvermoon Priest Trainer");
                SendQuickMenu(9316);
            }break;

        case 22: //Rogue
            {
                Plr->Gossip_SendPOI(9739.88, -7374.33, 6, 6, 0, "Silvermoon Rogue Trainer");
                SendQuickMenu(9316);
            }break;

        case 23: //Warlock
            {
                Plr->Gossip_SendPOI(9787.57, -7284.63, 6, 6, 0, "Silvermoon Warlock Trainer");
                SendQuickMenu(9316);
            }break;

        case 24: //Alchemy
            {
                Plr->Gossip_SendPOI(9998.09, -7214.36, 6, 6, 0, "Silvermoon Alchemy Trainer");
                SendQuickMenu(9316);
            }break;

        case 25: //Blacksmithing
            {
                Plr->Gossip_SendPOI(9841.43, -7361.53, 6, 6, 0, "Silvermoon Blacksmithing Trainer");
                SendQuickMenu(9340);
            }break;

        case 26: //Cooking
            {
                Plr->Gossip_SendPOI(9577.26, -7243.6, 6, 6, 0, "Silvermoon Cooking Trainer");
                SendQuickMenu(9316);
            }break;

        case 27: //Enchanting
            {
                Plr->Gossip_SendPOI(9962.57, -7246.18, 6, 6, 0, "Silvermoon Enchanting Trainer");
                SendQuickMenu(9341);
            }break;

        case 28: //Engineering
            {
                Plr->Gossip_SendPOI(9820.18, -7329.56, 6, 6, 0, "Silvermoon Engineering Trainer");
                SendQuickMenu(9316);
            }break;

        case 29: //First Aid
            {
                Plr->Gossip_SendPOI(9579.8, -7343.71, 6, 6, 0, "Silvermoon First Aid Trainer");
                SendQuickMenu(9316);
            }break;

        case 30: //Fishing
            {
                Plr->Gossip_SendPOI(9607.73, -7324.3, 6, 6, 0, "Silvermoon Fishing Trainer");
                SendQuickMenu(9316);
            }break;

        case 31: //Jewelcrafting
            {
                Plr->Gossip_SendPOI(9553.54, -7506.43, 6, 6, 0, "Silvermoon Jewelcrafting Trainer");
                SendQuickMenu(9346);
            }break;

        case 32: //Herbalism
            {
                Plr->Gossip_SendPOI(10004.4, -7216.86, 6, 6, 0, "Silvermoon Herbalism Trainer");
                SendQuickMenu(9316);
            }break;

        case 33: //Leatherworking
            {
                Plr->Gossip_SendPOI(9503.72, -7430.16, 6, 6, 0, "Silvermoon Leatherworking Trainer");
                SendQuickMenu(9347);
            }break;

        case 34: //Mining
            {
                Plr->Gossip_SendPOI(9805.1, -7355.56, 6, 6, 0, "Silvermoon Mining Trainer");
                SendQuickMenu(9348);
            }break;

        case 35: //Skinning
            {
                Plr->Gossip_SendPOI(9513.37, -7429.4, 6, 6, 0, "Silvermoon Skinning Trainer");
                SendQuickMenu(9316);
            }break;

		case 36: //Tailoring
            {
                Plr->Gossip_SendPOI(9750.55, -7095.28, 6, 6, 0, "Silvermoon Tailor");
                SendQuickMenu(9350);
            }break;
        }
    }
};

class SCRIPT_DECL ExodarGuard : public GossipScript
{
public:
    ADD_GOSSIP_FACTORY_FUNCTION(ExodarGuard);
    void GossipHello(Creature * pCreature, Player * Plr, bool AutoSend)
    {
        GossipMenu *Menu;
        objmgr.CreateGossipMenuForPlayer(&Menu, pCreature->GetGUID(), 9316, Plr);
        Menu->AddItem(0, "Auction House", 1);
        Menu->AddItem(0, "Bank", 2);
        Menu->AddItem(0, "Gulid Master", 3);
        Menu->AddItem(0, "Hippogryph Master", 4);
        Menu->AddItem(0, "Inn", 5);    
        Menu->AddItem(0, "Mailbox", 6);
        Menu->AddItem(0, "Stable Master", 7);
        Menu->AddItem(0, "Weapon Trainer", 8);
        Menu->AddItem(0, "Battlemaster", 9);
        Menu->AddItem(0, "Class Trainer", 10);
        Menu->AddItem(0, "Profession Trainer", 11);
        if(AutoSend)
            Menu->SendTo(Plr);
    }

    void GossipSelectOption(Creature* pCreature, Player* Plr, uint32 Id, uint32 IntId)
    {
        GossipMenu * Menu;
        switch(IntId)
        {
        case 0:     // Return to start
            GossipHello(pCreature, Plr, true);
            break;

            //////////////////////
            // Main menu handlers
            /////

        case 1:     // Auction House
            SendQuickMenu(9316);
            Plr->Gossip_SendPOI(-4023.6, -11739.3, 6, 6, 0, "Exodar Auction House");
            break;

        case 2:     // Bank
            SendQuickMenu(9316);
            Plr->Gossip_SendPOI(-3923.89, -11544.5, 6, 6, 0, "Exodar Bank");
            break;

        case 3:     // Guild Master
            SendQuickMenu(9316);
            Plr->Gossip_SendPOI(-4092.57, -11626.5, 6, 6, 0, "Exodar Guild Master");
            break;

        case 4:     // Hippogryph Master
            SendQuickMenu(9530);
            Plr->Gossip_SendPOI(-4060.46, -11787.1, 6, 6, 0, "Exodar Hippogryph Master");
            break;

        case 5:     // Inn
            SendQuickMenu(9316);
            Plr->Gossip_SendPOI(-3741.87, -11695.1, 6, 6, 0, "Exodar Inn");
            break;

        case 6:     // Mailbox
            SendQuickMenu(9316);
            //Plr->Gossip_SendPOI(1, 1, 6, 6, 0, "Exodar Mailbox"); // Still need to check it's location.
            break;

        case 7:     // Stable Master
            SendQuickMenu(9316);
            Plr->Gossip_SendPOI(-4146, -12492.4, 6, 6, 0, "Esbina");
            break;

        case 8:     // Weapon Trainer
            SendQuickMenu(9316);
            Plr->Gossip_SendPOI(-4215.68, -11628.9, 6, 6, 0, "Handiir");
            break;

	    case 9:     // Battlemasters
			objmgr.CreateGossipMenuForPlayer(&Menu, pCreature->GetGUID(), 9316, Plr);
            Menu->AddItem(0, "Alterac Valley"  , 12);
            Menu->AddItem(0, "Arathi Basin"    , 13);
			Menu->AddItem(0, "Arena"           , 14);
			Menu->AddItem(0, "Eye of the Storm", 15);
            Menu->AddItem(0, "Warsong Gulch"   , 16);
            Menu->SendTo(Plr);
            break;

        case 10:    // Class Trainers
            {
                objmgr.CreateGossipMenuForPlayer(&Menu, pCreature->GetGUID(), 9316, Plr);
                Menu->AddItem( 0, "Druid"       , 17);
                Menu->AddItem( 0, "Hunter"      , 18);
                Menu->AddItem( 0, "Mage"        , 19);
                Menu->AddItem( 0, "Paladin"     , 20);
                Menu->AddItem( 0, "Priest"      , 21);
                Menu->AddItem( 0, "Shaman"      , 22);
                Menu->AddItem( 0, "Warrior"     , 23);
                Menu->SendTo(Plr);
            }break;

        case 11:    // Profession Trainers
            {
                objmgr.CreateGossipMenuForPlayer(&Menu, pCreature->GetGUID(), 9316, Plr);
                Menu->AddItem( 0, "Alchemy"           , 24);
                Menu->AddItem( 0, "Blacksmithing"     , 25);
                Menu->AddItem( 0, "Cooking"           , 26);
                Menu->AddItem( 0, "Enchanting"        , 27);
                Menu->AddItem( 0, "Engineering"       , 28);
                Menu->AddItem( 0, "First Aid"         , 29);
                Menu->AddItem( 0, "Fishing"           , 30);
				Menu->AddItem( 0, "Jewelcrafting"     , 31);
                Menu->AddItem( 0, "Herbalism"         , 32);
                Menu->AddItem( 0, "Leatherworking"    , 33);
                Menu->AddItem( 0, "Mining"            , 34);
                Menu->AddItem( 0, "Skinning"          , 35);
                Menu->AddItem( 0, "Tailoring"         , 36);
                Menu->SendTo(Plr);
            }break;

            //////////////////
            // Battlemaster submenu
            ////////

        case 12://AV
            {
                Plr->Gossip_SendPOI(-3963.2, -11346.0, 6, 6, 0, "Alterac Valley Battlemaster");
                SendQuickMenu(9316);
            }break;

        case 13://AB
            {
                Plr->Gossip_SendPOI(-3998.5, -11326.3, 6, 6, 0, "Arathi Basin Battlemaster");
                SendQuickMenu(9316);
            }break;

        case 14://A
            {
                Plr->Gossip_SendPOI(-3735.7, -11683.63, 6, 6, 0, "Arena Battlemaster"); 
				SendQuickMenu(9316);
            }break;

        case 15://EoS
            {
                Plr->Gossip_SendPOI(-3961.6, -11348.2, 6, 6, 0, "Eye Of The Storm Battlemaster");
                SendQuickMenu(9316);
            }break;

        case 16://WSG
            {
                Plr->Gossip_SendPOI(-3961.27, -11389.6, 6, 6, 0, "Warsong Gulch Battlemaster");
                SendQuickMenu(9316);
            }break;

            ////////////////
            // Class trainer submenu
            ////////

        case 17: //Druid
            {
                Plr->Gossip_SendPOI(-4170.3, -11578.3, 6, 6, 0, "Exodar Druid Trainer"); // Can't find him in Exodar, so probably this will be deleted (no infos on sites too)
                SendQuickMenu(9316);
            }break;

        case 18: //Hunter
            {
                Plr->Gossip_SendPOI(-4232.18, -11569.2, 6, 6, 0, "Exodar Hunter Trainer");
                SendQuickMenu(9316);
            }break;

        case 19: //Mage
            {
                Plr->Gossip_SendPOI(-4057.32, -11556.5, 6, 6, 0, "Exodar Mage Trainer");
                SendQuickMenu(9316);
            }break;

        case 20: //Paladin
            {
                Plr->Gossip_SendPOI(-4191.2, -11470.4, 6, 6, 0, "Exodar Paladin Trainer");
                SendQuickMenu(9316);
            }break;

        case 21: //Priest
            {
                Plr->Gossip_SendPOI(-3969.63, -11482.8, 6, 6, 0, "Exodar Priest Trainer");
                SendQuickMenu(9316);
            }break;

        case 22: //Shaman
            {
                Plr->Gossip_SendPOI(-3886.88, -11339.7, 6, 6, 0, "Exodar Shaman Trainer");
                SendQuickMenu(9556);
            }break;

        case 23: //Warrior
            {
                Plr->Gossip_SendPOI(-4189.43, -11653.7, 6, 6, 0, "Exodar Warrior Trainer");
                SendQuickMenu(9316);
            }break;

        case 24: //Alchemy
            {
                Plr->Gossip_SendPOI(-4032.95, -11361.0, 6, 6, 0, "Exodar Alchemy Trainer");
                SendQuickMenu(9316);
            }break;

        case 25: //Blacksmithing
            {
                Plr->Gossip_SendPOI(-4243.9, -11706.7, 6, 6, 0, "Exodar Blacksmithing Trainer");
                SendQuickMenu(9340);
            }break;

        case 26: //Cooking
            {
                Plr->Gossip_SendPOI(-4710, -12393.8, 6, 6, 0, "Exodar Cooking Trainer"); // Can't find correct location in Exodar.
                SendQuickMenu(9316);
            }break;

        case 27: //Enchanting
            {
                Plr->Gossip_SendPOI(-3889.3, -11495, 6, 6, 0, "Exodar Enchanting Trainer");
                SendQuickMenu(9341);
            }break;

        case 28: //Engineering
            {
                Plr->Gossip_SendPOI(-4257.68, -11640.3, 6, 6, 0, "Exodar Engineering Trainer");
                SendQuickMenu(9316);
            }break;

        case 29: //First Aid
            {
                Plr->Gossip_SendPOI(-3766.05, -11481.8, 6, 6, 0, "Exodar First Aid Trainer");
                SendQuickMenu(9316);
            }break;

        case 30: //Fishing
            {
                Plr->Gossip_SendPOI(-3712.96, -11404.0, 6, 6, 0, "Exodar Fishing Trainer");
                SendQuickMenu(9316);
            }break;

        case 31: //Jewelcrafting
            {
                Plr->Gossip_SendPOI(-3787.7, -11547.6, 6, 6, 0, "Exodar Jewelcrafting Trainer");
                SendQuickMenu(9346);
            }break;

        case 32: //Herbalism
            {
                Plr->Gossip_SendPOI(-4052.5, -11356.6, 6, 6, 0, "Exodar Herbalism Trainer");
                SendQuickMenu(9316);
            }break;

        case 33: //Leatherworking
            {
                Plr->Gossip_SendPOI(-4140.6, -11776.7, 6, 6, 0, "Exodar Leatherworking Trainer");
                SendQuickMenu(9347);
            }break;

        case 34: //Mining
            {
                Plr->Gossip_SendPOI(-4228.17, -11697.1, 6, 6, 0, "Exodar Mining Trainer");
                SendQuickMenu(9348);
            }break;

        case 35: //Skinning
            {
                Plr->Gossip_SendPOI(-4134.97, -11760.5, 6, 6, 0, "Exodar Skinning Trainer");
                SendQuickMenu(9316);
            }break;

		case 36: //Tailoring
            {
                Plr->Gossip_SendPOI(-4095.78, -11746.9, 6, 6, 0, "Exodar Tailor");
                SendQuickMenu(9350);
            }break;
        }
    }
};

class SCRIPT_DECL OrgrimmarGuard : public GossipScript
{
public:
    ADD_GOSSIP_FACTORY_FUNCTION(OrgrimmarGuard);
    void GossipHello(Creature * pCreature, Player * Plr, bool AutoSend)
    {
        GossipMenu *Menu;
        objmgr.CreateGossipMenuForPlayer(&Menu, pCreature->GetGUID(), 2593, Plr);
        Menu->AddItem(0, "The bank", 1);
        Menu->AddItem(0, "The wind rider master", 2);
        Menu->AddItem(0, "The guild master", 3);
        Menu->AddItem(0, "The inn", 4);
        Menu->AddItem(0, "The mailbox", 5);
        Menu->AddItem(0, "The auction house", 6);    
        Menu->AddItem(0, "The zeppelin master", 7);
        Menu->AddItem(0, "The weapon master", 8);
        Menu->AddItem(0, "The stable master", 9);
        Menu->AddItem(0, "The officers' lounge", 10);
        Menu->AddItem(0, "The battlemaster", 11);
        Menu->AddItem(0, "A class trainer", 12);
        Menu->AddItem(0, "A profession trainer", 13);
        if(AutoSend)
            Menu->SendTo(Plr);
    }

    void GossipSelectOption(Creature* pCreature, Player* Plr, uint32 Id, uint32 IntId)
    {
        GossipMenu * Menu;
        switch(IntId)
        {
        case 0:     // Return to start
            GossipHello(pCreature, Plr, true);
            break;

            //////////////////////
            // Main menu handlers
            /////

        case 1:     // The bank
            SendQuickMenu(2554);
            Plr->Gossip_SendPOI(1631.51, -4375.33, 6, 6, 0, "Bank of Orgrimmar");
            break;

        case 2:     // The wind rider master
            SendQuickMenu(2555);
            Plr->Gossip_SendPOI(1676.6, -4332.72, 6, 6, 0, "The Sky Tower");
            break;

        case 3:     // The guild master
            SendQuickMenu(2556);
            Plr->Gossip_SendPOI(1576.93, -4294.75, 6, 6, 0, "Horde Embassy");
            break;

        case 4:     // The inn
            SendQuickMenu(2557);
            Plr->Gossip_SendPOI(1644.51, -4447.27, 6, 6, 0, "Orgrimmar Inn");
            break;

        case 5:     // The mailbox
            SendQuickMenu(2558);
            Plr->Gossip_SendPOI(1622.53, -4388.79, 6, 6, 0, "Orgrimmar Mailbox");
            break;

        case 6:     // The auction house
            SendQuickMenu(3075);
            Plr->Gossip_SendPOI(1679.21, -4450.1, 6, 6, 0, "Orgrimmar Auction House");
			break;

        case 7:     // The zeppelin master
            SendQuickMenu(3173);
            Plr->Gossip_SendPOI(1337.36, -4632.7, 6, 6, 0, "Orgrimmar Zeppelin Tower");
            break;

        case 8:     // The weapon master
            SendQuickMenu(4519);
            Plr->Gossip_SendPOI(2092.56, -4823.95, 6, 6, 0, "Sayoc & Hanashi");
            break;

	    case 9:     // The stable master
			SendQuickMenu(5974);
            Plr->Gossip_SendPOI(2133.12, -4663.93, 6, 6, 0, "Xon'cha");
            break;

        case 10:    // The officers' lounge
			SendQuickMenu(7046);
			Plr->Gossip_SendPOI(1633.56, -4249.37, 6, 6, 0, "Hall of Legends");
            break;

        case 11:    // The battlemaster
			SendQuickMenu(7521);
			Plr->Gossip_SendPOI(1990.41, -4794.15, 6, 6, 0, "Battlemasters Orgrimmar");
            break;

        case 12:    // A class trainer
            {
                objmgr.CreateGossipMenuForPlayer(&Menu, pCreature->GetGUID(), 2599, Plr);
                Menu->AddItem( 0, "Hunter"       , 14);
                Menu->AddItem( 0, "Mage"         , 15);
                Menu->AddItem( 0, "Priest"       , 16);
                Menu->AddItem( 0, "Shaman"       , 17);
                Menu->AddItem( 0, "Rogue"        , 18);
                Menu->AddItem( 0, "Warlock"      , 19);
                Menu->AddItem( 0, "Warrior"      , 20);
                Menu->AddItem( 0, "Paladin"      , 21);
                Menu->SendTo(Plr);
            }break;

        case 13:    // A profession trainer
            {
                objmgr.CreateGossipMenuForPlayer(&Menu, pCreature->GetGUID(), 2594, Plr);
                Menu->AddItem( 0, "Alchemy"           , 22);
                Menu->AddItem( 0, "Blacksmithing"     , 23);
                Menu->AddItem( 0, "Cooking"           , 24);
                Menu->AddItem( 0, "Enchanting"        , 25);
                Menu->AddItem( 0, "Engineering"       , 26);
                Menu->AddItem( 0, "First Aid"         , 27);
                Menu->AddItem( 0, "Fishing"           , 28);
                Menu->AddItem( 0, "Herbalism"         , 29);
                Menu->AddItem( 0, "Leatherworking"    , 30);
                Menu->AddItem( 0, "Mining"            , 31);
                Menu->AddItem( 0, "Skinning"          , 32);
                Menu->AddItem( 0, "Tailoring"         , 33);
                Menu->SendTo(Plr);
            }break;

            ////////////////
            // Class trainer submenu
            ////////

        case 14: //Hunter
            {
                Plr->Gossip_SendPOI(2114.84, -4625.31, 6, 6, 0, "Orgrimmar Hunter's Hall");
                SendQuickMenu(2559);
            }break;

        case 15: //Mage
            {
                Plr->Gossip_SendPOI(1451.26, -4223.33, 6, 6, 0, "Darkbriar Lodge");
                SendQuickMenu(2560);
            }break;

        case 16: //Priest
            {
                Plr->Gossip_SendPOI(1442.21, -4183.24, 6, 6, 0, "Spirit Lodge");
                SendQuickMenu(2561);
            }break;

        case 17: //Shaman
            {
                Plr->Gossip_SendPOI(1925.34, -4181.89, 6, 6, 0, "Thrall's Fortress");
                SendQuickMenu(2562);
            }break;

        case 18: //Rogue
            {
                Plr->Gossip_SendPOI(1773.39, -4278.97, 6, 6, 0, "Shadowswift Brotherhood");
                SendQuickMenu(2563);
            }break;

        case 19: //Warlock
            {
                Plr->Gossip_SendPOI(1849.57, -4359.68, 6, 6, 0, "Darkfire Enclave");
                SendQuickMenu(2564);
            }break;

        case 20: //Warrior
            {
                Plr->Gossip_SendPOI(1983.92, -4794.2, 6, 6, 0, "Hall of the Brave");
                SendQuickMenu(2565);
            }break;

		case 21: //Paladin
            {
                Plr->Gossip_SendPOI(1937.53, -4141.0, 6, 6, 0, "Thrall's Fortress");
                SendQuickMenu(2566);
            }break;

        case 22: //Alchemy
            {
                Plr->Gossip_SendPOI(1955.17, -4475.79, 6, 6, 0, "Yelmak's Alchemy and Potions");
                SendQuickMenu(2497);
            }break;

        case 23: //Blacksmithing
            {
                Plr->Gossip_SendPOI(2054.34, -4831.85, 6, 6, 0, "The Burning Anvil");
                SendQuickMenu(2499);
            }break;

        case 24: //Cooking
            {
                Plr->Gossip_SendPOI(1780.96, -4481.31, 6, 6, 0, "Borstan's Firepit");
                SendQuickMenu(2500);
            }break;

        case 25: //Enchanting
            {
                Plr->Gossip_SendPOI(1917.5, -4434.95, 6, 6, 0, "Godan's Runeworks");
                SendQuickMenu(2501);
            }break;

        case 26: //Engineering
            {
                Plr->Gossip_SendPOI(2038.45, -4744.75, 6, 6, 0, "Nogg's Machine Shop");
                SendQuickMenu(2653);
            }break;

        case 27: //First Aid
            {
                Plr->Gossip_SendPOI(1485.21, -4160.91, 6, 6, 0, "Survival of the Fittest");
                SendQuickMenu(2502);
            }break;

        case 28: //Fishing
            {
                Plr->Gossip_SendPOI(1994.15, -4655.7, 6, 6, 0, "Lumak's Fishing");
                SendQuickMenu(2503);
            }break;

        case 29: //Herbalism
            {
                Plr->Gossip_SendPOI(1898.61, -4454.93, 6, 6, 0, "Jandi's Arboretum");
                SendQuickMenu(2504);
            }break;

        case 30: //Leatherworking
            {
                Plr->Gossip_SendPOI(1852.82, -4562.31, 6, 6, 0, "Kodohide Leatherworkers");
                SendQuickMenu(2513);
            }break;

        case 31: //Mining
            {
                Plr->Gossip_SendPOI(2029.79, -4704, 6, 6, 0, "Red Canyon Mining");
                SendQuickMenu(2515);
            }break;

        case 32: //Skinning
            {
                Plr->Gossip_SendPOI(1852.82, -4562.31, 6, 6, 0, "Kodohide Leatherworkers");
                SendQuickMenu(2516);        
            }break;

		case 33: //Tailoring
            {
                Plr->Gossip_SendPOI(1802.66, -4560.66, 6, 6, 0, "Magar's Cloth Goods");
                SendQuickMenu(2518);
            }break;
        }
    }
};

class SCRIPT_DECL ThunderbluffGuard : public GossipScript
{
public:
    ADD_GOSSIP_FACTORY_FUNCTION(ThunderbluffGuard);
    void GossipHello(Creature * pCreature, Player * Plr, bool AutoSend)
    {
        GossipMenu *Menu;
        objmgr.CreateGossipMenuForPlayer(&Menu, pCreature->GetGUID(), 3543, Plr);
        Menu->AddItem(0, "The bank", 1);
        Menu->AddItem(0, "The wind rider master", 2);
        Menu->AddItem(0, "The guild master", 3);
        Menu->AddItem(0, "The inn", 4);
        Menu->AddItem(0, "The mailbox", 5);
        Menu->AddItem(0, "The auction house", 6);    
        Menu->AddItem(0, "The weapon master", 7);
        Menu->AddItem(0, "The stable master", 8);
        Menu->AddItem(0, "The battlemaster", 9);
        Menu->AddItem(0, "A class trainer", 10);
        Menu->AddItem(0, "A profession trainer", 11);
        if(AutoSend)
            Menu->SendTo(Plr);
    }

    void GossipSelectOption(Creature* pCreature, Player* Plr, uint32 Id, uint32 IntId)
    {
        GossipMenu * Menu;
        switch(IntId)
        {
        case 0:     // Return to start
            GossipHello(pCreature, Plr, true);
            break;

            //////////////////////
            // Main menu handlers
            /////

        case 1:     // The bank
            SendQuickMenu(1292);
            Plr->Gossip_SendPOI(-1257.8, 24.14, 6, 6, 0, "Thunder Bluff Bank");
            break;

        case 2:     // The wind rider master
            SendQuickMenu(1293);
            Plr->Gossip_SendPOI(-1196.43, 28.26, 6, 6, 0, "Wind Rider Roost");
            break;

        case 3:     // The guild master
            SendQuickMenu(1291);
            Plr->Gossip_SendPOI(-1296.5, 127.57, 6, 6, 0, "Thunder Bluff Civic Information");
            break;

        case 4:     // The inn
            SendQuickMenu(3153);
            Plr->Gossip_SendPOI(-1296, 39.7, 6, 6, 0, "Thunder Bluff Inn");
            break;

        case 5:     // The mailbox
            SendQuickMenu(3154);
            Plr->Gossip_SendPOI(-1263.59, 44.36, 6, 6, 0, "Thunder Bluff Mailbox");
            break;

        case 6:     // The auction house
            SendQuickMenu(3155);
            Plr->Gossip_SendPOI(-1205.51, 105.74, 6, 6, 0, "Thunder Bluff Auction house");
			break;

        case 7:     // The weapon master
            SendQuickMenu(4520);
            Plr->Gossip_SendPOI(-1282.31, 89.56, 6, 6, 0, "Ansekhwa");
            break;

	    case 8:     // The stable master
			SendQuickMenu(5977);
            Plr->Gossip_SendPOI(-1270.19, 48.84, 6, 6, 0, "Bulrug");
            break;

        case 9:    // The battlemaster
			SendQuickMenu(7527);
			Plr->Gossip_SendPOI(-1391.22, -81.33, 6, 6, 0, "Battlemasters Thunder Bluff");
            break;

        case 10:    // A class trainer
            {
                objmgr.CreateGossipMenuForPlayer(&Menu, pCreature->GetGUID(), 3542, Plr);
                Menu->AddItem( 0, "Druid"       , 12);
                Menu->AddItem( 0, "Hunter"      , 13);
                Menu->AddItem( 0, "Mage"        , 14);
                Menu->AddItem( 0, "Priest"      , 15);
                Menu->AddItem( 0, "Shaman"      , 16);
                Menu->AddItem( 0, "Warrior"     , 17);
                Menu->SendTo(Plr);
            }break;

        case 11:    // A profession trainer
            {
                objmgr.CreateGossipMenuForPlayer(&Menu, pCreature->GetGUID(), 3541, Plr);
                Menu->AddItem( 0, "Alchemy"           , 18);
                Menu->AddItem( 0, "Blacksmithing"     , 19);
                Menu->AddItem( 0, "Cooking"           , 20);
                Menu->AddItem( 0, "Enchanting"        , 21);
                Menu->AddItem( 0, "First Aid"         , 22);
                Menu->AddItem( 0, "Fishing"           , 23);
                Menu->AddItem( 0, "Herbalism"         , 24);
                Menu->AddItem( 0, "Leatherworking"    , 25);
                Menu->AddItem( 0, "Mining"            , 26);
                Menu->AddItem( 0, "Skinning"          , 27);
                Menu->AddItem( 0, "Tailoring"         , 28);
                Menu->SendTo(Plr);
            }break;

            ////////////////
            // Class trainer submenu
            ////////

        case 12: //Druid
            {
                Plr->Gossip_SendPOI(-1054.47, -285, 6, 6, 0, "Hall of Elders");
                SendQuickMenu(1294);
            }break;

        case 13: //Hunter
            {
                Plr->Gossip_SendPOI(-1416.32, -114.28, 6, 6, 0, "Hunter's Hall");
                SendQuickMenu(1295);
            }break;

        case 14: //Mage
            {
                Plr->Gossip_SendPOI(-1061.2, 195.5, 6, 6, 0, "Pools of Vision");
                SendQuickMenu(1296);
            }break;

        case 15: //Priest
            {
                Plr->Gossip_SendPOI(-1061.2, 195.5, 6, 6, 0, "Pools of Vision");
                SendQuickMenu(1297);
            }break;

        case 16: //Shaman
            {
                Plr->Gossip_SendPOI(-989.54, 278.25, 6, 6, 0, "Hall of Spirits");
                SendQuickMenu(1298);
            }break;

        case 17: //Warrior
			{
                Plr->Gossip_SendPOI(-1416.32, -114.28, 6, 6, 0, "Hunter's Hall");
                SendQuickMenu(1299);
            }break;

        case 18: //Alchemy
            {
                Plr->Gossip_SendPOI(-1085.56, 27.29, 6, 6, 0, "Bena's Alchemy");
                SendQuickMenu(1332);
            }break;

        case 19: //Blacksmithing
            {
                Plr->Gossip_SendPOI(-1239.75, 104.88, 6, 6, 0, "Karn's Smithy");
                SendQuickMenu(1333);
            }break;

        case 20: //Cooking
            {
                Plr->Gossip_SendPOI(-1214.5, -21.23, 6, 6, 0, "Aska's Kitchen");
                SendQuickMenu(1334);
            }break;

        case 21: //Enchanting
            {
                Plr->Gossip_SendPOI(-1112.65, 48.26, 6, 6, 0, "Dawnstrider Enchanters");
                SendQuickMenu(1335);
            }break;

        case 22: //First Aid
            {
                Plr->Gossip_SendPOI(-996.58, 200.5, 6, 6, 0, "Spiritual Healing");
                SendQuickMenu(1336);
            }break;

        case 23: //Fishing
            {
                Plr->Gossip_SendPOI(-1169.35, -68.87, 6, 6, 0, "Mountaintop Bait & Tackle");
                SendQuickMenu(1337);
            }break;

        case 24: //Herbalism
            {
                Plr->Gossip_SendPOI(-1137.7, -1.51, 6, 6, 0, "Holistic Herbalism");
                SendQuickMenu(1338);
            }break;

        case 25: //Leatherworking
            {
                Plr->Gossip_SendPOI(-1156.22, 66.86, 6, 6, 0, "Thunder Bluff Armorers");
                SendQuickMenu(1339);
            }break;

        case 26: //Mining
            {
                Plr->Gossip_SendPOI(-1249.17, 155, 6, 6, 0, "Stonehoof Geology");
                SendQuickMenu(1340);
            }break;

        case 27: //Skinning
            {
                Plr->Gossip_SendPOI(-1148.56, 51.18, 6, 6, 0, "Mooranta");
                SendQuickMenu(1343); 
            }break;

		case 28: //Tailoring
            {
                Plr->Gossip_SendPOI(-1156.22, 66.86, 6, 6, 0, "Thunder Bluff Armorers");
                SendQuickMenu(1341);
            }break;
        }
    }
};

class SCRIPT_DECL BloodhoofGuard : public GossipScript
{
public:
    ADD_GOSSIP_FACTORY_FUNCTION(BloodhoofGuard);
    void GossipHello(Creature * pCreature, Player * Plr, bool AutoSend)
    {
        GossipMenu *Menu;
        objmgr.CreateGossipMenuForPlayer(&Menu, pCreature->GetGUID(), 3543, Plr);
        Menu->AddItem(0, "The bank", 1);
        Menu->AddItem(0, "The wind rider master", 2);
        Menu->AddItem(0, "The inn", 3);
        Menu->AddItem(0, "The stable master", 4);
        Menu->AddItem(0, "A class trainer", 5);
        Menu->AddItem(0, "A profession trainer", 6);    
        if(AutoSend)
            Menu->SendTo(Plr);
    }

    void GossipSelectOption(Creature* pCreature, Player* Plr, uint32 Id, uint32 IntId)
    {
        GossipMenu * Menu;
        switch(IntId)
        {
        case 0:     // Return to start
            GossipHello(pCreature, Plr, true);
            break;

            //////////////////////
            // Main menu handlers
            /////

        case 1:     // The bank
            SendQuickMenu(4051);
            break;

        case 2:     // The wind rider master
            SendQuickMenu(4052);
            break;

        case 3:     // The inn
            SendQuickMenu(4053);
            Plr->Gossip_SendPOI(-2361.38, -349.19, 6, 6, 0, "Bloodhoof Village Inn");      
			break;

        case 4:     // The stable master
            SendQuickMenu(5976);
            Plr->Gossip_SendPOI(-2338.86, -357.56, 6, 6, 0, "Seikwa");
            break;

        case 5:     // A class trainer
			{
                objmgr.CreateGossipMenuForPlayer(&Menu, pCreature->GetGUID(), 4069, Plr);
                Menu->AddItem( 0, "Druid"       , 7);
                Menu->AddItem( 0, "Hunter"      , 8);
                Menu->AddItem( 0, "Shaman"      , 9);
                Menu->AddItem( 0, "Warrior"    , 10);
                Menu->SendTo(Plr);
			}break;

        case 6:     // A profession trainer
            {
                objmgr.CreateGossipMenuForPlayer(&Menu, pCreature->GetGUID(), 3541, Plr);
                Menu->AddItem( 0, "Alchemy"           , 11);
                Menu->AddItem( 0, "Blacksmithing"     , 12);
                Menu->AddItem( 0, "Cooking"           , 13);
                Menu->AddItem( 0, "Enchanting"        , 14);
                Menu->AddItem( 0, "First Aid"         , 15);
                Menu->AddItem( 0, "Fishing"           , 16);
                Menu->AddItem( 0, "Herbalism"         , 17);
                Menu->AddItem( 0, "Leatherworking"    , 18);
                Menu->AddItem( 0, "Mining"            , 19);
                Menu->AddItem( 0, "Skinning"          , 20);
                Menu->AddItem( 0, "Tailoring"         , 21);
                Menu->SendTo(Plr);
            }break;


            ////////////////
            // Class trainer submenu
            ////////

        case 7: //Druid
            {
                Plr->Gossip_SendPOI(-2312.15, -443.69, 6, 6, 0, "Gennia Runetotem");
                SendQuickMenu(4054);
            }break;

        case 8: //Hunter
            {
                Plr->Gossip_SendPOI(-2178.14, -406.14, 6, 6, 0, "Yaw Sharpmane");
                SendQuickMenu(4055);
            }break;

        case 9: //Shaman
            {
                Plr->Gossip_SendPOI(-2301.5, -439.87, 6, 6, 0, "Narm Skychaser");
                SendQuickMenu(4056);
            }break;

        case 10: //Warrior
			{
                Plr->Gossip_SendPOI(-2345.43, -494.11, 6, 6, 0, "Krang Stonehoof");
                SendQuickMenu(4057);
            }break;

        case 11: //Alchemy
            {
                SendQuickMenu(4058);
            }break;

        case 12: //Blacksmithing
            {
                SendQuickMenu(4059);
            }break;

        case 13: //Cooking
            {
                Plr->Gossip_SendPOI(-2263.34, -287.91, 6, 6, 0, "Pyall Silentstride");
                SendQuickMenu(4060);
            }break;

        case 14: //Enchanting
            {
				SendQuickMenu(4061);
            }break;

        case 15: //First Aid
            {
                Plr->Gossip_SendPOI(-2353.52, -355.82, 6, 6, 0, "Vira Younghoof");
                SendQuickMenu(4062);
            }break;

        case 16: //Fishing
            {
                Plr->Gossip_SendPOI(-2349.21, -241.37, 6, 6, 0, "Uthan Stillwater");
                SendQuickMenu(4063);
            }break;

        case 17: //Herbalism
            {
                SendQuickMenu(4064);
            }break;

        case 18: //Leatherworking
            {
                Plr->Gossip_SendPOI(-2257.12, -288.63, 6, 6, 0, "Chaw Stronghide");
                SendQuickMenu(4065);
            }break;

        case 19: //Mining
            {
                SendQuickMenu(4066);
            }break;

        case 20: //Skinning
            {
                Plr->Gossip_SendPOI(-2252.94, -291.32, 6, 6, 0, "Yonn Deepcut");
                SendQuickMenu(4067); 
            }break;

		case 21: //Tailoring
            {
                SendQuickMenu(4068);
            }break;
        }
    }
};

class SCRIPT_DECL RazorHillGuard : public GossipScript
{
public:
    ADD_GOSSIP_FACTORY_FUNCTION(RazorHillGuard);
    void GossipHello(Creature * pCreature, Player * Plr, bool AutoSend)
    {
        GossipMenu *Menu;
        objmgr.CreateGossipMenuForPlayer(&Menu, pCreature->GetGUID(), 4037, Plr);
        Menu->AddItem(0, "The bank", 1);
        Menu->AddItem(0, "The wind rider master", 2);
        Menu->AddItem(0, "The inn", 3);
        Menu->AddItem(0, "The stable master", 4);
        Menu->AddItem(0, "A class trainer", 5);
        Menu->AddItem(0, "A profession trainer", 6);    
        if(AutoSend)
            Menu->SendTo(Plr);
    }

    void GossipSelectOption(Creature* pCreature, Player* Plr, uint32 Id, uint32 IntId)
    {
        GossipMenu * Menu;
        switch(IntId)
        {
        case 0:     // Return to start
            GossipHello(pCreature, Plr, true);
            break;

            //////////////////////
            // Main menu handlers
            /////

        case 1:     // The bank
            SendQuickMenu(4032);
            break;

        case 2:     // The wind rider master
            SendQuickMenu(4033);
            break;

        case 3:     // The inn
            SendQuickMenu(4034);
            Plr->Gossip_SendPOI(338.7, -4688.87, 6, 6, 0, "Razor Hill Inn");      
			break;

        case 4:     // The stable master
            SendQuickMenu(5973);
            Plr->Gossip_SendPOI(330.31, -4710.66, 6, 6, 0, "Shoja'my");
            break;

        case 5:     // A class trainer
			{
                objmgr.CreateGossipMenuForPlayer(&Menu, pCreature->GetGUID(), 4035, Plr);
                Menu->AddItem( 0, "Hunter"       , 7);
				Menu->AddItem( 0, "Mage"         , 8);
				Menu->AddItem( 0, "Priest"       , 9);
				Menu->AddItem( 0, "Rogue"       , 10);
                Menu->AddItem( 0, "Shaman"      , 11);
				Menu->AddItem( 0, "Warlock"     , 12);
                Menu->AddItem( 0, "Warrior"     , 13);
                Menu->SendTo(Plr);
			}break;

        case 6:     // A profession trainer
            {
                objmgr.CreateGossipMenuForPlayer(&Menu, pCreature->GetGUID(), 3541, Plr);
                Menu->AddItem( 0, "Alchemy"           , 14);
                Menu->AddItem( 0, "Blacksmithing"     , 15);
                Menu->AddItem( 0, "Cooking"           , 16);
                Menu->AddItem( 0, "Enchanting"        , 17);
                Menu->AddItem( 0, "Engineering"       , 18);
                Menu->AddItem( 0, "First Aid"         , 19);
                Menu->AddItem( 0, "Fishing"           , 20);
                Menu->AddItem( 0, "Herbalism"         , 21);
                Menu->AddItem( 0, "Leatherworking"    , 22);
                Menu->AddItem( 0, "Mining"            , 23);
                Menu->AddItem( 0, "Skinning"          , 24);
                Menu->AddItem( 0, "Tailoring"         , 25);
                Menu->SendTo(Plr);
            }break;


            ////////////////
            // Class trainer submenu
            ////////

        case 7: //Hunter
            {
                Plr->Gossip_SendPOI(276, -4706.72, 6, 6, 0, "Thotar");
                SendQuickMenu(4013);
            }break;        

        case 8: //Mage
            {
                Plr->Gossip_SendPOI(-839.33, -4935.6, 6, 6, 0, "Un'Thuwa");
                SendQuickMenu(4014);
            }break;

        case 9: //Priest
            {
                Plr->Gossip_SendPOI(296.22, -4828.1, 6, 6, 0, "Tai'jin");
                SendQuickMenu(4015);
            }break;

        case 10: //Rogue
			{
                Plr->Gossip_SendPOI(265.76, -4709, 6, 6, 0, "Kaplak");
                SendQuickMenu(4016);
            }break;

        case 11: //Shaman
            {
                Plr->Gossip_SendPOI(307.79, -4836.97, 6, 6, 0, "Swart");
                SendQuickMenu(4017);
            }break;

        case 12: //Warlock
            {
                Plr->Gossip_SendPOI(355.88, -4836.45, 6, 6, 0, "Dhugru Gorelust");
                SendQuickMenu(4018);
            }break;

        case 13: //Warrior
            {
                Plr->Gossip_SendPOI(312.3, -4824.66, 6, 6, 0, "Tarshaw Jaggedscar");
                SendQuickMenu(4019);
            }break;

        case 14: //Alchemy
            {
				Plr->Gossip_SendPOI(-800.25, -4894.33, 6, 6, 0, "Miao'zan");
                SendQuickMenu(4020);
            }break;

        case 15: //Blacksmithing
            {
				Plr->Gossip_SendPOI(373.24, -4716.45, 6, 6, 0, "Dwukk");
                SendQuickMenu(4021);
            }break;

        case 16: //Cooking
            {
                SendQuickMenu(4022);
            }break;

        case 17: //Enchanting
            {
				SendQuickMenu(4023);
            }break;
			
        case 18: //Engineering
            {
                Plr->Gossip_SendPOI(368.95, -4723.95, 6, 6, 0, "Mukdrak");
                SendQuickMenu(4024);
            }break;

        case 19: //First Aid
            {
                Plr->Gossip_SendPOI(327.17, -4825.62, 6, 6, 0, "Rawrk");
                SendQuickMenu(4025);
            }break;

        case 20: //Fishing
            {
                Plr->Gossip_SendPOI(-1065.48, -4777.43, 6, 6, 0, "Lau'Tiki");
                SendQuickMenu(4026);
            }break;

        case 21: //Herbalism
            {
				Plr->Gossip_SendPOI(-836.25, -4896.89, 6, 6, 0, "Mishiki");
                SendQuickMenu(4027);
            }break;

        case 22: //Leatherworking
            {
                SendQuickMenu(4028);
            }break;

        case 23: //Mining
            {
				Plr->Gossip_SendPOI(366.94, -4705, 6, 6, 0, "Krunn");
                SendQuickMenu(4029);
            }break;

        case 24: //Skinning
            {
                Plr->Gossip_SendPOI(-2252.94, -291.32, 6, 6, 0, "Yonn Deepcut");
                SendQuickMenu(4030); 
            }break;

		case 25: //Tailoring
            {
                SendQuickMenu(4031);
            }break;
        }
    }
};

class SCRIPT_DECL BrillGuard : public GossipScript
{
public:
    ADD_GOSSIP_FACTORY_FUNCTION(BrillGuard);
    void GossipHello(Creature * pCreature, Player * Plr, bool AutoSend)
    {
        GossipMenu *Menu;
        objmgr.CreateGossipMenuForPlayer(&Menu, pCreature->GetGUID(), 2593, Plr);
        Menu->AddItem(0, "The bank", 1);
        Menu->AddItem(0, "The bat handler", 2);
        Menu->AddItem(0, "The inn", 3);
        Menu->AddItem(0, "The stable master", 4);
        Menu->AddItem(0, "A class trainer", 5);
        Menu->AddItem(0, "A profession trainer", 6);    
        if(AutoSend)
            Menu->SendTo(Plr);
    }

    void GossipSelectOption(Creature* pCreature, Player* Plr, uint32 Id, uint32 IntId)
    {
        GossipMenu * Menu;
        switch(IntId)
        {
        case 0:     // Return to start
            GossipHello(pCreature, Plr, true);
            break;

            //////////////////////
            // Main menu handlers
            /////

        case 1:     // The bank
            SendQuickMenu(4074);
            break;

        case 2:     // The bat handler
            SendQuickMenu(4075);
            break;

        case 3:     // The inn
            SendQuickMenu(4076);
            Plr->Gossip_SendPOI(2246.68, 241.89, 6, 6, 0, "Gallows` End Tavern"); 
			break;

        case 4:     // The stable master
            SendQuickMenu(5978);
            Plr->Gossip_SendPOI(2267.66, 319.32, 6, 6, 0, "Morganus");
            break;

        case 5:     // A class trainer
			{
                objmgr.CreateGossipMenuForPlayer(&Menu, pCreature->GetGUID(), 4292, Plr);
                Menu->AddItem( 0, "Mage"         , 7);
				Menu->AddItem( 0, "Paladin"      , 8);
				Menu->AddItem( 0, "Priest"       , 9);
				Menu->AddItem( 0, "Rogue"       , 10);
				Menu->AddItem( 0, "Warlock"     , 11);
                Menu->AddItem( 0, "Warrior"     , 12);
                Menu->SendTo(Plr);
			}break;

        case 6:     // A profession trainer
            {
                objmgr.CreateGossipMenuForPlayer(&Menu, pCreature->GetGUID(), 4300, Plr);
                Menu->AddItem( 0, "Alchemy"           , 13);
                Menu->AddItem( 0, "Blacksmithing"     , 14);
                Menu->AddItem( 0, "Cooking"           , 15);
                Menu->AddItem( 0, "Enchanting"        , 16);
                Menu->AddItem( 0, "Engineering"       , 17);
                Menu->AddItem( 0, "First Aid"         , 18);
                Menu->AddItem( 0, "Fishing"           , 19);
                Menu->AddItem( 0, "Herbalism"         , 20);
                Menu->AddItem( 0, "Leatherworking"    , 21);
                Menu->AddItem( 0, "Mining"            , 22);
                Menu->AddItem( 0, "Skinning"          , 23);
                Menu->AddItem( 0, "Tailoring"         , 24);
                Menu->SendTo(Plr);
            }break;


            ////////////////
            // Class trainer submenu
            ////////

        case 7: //Mage
            {
                Plr->Gossip_SendPOI(2259.18, 240.93, 6, 6, 0, "Cain Firesong");
                SendQuickMenu(4077);
            }break;        

        case 8: //Paladin
            {
                SendQuickMenu(0); // Need to add correct text
            }break;

        case 9: //Priest
            {
                Plr->Gossip_SendPOI(2259.18, 240.93, 6, 6, 0, "Dark Cleric Beryl");
                SendQuickMenu(4078);
            }break;

        case 10: //Rogue
			{
                Plr->Gossip_SendPOI(2259.18, 240.93, 6, 6, 0, "Marion Call");
                SendQuickMenu(4079);
            }break;

        case 11: //Warlock
            {
                Plr->Gossip_SendPOI(2259.18, 240.93, 6, 6, 0, "Rupert Boch");
                SendQuickMenu(4080);
            }break;

        case 12: //Warrior
            {
                Plr->Gossip_SendPOI(2256.48, 240.32, 6, 6, 0, "Austil de Mon");
                SendQuickMenu(4081);
            }break;

        case 13: //Alchemy
            {
				Plr->Gossip_SendPOI(2263.25, 344.23, 6, 6, 0, "Carolai Anise");
                SendQuickMenu(4082);
            }break;

        case 14: //Blacksmithing
            {
                SendQuickMenu(4083);
            }break;

        case 15: //Cooking
            {
                SendQuickMenu(4084);
            }break;

        case 16: //Enchanting
            {
				Plr->Gossip_SendPOI(2250.35, 249.12, 6, 6, 0, "Vance Undergloom");
				SendQuickMenu(4085);
            }break;
			
        case 17: //Engineering
            {
                SendQuickMenu(4086);
            }break;

        case 18: //First Aid
            {
                Plr->Gossip_SendPOI(2246.68, 241.89, 6, 6, 0, "Nurse Neela");
                SendQuickMenu(4087);
            }break;

        case 19: //Fishing
            {
                Plr->Gossip_SendPOI(2292.37, -10.72, 6, 6, 0, "Clyde Kellen");
                SendQuickMenu(4088);
            }break;

        case 20: //Herbalism
            {
				Plr->Gossip_SendPOI(2268.21, 331.69, 6, 6, 0, "Faruza");
                SendQuickMenu(4089);
            }break;

        case 21: //Leatherworking
            {
				Plr->Gossip_SendPOI(2027, 78.72, 6, 6, 0, "Shelene Rhobart");
                SendQuickMenu(4090);
            }break;

        case 22: //Mining
            {
                SendQuickMenu(4091);
            }break;

        case 23: //Skinning
            {
                Plr->Gossip_SendPOI(2027, 78.72, 6, 6, 0, "Rand Rhobart");
                SendQuickMenu(4092); 
            }break;

		case 24: //Tailoring
            {
				Plr->Gossip_SendPOI(2160.45, 659.93, 6, 6, 0, "Bowen Brisboise");
                SendQuickMenu(4093);
            }break;
        }
    }
};

class SCRIPT_DECL IronforgeGuard : public GossipScript
{
public:
    ADD_GOSSIP_FACTORY_FUNCTION(IronforgeGuard);
    void GossipHello(Creature * pCreature, Player * Plr, bool AutoSend)
    {
        GossipMenu *Menu;
        objmgr.CreateGossipMenuForPlayer(&Menu, pCreature->GetGUID(), 2760, Plr);
        Menu->AddItem(0, "Auction House", 1);
        Menu->AddItem(0, "Bank of Ironforge", 2);
        Menu->AddItem(0, "Deeprun Tram", 3);
        Menu->AddItem(0, "Gryphon Master", 4);
        Menu->AddItem(0, "Guild Master", 5);
        Menu->AddItem(0, "The Inn", 6);    
        Menu->AddItem(0, "Mailbox", 7);
        Menu->AddItem(0, "Stable Master", 8);
        Menu->AddItem(0, "Weapons Trainer", 9);
        Menu->AddItem(0, "Battlemaster", 10);
        Menu->AddItem(0, "Class Trainer", 11);
        Menu->AddItem(0, "Profession Trainer", 12);
        if(AutoSend)
            Menu->SendTo(Plr);
    }

    void GossipSelectOption(Creature* pCreature, Player* Plr, uint32 Id, uint32 IntId)
    {
        GossipMenu * Menu;
        switch(IntId)
        {
        case 0:     // Return to start
            GossipHello(pCreature, Plr, true);
            break;

            //////////////////////
            // Main menu handlers
            /////

        case 1:     // Auction House
            SendQuickMenu(3014);
            Plr->Gossip_SendPOI(-4957.39, -911.6, 6, 6, 0, "Ironforge Auction House");
            break;

        case 2:     // Bank of Ironforge
            SendQuickMenu(2761);
            Plr->Gossip_SendPOI(-4891.91, -991.47, 6, 6, 0, "The Vault");
            break;

        case 3:     // Deeprun Tram
            SendQuickMenu(3814);
            Plr->Gossip_SendPOI(-4835.27, -1294.69, 6, 6, 0, "Deeprun Tram");
            break;

        case 4:     // Gryphon Master
            SendQuickMenu(2762);
            Plr->Gossip_SendPOI(-4821.52, -1152.3, 6, 6, 0, "Ironforge Gryphon Master");
            break;

        case 5:     // Guild Master
            SendQuickMenu(2764);
            Plr->Gossip_SendPOI(-5021, -996.45, 6, 6, 0, "Ironforge Visitor's Center");
            break;

        case 6:     // The Inn
            SendQuickMenu(2768);
            Plr->Gossip_SendPOI(-4850.47, -872.57, 6, 6, 0, "Stonefire Tavern");
			break;

        case 7:     // Mailbox
            SendQuickMenu(2769);
            Plr->Gossip_SendPOI(-4845.7, -880.55, 6, 6, 0, "Ironforge Mailbox");
            break;

	    case 8:     // Stable Master
			SendQuickMenu(5986);
            Plr->Gossip_SendPOI(-5010.2, -1262, 6, 6, 0, "Ulbrek Firehand");
            break;

        case 9:    // Weapon Trainer
			SendQuickMenu(4518);
			Plr->Gossip_SendPOI(-5040, -1201.88, 6, 6, 0, "Bixi and Buliwyf");
            break;

        case 10:    // Battlemaster
			SendQuickMenu(7527);
			Plr->Gossip_SendPOI(-5038.54, -1266.44, 6, 6, 0, "Battlemasters Ironforge");
            break;

        case 11:    // A class trainer
            {
                objmgr.CreateGossipMenuForPlayer(&Menu, pCreature->GetGUID(), 2766, Plr);
                Menu->AddItem( 0, "Hunter"      , 13);
                Menu->AddItem( 0, "Mage"        , 14);
                Menu->AddItem( 0, "Paladin"     , 15);
                Menu->AddItem( 0, "Priest"      , 16);
                Menu->AddItem( 0, "Rogue"       , 17);
                Menu->AddItem( 0, "Warlock"     , 18);
                Menu->AddItem( 0, "Warrior"     , 19);
                Menu->AddItem( 0, "Shaman"      , 20);
                Menu->SendTo(Plr);
            }break;

        case 12:    // A profession trainer
            {
                objmgr.CreateGossipMenuForPlayer(&Menu, pCreature->GetGUID(), 2793, Plr);
                Menu->AddItem( 0, "Alchemy"           , 21);
                Menu->AddItem( 0, "Blacksmithing"     , 22);
                Menu->AddItem( 0, "Cooking"           , 23);
                Menu->AddItem( 0, "Enchanting"        , 24);
				Menu->AddItem( 0, "Engineering"       , 25);
                Menu->AddItem( 0, "First Aid"         , 26);
                Menu->AddItem( 0, "Fishing"           , 27);
                Menu->AddItem( 0, "Herbalism"         , 28);
                Menu->AddItem( 0, "Leatherworking"    , 29);
                Menu->AddItem( 0, "Mining"            , 30);
                Menu->AddItem( 0, "Skinning"          , 31);
                Menu->AddItem( 0, "Tailoring"         , 32);
                Menu->SendTo(Plr);
            }break;

            ////////////////
            // Class trainer submenu
            ////////

        case 13: //Hunter
            {
                Plr->Gossip_SendPOI(-5023, -1253.68, 6, 6, 0, "Hall of Arms");
                SendQuickMenu(2770);
            }break;

        case 14: //Mage
            {
                Plr->Gossip_SendPOI(-4627, -926.45, 6, 6, 0, "Hall of Mysteries");
                SendQuickMenu(2771);
            }break;

        case 15: //Paladin
            {
                Plr->Gossip_SendPOI(-4627.02, -926.45, 6, 6, 0, "Hall of Mysteries");
                SendQuickMenu(2773);
            }break;

        case 16: //Priest
            {
                Plr->Gossip_SendPOI(-4627, -926.45, 6, 6, 0, "Hall of Mysteries");
                SendQuickMenu(2772);
            }break;

        case 17: //Rogue
            {
                Plr->Gossip_SendPOI(-4647.83, -1124, 6, 6, 0, "Ironforge Rogue Trainer");
                SendQuickMenu(2774);
            }break;

        case 18: //Warlock
			{
                Plr->Gossip_SendPOI(-4605, -1110.45, 6, 6, 0, "Ironforge Warlock Trainer");
                SendQuickMenu(2775);
            }break;

        case 19: //Warrior
			{
                Plr->Gossip_SendPOI(-5023.08, -1253.68, 6, 6, 0, "Hall of Arms");
                SendQuickMenu(2776);
            }break;

        case 20: //Shaman
			{
                Plr->Gossip_SendPOI(-4722.02, -1150.66, 6, 6, 0, "Ironforge Shaman Trainer");
                SendQuickMenu(1299);
            }break;

        case 21: //Alchemy
            {
                Plr->Gossip_SendPOI(-4858.5, -1241.83, 6, 6, 0, "Berryfizz's Potions and Mixed Drinks");
                SendQuickMenu(2794);
            }break;

        case 22: //Blacksmithing
            {
                Plr->Gossip_SendPOI(-4796.97, -1110.17, 6, 6, 0, "The Great Forge");
                SendQuickMenu(2795);
            }break;

        case 23: //Cooking
            {
                Plr->Gossip_SendPOI(-4767.83, -1184.59, 6, 6, 0, "The Bronze Kettle");
                SendQuickMenu(2796);
            }break;

        case 24: //Enchanting
            {
                Plr->Gossip_SendPOI(-4803.72, -1196.53, 6, 6, 0, "Thistlefuzz Arcanery");
                SendQuickMenu(2797);
            }break;

		case 25: //Engineering
            {
                Plr->Gossip_SendPOI(-4799.56, -1250.23, 6, 6, 0, "Springspindle's Gadgets");
                SendQuickMenu(2798);
            }break;

        case 26: //First Aid
            {
                Plr->Gossip_SendPOI(-4881.6, -1153.13, 6, 6, 0, "Ironforge Physician");
                SendQuickMenu(2799);
            }break;

        case 27: //Fishing
            {
                Plr->Gossip_SendPOI(-4597.91, -1091.93, 6, 6, 0, "Traveling Fisherman");
                SendQuickMenu(2800);
            }break;

        case 28: //Herbalism
            {
                Plr->Gossip_SendPOI(-4876.9, -1151.92, 6, 6, 0, "Ironforge Physician");
                SendQuickMenu(2801);
            }break;

        case 29: //Leatherworking
            {
                Plr->Gossip_SendPOI(-4745, -1027.57, 6, 6, 0, "Finespindle's Leather Goods");
                SendQuickMenu(2802);
            }break;

        case 30: //Mining
            {
                Plr->Gossip_SendPOI(-4705.06, -1116.43, 6, 6, 0, "Deepmountain Mining Guild");
                SendQuickMenu(2804);
            }break;

        case 31: //Skinning
            {
                Plr->Gossip_SendPOI(-4745, -1027.57, 6, 6, 0, "Finespindle's Leather Goods");
                SendQuickMenu(2805); 
            }break;

		case 32: //Tailoring
            {
                Plr->Gossip_SendPOI(-4719.60, -1056.96, 6, 6, 0, "Stonebrow's Clothier");
                SendQuickMenu(2807);
            }break;
        }
    }
};

class SCRIPT_DECL KharanosGuard : public GossipScript
{
public:
    ADD_GOSSIP_FACTORY_FUNCTION(KharanosGuard);
    void GossipHello(Creature * pCreature, Player * Plr, bool AutoSend)
    {
        GossipMenu *Menu;
        objmgr.CreateGossipMenuForPlayer(&Menu, pCreature->GetGUID(), 2593, Plr);
        Menu->AddItem(0, "Bank", 1);
        Menu->AddItem(0, "Gryphon Master", 2);
        Menu->AddItem(0, "Guild Master", 3);
        Menu->AddItem(0, "The Inn", 4);
        Menu->AddItem(0, "Stable Master", 5);
        Menu->AddItem(0, "Class Trainer", 6);
        Menu->AddItem(0, "Profession Trainer", 7);
        if(AutoSend)
            Menu->SendTo(Plr);
    }

    void GossipSelectOption(Creature* pCreature, Player* Plr, uint32 Id, uint32 IntId)
    {
        GossipMenu * Menu;
        switch(IntId)
        {
        case 0:     // Return to start
            GossipHello(pCreature, Plr, true);
            break;

            //////////////////////
            // Main menu handlers
            /////

        case 1:     //Bank
            SendQuickMenu(4288);
            break;

        case 2:     //Gryphon Master
            SendQuickMenu(4289);
            break;

        case 3:     //Guild Master
            SendQuickMenu(4290);
			break;
			
        case 4:     //The Inn
            SendQuickMenu(4291);
            Plr->Gossip_SendPOI(-5582.66, -525.89, 6, 6, 0, "Thunderbrew Distillery");
			break;

        case 5:     //Stable Master
            SendQuickMenu(5985);
            Plr->Gossip_SendPOI(-5604, -509.58, 6, 6, 0, "Shelby Stoneflint");
            break;

        case 6:     //Class Trainer
			{
                objmgr.CreateGossipMenuForPlayer(&Menu, pCreature->GetGUID(), 4292, Plr);
                Menu->AddItem( 0, "Hunter"       , 8);
				Menu->AddItem( 0, "Mage"         , 9);
				Menu->AddItem( 0, "Paladin"      ,10);
				Menu->AddItem( 0, "Priest"      , 11);
                Menu->AddItem( 0, "Rogue"       , 12);
				Menu->AddItem( 0, "Warlock"     , 13);
                Menu->AddItem( 0, "Warrior"     , 14);
                Menu->SendTo(Plr);
			}break;

        case 7:     // Profession Trainer
            {
                objmgr.CreateGossipMenuForPlayer(&Menu, pCreature->GetGUID(), 4300, Plr);
                Menu->AddItem( 0, "Alchemy"           , 15);
                Menu->AddItem( 0, "Blacksmithing"     , 16);
                Menu->AddItem( 0, "Cooking"           , 17);
                Menu->AddItem( 0, "Enchanting"        , 18);
                Menu->AddItem( 0, "Engineering"       , 19);
                Menu->AddItem( 0, "First Aid"         , 20);
                Menu->AddItem( 0, "Fishing"           , 21);
                Menu->AddItem( 0, "Herbalism"         , 22);
                Menu->AddItem( 0, "Leatherworking"    , 23);
                Menu->AddItem( 0, "Mining"            , 24);
                Menu->AddItem( 0, "Skinning"          , 25);
                Menu->AddItem( 0, "Tailoring"         , 26);
                Menu->SendTo(Plr);
            }break;


            ////////////////
            // Class trainer submenu
            ////////

        case 8: //Hunter
            {
                Plr->Gossip_SendPOI(-5618.29, -454.25, 6, 6, 0, "Grif Wildheart");
                SendQuickMenu(4293);
            }break;

        case 9: //Mage
            {
                Plr->Gossip_SendPOI(-5585.6, -539.99, 6, 6, 0, "Magis Sparkmantle");
                SendQuickMenu(4294);
            }break;

        case 10: //Paladin
            {
                Plr->Gossip_SendPOI(-5585.6, -539.99, 6, 6, 0, "Azar Stronghammer");
                SendQuickMenu(4295);
            }break;

        case 11: //Priest
			{
                Plr->Gossip_SendPOI(-5591.74, -525.61, 6, 6, 0, "Maxan Anvol");
                SendQuickMenu(4296);
            }break;

        case 12: //Rogue
            {
                Plr->Gossip_SendPOI(-5602.75, -542.4, 6, 6, 0, "Hogral Bakkan");
                SendQuickMenu(4297);
            }break;

        case 13: //Warlock
            {
                Plr->Gossip_SendPOI(-5641.97, -523.76, 6, 6, 0, "Gimrizz Shadowcog");
                SendQuickMenu(4298);
            }break;

        case 14: //Warrior
            {
                Plr->Gossip_SendPOI(-5604.79, -529.38, 6, 6, 0, "Granis Swiftaxe");
                SendQuickMenu(4299);
            }break;

        case 15: //Alchemy
            {
                SendQuickMenu(4301);
            }break;

        case 16: //Blacksmithing
            {
				Plr->Gossip_SendPOI(-5584.72, -428.41, 6, 6, 0, "Tognus Flintfire");
                SendQuickMenu(4302);
            }break;

        case 17: //Cooking
            {
				Plr->Gossip_SendPOI(-5596.85, -541.43, 6, 6, 0, "Gremlock Pilsnor");
                SendQuickMenu(4303);
            }break;

        case 18: //Enchanting
            {
				SendQuickMenu(4304);
            }break;
			
        case 19: //Engineering
            {
                SendQuickMenu(4305);
            }break;

        case 20: //First Aid
            {
                Plr->Gossip_SendPOI(-5603.67, -523.57, 6, 6, 0, "Thamner Pol");
                SendQuickMenu(4306);
            }break;

        case 21: //Fishing
            {
                Plr->Gossip_SendPOI(-5202.39, -51.36, 6, 6, 0, "Paxton Ganter");
                SendQuickMenu(4307);
            }break;

        case 22: //Herbalism
            {
                SendQuickMenu(4308);
            }break;

        case 23: //Leatherworking
            {
                SendQuickMenu(4310);
            }break;

        case 24: //Mining
            {
				Plr->Gossip_SendPOI(-5531, -666.53, 6, 6, 0, "Yarr Hamerstone");
                SendQuickMenu(4311);
            }break;

        case 25: //Skinning
            {
                SendQuickMenu(4312);
            }break;

		case 26: //Tailoring
            {
                SendQuickMenu(4313);
            }break;
        }
    }
};

void SetupGuardGossip(ScriptMgr * mgr)
{
    /* Guard List */
    mgr->register_gossip_script(1423,  &GoldshireGuard::Create);        // Stormwind Guard 
    mgr->register_gossip_script(68,    &StormwindGuard::Create);        // Stormwind City Guard
    mgr->register_gossip_script(1976,  &StormwindGuard::Create);        // Stormwind City Patroller
    mgr->register_gossip_script(4262,  &DarnassusGuard::Create);        // Darnassus Sentinel
    mgr->register_gossip_script(5624,  &UndercityGuard::Create);		// Undercity Guardian
    mgr->register_gossip_script(3571,  &DolanaarGuard::Create);			// Teldrassil Sentinel
	mgr->register_gossip_script(16222, &SilvermoonGuard::Create);	    // Silvermoon City Guardian
	//mgr->register_gossip_script(18103, &SilvermoonGuard::Create);	    // Arcane Guardian
	//mgr->register_gossip_script(20672, &SilvermoonGuard::Create);	    // Royal Guard
	mgr->register_gossip_script(16733, &ExodarGuard::Create);	        // Exodar Peacekeeper
	//mgr->register_gossip_script(20674, &ExodarGuard::Create);	        // Shield of Velen
	mgr->register_gossip_script(3296, &OrgrimmarGuard::Create);	        // Orgrimmar Grunt
	mgr->register_gossip_script(3084, &ThunderbluffGuard::Create);	    // Bluffwatcher
	mgr->register_gossip_script(3222, &BloodhoofGuard::Create);			// Brave Wildrunner
	mgr->register_gossip_script(3224, &BloodhoofGuard::Create);			// Brave Cloudmane
	mgr->register_gossip_script(3220, &BloodhoofGuard::Create);			// Brave Darksky
	mgr->register_gossip_script(3219, &BloodhoofGuard::Create);			// Brave Leaping Deer
	mgr->register_gossip_script(3217, &BloodhoofGuard::Create);			// Brave Dawneagle
	mgr->register_gossip_script(3215, &BloodhoofGuard::Create);			// Brave Strongbash
	mgr->register_gossip_script(3218, &BloodhoofGuard::Create);			// Brave Swiftwind
	mgr->register_gossip_script(3221, &BloodhoofGuard::Create);			// Brave Rockhorn
	mgr->register_gossip_script(3223, &BloodhoofGuard::Create);			// Brave Rainchaser
	mgr->register_gossip_script(3212, &BloodhoofGuard::Create);			// Brave Ironhorn
	mgr->register_gossip_script(5953, &RazorHillGuard::Create);			// Razor Hill Grunt
	mgr->register_gossip_script(5725, &BrillGuard::Create);				// Deathguard Lundmark
	mgr->register_gossip_script(1738, &BrillGuard::Create);				// Deathguard Terrence
	mgr->register_gossip_script(1652, &BrillGuard::Create);				// Deathguard Burgess
	mgr->register_gossip_script(1746, &BrillGuard::Create);				// Deathguard Cyrus
	mgr->register_gossip_script(1745, &BrillGuard::Create);				// Deathguard Morris
	mgr->register_gossip_script(1743, &BrillGuard::Create);				// Deathguard Lawrence
	mgr->register_gossip_script(1744, &BrillGuard::Create);				// Deathguard Mort
	mgr->register_gossip_script(1496, &BrillGuard::Create);				// Deathguard Dillinger
	mgr->register_gossip_script(1742, &BrillGuard::Create);				// Deathguard Bartholomew
	mgr->register_gossip_script(5595, &IronforgeGuard::Create);			// Ironforge Guard
	mgr->register_gossip_script(727,  &KharanosGuard::Create);			// Ironforge Mountaineer
}	

	// To Bloodhoof Guards - I don't know if those are all guards with dialog menu,
	// but they were all I could find. Same to Deathguards.

	// I wasn't sure of commented guards so if they are correct (I mean gossip to NPC)
	// then delete comment.

	// To do:
	//	- Rewrite Exodar and Silvermoon Scripts to delete part about Battlemasters,
	//	  because there must be only one options for all masters.
	//	- Make whole menu (with options) blizzlike for Exodar and Silvermoon (need
	//	  infos about it with screenshots of menus (normal, class & profession) as
	//	  screens/infos about map arrow directory and it's name.
	//	- Added (if needed) Scripts for "mini cities" (like Brill) for Dranaei's & Blood
	//	  Elf's starting locations.
	//	- Add (eventually) missing guards which should use one of those guard menus.

