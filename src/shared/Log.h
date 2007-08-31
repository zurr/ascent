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
 */

#ifndef WOWSERVER_LOG_H
#define WOWSERVER_LOG_H

#include "Common.h"
#include "Singleton.h"

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
std::string FormatOutputString(const char * Prefix, const char * Description, bool useTimeStamp);

class SERVER_DECL oLog : public Singleton< oLog > {
public:
  void outString( const char * str, ... );
  void outError( const char * err, ... );
  void outBasic( const char * str, ... );
  void outDetail( const char * str, ... );
  void outDebug( const char * str, ... );
  void outMenu( const char * str, ... );

  void fLogText(const char *text);
  void SetLogging(bool enabled);
  
  void Init(int32 fileLogLevel, int32 screenLogLevel);
  void SetFileLoggingLevel(int32 level);
  void SetScreenLoggingLevel(int32 level);

  void outColor(uint32 colorcode, const char * str, ...);
  
#ifdef WIN32
  HANDLE stdout_handle, stderr_handle;
#endif
  int32 m_fileLogLevel;
  int32 m_screenLogLevel;
};

class SessionLogWriter
{
	FILE * m_file;
	char * m_filename;
public:
	SessionLogWriter(const char * filename, bool open);
	~SessionLogWriter();

	void write(const char* format, ...);
	void writefromsession(WorldSession* session, const char* format, ...);
	inline bool IsOpen() { return (m_file != NULL); }
	void Open();
	void Close();
};

extern SessionLogWriter * Anticheat_Log;
extern SessionLogWriter * GMCommand_Log;

#define sLog oLog::getSingleton()
#define sCheatLog (*Anticheat_Log)
#define sGMLog (*GMCommand_Log)

class WorldLog : public Singleton<WorldLog>
{
public:
	WorldLog();
	~WorldLog();

	void LogPacket(uint32 len, uint16 opcode, const uint8* data, uint8 direction);
	void Enable();
	void Disable();
private:
	FILE * m_file;
	Mutex mutex;
	bool bEnabled;
};

#define sWorldLog WorldLog::getSingleton()

#endif

