/****************************************************************************
 *
 * SpellHandler Plugin
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
 */

#include "StdAfx.h"
#include "Setup.h"

extern "C" SCRIPT_DECL uint32 _exp_get_version()
{
    return MAKE_SCRIPT_VERSION(SCRIPTLIB_VERSION_MAJOR, SCRIPTLIB_VERSION_MINOR);
}

extern "C" SCRIPT_DECL void _exp_script_register(ScriptMgr* mgr)
{
    SetupShamanSpells(mgr);
    SetupWarlockSpells(mgr);
    SetupWarriorSpells(mgr);
    SetupHunterSpells(mgr);
    SetupItemSpells_1(mgr);
    SetupMageSpells(mgr);
    SetupPaladinSpells(mgr);
    SetupRogueSpells(mgr);
}

#ifdef WIN32

BOOL APIENTRY DllMain( HANDLE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved )
{
    return TRUE;
}

#endif
