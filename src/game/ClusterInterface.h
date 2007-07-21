#ifdef CLUSTERING
#ifndef _CLUSTERINTERFACE_H
#define _CLUSTERINTERFACE_H

#define MAX_SESSIONS 3000
#include "../realmserver/Structures.h"

class ClusterInterface;
typedef void(ClusterInterface::*ClusterInterfaceHandler)(WorldPacket&);

class ClusterInterface : public Singleton<ClusterInterface>
{
	typedef HM_NAMESPACE::hash_map<uint32, RPlayerInfo*> OnlinePlayerMap;
	OnlinePlayerMap _onlinePlayers;
	WSClient * _clientSocket;
	FastQueue<WorldPacket*, Mutex> _pckQueue;
	time_t _lastConnectTime;
	WorldSession * _sessions[MAX_SESSIONS];
public:

	static ClusterInterfaceHandler PHandlers[IMSG_NUM_TYPES];
	static void InitHandlers();

	ClusterInterface();
	~ClusterInterface();

	void ForwardWoWPacket(uint16 opcode, uint32 size, const void * data, uint32 sessionid);
	void ConnectToRealmServer();
	
	RPlayerInfo * GetPlayer(uint32 guid)
	{
		// this should use a mutex - burlex
		OnlinePlayerMap::iterator itr = _onlinePlayers.find(guid);
		return (itr == _onlinePlayers.end()) ? 0 : itr->second;
	}

	inline WorldSession * GetSession(uint32 sid) { return _sessions[sid]; }

	void HandleAuthRequest(WorldPacket & pck);
	void HandleAuthResult(WorldPacket & pck);
	void HandleRegisterResult(WorldPacket & pck);
	void HandleCreateInstance(WorldPacket & pck);
	void HandleDestroyInstance(WorldPacket & pck);
	void HandlePlayerLogin(WorldPacket & pck);
	void HandlePackedPlayerInfo(WorldPacket & pck);
	void HandleWoWPacket(WorldPacket & pck);

	inline void QueuePacket(WorldPacket * pck) { _pckQueue.Push(pck); }

	void Update();
	void DestroySession(uint32 sid);

	inline void SendPacket(WorldPacket * data) { if(_clientSocket) _clientSocket->SendPacket(data); }
	inline void SetSocket(WSClient * s) { _clientSocket = s; }
};

#define sClusterInterface ClusterInterface::getSingleton()

#endif
#endif
