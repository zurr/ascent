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

SessionPacketHandler Session::Handlers[NUM_MSG_TYPES];
void Session::InitHandlers()
{
	memset(Handlers, 0, sizeof(void*) * NUM_MSG_TYPES);
	Handlers[CMSG_PLAYER_LOGIN] = &Session::HandlePlayerLogin;
	Handlers[CMSG_CHAR_ENUM] = &Session::HandleCharacterEnum;
	Handlers[CMSG_ITEM_QUERY_SINGLE] = &Session::HandleItemQuerySingleOpcode;
}

Session::Session(uint32 id) : m_sessionId(id)
{
	m_socket = 0;
	m_server = 0;
	m_accountId = 0;
	m_currentPlayer = 0;
	m_latency = 0;
	m_accountFlags = 0;
	m_build = 0;
	m_nextServer = 0;
}

void Session::Update()
{
	WorldPacket * pck;
	uint16 opcode;
	while((pck = m_readQueue.Pop()))
	{
		opcode = pck->GetOpcode();

		/* can we handle it ourselves? */
		if(Session::Handlers[opcode] != 0)
		{
			(this->*Session::Handlers[opcode])(*pck);
		}
		else
		{
			/* no? pass it back to the worker server for handling. */
			if(m_server) m_server->SendWoWPacket(this, pck);
		}
		delete pck;
	}
}
