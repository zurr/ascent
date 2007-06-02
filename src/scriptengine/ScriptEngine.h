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

#include "gmMachine.h"
#include "gmUserObject.h"
#include "gmCall.h"

#ifndef SCRIPTENGINE_H
#define SCRIPTENGINE_H

/** @class ScriptEngine
 * Provides an interface for creatures to interface with serverside scripts. This class is created
 * once per instance.
 */

class ScriptEngine
{
	/** gmMachine Instance
	 */
	gmMachine * m_machine;

	/** gmType variables (for player, unit, gameobject)
	 */
	gmType m_playerType;
	gmType m_unitType;
	gmType m_gameObjectType;
	gmType m_questType;
	gmType m_spellType;
	gmType m_auraType;
	gmType m_areaTriggerType;
	gmType m_scriptEngineType;
	
	/** ScriptMap - binding of events to gm functions
	 */
	typedef HM_NAMESPACE::hash_map<uint32, map<uint32, gmFunctionObject*> > ScriptMap;
	typedef HM_NAMESPACE::hash_map<uint32, gmFunctionObject*> SingleScriptMap;
	ScriptMap m_playerMap;
	ScriptMap m_unitMap;
	ScriptMap m_gameObjectMap;
	ScriptMap m_questMap;
	ScriptMap m_spellMap;
	ScriptMap m_auraMap;
	SingleScriptMap m_areaTriggerMap;

	/** Mutex - ensures a script isn't executed twice at once
	 */
	Mutex m_lock;

	/** User objects, used in passing of arguments and used for 'this' pointer.
	 */
	gmVariable m_variables[10];
	gmUserObject * m_userObjects[10];

	/** Gets the function table of scriptable exports.
	 */
	void SetPlayerFunctionTable();
	void SetUnitFunctionTable();
	void SetGameObjectFunctionTable();
	void SetQuestFunctionTable();
	void SetSpellFunctionTable();
	void SetAuraFunctionTable();
	void SetAreaTriggerFunctionTable();
	void SetScriptEngineFunctionTable();

public:
	/** Function pointer setting/adding public functions
	 */
	inline void AddAreaTriggerEvent(uint32 Entry, gmFunctionObject * func) { m_areaTriggerMap[Entry] = func; }

	/** Constructor - does nothing but nulls out variables
	 */
	ScriptEngine();

	/** Destructor - does nothing but frees pointers
	 */
	~ScriptEngine();

	/** Reloads the script system, recompiling all scripts and registering any functions.
	 */
	void Reload();

	/** Registers a script trigger to a function in script.
	 */
	void RegisterAreaTriggerEvent(uint32 Entry, gmFunctionObject * Function, gmType Type);

	/** Executes a script file
	 */
	void ExecuteScriptFile(const char * filename);

	/** Dumps the error log from the script engine
	 */
	void DumpErrors();

	/** Sets up a gm call with the specified number of arguments
	 */
	void DoGMCall(gmFunctionObject * obj, gmUserObject * thisObj, ...);

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/** Looks up script for areatrigger id x and executes it, return value of true if you can activate it.
	 */
	bool OnActivateAreaTrigger(AreaTrigger * at, Player * plr);
};

/* gonna make one global scriptengine for testing */
extern ScriptEngine * ScriptSystem;

#endif

