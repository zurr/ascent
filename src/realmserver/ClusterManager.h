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

#define MAX_WORKER_SERVERS 100
#define MAX_SINGLE_MAPID 600

struct Instance
{
	uint32 InstanceId;
	uint32 MapId;
	WServer * Server;
};

#define IS_INSTANCE(a) (((a)>1)&&((a)!=530))
#define IS_MAIN_MAP(a) (((a)<2)||((a)==530))

class ClusterMgr : public Singleton<ClusterMgr>
{
	typedef map<uint32, Instance*> InstanceMap;

	WServer * WorkerServers[MAX_WORKER_SERVERS];
	Instance * SingleInstanceMaps[MAX_SINGLE_MAPID];
	
	InstanceMap Instances;
	uint32 m_maxInstanceId;
	uint32 m_maxWorkerServer;

public:
	ClusterMgr();

	WServer * GetServerByInstanceId(uint32 InstanceId);
	WServer * GetServerByMapId(uint32 MapId);

	Instance * GetInstanceByInstanceId(uint32 InstanceId);
	Instance * GetInstanceByMapId(uint32 MapId);

	WServer * CreateWorkerServer(WSSocket * s);
	inline WServer * GetWorkerServer(uint32 Id) { return (Id < MAX_WORKER_SERVERS) ? WorkerServers[Id] : 0; }
	void AllocateInitialInstances(WServer * server, vector<uint32>& preferred);

	// find the worker server with the least load for the new instance
	WServer * GetWorkerServerForNewInstance();

	/* create new instance, or a main map */
	Instance * CreateInstance(uint32 MapId, WServer * server);

	/* create new instance based on template, or a saved instance */
	Instance * CreateInstance(uint32 InstanceId, uint32 MapId);

	/* gets the instance struct by instance id */
	Instance * GetInstance(uint32 InstanceId)
	{
		InstanceMap::iterator itr = Instances.find(InstanceId);
		return (itr == Instances.end()) ? 0 : itr->second;
	}

	/* distribute packet to all worker servers */
	inline void DistributePacketToAll(WorldPacket * data) { DistributePacketToAll(data, 0); }

	/* distribute packet to all worker server excluding one */
	void DistributePacketToAll(WorldPacket * data, WServer * exclude);

	/* loop */
	void Update();

};


#define sClusterMgr ClusterMgr::getSingleton()