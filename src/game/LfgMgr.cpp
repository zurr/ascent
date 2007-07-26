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

#include "StdAfx.h"

initialiseSingleton( LfgMgr );

LfgMgr::LfgMgr()
{
	
	
}

LfgMgr::~LfgMgr()
{
	
	
}

void LfgMgr::RemoveFromLfgQueue(Player *pl,uint32 LfgDungeonId)
{	
	if(!LfgDungeonId)
		return;
	
	std::set<Player*>::iterator itr;
	for(itr = GetLfgQueueBegin(LfgDungeonId); itr != GetLfgQueueEnd(LfgDungeonId) ; ++itr)
	{
		if((*itr) == pl)
		{
			m_LFGqueueMembers[LfgDungeonId].erase(itr);
			return;
		}
	}
}	

void LfgMgr::SetPlayerInLFGqueue(Player *pl,uint32 LfgDungeonId)
{
	m_LFGqueueMembers[LfgDungeonId].insert(pl);
}

uint32 LfgMgr::GetLfgQueueSize(uint32 LfgDungeonId)
{
	return m_LFGqueueMembers[LfgDungeonId].size();	
}

std::set<Player*>::iterator LfgMgr::GetLfgQueueBegin(uint32 LfgDungeonId)
{
	return m_LFGqueueMembers[LfgDungeonId].begin();
}

std::set<Player*>::iterator LfgMgr::GetLfgQueueEnd(uint32 LfgDungeonId)
{
	return m_LFGqueueMembers[LfgDungeonId].end();
}
