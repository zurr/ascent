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

// Class CThread - Base class for all threads in the
// server, and allows for easy management by ThreadMgr.

#ifndef _MAPUPDATER_H
#define _MAPUPDATER_H

class MapMgr;
class Object;
class Player;
class WorldSession;
class Creature;
class GameObject;

#define MAPMGR_UPDATEOBJECT_LOOP_DELAY 100
#define MAPMGR_SESSION_UPDATE_DELAY	50

#define MAPMGR_UPDATE_DELAY			100

enum CThreadState
{
	THREADSTATE_TERMINATE = 0,
	THREADSTATE_PAUSED	= 1,
	THREADSTATE_SLEEPING  = 2,
	THREADSTATE_BUSY	  = 3,
	THREADSTATE_AWAITING  = 4,
};

enum CThreadType
{
	THREADTYPE_UNASSIGNED,	  
	THREADTYPE_OBJECTUPDATER,
	THREADTYPE_MAPMGR,
	THREADTYPE_WORLDRUNNABLE,
	THREADTYPE_MAILDELIVERY,
	THREADTYPE_CONSOLEINTERFACE,
	THREADTYPE_IRCBOT,
	THREADTYPE_AUCTIONHOUSE,
	THREADTYPE_NETWORK,
	THREADTYPE_SESSIONUPDATER,
	THREADTYPE_SOCKETUPDATER,
	THREADTYPE_DATABASE,
	THREADTYPE_IOCPLISTENER,
	THREADTYPE_IOCPWORKER,
};

struct NameTableEntry;
extern NameTableEntry g_threadStates[];
extern NameTableEntry g_threadTypes[];

class SERVER_DECL CThread : public ThreadBase
{
	friend class ThreadMgr;
public:
	CThread();
	~CThread();

	inline void SetThreadState(CThreadState thread_state) { ThreadState = thread_state; }
	inline CThreadState GetThreadState() { return ThreadState; }
	inline CThreadType GetThreadType() { return ThreadType; }
	int GetThreadId() { return ThreadId; }
	time_t GetStartTime() { return start_time; }
	virtual void run();
	void manual_start();

protected:
	CThreadState ThreadState;
	CThreadType  ThreadType;
	time_t start_time;
	int ThreadId;
};

#endif
