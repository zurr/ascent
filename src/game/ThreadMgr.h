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

// Class ThreadMgr - Pool that stores all threads
// and keeps track of them, watches for overloading,
// handles shutdown.

#ifndef _MAPTHREADMGR_H
#define _MAPTHREADMGR_H

class CThread;
class MapMgr;

typedef std::set<CThread*> ThreadSet;

class SessionUpdaterThread;
class SocketUpdaterThread;

class ThreadMgr : public Singleton < ThreadMgr >
{
public:
	ThreadMgr();
	~ThreadMgr();

	void Initialize();
	void Shutdown();
	
	void OnMapMgrCreated(MapMgr *mapMgr);

	void AddThread(CThread *thread);
	void RemoveThread(CThread *thread);
	int GenerateThreadId();
	void GetThreadById(int id); // slow!
	CThread* GetThreadByType(CThreadType type);

	inline ThreadSet::iterator Begin() { return mThreads.begin(); }
	inline ThreadSet::iterator End() { return mThreads.end(); }

	inline void SetBusy(bool val) { m_SetBusy = val; }

	std::string ShowStatus();
	void AppendThreadStatus(CThread* thread, std::stringstream &stream);

	void PauseAllThreads();
	void ResumeAllThreads();

	inline uint32 GetThreadCount() { return mThreads.size(); }

private:
	ThreadSet mThreads;
	int ThreadIdCounter;
	bool m_SetBusy;
	Mutex _mutex;
};

#define sThreadMgr ThreadMgr::getSingleton( )
#endif
