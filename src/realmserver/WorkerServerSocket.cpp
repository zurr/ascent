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
#include "svn_revision.h"

WSSocket::WSSocket(SOCKET fd) : Socket(fd, 100000, 100000)
{
	_authenticated = false;
	_remaining = 0;
	_cmd = 0;
	_ws = NULL;
}

WSSocket::~WSSocket()
{

}

void WSSocket::HandleAuthRequest(WorldPacket & pck)
{
	uint8 key[20];
	uint32 build;
	string ver;
	pck.read(key, 20);
	pck >> build;
	pck >> ver;

	Log.Notice("WSSocket", "Auth reply, server is %s build %u", ver.c_str(), build);

	// accept it
	WorldPacket data(ISMSG_AUTH_RESULT, 4);
	data << uint32(1);
	SendPacket(&data);
	_authenticated = true;
}

void WSSocket::OnRead()
{
    for(;;)
	{
		if(!_cmd)
		{
			if(GetReadBufferSize() < 6)
				break;

			Read(2, (uint8*)&_cmd);
			Read(4, (uint8*)&_remaining);
			//_remaining = ntohl(_remaining);
		}

        if(_remaining && GetReadBufferSize() < _remaining)
			break;

		if(_cmd == ICMSG_WOW_PACKET)
		{
			/* optimized version for packet passing, to reduce latency! ;) */
			uint32 sid = *(uint32*)&m_readBuffer[0];
			uint16 op  = *(uint16*)&m_readBuffer[4];
			uint32 sz  = *(uint32*)&m_readBuffer[6];
			Session * session = sClientMgr.GetSession(sid);
			if(session != NULL && session->GetSocket() != NULL)
				session->GetSocket()->OutPacket(op, sz, m_readBuffer + 10);

			RemoveReadBufferBytes(sz + 10/*header*/, false);
			_cmd = 0;
			continue;
		}
		WorldPacket * pck = new WorldPacket(_cmd, _remaining);
		_cmd = 0;
		pck->resize(_remaining);
		Read(_remaining, (uint8*)pck->contents());

		if(_authenticated)
		{
			// push to queue
			if(!_ws)
			{
				if(pck->GetOpcode() == ICMSG_REGISTER_WORKER)
				{
					// handle register worker
					HandleRegisterWorker(*pck);
				}

				/* I deliberately don't delete pck here for a reason :P */
			}
			else
			{
				_ws->QueuePacket(pck);
			}
		}
		else
		{
			if(pck->GetOpcode() != ICMSG_AUTH_REPLY)
				Disconnect();
			else
				HandleAuthRequest(*pck);
			
			delete pck;
		}
	}
}

void WSSocket::HandleRegisterWorker(WorldPacket & pck)
{
	uint32 build;
	pck >> build;

	// TODO: Check the build of the server
	WServer * new_server = sClusterMgr.CreateWorkerServer(this);
    if(new_server == 0)
	{
		WorldPacket data(ISMSG_REGISTER_RESULT, 10);
		data << uint32(0);
		SendPacket(&data);
		delete &pck;
		return;
	}

	/* because we don't have any locks in the managers, this has to execute
	   in the other thread. this is why I haven't deleted the packet yet
	 */
    _ws = new_server;
	pck.rpos(0);
	_ws->QueuePacket(&pck);
}

void WSSocket::SendPacket(WorldPacket * pck)
{
	bool rv;
	uint32 size = pck->size();
	uint16 opcode = pck->GetOpcode();
	if(!IsConnected())
		return;

	BurstBegin();

	// Pass the header to our send buffer
	rv = BurstSend((const uint8*)&opcode, 2);
	rv = BurstSend((const uint8*)&size, 4);

	// Pass the rest of the packet to our send buffer (if there is any)
	if(size > 0 && rv)
		rv = BurstSend((const uint8*)pck->contents(), size);

	if(rv) BurstPush();
	BurstEnd();
}

void WSSocket::SendWoWPacket(Session * from, WorldPacket * pck)
{
	bool rv;
	uint32 size1 = pck->size();
	uint16 opcode1 = pck->GetOpcode();
	uint32 size2 = size1 + 10;
	uint32 opcode2 = ISMSG_WOW_PACKET;
	uint32 id = from->GetSessionId();
	if(!IsConnected())
		return;

	BurstBegin();

	// Pass the header to our send buffer
	BurstSend((const uint8*)&opcode2, 2);
	BurstSend((const uint8*)&size2, 4);
	BurstSend((const uint8*)&id, 4);
	BurstSend((const uint8*)&opcode1, 2);
	rv=BurstSend((const uint8*)&size1, 4);	

	// Pass the rest of the packet to our send buffer (if there is any)
	if(size1 > 0 && rv)
		rv = BurstSend(pck->contents(), size1);

	if(rv) BurstPush();
	BurstEnd();
}

void WSSocket::OnConnect()
{
	WorldPacket data(ISMSG_AUTH_REQUEST, 4);
	data << uint32(g_getRevision());
	SendPacket(&data);
}
