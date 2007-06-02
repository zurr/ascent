/****************************************************************************
 *
 * Script Engine
 * Copyright (c) 2007 Antrix Team
 *
 * This file may be distributed under the terms of the Q Public License
 * as defined by Trolltech ASA of Norway and appearing in the file
 * COPYING included in the packaging of this file.
 *
 * This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 * WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 * GameMonkey Script License
 * Copyright (c) 2003 Auran Development Ltd.
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
 * associated documentation files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do
 * so, subject to the following conditions:
 * The above copyright notice and this permission notice shall be included in all copies or substantial
 * portions of the Software.
 *
 */

#include "StdAfx.h"
ScriptEngine * ScriptSystem;

#ifndef WIN32
#include <dirent.h>
#endif

ScriptEngine::ScriptEngine()
{
	m_playerType = m_unitType = m_gameObjectType = m_questType = m_spellType = m_auraType = m_areaTriggerType = m_scriptEngineType = -1;
	m_machine = 0;
}

ScriptEngine::~ScriptEngine()
{
	if(m_machine)
		delete m_machine;
}

void ScriptEngine::Reload()
{
	if(m_machine)
	{
		m_playerType = m_unitType = m_gameObjectType = m_questType = m_spellType = m_auraType = m_areaTriggerType = m_scriptEngineType = -1;
		for(int i = 0; i < 10; ++i)
			m_userObjects[i] = 0;

		m_playerMap.clear();
		m_unitMap.clear();
		m_gameObjectMap.clear();
		m_questMap.clear();
		m_auraMap.clear();
		m_areaTriggerMap.clear();
		
		m_machine->ResetAndFreeMemory();
		delete m_machine;

		m_machine = 0;
	}

	m_machine = new gmMachine;
	
	/* create our types */
	m_playerType		= m_machine->CreateUserType("Player");
	m_unitType			= m_machine->CreateUserType("Unit");
	m_gameObjectType	= m_machine->CreateUserType("GameObject");
	m_questType			= m_machine->CreateUserType("Quest");
	m_spellType			= m_machine->CreateUserType("Spell");
	m_auraType			= m_machine->CreateUserType("Aura");
	m_areaTriggerType	= m_machine->CreateUserType("AreaTrigger");
	m_scriptEngineType	= m_machine->CreateUserType("ScriptEngine");

	/* register all our functions inside the machine */
	SetPlayerFunctionTable();
	SetUnitFunctionTable();
	SetGameObjectFunctionTable();
	SetQuestFunctionTable();
	SetSpellFunctionTable();
	SetAuraFunctionTable();
	SetAreaTriggerFunctionTable();
	SetScriptEngineFunctionTable();

	/* allocate our user data variables */
	for(int i = 0; i < 10; ++i)
	{
		m_userObjects[i] = m_machine->AllocUserObject(this, m_scriptEngineType);
		m_machine->AddCPPOwnedGMObject(m_userObjects[i]);
	}

#ifdef WIN32
	/* compile the scripts */
	WIN32_FIND_DATA fd;
	HANDLE f = FindFirstFile("scripts\\*.gm", &fd);
	if(f != INVALID_HANDLE_VALUE)
	{
		do 
		{
			string fname = "scripts\\";
			fname += fd.cFileName;

			ExecuteScriptFile(fname.c_str());
		} while(FindNextFile(f, &fd));
		FindClose(f);
	}
#else
	/* compile scripts */
	struct dirent ** list;
	int filecount = scandir(PREFIX "/lib/", &list, 0, 0);
	if(!filecount || !list)
		return;
	
	char * ext;
	while(filecount--)
	{
		ext = strrchr(list[filecount]->d_name, '.');
		if(ext != NULL && !strcmp(ext, ".gm"))
		{
			string full_path = "scripts/" + string(list[filecount]->d_name);
			ExecuteScriptFile(full_path.c_str());
		}
		free(list[filecount]);
	}
	free(list);
#endif
}

void ScriptEngine::ExecuteScriptFile(const char * filename)
{
	char * data;
	FILE * f = fopen(filename, "rb");
	fseek(f, 0, SEEK_END);
	int size = ftell(f);
	fseek(f, 0, SEEK_SET);

	data = new char[size+1];
	fread(data, 1, size, f);
	data[size] = 0;

	fclose(f);

	/* setup 'this' pointer */
	m_userObjects[0]->m_user = this;
	m_userObjects[0]->m_userType = m_scriptEngineType;

	/* set 'this' variable */
	m_variables[0].SetUser(m_userObjects[0]);

	int threadid;
	int no_errors = m_machine->ExecuteString(data, &threadid, true, filename, &m_variables[0]);
	if(no_errors)
	{
		printf("Errors occured while compiling %s.\n", filename);
		DumpErrors();
	}
}

void ScriptEngine::DumpErrors()
{
	const char * message;
	bool first = true;
	while(message = m_machine->GetLog().GetEntry(first))
		printf("  %s", message);
}

bool ScriptEngine::OnActivateAreaTrigger(AreaTrigger * at, Player * plr)
{
	SingleScriptMap::iterator itr = m_areaTriggerMap.find(at->AreaTriggerID);
	if(itr == m_areaTriggerMap.end())
		return true;

	/*map<uint32, gmFunctionObject*>::iterator it2 = itr->second.find(0);
	if(it2 == itr->second.end())
		return true;*/

	m_lock.Acquire();

	ASSERT(itr->second->GetType() == GM_FUNCTION);

	// Setup 'this' pointer to the areatrigger struct.
	m_userObjects[0]->m_user = at;
	m_userObjects[0]->m_userType = m_areaTriggerType;
	m_variables[0].SetUser(m_userObjects[0]);

	// Setup the first argument (the player entering)
	m_userObjects[1]->m_user = plr;
	m_userObjects[1]->m_userType = m_playerType;
	m_variables[1].SetUser(m_userObjects[1]);

	// Setup the call.
	gmCall call;
	if(call.BeginFunction(m_machine, itr->second, m_variables[0], false))
	{
		call.AddParam(m_variables[1]);
		call.End();
        
		int res;
		if(!call.GetReturnedInt(res))
		{
			printf("Call failed.");
			DumpErrors();
			m_lock.Release();
			return true;
		}

		m_lock.Release();
		return (res > 0) ? true : false;
	}
	else
	{
		printf("Could not find function!");
		DumpErrors();
		m_lock.Release();
		return true;
	}
}

