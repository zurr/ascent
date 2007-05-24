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
