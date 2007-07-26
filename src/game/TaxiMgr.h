/****************************************************************************
 *
 * Taxi System
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

#ifndef __TAXIMGR_H
#define __TAXIMGR_H

#define TAXI_TRAVEL_SPEED 32

class Player;

struct TaxiNode {
	uint32 id;
	float x,y,z;
	uint32 mapid;
	uint32 horde_mount;
	uint32 alliance_mount;
};

struct TaxiPathNode 
{
	float x,y,z;
	uint32 mapid;
};

class TaxiPath {
	friend class TaxiMgr;

public:
	TaxiPath() 
	{ 
		m_length = 0;
	}

	~TaxiPath() 
	{
		while(m_pathNodes.size())
		{
			TaxiPathNode *pn = m_pathNodes.begin()->second;
			m_pathNodes.erase(m_pathNodes.begin());
			delete pn;
		}
	}

	inline const float & getLength( ) const { return m_length; };
	void ComputeLen();
	void SetPosForTime(float &x, float &y, float &z, uint32 time, uint32* lastnode);
	inline uint32 GetID() { return id; }
	void SendMoveForTime(Player *riding, Player *to, uint32 time);
	void AddPathNode(uint32 index, TaxiPathNode* pn) { m_pathNodes[index] = pn; }
	inline uint32 GetNodeCount() { return m_pathNodes.size(); }
	TaxiPathNode* GetPathNode(uint32 i);
	inline uint32 GetPrice() { return price; }

protected:

	std::map<uint32, TaxiPathNode*> m_pathNodes;

	float m_length;
	uint32 id, to, from, price;
};


class TaxiMgr :  public Singleton < TaxiMgr >
{
public:
	TaxiMgr() 
	{
		_LoadTaxiNodes();
		_LoadTaxiPaths();
	}

	~TaxiMgr() 
	{ 
		while(m_taxiPaths.size())
		{
			TaxiPath *p = m_taxiPaths.begin()->second;
			m_taxiPaths.erase(m_taxiPaths.begin());
			delete p;
		}
		while(m_taxiNodes.size())
		{
			TaxiNode *n = m_taxiNodes.begin()->second;
			m_taxiNodes.erase(m_taxiNodes.begin());
			delete n;
		}
	}

	TaxiPath *GetTaxiPath(uint32 path);
	TaxiPath *GetTaxiPath(uint32 from, uint32 to);
	TaxiNode *GetTaxiNode(uint32 node);

	uint32 GetNearestTaxiNode( float x, float y, float z, uint32 mapid );
	bool GetGlobalTaxiNodeMask( uint32 curloc, uint32 *Mask );


private:
	void _LoadTaxiNodes();
	void _LoadTaxiPaths();

	HM_NAMESPACE::hash_map<uint32, TaxiNode*> m_taxiNodes;
	HM_NAMESPACE::hash_map<uint32, TaxiPath*> m_taxiPaths;
};

#define sTaxiMgr TaxiMgr::getSingleton()

#endif
