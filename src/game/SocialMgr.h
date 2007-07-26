/****************************************************************************
 *
 * Friends System
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

#ifndef __SOCIALMGR_H
#define __SOCIALMGR_H

struct SocialStr
{
	uint64 pGuid;
	uint32 Area;
	uint8 Level;
	uint8 Class;
};

class SocialMgr;
class SocialMgr :  public Singleton < SocialMgr >
{
public:
	SocialMgr();
	~SocialMgr();

	void AddFriend(Player* plr, std::string friendname);
	void AddIgnore(Player* plr, std::string ignorename);
	void DelFriend(Player* plr, uint64 friendguid);
	void DelIgnore(Player* plr, uint64 ignoreguid);

	bool IsFriend(uint64 plrguid, uint64 target);
	inline bool HasFriend(uint64 plrguid, uint64 mfriend); // Checking before adding a friend
	bool IsIgnore(uint64 plrguid, uint64 target);
	inline bool HasIgnore(uint64 plrguid, uint64 mignore); // Checking before adding an ignore
	bool HasIgnore(Player* plr, Player* mignore); // Checking for whispers blocking

	void SendFriendList(Player* plr);
	void SendIgnoreList(Player* plr);
	void SendUpdateToFriends(Player* plr);

	inline void SendOnlinePkt(Player* plr, SocialStr* pNfo);
	inline void SendOfflinePkt(Player* plr, uint64 fGuid);

	void LoggedIn(Player* plr); // To send a friend status update 'online' to people having him in their list
	void LoggedOut(Player* plr); // To send a friend status update 'offline' to people having him in their list
	void RemovePlayer(Player* plr); // When deleting a character from the server

	void LoadFromDB();
	
private:
	std::map<uint64, std::set<uint64>*> m_isInFriendList;
	std::map<uint64, std::set<uint64>*> m_hasInFriendList;
	std::map<uint64, std::set<uint64>*> m_isInIgnoreList;
	std::map<uint64, std::set<uint64>*> m_hasInIgnoreList;
	std::map<uint64, std::set<uint64>*> m_needsBlockNotice;

};

#define sSocialMgr SocialMgr::getSingleton()

#endif
