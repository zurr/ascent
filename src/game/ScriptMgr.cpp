/****************************************************************************
 *
 * General Object Type File
 * Copyright (c) 2007 Antrix Team
 *
 * This file may be distributed under the terms of the Q Public License
 * as defined by Trolltech ASA of Norway and appearing in the file
 * COPYING included in the packaging of this file.
 *
 * This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 * WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 */

#include "StdAfx.h"
#ifndef WIN32
#include <dlfcn.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <cstdlib>
#include <cstring>
#endif

initialiseSingleton(ScriptMgr);

void ScriptMgr::LoadScripts()
{
	sLog.outString("Loading External Script Libraries...");
	sLog.outString("");

	string start_path = Config.MainConfig.GetStringDefault("Script.BinaryLocation", "script_bin") + "\\";
	string search_path = start_path + "*.";

	/* Loading system for win32 */
#ifdef WIN32
	search_path += "dll";

	WIN32_FIND_DATA data;
	uint32 count = 0;
	HANDLE find_handle = FindFirstFile(search_path.c_str(), &data);
	if(find_handle == INVALID_HANDLE_VALUE)
		sLog.outError("  No external scripts found! Server will continue to function with limited functionality.");
	else
	{
		do
		{
			string full_path = start_path + data.cFileName;
			HMODULE mod = LoadLibrary(full_path.c_str());
			printf("  %s : 0x%.08X : ", data.cFileName, ((uint32)mod));
			if(mod == 0)
				printf("error!\n");
			else
			{
				// find version import
				exp_get_version vcall = (exp_get_version)GetProcAddress(mod, "_exp_get_version");
				exp_script_register rcall = (exp_script_register)GetProcAddress(mod, "_exp_script_register");
				if(vcall == 0 || rcall == 0)
				{
					printf("version functions not found!\n");
					FreeLibrary(mod);
				}
				else
				{
					uint32 version = vcall();
					if(UINT32_LOPART(version) == SCRIPTLIB_VERSION_MINOR && UINT32_HIPART(version) == SCRIPTLIB_VERSION_MAJOR)
					{
						_handles.push_back(((SCRIPT_MODULE)mod));
						printf("v%u.%u : ", UINT32_HIPART(version), UINT32_LOPART(version));
						rcall(this);
						printf("loaded.\n");

						++count;
					}
					else
					{
						FreeLibrary(mod);
						printf("version mismatch!\n");						
					}
				}
			}
		}
		while(FindNextFile(find_handle, &data));
		FindClose(find_handle);
		sLog.outString("");
		sLog.outString("Loaded %u external libraries.", count);
		sLog.outString("");
	}
#else
	/* Loading system for *nix */
	struct dirent ** list;
	int filecount = scandir(PREFIX "/lib/", &list, 0, 0);
	uint32 count = 0;

	if(!filecount || !list)
		sLog.outError("  No external scripts found! Server will continue to function with limited functionality.");
	else
	{
char *ext;
		while(filecount--)
		{
			ext = strrchr(list[filecount]->d_name, '.');
			if (ext != NULL && !strcmp(ext, "so")) {
				string full_path = "../lib/" + string(list[filecount]->d_name);
				SCRIPT_MODULE mod = dlopen(full_path.c_str(), RTLD_NOW);
				printf("  %s : 0x%08X : ", list[filecount]->d_name, mod);
				if(mod == 0)
					printf("error!\n");
				else
				{
					// find version import
					exp_get_version vcall = (exp_get_version)dlsym(mod, "_exp_get_version");
					exp_script_register rcall = (exp_script_register)dlsym(mod, "_exp_script_register");
					if(vcall == 0 || rcall == 0)
					{
						printf("version functions not found!\n");
						dlclose(mod);
					}
					else
					{
						uint32 version = vcall();
						if(UINT32_LOPART(version) == SCRIPTLIB_VERSION_MINOR && UINT32_HIPART(version) == SCRIPTLIB_VERSION_MAJOR)
						{
							_handles.push_back(((SCRIPT_MODULE)mod));
							printf("v%u.%u : ", UINT32_HIPART(version), UINT32_LOPART(version));
							rcall(this);
							printf("loaded.\n");

							++count;
						}
						else
						{
							dlclose(mod);
							printf("version mismatch!\n");						
						}
					}
				}
			}
			free(list[filecount]);
		}
		free(list);
		sLog.outString("");
		sLog.outString("Loaded %u external libraries.", count);
		sLog.outString("");
	}
#endif
}

