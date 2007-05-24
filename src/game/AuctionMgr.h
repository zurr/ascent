/****************************************************************************
 *
 * Auction System
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

#ifndef __AUCTIONMGR_H
#define __AUCTIONMGR_H

class AuctionHouse;
class AuctionMgr : public Singleton <AuctionMgr>
{
public:
	AuctionMgr()
	{
		loopcount = 0;
		maxId = 1;
	}

	~AuctionMgr()
	{
		vector<AuctionHouse*>::iterator itr = auctionHouses.begin();
		for(; itr != auctionHouses.end(); ++itr)
			delete (*itr);
	}

	void LoadAuctionHouses();
	void Update();

	AuctionHouse * GetAuctionHouse(uint32 Entry);

	inline uint32 GenerateAuctionId()
	{ 
		lock.Acquire();
		uint32 id=++maxId;
		lock.Release();
		return id;
	}

private:
	HM_NAMESPACE::hash_map<uint32, AuctionHouse*> auctionHouseEntryMap;
	vector<AuctionHouse*> auctionHouses;
	volatile uint32 maxId;
	Mutex lock;
	uint32 loopcount;
};

#define sAuctionMgr AuctionMgr::getSingleton()

#endif
