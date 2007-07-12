/****************************************************************************
 *
 * Auction House System
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

initialiseSingleton( AuctionMgr );

void AuctionMgr::LoadAuctionHouses()
{
	sLog.outString("Loading Auction Houses...");

	QueryResult * res = CharacterDatabase.Query("SELECT MAX(auctionId) FROM auctions");
	if(res)
	{
		maxId = res->Fetch()[0].GetUInt32();
		delete res;
	}

	res = WorldDatabase.Query("SELECT DISTINCT AHid FROM auctionhouse ORDER BY AHid");
	AuctionHouse * ah;
	map<uint32, AuctionHouse*> tempmap;
	if(res)
	{
		do
		{
			ah = new AuctionHouse(res->Fetch()[0].GetUInt32());
			ah->LoadAuctions();
			auctionHouses.push_back(ah);
			tempmap.insert( make_pair( res->Fetch()[0].GetUInt32(), ah ) );
		}while(res->NextRow());
		delete res;
	}

	res = WorldDatabase.Query("SELECT auctioneer, AHid FROM auctionhouse");
	if(res)
	{
		do 
		{
			auctionHouseEntryMap.insert( make_pair( res->Fetch()[0].GetUInt32(), tempmap[res->Fetch()[1].GetUInt32()] ) );
		} while(res->NextRow());
		delete res;
	}
}

AuctionHouse * AuctionMgr::GetAuctionHouse(uint32 Entry)
{
	HM_NAMESPACE::hash_map<uint32, AuctionHouse*>::iterator itr = auctionHouseEntryMap.find(Entry);
	if(itr == auctionHouseEntryMap.end()) return NULL;
	return itr->second;
}

void AuctionMgr::Update()
{
	if((++loopcount % 100))
		return;
		
	vector<AuctionHouse*>::iterator itr = auctionHouses.begin();
	for(; itr != auctionHouses.end(); ++itr)
	{
		(*itr)->UpdateDeletionQueue();

		// Actual auction loop is on a seperate timer.
		if(!(loopcount % 1200))
			(*itr)->UpdateAuctions();
	}
}
