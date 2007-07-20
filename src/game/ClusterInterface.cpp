#include "StdAfx.h"

#ifdef CLUSTERING

ClusterInterfaceHandler ClusterInterface::PHandlers[IMSG_NUM_TYPES];

void ClusterInterface::InitHandlers()
{
	memset(PHandlers, 0, sizeof(void*) * IMSG_NUM_TYPES);
}

ClusterInterface::ClusterInterface()
{
	ClusterInterface::InitHandlers();
}

ClusterInterface::~ClusterInterface()
{

}

void ClusterInterface::ForwardWoWPacket(uint16 opcode, uint32 size, const void * data, uint32 sessionid)
{
	uint32 size2 = 10 + size;
	uint16 opcode2 = ICMSG_WOW_PACKET;

	if(!_clientSocket) return;			// Shouldn't happen
	
	_clientSocket->LockWriteBuffer();
	_clientSocket->Write(&opcode2, 2);
	_clientSocket->Write(&size2, 4);
	_clientSocket->Write(&sessionid, 4);
	_clientSocket->Write(&opcode, 2);
    _clientSocket->Write(&size, 4);
	if(size)
		_clientSocket->Write(data, size);

	_clientSocket->UnlockWriteBuffer();
}

void ClusterInterface::ConnectToRealmServer()
{
	// todo: replace with config
	const char * hostname = "localhost";
	uint16 port = 10010;

    WSClient * s = ConnectTCPSocket<WSClient>(hostname, port);
	if(!s)
	{
		Log.Error("ClusterInterface", "Could not connect to %s:%u", hostname, port);
		return;
	}

	_clientSocket = s;
}

void ClusterInterface::HandleAuthRequest(WorldPacket & pck)
{

}

void ClusterInterface::HandleAuthResult(WorldPacket & pck)
{

}


void ClusterInterface::HandleRegisterResult(WorldPacket & pck)
{

}

void ClusterInterface::HandleCreateInstance(WorldPacket & pck)
{

}

void ClusterInterface::HandleDestroyInstance(WorldPacket & pck)
{

}

void ClusterInterface::HandlePlayerLogin(WorldPacket & pck)
{

}

void ClusterInterface::HandlePackedPlayerInfo(WorldPacket & pck)
{
	RPlayerInfo * pi;
	// todo: uncompress
	uint32 count;
	pck >> count;
	for(uint32 i = 0; i < count; ++i)
	{
		pi = new RPlayerInfo;
		pi->Unpack(pck);
		_onlinePlayers[pi->Guid] = pi;
	}
}

#endif
