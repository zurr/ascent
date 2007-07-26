#include "RStdAfx.h"

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
	data << uint32(1);
	SendPacket(&data);
}