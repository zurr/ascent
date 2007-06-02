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

#ifndef SCRIPT_FUNCTIONS_H
#define SCRIPT_FUNCTIONS_H

#include "ScriptEngine.h"

/* Helper Functions */
template<class T>
T * GetThisPointer(gmThread * a_thread)
{
	return (T*)a_thread->ThisUser();
}

/* Script Engine Functions */
int GM_CDECL ScriptEngine_RegisterUnitEvent(gmThread * a_thread);
int GM_CDECL ScriptEngine_RegisterGameObjectEvent(gmThread * a_thread);
int GM_CDECL ScriptEngine_RegisterSpellEvent(gmThread * a_thread);
int GM_CDECL ScriptEngine_RegisterAreaTriggerEvent(gmThread * a_thread);
int GM_CDECL ScriptEngine_RegisterPlayerEvent(gmThread * a_thread);

/* Player Functions */
int GM_CDECL Player_BroadcastMessage(gmThread * a_thread);
int GM_CDECL Player_SendAreaTriggerMessage(gmThread * a_thread);
int GM_CDECL Player_GetClass(gmThread * a_thread);
int GM_CDECL Player_GetRace(gmThread * a_thread);
int GM_CDECL Player_Teleport(gmThread * a_thread);

/* AreaTrigger Functions */
int GM_CDECL AreaTrigger_GetEntry(gmThread * a_thread);

#endif
