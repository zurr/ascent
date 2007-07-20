#ifdef CLUSTERING
#ifndef _WORKERSERVER_CLIENT_H
#define _WORKERSERVER_CLIENT_H

class WSClient : public Socket
{
	bool _authenticated;
	uint32 _remaining;
	uint16 _cmd;
public:
	WSClient(SOCKET fd, const sockaddr_in * addr);
	~WSClient();

	void OnRead();
	void OnConnect();
};

#endif
#endif
