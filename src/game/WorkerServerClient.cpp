#include "StdAfx.h"
#ifdef CLUSTERING

WSClient::WSClient(SOCKET fd, const sockaddr_in * addr) : TcpSocket(fd, 1024576, 1024576, false, addr)
{

}

WSClient::~WSClient()
{

}

void WSClient::OnRecvData()
{
	BaseBuffer & ibuf = *GetReadBuffer();
	for(;;)
	{
		if(!_cmd)
		{
			if(ibuf.GetSize() < 6)
				break;

			ibuf.Read(&_cmd, 2);
			ibuf.Read(&_remaining, 4);
			_remaining = ntohl(_remaining);
		}

		if(_remaining && ibuf.GetSize() < _remaining)
			break;

		WorldPacket * pck = new WorldPacket(_cmd, _remaining);
		pck->resize(_remaining);
		ibuf.Read((void*)pck->contents(), _remaining);

		/* we could handle auth here */
		sClusterInterface.QueuePacket(pck);
	}
}

void WSClient::OnConnect()
{
	
}

#endif
