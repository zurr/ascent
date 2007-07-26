/****************************************************************************
 *
 * LFG System
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
