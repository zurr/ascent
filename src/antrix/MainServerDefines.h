#ifndef _MAINSERVER_DEFINES_H
#define _MAINSERVER_DEFINES_H

#include "../shared/TextLogger.h"

class Database;

SERVER_DECL extern Database* Database_Main;
extern TextLogger * Crash_Log;

#define sDatabase (*Database_Main)

#endif
