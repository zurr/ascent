/****************************************************************************
 *
 * General Object Type File
 * Copyright (c) 2007 Team Ascent
 *
 * This work is licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 3.0
 * License. To view a copy of this license, visit
 * http://creativecommons.org/licenses/by-nc-sa/3.0/ or send a letter to Creative Commons,
 * 543 Howard Street, 5th Floor, San Francisco, California, 94105, USA.
 *
 * EXCEPT TO THE EXTENT REQUIRED BY APPLICABLE LAW, IN NO EVENT WILL LICENSOR BE LIABLE TO YOU
 * ON ANY LEGAL THEORY FOR ANY SPECIAL, INCIDENTAL, CONSEQUENTIAL, PUNITIVE OR EXEMPLARY DAMAGES
 * ARISING OUT OF THIS LICENSE OR THE USE OF THE WORK, EVEN IF LICENSOR HAS BEEN ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGES.
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
		{ "BroadcastMessage",					Player_BroadcastMessage				},
		{ "SendAreaTriggerMessage",				Player_SendAreaTriggerMessage		},
		{ "GetLevel",							Player_GetLevel						},
		{ "GetClass",							Player_GetClass						},
		{ "GetRace",							Player_GetRace						},
		{ "Teleport",							Player_Teleport						},
		{ "GetStanding",						Player_GetReputationValue			},
		{ "GetStandingRank",					Player_GetReputationRank			},
		{ "HasFinishedQuest",					Player_HasFinishedQuest				},
		{ "LearnSpell",							Player_LearnSpell					},
		{ "UnlearnSpell",						Player_RemoveSpell					},
		{ "AddItem",							Player_AddItem						},
		{ "RemoveItem",							Player_RemoveItem					},
		{ "MarkQuestObjectiveAsComplete",		Player_MarkQuestObjectiveAsComplete	},
		{ "SendSystemMessage",					Player_SendSystemMessage			},
		{ "SendNotification",					Player_SendNotification				},
		{ "Knockback",							Player_Knockback					},
		{ "JoinInstance",						Player_JoinInstance					},
		{ "SetLevel",							Player_SetLevel						},
		{ "LevelUp",							Player_LevelUp						},
		{ "Kick",								Player_Kick							},

		// Imported Unit Functions
		{ "GetHealthPct",						Unit_GetHealthPct					},
		{ "Emote",								Unit_Emote							},
		{ "SendChatMessage",					Unit_SendChatMessage				},
		{ "Despawn",							Unit_Despawn						},
		{ "CastSpell",							Unit_CastSpell						},
		{ "CastSpellOnTarget",					Unit_CastSpellOnTarget				},
		{ "SpawnMonster",						Unit_SpawnMonster					},
		{ "RegisterEvent",						GM_RegisterEvent					},
		{ "SpawnWithoutWorld",					Unit_SpawnWithoutWorld				},
		{ "StopMovement",						Unit_HaltMovement					},

		{ "GetUnit",							Unit_GetUnit						},
		{ "GetGameObject",						Unit_GetGameObject					},
		{ "GetPlayer",							Unit_GetPlayer						},
		{ "GetGuid",							Unit_GetGuid						},
		{ "PlaySoundToSet",						Unit_PlaySoundToSet					},
		{ "GetName",							Unit_GetName						},
		{ "GetClosestPlayer",					Unit_GetClosestPlayer				},
		{ "GetClosestUnit",						Unit_GetClosestUnit					},
		{ "GetClosestCreatureByEntry",			Unit_GetClosestUnitByEntry			},
		{ "GetPositionX",						Unit_GetPositionX					},
		{ "GetPositionY",						Unit_GetPositionY					},
		{ "GetPositionZ",						Unit_GetPositionZ					},
		{ "GetFacing",							Unit_GetFacing						},
		{ "AttackReaction",						Unit_AddToHated						},
	};

	m_machine->RegisterTypeLibrary(m_playerType, table, sizeof(table) / sizeof(table[0]));
}

void ScriptEngine::SetUnitFunctionTable()
{
	static gmFunctionEntry table[] = {
		{ "GetHealthPct",						Unit_GetHealthPct					},
		{ "Emote",								Unit_Emote							},
		{ "SendChatMessageAltEntry",			Unit_SendChatMessageAltEntry		},
		{ "SendChatMessage",					Unit_SendChatMessage				},
		{ "Despawn",							Unit_Despawn						},
		{ "CastSpell",							Unit_CastSpell						},
		{ "CastSpellOnTarget",					Unit_CastSpellOnTarget				},
		{ "TimedEmote",							Unit_TimedEmote						},
		{ "RegisterTimer",						Unit_RegisterTimer					},
		{ "DeregisterTimer",					Unit_DeregisterTimer				},
		{ "SpawnMonster",						Unit_SpawnMonster					},
		{ "SetStandState",						Unit_SetStandState					},
		{ "RegisterEvent",						GM_RegisterEvent					},
		{ "SpawnWithoutWorld",					Unit_SpawnWithoutWorld				},
		{ "AddToWorld",							Unit_AddToWorld						},
		{ "CreateCustomWaypointMap",			Unit_CreateCustomWaypointMap		},
		{ "CreateWaypoint",						Unit_CreateWaypoint					},
		{ "MoveToWaypoint",						Unit_MoveToWaypoint					},
		{ "Delete",								Unit_Delete							},
		{ "SetCombatCapable",					Unit_SetCombatCapable				},
		{ "StopMovement",						Unit_HaltMovement					},
		{ "SetMovementType",					Unit_SetMovementType				},
		{ "SetEscortTarget",					Unit_SetEscortTarget				},
		{ "GetEscortTarget",					Unit_GetEscortTarget				},
		{ "HasEscortTarget",					Unit_HasEscortTarget				},
		{ "SetNPCFlags",						Unit_SetNPCFlags					},
		{ "DestroyCustomWaypointMap",			Unit_DestroyCustomWaypointMap		},
		{ "ClearEscortTarget",					Unit_ClearEscortTarget				},
		{ "GetUnit",							Unit_GetUnit						},
		{ "GetGameObject",						Unit_GetGameObject					},
		{ "GetPlayer",							Unit_GetPlayer						},
		{ "GetGuid",							Unit_GetGuid						},
		{ "ChangeEntry",						Unit_ChangeEntry					},
		{ "ChangeModel",						Unit_ChangeModel					},
		{ "ChangeScale",						Unit_ChangeScale					},
		{ "ChangeFaction",						Unit_ChangeFaction					},
		{ "TextEmote",							Unit_TextEmote						},
		{ "PlaySoundToSet",						Unit_PlaySoundToSet					},
		{ "SendYellMessage",					Unit_SendYellMessage				},
		{ "GetName",							Unit_GetName						},
		{ "GetClosestPlayer",					Unit_GetClosestPlayer				},
		{ "GetClosestUnit",						Unit_GetClosestUnit					},
		{ "IsInCombat",							Unit_InCombat						},
		{ "GetClosestCreatureByEntry",			Unit_GetClosestUnitByEntry			},
		{ "GetPositionX",						Unit_GetPositionX					},
		{ "GetPositionY",						Unit_GetPositionY					},
		{ "GetPositionZ",						Unit_GetPositionZ					},
		{ "GetFacing",							Unit_GetFacing						},
		{ "ReturnToSpawn",						Unit_ReturnToSpawn					},
	};

	m_machine->RegisterTypeLibrary(m_unitType, table, sizeof(table) / sizeof(table[0]));
}

void ScriptEngine::SetQuestFunctionTable()
{
	static gmFunctionEntry table[] = {
		{ "GetID",								Quest_GetID						},
	};

	m_machine->RegisterTypeLibrary(m_questType, table, sizeof(table) / sizeof(table[0]));
}

void ScriptEngine::SetGameObjectFunctionTable()
{
	static gmFunctionEntry table[] = {
		{ "Despawn",							GameObject_Despawn					},
		{ "SetActive",							GameObject_SetActive				},
		{ "PlayCustomAnim",						GameObject_PlayCustomAnim			},
		{ "GetUnit",							Unit_GetUnit						},
		{ "GetGameObject",						Unit_GetGameObject					},
		{ "GetPlayer",							Unit_GetPlayer						},
		{ "GetGuid",							Unit_GetGuid						},
		{ "RegisterEvent",						GM_RegisterEvent					},
		{ "PlaySoundToSet",						Unit_PlaySoundToSet					},
		{ "SpawnMonster",						Unit_SpawnMonster					},
		{ "GetClosestPlayer",					Unit_GetClosestPlayer				},
		{ "GetClosestUnit",						Unit_GetClosestUnit					},
		{ "GetClosestCreatureByEntry",			Unit_GetClosestUnitByEntry			},
		{ "GetPositionX",						Unit_GetPositionX					},
		{ "GetPositionY",						Unit_GetPositionY					},
		{ "GetPositionZ",						Unit_GetPositionZ					},
		{ "GetFacing",							Unit_GetFacing						},
	};

	m_machine->RegisterTypeLibrary(m_gameObjectType, table, sizeof(table) / sizeof(table[0]));
}

void ScriptEngine::SetSpellFunctionTable()
{
	/*static gmFunctionEntry table[] = {
		{ "test", 0 },
	};*/

	//m_machine->RegisterTypeLibrary(m_playerType, table, sizeof(table) / sizeof(table[0]));
}

void ScriptEngine::SetAuraFunctionTable()
{
	/*static gmFunctionEntry table[] = {
		{ "test", 0 },
	};*/

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

	// Register Normal Functions
	m_machine->RegisterLibraryFunction("Rand", GM_RAND, 0, 0);
	m_machine->RegisterLibraryFunction("GetUnitBySqlId", GM_GetUnitBySqlId, 0, 0);
	m_machine->RegisterLibraryFunction("sprintf", GM_RAND, 0, 0);
	m_machine->RegisterLibraryFunction("GetDistance", GM_GetDistance, 0, 0);
	
}