void ScriptMgr::UnloadScripts()
{
	LibraryHandleMap::iterator itr = _handles.begin();
	for(; itr != _handles.end(); ++itr)
	{
#ifdef WIN32
		FreeLibrary(((HMODULE)*itr));
#else
		dlclose(*itr);
#endif
	}
	_handles.clear();
}

void ScriptMgr::register_creature_script(uint32 entry, exp_create_creature_ai callback)
{
	_creatures.insert( CreatureCreateMap::value_type( entry, callback ) );
}

void ScriptMgr::register_gameobject_script(uint32 entry, exp_create_gameobject_ai callback)
{
	_gameobjects.insert( GameObjectCreateMap::value_type( entry, callback ) );
}

void ScriptMgr::register_dummy_aura(uint32 entry, exp_handle_dummy_aura callback)
{
	_auras.insert( HandleDummyAuraMap::value_type( entry, callback ) );
}

void ScriptMgr::register_dummy_spell(uint32 entry, exp_handle_dummy_spell callback)
{
	_spells.insert( HandleDummySpellMap::value_type( entry, callback ) );
}

void ScriptMgr::register_gossip_script(uint32 entry, exp_create_gossip_script callback)
{
	_gossips.insert( GossipCreateMap::value_type( entry, callback ) );
}

CreatureAIScript* ScriptMgr::CreateAIScriptClassForEntry(Creature* pCreature)
{
	CreatureCreateMap::iterator itr = _creatures.find(pCreature->GetEntry());
	if(itr == _creatures.end())
		return NULL;

	exp_create_creature_ai function_ptr = itr->second;
	return (function_ptr)(pCreature);
}

GameObjectAIScript * ScriptMgr::CreateAIScriptClassForGameObject(uint32 uEntryId, GameObject* pGameObject)
{
	GameObjectCreateMap::iterator itr = _gameobjects.find(pGameObject->GetEntry());
	if(itr == _gameobjects.end())
		return NULL;

	exp_create_gameobject_ai function_ptr = itr->second;
	return (function_ptr)(pGameObject);
}

GossipScript * ScriptMgr::GetGossipScript(uint32 uEntryId)
{
	GossipCreateMap::iterator itr = _gossips.find(uEntryId);
	if(itr == _gossips.end())
	{
		return new GossipScript;
	}

	exp_create_gossip_script function_ptr = itr->second;
	return (function_ptr)();
}

bool ScriptMgr::CallScriptedDummySpell(uint32 uSpellId, uint32 i, Spell* pSpell)
{
	HandleDummySpellMap::iterator itr = _spells.find(uSpellId);
	if(itr == _spells.end())
		return false;

	exp_handle_dummy_spell function_ptr = itr->second;
	return (function_ptr)(i, pSpell);
}

bool ScriptMgr::CallScriptedDummyAura(uint32 uSpellId, uint32 i, Aura* pAura, bool apply)
{
	HandleDummyAuraMap::iterator itr = _auras.find(uSpellId);
	if(itr == _auras.end())
		return false;

	exp_handle_dummy_aura function_ptr = itr->second;
	return (function_ptr)(i, pAura, apply);
}

/* CreatureAI Stuff */
CreatureAIScript::CreatureAIScript(Creature* creature) : _unit(creature)
{

}

void CreatureAIScript::RegisterAIUpdateEvent(uint32 frequency)
{
	sEventMgr.AddEvent(_unit, &Creature::CallScriptUpdate, EVENT_SCRIPT_UPDATE_EVENT, frequency, 0);
}

