/****************************************************************************
 *
 * Auction House System
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

#ifndef AUCTIONHOUSE_H
#define AUCTIONHOUSE_H

#include "../shared/Threading/RWLock.h"

enum AuctionRemoveType
{
	AUCTION_REMOVE_EXPIRED,
	AUCTION_REMOVE_WON,
	AUCTION_REMOVE_CANCELLED,
};
enum AUCTIONRESULT
{
	AUCTION_CREATE,// = 1
	AUCTION_CANCEL,// = 2
	AUCTION_BID,
	AUCTION_BUYOUT,

};
enum AuctionMailResult
{
	AUCTION_OUTBID,
	AUCTION_WON,
	AUCTION_SOLD,
	AUCTION_EXPIRED,
	AUCTION_EXPIRED2,
	AUCTION_CANCELLED,
};

struct Auction
{
	uint32 Id;
	
	uint32 Owner;
	uint32 HighestBidder;
	uint32 HighestBid;
	
	uint32 BuyoutPrice;
	uint32 DepositAmount;
	
	uint32 ExpiryTime;
	Item * pItem;

	void DeleteFromDB();
	void SaveToDB(uint32 AuctionHouseId);
	void UpdateInDB();
	void AddToPacket(WorldPacket & data);
	bool Deleted;
	uint32 DeletedReason;
};

class AuctionHouse
{
public:
	AuctionHouse(uint32 ID);
	~AuctionHouse();

	inline uint32 GetID() { return dbc->id; }
	void LoadAuctions();

	void UpdateAuctions();
	void UpdateDeletionQueue();

	void RemoveAuction(Auction * auct);
	void AddAuction(Auction * auct);
	Auction * GetAuction(uint32 Id);
	void QueueDeletion(Auction * auct, uint32 Reason);

	void SendOwnerListPacket(Player * plr, WorldPacket * packet);
	void SendBidListPacket(Player * plr, WorldPacket * packet);
	void SendAuctionNotificationPacket(Player * plr, Auction * auct);
	void SendAuctionList(Player * plr, WorldPacket * packet);

private:
	RWLock itemLock;
	HM_NAMESPACE::hash_map<uint64, Item*> auctionedItems;

	RWLock auctionLock;
	HM_NAMESPACE::hash_map<uint32, Auction*> auctions;

	Mutex removalLock;
	list<Auction*> removalList;

	AuctionHouseDBC * dbc;

public:
	float cut_percent;
	float deposit_percent;
};
#endif

