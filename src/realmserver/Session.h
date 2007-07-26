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

#ifndef _R_SESSION_H
#define _R_SESSION_H

typedef void(Session::*SessionPacketHandler)(WorldPacket&);

class Session
{
public:
	friend class WorldSocket;
	Session(uint32 id);
	~Session();

protected:
	FastQueue<WorldPacket*, Mutex> m_readQueue;
	WorldSocket * m_socket;
	WServer * m_server;
	WServer * m_nextServer;
	uint32 m_sessionId;
	uint32 m_accountId;
	RPlayerInfo * m_currentPlayer;
	uint32 m_latency;
	uint32 m_accountFlags;
	string m_GMPermissions;
	string m_accountName;
	uint32 m_build;
	static SessionPacketHandler Handlers[NUM_MSG_TYPES];

public:
	static void InitHandlers();
	void Update();
	inline RPlayerInfo * GetPlayer() { return m_currentPlayer; }

	inline void ClearCurrentPlayer() { m_currentPlayer = 0; }
	inline void ClearServers() { m_nextServer = m_server = 0; }
	inline void SetNextServer() { m_server = m_nextServer; }
	inline void SetServer(WServer * s) { m_server = s; }
	inline WServer * GetServer() { return m_server; }
	inline WorldSocket * GetSocket() { return m_socket; }
	inline uint32 GetAccountId() { return m_accountId; }
	inline uint32 GetSessionId() { return m_sessionId; }

	void SendPacket(WorldPacket * data)
	{
		if(m_socket && m_socket->IsConnected())
			m_socket->SendPacket(data);
	}

	void HandlePlayerLogin(WorldPacket & pck);
	void HandleCharacterEnum(WorldPacket & pck);
	void HandleCharacterCreate(WorldPacket & pck);
	void HandleCharacterDelete(WorldPacket & pck);
	void HandleCharacterRename(WorldPacket & pck);
	
	void HandleItemQuerySingleOpcode(WorldPacket & pck);
	void HandleCreatureQueryOpcode(WorldPacket & pck);
	void HandleGameObjectQueryOpcode(WorldPacket & pck);
	void HandleItemPageQueryOpcode(WorldPacket & pck);
	void HandleNpcTextQueryOpcode(WorldPacket & pck);
};

#endif