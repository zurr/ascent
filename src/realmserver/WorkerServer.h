
#define MAX_SESSIONS_PER_SERVER 1000

struct Instance;
typedef void(WServer::*WServerHandler)(WorldPacket &);

class WServer
{
	static WServerHandler PHandlers[IMSG_NUM_TYPES];
	uint32 m_id;
	WSSocket * m_socket;
	FastQueue<WorldPacket*, Mutex> m_recvQueue;
	list<Instance*> m_instances;

public:
	static void InitHandlers();
	WServer(uint32 id, WSSocket * s);
	~WServer();

	inline uint32 GetInstanceCount() { return m_instances.size(); }
	inline void SendPacket(WorldPacket * data) { if(m_socket) m_socket->SendPacket(data); }
	inline void SendWoWPacket(Session * from, WorldPacket * data) { if(m_socket) m_socket->SendWoWPacket(from, data); }
	inline void AddInstance(Instance * pInstance) { m_instances.push_back(pInstance); }
	inline void QueuePacket(WorldPacket * data) { m_recvQueue.Push(data); }
	inline uint32 GetID() { return m_id; }

	void Update();

protected:
	/* packet handlers */
	void HandleRegisterWorker(WorldPacket & pck);
	void HandleWoWPacket(WorldPacket & pck);
	void HandlePlayerLoginResult(WorldPacket & pck);
	void HandlePlayerLogout(WorldPacket & pck);
	void HandleTeleportRequest(WorldPacket & pck);
};
