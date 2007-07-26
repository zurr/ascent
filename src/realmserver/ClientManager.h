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

#define MAX_SESSIONS 3000

class ClientMgr : public Singleton<ClientMgr>
{
public:
	typedef HM_NAMESPACE::hash_map<uint32, RPlayerInfo*> ClientMap;

protected:
	ClientMap m_clients;
	uint32 m_maxSessionId;
	Session * m_sessions[MAX_SESSIONS];

public:
	ClientMgr();
	~ClientMgr();
	
	/* create rplayerinfo struct */
	RPlayerInfo * CreateRPlayer(uint32 guid);

	/* destroy rplayerinfo struct */
	void DestroyRPlayerInfo(uint32 guid);

	/* get rplayer */
	inline RPlayerInfo * GetRPlayer(uint32 guid)
	{
		ClientMap::iterator itr = m_clients.find(guid);
		return (itr != m_clients.end()) ? itr->second : 0;
	}
	/* send "mini" client data to all servers */
	void SendPackedClientInfo(WServer * server);

	/* get session by id */
	inline Session * GetSession(uint32 Id) { return (Id < MAX_SESSIONS) ? m_sessions[Id] : 0; }

	/* create a new session, returns null if the player is already logged in */
	Session * CreateSession(uint32 AccountId);

	/* updates sessions */
	void Update();
};

#define sClientMgr ClientMgr::getSingleton()