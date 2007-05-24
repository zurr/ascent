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

#ifndef WOWSERVER_LOG_H
#define WOWSERVER_LOG_H

#include "Common.h"
#include "Singleton.h"

extern uint8 loglevel;
class WorldPacket;
class WorldSession;

#ifdef WIN32

#define TRED FOREGROUND_RED | FOREGROUND_INTENSITY
#define TGREEN FOREGROUND_GREEN | FOREGROUND_INTENSITY
#define TYELLOW FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY
#define TNORMAL FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE
#define TWHITE TNORMAL | FOREGROUND_INTENSITY
#define TBLUE FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY

#else

#define TRED 1
#define TGREEN 2
#define TYELLOW 3
#define TNORMAL 4
#define TWHITE 5
#define TBLUE 6

#endif

class Log : public Singleton< Log > {
public:
  void outString( const char * str, ... );
  void outError( const char * err, ... );
  void outBasic( const char * str, ... );
  void outDetail( const char * str, ... );
  void outDebug( const char * str, ... );
  void outMenu( const char * str, ... );
  void InitFile( const char * filename);
  FILE *outfile;
  bool screenenabled;
  bool fileenabled;
  void fLogText(const char *text);
  void SetLogging(bool enabled);
  void Init();
  void CloseFile();

  void outColor(uint32 colorcode, const char * str, ...);

#ifdef WIN32
  HANDLE stdout_handle, stderr_handle;
#endif
};

class Anticheat_Log : public Singleton<Anticheat_Log>
{
public:
	Anticheat_Log();
	~Anticheat_Log();

	void init(const char* filename);
	void write(const char* format, ...);
	void writefromsession(WorldSession* session, const char* format, ...);
	void close();

protected:
	FILE * out;
	Mutex MUTEX;
};

class GMCommand_Log : public Singleton<GMCommand_Log>
{
public:
	GMCommand_Log();
	~GMCommand_Log();

	void init(const char* filename);
	void write(const char* format, ...);
	void writefromsession(WorldSession* session, const char* format, ...);
	void close();

protected:
	FILE * out;
	Mutex MUTEX;
};

SERVER_DECL void script_errorlog(const char* str, ...);
SERVER_DECL void script_debuglog(const char* str, ...);

#define sLog Log::getSingleton()
#define sCheatLog Anticheat_Log::getSingleton()
#define sGMLog GMCommand_Log::getSingleton()

class WorldLog : public Singleton<WorldLog> 
{
public:
	WorldLog();
	~WorldLog();
	void LogPacket(uint32 len, uint16 opcode, const uint8* data, uint8 direction);
private:
	Mutex mutex;
	bool bEnabled;
	FILE *fout;
};

#define sWorldLog WorldLog::getSingleton()
#endif