void CreatureAIScript::RemoveAIUpdateEvent()
{
	sEventMgr.RemoveEvents(_unit, EVENT_SCRIPT_UPDATE_EVENT);
}

/* GameObjectAI Stuff */

GameObjectAIScript::GameObjectAIScript(GameObject* goinstance) : _gameobject(goinstance)
{

}

void GameObjectAIScript::RegisterAIUpdateEvent(uint32 frequency)
{
	sEventMgr.AddEvent(_gameobject, &GameObject::CallScriptUpdate, EVENT_SCRIPT_UPDATE_EVENT, frequency, 0);
}


/* InstanceAI Stuff */

InstanceScript::InstanceScript(MapMgr *instance) : _instance(instance)
{
}

/* QuestScript Stuff */

QuestScript::QuestScript(QuestLogEntry *qle) : _qLogEntry(qle)
{
}

void QuestScript::RegisterQuestEvent(uint32 frequency)
{
	sEventMgr.AddEvent(_qLogEntry, &QuestLogEntry::CallScriptUpdate, EVENT_SCRIPT_UPDATE_EVENT, frequency, 0);
}

void QuestScript::RemoveQuestEvent()
{
	sEventMgr.RemoveEvents(_qLogEntry, EVENT_SCRIPT_UPDATE_EVENT);
}

/* Gossip Stuff*/

GossipScript::GossipScript()
{
	AutoCreated = false;
}

void GossipScript::GossipEnd(Creature* pCreature, Player* Plr)
{
	Plr->CleanupGossipMenu();
}

void GossipScript::GossipHello(Creature* pCreature, Player* Plr, bool AutoSend)
{
	GossipMenu *Menu;
	uint32 TextID = 2;
	Trainer *pTrainer = pCreature->GetTrainer();
	uint32 Text = objmgr.GetGossipTextForNpc(pCreature->GetEntry());
	if(Text != 0)
	{
		GossipText * text = objmgr.GetGossipText(Text);
		if(text != 0)
			TextID = Text;
	}

	objmgr.CreateGossipMenuForPlayer(&Menu, pCreature->GetGUID(), TextID, Plr);
	
	uint32 flags = pCreature->GetUInt32Value(UNIT_NPC_FLAGS);

	if(flags & UNIT_NPC_FLAG_VENDOR)
		Menu->AddItem(1, "I would like to browse your goods", 1);
	
	if(flags & UNIT_NPC_FLAG_TRAINER && pTrainer != 0)
	{
		string name = pCreature->GetCreatureName()->Name;
		string::size_type pos = name.find(" ");	  // only take first name
		if(pos != string::npos)
			name = name.substr(0, pos);

		string msg = "I seek ";

		if(pTrainer->RequiredClass && pTrainer->RequiredClass != Plr->getClass())
		{
			//							   WARRIOR   PALADIN HUNTER  ROGUE   PRIEST  -   SHAMAN  MAGE   WARLOCK  - DRUID
			uint32 notclass[13] =   {0	  ,5721	 ,3976   ,5839   ,4797   ,4435   ,0  ,5006   ,539	,5836   ,0,4774};
			Menu->SetTextID(notclass[pTrainer->RequiredClass]);
		} else if(pTrainer->RequiredClass) {
			//							   WARRIOR   PALADIN HUNTER  ROGUE   PRIEST  -   SHAMAN  MAGE   WARLOCK  - DRUID
			uint32 isclass[13] =	{0	  ,1040	 ,3974   ,4864   ,4835   ,4436   ,0  ,5005   ,538	,5835   ,0,4775};
			Menu->SetTextID(isclass[pTrainer->RequiredClass]);

			switch(Plr->getClass())
			{
			case MAGE:
				msg += "mage";
				break;
			case SHAMAN:
				msg += "shaman";
				break;
			case WARRIOR:
				msg += "warrior";
				break;
			case PALADIN:
				msg += "paladin";
				break;
			case WARLOCK:
				msg += "warlock";
				break;
			case HUNTER:
				msg += "hunter";
				break;
			case ROGUE:
				msg += "rogue";
				break;
			case DRUID:
				msg += "druid";
				break;
			case PRIEST:
				msg += "priest";
				break;
			}
			msg += " training, ";
			msg += name;
			msg += ".";

			Menu->AddItem(3, msg.c_str(), 2);

		}
		else
		{
			msg += "training, ";
			msg += name;
			msg += ".";

			Menu->AddItem(3, msg.c_str(), 2);
		}
	}

	if(flags & UNIT_NPC_FLAG_TAXIVENDOR)
		Menu->AddItem(2, "Give me a ride.", 3);

	if(flags & UNIT_NPC_FLAG_AUCTIONEER)
		Menu->AddItem(0, "I would like to make a bid.", 4);

	if(flags & UNIT_NPC_FLAG_INNKEEPER)
		Menu->AddItem(5, "Make this inn your home.", 5);

	if(flags & UNIT_NPC_FLAG_BANKER)
		Menu->AddItem(0, "I would like to check my deposit box.", 6);

	if(flags & UNIT_NPC_FLAG_SPIRITHEALER)
		Menu->AddItem(0, "Bring me back to life.", 7);

	if(flags & UNIT_NPC_FLAG_PETITIONER)
		Menu->AddItem(0, "How do I create a guild?", 8);

	if(flags & UNIT_NPC_FLAG_TABARDCHANGER)
		Menu->AddItem(0, "I want to create a guild crest.", 9);

	/*if(flags & UNIT_NPC_FLAG_BATTLEFIELDPERSON)
		Menu->AddItem(0, "I would like to go to the battleground.", 10);*/

	if( pTrainer &&
		pTrainer->RequiredClass &&					  // class trainer
		pTrainer->RequiredClass == Plr->getClass() &&   // correct class
		pCreature->getLevel() > 10 &&				   // creature level
		Plr->getLevel() > 10 )						  // player level
	{
		Menu->AddItem(0, "I would like to reset my talents.", 11);
	}

	if(AutoSend)
		Menu->SendTo(Plr);
}

