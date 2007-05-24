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

/* *
   @file CrashHandler.h
   Handles crashes/exceptions on a win32 based platform, writes a dump file,
   for later bug fixing.
*/

#ifndef _CRASH_HANDLER_H
#define _CRASH_HANDLER_H

bool HookCrashReporter(bool logon);

#ifdef WIN32

#include <Windows.h>
#include <DbgHelp.h>
#include "StackWalker.h"

class CStackWalker : public StackWalker
{
public:
	void OnOutput(LPCSTR szText);
	void OnSymInit(LPCSTR szSearchPath, DWORD symOptions, LPCSTR szUserName);
	void OnLoadModule(LPCSTR img, LPCSTR mod, DWORD64 baseAddr, DWORD size, DWORD result, LPCSTR symType, LPCSTR pdbName, ULONGLONG fileVersion);
	void OnCallstackEntry(CallstackEntryType eType, CallstackEntry &entry);
	void OnDbgHelpErr(LPCSTR szFuncName, DWORD gle, DWORD64 addr);
};

void StartCrashHandler();
void OnCrash(bool Terminate);

typedef struct _EXCEPTION_POINTERS EXCEPTION_POINTERS, *PEXCEPTION_POINTERS;
int __cdecl HandleCrash(PEXCEPTION_POINTERS pExceptPtrs);

#define THREAD_TRY_EXECUTION __try 
#define THREAD_HANDLE_CRASH  __except(HandleCrash(GetExceptionInformation())) {}

#define THREAD_TRY_EXECUTION2 __try {
#define THREAD_HANDLE_CRASH2  } __except(HandleCrash(GetExceptionInformation())) {}

#else

// We dont wanna confuse nix ;p
#define THREAD_TRY_EXECUTION 
#define THREAD_HANDLE_CRASH 

#define THREAD_TRY_EXECUTION2 ;
#define THREAD_HANDLE_CRASH2 ;

#endif

#endif

