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

#include "RStdAfx.h"

initialiseSingleton(ClientMgr);

ClientMgr::ClientMgr()
{
	Session::InitHandlers();
	m_maxSessionId = 0;
	memset(m_sessions, 0, MAX_SESSIONS * sizeof(void*));
	Log.Success("ClientMgr", "Interface Created");
}

ClientMgr::~ClientMgr()
{

};

void ClientMgr::SendPackedClientInfo(WServer * server)
{
	if(!m_clients.size())
		return;

	WorldPacket data(ISMSG_PACKED_PLAYER_INFO, sizeof(RPlayerInfo) * m_clients.size() + 4);
	data << uint32(m_clients.size());

	/* pack them all togther, w000t! */
	ClientMap::iterator itr = m_clients.begin();
	RPlayerInfo * pi;
	for(; itr != m_clients.end(); ++itr)
	{
		pi = itr->second;
		pi->Pack(data);
	}

    /* TODO: compress the packet */
	server->SendPacket(&data);
}

Session * ClientMgr::CreateSession(uint32 AccountId)
{
	uint32 i = 0;
	uint32 j = 1;

	/* make sure nothing with this id exists already */
	for(; j < MAX_SESSIONS; ++j)
	{
		if(m_sessions[j] == 0)
		{
			if(!i)
				i = j;
		}
		else
		{
			if(m_sessions[i]->GetAccountId() == AccountId)
			{
				Log.Error("ClientMgr", "Could not create session for account %u due to a session already existing from %s", AccountId, 
					m_sessions[i]->GetSocket()->GetRemoteIP().c_str());

				return 0;
			}
		}
	}

	if(i == 0)
		return 0;

	if(m_maxSessionId < i)
	{
		Log.Debug("ClientMgr", "New max session id: %u", i);
		m_maxSessionId = i;
	}

	Log.Debug("ClientMgr", "Allocating session %u for account id %u", i, AccountId);
	m_sessions[i] = new Session(i);
	return m_sessions[i];
}

void ClientMgr::Update()
{
	for(uint32 i = 1; i <= m_maxSessionId; ++i)
		if(m_sessions[i])
			m_sessions[i]->Update();
}

RPlayerInfo * ClientMgr::CreateRPlayer(uint32 guid)
{
	ASSERT(m_clients.find(guid) == m_clients.end());
	RPlayerInfo * rp = new RPlayerInfo;
	rp->Guid = guid;
	m_clients[guid] = rp;
	return rp;
}

void ClientMgr::DestroyRPlayerInfo(uint32 guid)
{
	RPlayerInfo * rp;
	ClientMap::iterator itr = m_clients.find(guid);
	if(itr != m_clients.end())
	{
		rp = itr->second;
		m_clients.erase(itr);
		delete rp;
	}
}
