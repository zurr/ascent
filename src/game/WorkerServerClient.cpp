#include "StdAfx.h"
#ifdef CLUSTERING

WSClient::WSClient(SOCKET fd) : Socket(fd, 1024576, 1024576)
{
	_remaining = 0;
	_cmd = 0;
}

WSClient::~WSClient()
{

}

void WSClient::OnRead()
{
	for(;;)
	{
		if(!_cmd)
		{
			if(GetReadBufferSize() < 6)
				break;

			Read(2, (uint8*)&_cmd);
			Read(4, (uint8*)&_remaining);
		}

		if(_remaining && GetReadBufferSize() < _remaining)
			break;

		WorldPacket * pck = new WorldPacket(_cmd, _remaining);
		_cmd = 0;
		pck->resize(_remaining);
		Read(_remaining, (uint8*)pck->contents());

		/* we could handle auth here */
		printf("Got packet from RS: %u\n", pck->GetOpcode());
		sClusterInterface.QueuePacket(pck);
	}
}

void WSClient::OnConnect()
{
	sClusterInterface.SetSocket(this);
}

void WSClient::SendPacket(WorldPacket * data)
{
	bool rv;
	uint32 size = data->size();
	uint16 opcode = data->GetOpcode();
	if(!IsConnected())
		return;

	BurstBegin();

	// Pass the header to our send buffer
	rv = BurstSend((const uint8*)&opcode, 2);
	rv = BurstSend((const uint8*)&size, 4);

	// Pass the rest of the packet to our send buffer (if there is any)
	if(size > 0 && rv)
		rv = BurstSend((const uint8*)data->contents(), size);

	if(rv) BurstPush();
	BurstEnd();
}

#endif