void GossipScript::GossipSelectOption(Creature* pCreature, Player* Plr, uint32 Id, uint32 IntId)
{
	switch(IntId)
	{
	case 1:
		// vendor
		Plr->GetSession()->SendInventoryList(pCreature);
		break;
	case 2:
		// trainer
		Plr->GetSession()->SendTrainerList(pCreature);
		break;
	case 3:
		// taxi
		Plr->GetSession()->SendTaxiList(pCreature);
		break;
	case 4:
		// auction
		Plr->GetSession()->SendAuctionList(pCreature);
		break;
	case 5:
		// innkeeper
		Plr->GetSession()->SendInnkeeperBind(pCreature);
		break;
	case 6:
		// banker
		Plr->GetSession()->SendBankerList(pCreature);
		break;
	case 7:
		// spirit
		Plr->GetSession()->SendSpiritHealerRequest(pCreature);
		break;
	case 8:
		// petition
		Plr->GetSession()->SendCharterRequest(pCreature);
		break;
	case 9:
		// guild crest
		Plr->GetSession()->SendTabardHelp(pCreature);
		break;
	case 10:
		// battlefield
		Plr->GetSession()->SendBattlegroundList(pCreature, 2);
		break;
	case 11:
		// switch to talent reset message
		{
			GossipMenu *Menu;
			objmgr.CreateGossipMenuForPlayer(&Menu, pCreature->GetGUID(), 5674, Plr);
			Menu->AddItem(0, "I understand, continue.", 12);
			Menu->SendTo(Plr);
		}break;
	case 12:
		// talent reset
		{
			Plr->Gossip_Complete();
			Plr->SendTalentResetConfirm();
		}break;

	default:
		script_errorlog("Unknown IntId %u on entry %u", IntId, pCreature->GetEntry());
		break;
	}	
}

void GossipScript::Destroy()
{
	delete this;
}

