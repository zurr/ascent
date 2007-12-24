/*
 * Ascent MMORPG Server
 * Copyright (C) 2005-2007 Ascent Team <http://www.ascentemu.com/>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "StdAfx.h"

initialiseSingleton( SocialMgr );

SocialMgr::SocialMgr()
{
	m_isInFriendList.clear();
	m_hasInFriendList.clear();
	m_isInIgnoreList.clear();
	m_hasInIgnoreList.clear();
	m_needsBlockNotice.clear();
}

SocialMgr::~SocialMgr()
{
	SocialMap::iterator iter;
	for(iter = m_isInFriendList.begin(); iter != m_isInFriendList.end(); ++iter) {
		delete iter->second;
	}
	m_isInFriendList.clear();
	for(iter = m_hasInFriendList.begin(); iter != m_hasInFriendList.end(); ++iter) {
		delete iter->second;
	}
	m_hasInFriendList.clear();
	for(iter = m_isInIgnoreList.begin(); iter != m_isInIgnoreList.end(); ++iter) {
		delete iter->second;
	}
	m_isInIgnoreList.clear();
	for(iter = m_hasInIgnoreList.begin(); iter != m_hasInIgnoreList.end(); ++iter) {
		delete iter->second;
	}
	m_hasInIgnoreList.clear();
	for(iter = m_needsBlockNotice.begin(); iter != m_needsBlockNotice.end(); ++iter) {
		delete iter->second;
	}
	m_needsBlockNotice.clear();
}

void SocialMgr::SendFriendList(Player* plr)
{
	if(!plr)
		return;

	WorldPacket data( SMSG_FRIEND_LIST , 512 );
	uint32 plrguid = plr->GetGUIDLow();

	SocialMap::iterator fItr = m_hasInFriendList.find(plrguid);
	if( fItr == m_hasInFriendList.end() )
	{
		data << (uint8)0;
		if(plr->GetSession())
			plr->GetSession()->SendPacket( &data );
		return;
	}

	SocialSet* fList = fItr->second;
	SocialSet::iterator itr;
	Player* pFriend = NULL;
	bool isGm = (plr->GetSession()->GetPermissionCount() > 0);

	data << (uint8)fList->size();
	for(itr = fList->begin(); itr != fList->end(); itr++) 
	{
		data << uint64(*itr);
		pFriend = objmgr.GetPlayer( *itr );
		if(pFriend && (!pFriend->m_isGmInvisible || isGm))
			data << uint8(1) << (uint32)pFriend->GetZoneId() << (uint32)pFriend->getLevel() << (uint32)pFriend->getClass();
		else
			data << uint8(0);
	}

	plr->GetSession()->SendPacket( &data );
}

void SocialMgr::SendIgnoreList(Player* plr)
{
	if(!plr)
		return;

	WorldPacket data( SMSG_IGNORE_LIST, 512 );
	uint32 plrguid = plr->GetGUIDLow();

	SocialMap::iterator iItr;
	iItr = m_hasInIgnoreList.find(plrguid);
	if( iItr == m_hasInIgnoreList.end() )
	{
		data << (uint8)0;
		if(plr->GetSession())
			plr->GetSession()->SendPacket( &data );
		return;
	}

	SocialSet* iList = iItr->second;
	SocialSet::iterator itr;

	data << (uint8)iList->size();
	for(itr = iList->begin(); itr != iList->end(); itr++)
	{
		data << uint64(*itr);
	}

	plr->GetSession()->SendPacket( &data );
}

void SocialMgr::SendUpdateToFriends(Player* plr)
{
	if(!plr)
		return;

	SocialMap::iterator Itr = m_isInFriendList.find(plr->GetGUIDLow());
	if(Itr == m_isInFriendList.end())
		return;

	SocialSet* iList = Itr->second;
	SocialSet::iterator itr;
	Player* cPlayer = NULL;

	for(itr = iList->begin(); itr != iList->end(); itr++)
	{
		cPlayer = objmgr.GetPlayer(*itr);
		if(cPlayer)
			SendFriendList(cPlayer);
	}
}

void SocialMgr::AddFriend(Player* plr, std::string friendName)
{
	if(!plr)
		return;

	WorldPacket data( SMSG_FRIEND_STATUS, 22 );

	PlayerInfo* playerInfo = objmgr.GetPlayerInfo(plr->GetGUIDLow());
	PlayerInfo* friendInfo = objmgr.GetPlayerInfoByName(friendName);

	if(playerInfo == NULL)
	{ // We shouldn't get here
		data << (uint8)FRIEND_DB_ERROR;
		plr->GetSession()->SendPacket( &data );
		return;
	}
	if( friendInfo == NULL)
	{
		sLog.outDetail("WORLD: Guid of %s not found ", friendName.c_str() );
		data << (uint8)FRIEND_NOT_FOUND;
		plr->GetSession()->SendPacket( &data );
		return;
	}
	if( friendInfo->team != playerInfo->team )
	{
		sLog.outDebug("SocialMgr: %s tried to add an ennemy to his friendlist", plr->GetName());
		data << (uint8)FRIEND_ENEMY << (uint64)friendInfo->guid;
		plr->GetSession()->SendPacket( &data );
		return;
	}

	uint32 pGuid = playerInfo->guid;
	uint32 fGuid = friendInfo->guid;
	Player *pFriend = objmgr.GetPlayer(fGuid);

	if ( pGuid == fGuid )
	{
		sLog.outDebug("SocialMgr: %s tried to add himself to his friendlist", plr->GetName());
		data << (uint8)FRIEND_SELF << (uint64)fGuid;
		plr->GetSession()->SendPacket(&data);
		return;
	}
	if( HasFriend(pGuid, fGuid) )
	{
		sLog.outDebug("SocialMgr: %s tried to add someone who is already in his friendlist", plr->GetName());
		data << (uint8)FRIEND_ALREADY << (uint64)fGuid;
		plr->GetSession()->SendPacket(&data);
		return;
	}

	sLog.outDebug("SocialMgr: %s added %s to his friendlist", playerInfo->name, friendInfo->name);

	if ( pFriend )
	{
		data << (uint8)FRIEND_ADDED_ONLINE << (uint64)fGuid << (uint8)1;
		data << (uint32)pFriend->GetZoneId() << (uint32)pFriend->getLevel() << (uint32)pFriend->getClass();
	}
	else
		data << (uint8)FRIEND_ADDED_OFFLINE << (uint64)friendInfo->guid;

	if( m_isInFriendList.find(fGuid) == m_isInFriendList.end() )
		m_isInFriendList[fGuid] = new SocialSet;

	if( m_hasInFriendList.find(pGuid) == m_hasInFriendList.end() )
		m_hasInFriendList[pGuid] = new SocialSet;

	m_hasInFriendList[pGuid]->insert(fGuid);
	m_isInFriendList[fGuid]->insert(pGuid);

	plr->GetSession()->SendPacket( &data );
	CharacterDatabase.Execute("INSERT INTO social(guid,socialguid,flags) VALUES (%u,%u,'FRIEND')",pGuid,fGuid);
}

void SocialMgr::AddIgnore(Player* plr, std::string ignoreName)
{
	if(!plr)
		return;

	WorldPacket data( SMSG_FRIEND_STATUS, 10 );

	PlayerInfo* playerInfo = objmgr.GetPlayerInfo(plr->GetGUIDLow());
	PlayerInfo* ignoreInfo = objmgr.GetPlayerInfoByName(ignoreName);
	if (!ignoreInfo)
	{
		sLog.outDetail( "WORLD: Guid of %s not found ", ignoreName.c_str() );
		data << (uint8)FRIEND_IGNORE_NOT_FOUND;
		plr->GetSession()->SendPacket( &data );
		return;
	}

	if(playerInfo == NULL)
	{
		data << (uint8)FRIEND_DB_ERROR;
		plr->GetSession()->SendPacket( &data );
		return;
	}

	uint32 pGuid = playerInfo->guid;
	uint32 iGuid = ignoreInfo->guid;

	if( pGuid == iGuid )
	{
		sLog.outDebug("SocialMgr: %s tried to add himself to his friendlist", plr->GetName());
		data << (uint8)FRIEND_IGNORE_SELF << (uint64)iGuid;
		plr->GetSession()->SendPacket(&data);
		return;
	}
	if( HasIgnore(playerInfo->guid, ignoreInfo->guid) )
	{
		sLog.outDebug("SocialMgr: %s tried to add someone who is already in his ignorelist", plr->GetName());
		data << (uint8)FRIEND_IGNORE_ALREADY << (uint64)iGuid;
		plr->GetSession()->SendPacket(&data);
		return;
	}

	sLog.outDebug("SocialMgr: %s added %s to his ignorelist", plr->GetName(), ignoreInfo->name);

	data << (uint8)FRIEND_IGNORE_ADDED << (uint64)iGuid;

	if( m_isInIgnoreList.find(iGuid) == m_isInIgnoreList.end() )
		m_isInIgnoreList[iGuid] = new SocialSet;

	if( m_hasInIgnoreList.find(pGuid) == m_hasInIgnoreList.end() )
		m_hasInIgnoreList[pGuid] = new SocialSet;

	if( m_needsBlockNotice.find(iGuid) == m_needsBlockNotice.end() )
		m_needsBlockNotice[iGuid] = new SocialSet;

	m_isInIgnoreList[iGuid]->insert(pGuid);
	m_hasInIgnoreList[pGuid]->insert(iGuid);
	m_needsBlockNotice[iGuid]->insert(pGuid);

	plr->GetSession()->SendPacket( &data );
	CharacterDatabase.Execute("INSERT INTO social(guid,socialguid,flags,noticed) VALUES (%u,%u,'IGNORE',0)",pGuid,iGuid);
}

void SocialMgr::DelFriend(Player* plr, uint32 friendguid)
{
	if(!plr)
		return;

	WorldPacket data(9);
	uint32 plrguid = plr->GetGUIDLow();

	sLog.outDebug("SocialMgr: %s is deleting friendguid %d from his friendlist", plr->GetName(), friendguid);

	SocialMap::iterator Itr;
	Itr = m_hasInFriendList.find(plrguid);
	if( Itr != m_hasInFriendList.end() )
		Itr->second->erase( friendguid );

	Itr = m_isInFriendList.find(friendguid);
	if( Itr != m_isInFriendList.end() )
		Itr->second->erase( friendguid );

	data.Initialize( SMSG_FRIEND_STATUS );
	data << (uint8)FRIEND_REMOVED << (uint64)friendguid;

	plr->GetSession()->SendPacket( &data );
	CharacterDatabase.Execute("DELETE FROM social WHERE guid=%u AND socialguid=%u AND flags='FRIEND'",plrguid,friendguid);
}

void SocialMgr::DelIgnore(Player* plr, uint32 ignoreguid)
{
	if(!plr)
		return;

	WorldPacket data(9);
	uint32 plrguid = plr->GetGUIDLow();

	sLog.outDebug("SocialMgr: %s is deleting guid %d from his ignorelist", plr->GetName(), ignoreguid);
	
	SocialMap::iterator iItr;
	iItr = m_hasInIgnoreList.find(plrguid);
	if( iItr != m_hasInIgnoreList.end() )
		iItr->second->erase(ignoreguid);
	
	iItr = m_isInIgnoreList.find(ignoreguid);
	if( iItr != m_isInIgnoreList.end() )
		iItr->second->erase(plrguid);

	data.Initialize( SMSG_FRIEND_STATUS );
	data << (uint8)FRIEND_IGNORE_REMOVED << (uint64)ignoreguid;

	plr->GetSession()->SendPacket( &data );
	CharacterDatabase.Execute("DELETE FROM social WHERE guid=%u AND socialguid=%u AND flags='IGNORE'",plrguid,ignoreguid);
}

bool SocialMgr::IsFriend(uint32 plrguid, uint32 target)
{
	SocialMap::iterator Itr = m_isInFriendList.find( target );
	if( Itr == m_isInFriendList.end() )
		return false;

	SocialSet::iterator sItr = Itr->second->find( plrguid );
	if( sItr != Itr->second->end() )
		return true;
	return false;
}

bool SocialMgr::HasFriend(uint32 plrguid, uint32 mfriend)
{
	SocialMap::iterator Itr = m_hasInFriendList.find( plrguid );
	if( Itr == m_hasInFriendList.end() )
		return false;

	SocialSet::iterator sItr = Itr->second->find( mfriend );
	if( sItr != Itr->second->end() )
		return true;
	return false;
}

bool SocialMgr::IsIgnore(uint32 plrguid, uint32 target)
{
	SocialMap::iterator Itr = m_isInIgnoreList.find(target);
	if( Itr == m_isInIgnoreList.end() )
		return false;

	SocialSet::iterator sItr = Itr->second->find( plrguid );
	if( sItr != Itr->second->end() )
		return true;
	return false;
}

bool SocialMgr::HasIgnore(uint32 plrguid, uint32 mignore)
{
	SocialMap::iterator Itr = m_hasInIgnoreList.find(plrguid);
	if( Itr == m_hasInIgnoreList.end() )
		return false;

	SocialSet::iterator sItr = Itr->second->find( mignore );
	if( sItr != Itr->second->end() )
		return true;
	return false;
}

bool SocialMgr::HasIgnore(Player* plr, Player* mignore)
{
	SocialMap::iterator Itr;
	uint32 plrguid = plr->GetGUIDLow();
	uint32 ignguid = mignore->GetGUIDLow();

	if( HasIgnore(plrguid, ignguid) )
	{
		SocialSet::iterator iter;
		Itr = m_needsBlockNotice.find(ignguid);
		if( Itr != m_needsBlockNotice.end() )
		{
			for(iter = Itr->second->begin(); iter != Itr->second->end(); iter++)
			{
				if( (*iter) == plrguid )
				{
					std::stringstream ss;
					sChatHandler.SystemMessage(mignore->GetSession(), "%s is ignoring you, it isn't possible for you to whisper him/her.", plr->GetName());
					Itr->second->erase( iter );
					ss << "UPDATE social SET noticed=1 WHERE guid="<< plrguid <<" AND socialguid="<< ignguid <<" AND flags='IGNORE'";
					CharacterDatabase.Execute( ss.str().c_str() );
					return true;
				}
			}
		}
		return true;
	}
	return false;
}

ASCENT_INLINE void SocialMgr::SendOnlinePkt(Player* plr, SocialStr* pNfo)
{
	if(!plr)
	   return;

	WorldPacket data(SMSG_FRIEND_STATUS, 22);
	data << (uint8)FRIEND_ONLINE << (uint64)pNfo->pGuid << (uint8)1;
	data << pNfo->Area << (uint32)pNfo->Level << (uint32)pNfo->Class;

	plr->GetSession()->SendPacket( &data );
}

ASCENT_INLINE void SocialMgr::SendOfflinePkt(Player* plr, uint32 fGuid)
{
	if(!plr || !plr->GetSession())
		return;

	WorldPacket data(SMSG_FRIEND_STATUS, 10);
	data << (uint8)FRIEND_OFFLINE << (uint64)fGuid << (uint8)0;

	plr->GetSession()->SendPacket( &data );
}

void SocialMgr::LoggedIn(Player* plr)
{
	if(!plr)
		return;

	SocialStr* pNfo = new SocialStr;
	pNfo->pGuid = plr->GetGUIDLow();
	SocialMap::iterator Itr = m_isInFriendList.find(pNfo->pGuid);
	if( Itr == m_isInFriendList.end() )
	{
		delete pNfo;
		return;
	}

	pNfo->Level = plr->getLevel();
	pNfo->Class = plr->getClass();
	pNfo->Area = plr->GetZoneId();

	SocialSet::iterator sItr = Itr->second->begin();
	Player* fPlr = NULL;
	for( ; sItr != Itr->second->end(); sItr++ )
	{
		fPlr = objmgr.GetPlayer(*sItr);
		if( fPlr )
			SendOnlinePkt(fPlr, pNfo);
	}
	// naughty!
	delete pNfo;
}

void SocialMgr::LoggedOut(Player* plr)
{
	if(!plr)
		return;

	uint32 plrguid = plr->GetGUIDLow();
	SocialMap::iterator Itr = m_isInFriendList.find(plrguid);
	if( Itr == m_isInFriendList.end() )
		return;

	SocialSet::iterator sItr = Itr->second->begin();
	Player* fPlr = NULL;
	for( ; sItr != Itr->second->end(); sItr++ )
	{
		fPlr = objmgr.GetPlayer(*sItr);
		if( fPlr )
			SendOfflinePkt(fPlr, plrguid);
	}
}

void SocialMgr::RemovePlayer(uint32 plr)
{
	if(!plr)
		return;

	SocialMap::iterator Itr;
	SocialMap::iterator mItr;
	SocialSet::iterator sItr;

	Itr = m_isInFriendList.find( plr );
	if( Itr != m_isInFriendList.end() )
	{
		for(sItr = Itr->second->begin(); sItr != Itr->second->end(); sItr++)
		{
			mItr = m_hasInFriendList.find( *sItr );
			if( mItr != m_hasInFriendList.end() )
				mItr->second->erase( plr );
		}
		delete Itr->second;
		m_isInFriendList.erase(Itr);
	}

	Itr = m_isInIgnoreList.find( plr );
	if( Itr != m_isInIgnoreList.end() )
	{
		for(sItr = Itr->second->begin(); sItr != Itr->second->end(); sItr++)
		{
			mItr = m_hasInIgnoreList.find( *sItr );
			if( mItr != m_hasInIgnoreList.end() )
				mItr->second->erase( plr );
		}
		delete Itr->second;
		m_isInIgnoreList.erase(Itr);
	}

	Itr = m_needsBlockNotice.find(plr);
	if( Itr != m_needsBlockNotice.end() )
	{
		delete Itr->second;
		m_needsBlockNotice.erase(Itr);
	}

	CharacterDatabase.Execute("DELETE FROM social WHERE guid=%d OR socialguid=%d", uint32(plr), uint32(plr));
}

void SocialMgr::LoadFromDB()
{
	SocialMap::iterator Itr;
	uint32 plrguid = 0;
	uint32 friendguid = 0;
	uint32 ignoreguid = 0;
	bool noticed = false;

	QueryResult *result = CharacterDatabase.Query("SELECT guid,socialguid FROM social WHERE flags='FRIEND'");

	if(result)
	{
		do 
		{
			Field *field = result->Fetch();
			plrguid = field[0].GetUInt32();
			friendguid = field[1].GetUInt32();

			if( m_isInFriendList.find(friendguid) == m_isInFriendList.end() )
				m_isInFriendList[friendguid] = new SocialSet;

			if( m_hasInFriendList.find(plrguid) == m_hasInFriendList.end() )
				m_hasInFriendList[plrguid] = new SocialSet;

			m_isInFriendList[friendguid]->insert(plrguid);
			m_hasInFriendList[plrguid]->insert(friendguid);
		}
		while( result->NextRow() );
		delete result;
	}

	result = CharacterDatabase.Query("SELECT guid,socialguid,noticed FROM social WHERE flags='IGNORE'");
	if(result)
	{
		do 
		{
			Field *field = result->Fetch();
			plrguid = field[0].GetUInt32();
			ignoreguid = field[1].GetUInt32();
			noticed = field[2].GetBool();

			if( m_isInIgnoreList.find(ignoreguid) == m_isInIgnoreList.end() )
				m_isInIgnoreList[ignoreguid] = new SocialSet;

			if( m_hasInIgnoreList.find(plrguid) == m_hasInIgnoreList.end() )
				m_hasInIgnoreList[plrguid] = new SocialSet;

			if( !noticed )
			{
				if( m_needsBlockNotice.find(ignoreguid) == m_needsBlockNotice.end() )
					m_needsBlockNotice[ignoreguid] = new SocialSet;
				m_needsBlockNotice[ignoreguid]->insert(plrguid);
			}

			m_isInIgnoreList[ignoreguid]->insert(plrguid);
			m_hasInIgnoreList[plrguid]->insert(ignoreguid);
		}
		while( result->NextRow() );
		delete result;
	}
}
