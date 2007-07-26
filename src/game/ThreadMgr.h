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
