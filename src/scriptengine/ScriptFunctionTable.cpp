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

void ScriptEngine::SetPlayerFunctionTable()
{
	static gmFunctionEntry table[] = {
		{ "BroadcastMessage",					Player_BroadcastMessage			},
		{ "SendAreaTriggerMessage",				Player_SendAreaTriggerMessage	},
		{ "GetClass",							Player_GetClass					},
		{ "GetRace",							Player_GetRace					},
		{ "Teleport",							Player_Teleport					},
		{ "GetStanding",						Player_GetReputationValue		},
		{ "GetStandingRank",					Player_GetReputationRank		},
		{ "HasFinishedQuest",					Player_HasFinishedQuest			},
		{ "LearnSpell",							Player_LearnSpell				},
		{ "UnlearnSpell",						Player_RemoveSpell				},
		{ "AddItem",							Player_AddItem					},
		{ "RemoveItem",							Player_RemoveItem				},

		// Imported Unit Functions
		{ "Emote",								Unit_Emote						},
		{ "SendChatMessage",					Unit_SendChatMessage			},
		{ "Despawn",							Unit_Despawn					},
		{ "CastSpell",							Unit_CastSpell					},
		{ "CastSpellOnTarget",					Unit_CastSpellOnTarget			},
	};

	m_machine->RegisterTypeLibrary(m_playerType, table, sizeof(table) / sizeof(table[0]));
}

void ScriptEngine::SetUnitFunctionTable()
{
	static gmFunctionEntry table[] = {
		{ "Emote",								Unit_Emote						},
		{ "SendChatMessage",					Unit_SendChatMessage			},
		{ "Despawn",							Unit_Despawn					},
		{ "CastSpell",							Unit_CastSpell					},
		{ "CastSpellOnTarget",					Unit_CastSpellOnTarget			},
	};

	m_machine->RegisterTypeLibrary(m_unitType, table, sizeof(table) / sizeof(table[0]));
}

void ScriptEngine::SetQuestFunctionTable()
{
	static gmFunctionEntry table[] = {
		{ "test", 0 },
	};

	//m_machine->RegisterTypeLibrary(m_playerType, table, sizeof(table) / sizeof(table[0]));
}

void ScriptEngine::SetGameObjectFunctionTable()
{
	static gmFunctionEntry table[] = {
		{ "test", 0 },
	};

	//m_machine->RegisterTypeLibrary(m_playerType, table, sizeof(table) / sizeof(table[0]));
}

void ScriptEngine::SetSpellFunctionTable()
{
	static gmFunctionEntry table[] = {
		{ "test", 0 },
	};

	//m_machine->RegisterTypeLibrary(m_playerType, table, sizeof(table) / sizeof(table[0]));
}

void ScriptEngine::SetAuraFunctionTable()
{
	static gmFunctionEntry table[] = {
		{ "test", 0 },
	};

	//m_machine->RegisterTypeLibrary(m_playerType, table, sizeof(table) / sizeof(table[0]));
}

void ScriptEngine::SetAreaTriggerFunctionTable()
{
	static gmFunctionEntry table[] = {
		{ "GetEntry",							AreaTrigger_GetEntry			},
	};

	m_machine->RegisterTypeLibrary(m_areaTriggerType, table, sizeof(table) / sizeof(table[0]));
}

void ScriptEngine::SetScriptEngineFunctionTable()
{
	static gmFunctionEntry table[] = {
		{ "RegisterUnitEvent",					ScriptEngine_RegisterUnitEvent	},
		{ "RegisterGameObjectEvent",			ScriptEngine_RegisterGameObjectEvent },
		{ "RegisterPlayerEvent",				ScriptEngine_RegisterPlayerEvent },
		{ "RegisterAreaTriggerEvent",			ScriptEngine_RegisterAreaTriggerEvent },
		{ "RegisterQuestEvent",					ScriptEngine_RegisterQuestEvent	},
	};

	m_machine->RegisterTypeLibrary(m_scriptEngineType, table, sizeof(table) / sizeof(table[0]));
}
