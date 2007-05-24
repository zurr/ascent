/****************************************************************************
 *
 * LFG System
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

enum LFGTypes 
{
	LFG_NONE = 0,
	LFG_INSTANCE = 1,
	LFG_RAID = 2,
	LFG_QUEST = 3,
	LFG_ZONE = 4,
	LFG_HEROIC_DUNGEON = 5, // from client
};

#define MAX_DUNGEONS 166 // check max entrys +1 on lfgdungeons.dbc

class LfgMgr : public Singleton < LfgMgr >
{
public:	
	
	LfgMgr();
	~LfgMgr();
	
	void SetPlayerInLFGqueue(Player *pl,uint32 LfgDungeonId);
	void RemoveFromLfgQueue(Player *pl,uint32 LfgDungeonId);
	uint32 GetLfgQueueSize(uint32 LfgDungeonId);
	std::set<Player*>::iterator GetLfgQueueBegin(uint32 LfgDungeonId);
	std::set<Player*>::iterator GetLfgQueueEnd(uint32 LfgDungeonId);
	
protected:
	
	std::set<Player*> m_LFGqueueMembers[MAX_DUNGEONS];
	
	
};

#define sLfgMgr LfgMgr::getSingleton()
