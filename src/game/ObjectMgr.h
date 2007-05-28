/****************************************************************************
 *
 * General Object Type File
 * Copyright (c) 2007 Antrix Team
 *
 * This file may be distributed under the terms of the Q Public License
 * as defined by Trolltech ASA of Norway and appearing in the file
 * COPYING included in the packaging of this file.
 *
 * This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 * WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 */

#ifndef _OBJECTMGR_H
#define _OBJECTMGR_H
#include "../shared/Threading/RWLock.h"

inline bool FindXinYString(std::string& x, std::string& y)
{
	return y.find(x) != std::string::npos;
}

enum SpellSpecialType
{
	NOTHING=0,
	SEAL=1,
	BLESSING=2,
	CURSE=3,
	ASPECT=4,
	STING=5,
};

struct SpellExtraInfo
{
	uint32 Id;
	uint32 specialtype;
	uint32 enchantableslots;
	uint32 ExtraFlags;
};

struct GM_Ticket
{
	uint64 guid;
	std::string name;
	uint32 level;
	uint32 type;
	float posX;
	float posY;
	float posZ;
	std::string message;
	uint32 timestamp;
};

struct ThreatToSpellId
{
	uint32 spellId;
	int32 mod;
};

struct TrainerSpellOverride
{
	uint32 SpellID;
	uint32 Cost;
	uint32 RequiredSpell;
	uint32 DeleteSpell;
	uint32 RequiredSkill;
	uint32 RequiredSkillValue;
	uint8 RequiredLevel;
	int32 RequiredClass;
};

struct FishingZoneEntry
{
	uint32 MinSkill;
	uint32 MaxSkill;
};

struct ItemPage
{
	std::string text;
	uint32 next_page;
};

struct SpellReplacement
{
	uint32 count;
	uint32 *spells;
};

class Group;

struct GossipMenuItem
{
	uint32 Id;
	uint32 IntId;
	uint16 Icon;
	string Text;
};

struct SpellEntry;
struct TrainerSpell
{

	uint32 SpellID;
	SpellEntry* pSpell;

	uint32 SpellRank;
	
	uint32 RequiredSpell;
	uint32 DeleteSpell;

	uint32 RequiredSkillLine;
	uint32 RequiredSkillLineValue;
	uint32 TeachingLine;
	uint32 IsProfession;

	uint32 Cost;
	uint32 RequiredLevel;

	uint32 SpellType;
	uint32 TeachingSpellID;
	SpellEntry* pTrainingSpell;

	int32 RequiredClass;
};

struct Trainer
{
	uint32 SpellCount;
	TrainerSpell** SpellList;

	char* TalkMessage;
	uint32 TrainerType;
	uint32 RequiredClass;
};

struct LevelInfo
{
	uint32 HP;
	uint32 Mana;
	uint32 Stat[5];
	uint32 XPToNextLevel;
};

struct ReputationMod
{
	uint32 faction;
	int32 delta_min;
	int32 delta_max;
};

struct ReputationModifier
{
	uint32 entry;
	vector<ReputationMod> mods;
};

class SERVER_DECL GossipMenu
{
public:
	GossipMenu(uint64 Creature_Guid, uint32 Text_Id);
	void AddItem(GossipMenuItem* GossipItem);
	void AddItem(uint16 Icon, const char* Text, int32 Id = -1);
	void BuildPacket(WorldPacket& Packet);
	void SendTo(Player* Plr);
	GossipMenuItem GetItem(uint32 Id);
	inline void SetTextID(uint32 TID) { TextId = TID; }

protected:
	uint32 TextId;
	uint64 CreatureGuid;
	std::vector<GossipMenuItem> Menu;
};

class Charter
{
public:
	uint32 SignatureCount;
	uint32 Signatures[9];
	uint32 LeaderGuid;
	uint64 ItemGuid;
	uint32 CharterId;
	string GuildName;

	Charter(Field * fields);
	Charter(uint32 id, uint32 leader) : CharterId(id)
	{
		SignatureCount = 0;
		memset(Signatures, 0, sizeof(Signatures));
		LeaderGuid = ItemGuid = 0;
	}
	
	void SaveToDB();
	void Destroy();		 // When item is deleted.

	void AddSignature(uint32 PlayerGuid);
	void RemoveSignature(uint32 PlayerGuid);

	inline uint32 GetLeader() { return LeaderGuid; }
	inline uint32 GetID() { return CharterId; }

	inline bool IsFull() { return (SignatureCount == 9); }
};

typedef std::map<uint32, std::list<SpellEntry*>* > OverrideIdMap;

class SERVER_DECL ObjectMgr : public Singleton < ObjectMgr >, public EventableObject
{
public:
	ObjectMgr();
	~ObjectMgr();
	void LoadCreatureWaypoints();

	// other objects
	typedef std::set< Group * > GroupSet;
	typedef HM_NAMESPACE::hash_map<uint64, Item*> ItemMap;
	typedef HM_NAMESPACE::hash_map<uint32, GossipText*> GossipTextMap;
	typedef HM_NAMESPACE::hash_map<uint32, CorpseData*> CorpseCollectorMap;
	typedef HM_NAMESPACE::hash_map<uint32, GraveyardTeleport*> GraveyardMap;
	typedef HM_NAMESPACE::hash_map<uint32, PlayerInfo*> PlayerNameMap;
	typedef HM_NAMESPACE::hash_map<uint32, CreatureInfo*> CreatureNameMap;
	typedef HM_NAMESPACE::hash_map<uint32, GameObjectInfo*> GameObjectNameMap;
	typedef HM_NAMESPACE::hash_map<uint32, ItemPrototype*> ItemPrototypeMap;
	typedef HM_NAMESPACE::hash_map<uint32, PlayerCreateInfo*> PlayerCreateInfoMap;
	typedef HM_NAMESPACE::hash_map<uint32, Guild*> GuildMap;
	typedef HM_NAMESPACE::hash_map<uint32, TeleportCoords*> TeleportMap;
	typedef HM_NAMESPACE::hash_map<uint32, skilllinespell*> SLMap;
	typedef HM_NAMESPACE::hash_map<uint32, std::vector<CreatureItem>*> VendorMap;
	typedef HM_NAMESPACE::hash_map<uint32, PvPArea*> PvPAreaMap;
	typedef HM_NAMESPACE::hash_map<uint32, FishingZoneEntry*> FishingZoneMap;
	typedef HM_NAMESPACE::hash_map<uint32, ItemPage*> ItemPageMap;
	typedef HM_NAMESPACE::hash_map<uint32, SpellExtraInfo*> SpellExtraInfoMap;
	typedef HM_NAMESPACE::hash_map<uint32, Creature*> CreatureSqlIdMap;
	
	typedef HM_NAMESPACE::hash_map<uint32, Trainer*> TrainerMap;
	typedef HM_NAMESPACE::hash_map<uint32, std::vector<TrainerSpell*> > TrainerSpellMap;
	typedef std::map<uint32, LevelInfo*> LevelMap;
	typedef std::map<pair<uint32, uint32>, LevelMap* > LevelInfoMap;

	typedef std::map<uint32, std::list<ItemPrototype*>* > ItemSetContentMap;
	typedef std::map<uint32, uint32> NpcToGossipTextMap;
	typedef std::map<uint32, set<SpellEntry*> > PetDefaultSpellMap;
	typedef std::map<uint32, uint32> PetSpellCooldownMap;
	typedef std::map<uint32, SpellEntry*> TotemSpellMap;
	
	std::list<GM_Ticket*> GM_TicketList;
	TotemSpellMap m_totemSpells;
	std::list<ThreatToSpellId*> threatToSpells;
	OverrideIdMap mOverrideIdMap;

	Player* GetPlayer(const char* name, bool caseSensitive = true);
	Player* GetPlayer(uint32 guid);
	
	HM_NAMESPACE::hash_map<uint32, Corpse*>m_corpses;
	Mutex _corpseslock;
	Mutex m_corpseguidlock;
	uint32 m_hiCorpseGuid;
	
	Item * CreateItem(uint32 entry,Player * owner);
	Item * LoadItem(uint64 guid);
  
	void LoadSpellExtraData();
	SpellExtraInfo* GetSpellExtraData(uint32 id);

	//map entry extra data for each spellid
	SpellExtraInfoMap mExtraSpellData;

	// Groups
	Group * GetGroupByLeader(Player *pPlayer);
	Group * GetGroupById(uint32 id);
	inline uint32 GenerateGroupId()
	{
		return ++m_hiGroupId;
	}
	void AddGroup(Group* group) { mGroupSet.insert( group ); }
	void RemoveGroup(Group* group) { mGroupSet.erase( group ); }

	// player names
	void AddPlayerInfo(PlayerInfo *pn);
	PlayerInfo *GetPlayerInfo(uint32 guid );
	PlayerInfo *GetPlayerInfoByName(std::string & name);
	void DeletePlayerInfo(uint32 guid);

	// creature names
	CreatureInfo *GetCreatureName( uint32 id );

	// item prototypes
	ItemPrototype* GetItemPrototype(uint32 id);
	PlayerCreateInfo* GetPlayerCreateInfo(uint8 race, uint8 class_) const;

	// DK:Guild
	void AddGuild(Guild *pGuild);
	uint32 GetTotalGuildCount();
	bool RemoveGuild(uint32 guildId);
	Guild* GetGuild(uint32 guildId);  
	Guild* GetGuildByLeaderGuid(uint64 leaderGuid);  
	Guild* GetGuildByGuildName(std::string guildName);

	//Corpse Stuff
	Corpse *GetCorpseByOwner(uint32 ownerguid);
	void CorpseCollectorLoad();
	void CorpseCollectorUnload();
	void DespawnCorpse(uint64 Guid);
	void CorpseAddEventDespawn(Corpse *pCorpse);
	void DelinkPlayerCorpses(Player *pOwner);
	Corpse * CreateCorpse();
	void AddCorpse(Corpse*);
	void RemoveCorpse(Corpse*);
	Corpse * GetCorpse(uint32 corpseguid);
	//Gossip Stuff
	void AddGossipText(GossipText *pGText);
	GossipText *GetGossipText(uint32 ID);
	uint32 GetGossipTextForNpc(uint32 ID);

	//Death stuff
	void AddGraveyard(GraveyardTeleport *pgrave);
	inline GraveyardMap::iterator GetGraveyardListBegin() { return mGraveyards.begin(); }
	inline GraveyardMap::iterator GetGraveyardListEnd() { return mGraveyards.end(); }

	//Teleport Stuff
	void AddTeleportCoords(TeleportCoords* TC);
	TeleportCoords* GetTeleportCoords(uint32 id) const;

	// Gm Tickets
	void AddGMTicket(GM_Ticket *ticket);
	void remGMTicket(uint64 guid);
	GM_Ticket* GetGMTicket(uint64 guid);

	//GameObject names
	GameObjectInfo *GetGameObjectName_(uint32 ID);

	skilllinespell* GetSpellSkill(uint32 id);

	//PVP
	void AddPvPArea(PvPArea* pvparea);
	PvPArea* GetPvPArea(uint32 AreaId);

	//Vendors
	std::vector<CreatureItem> *GetVendorList(uint32 entry);
	void SetVendorList(uint32 Entry, std::vector<CreatureItem>* list_);

	//Totem
	SpellEntry* GetTotemSpell(uint32 spellId);

	// AI Threat by SpellId
	int32 GetAIThreatToSpellId(uint32 spellId);

	// Fishing
	FishingZoneEntry* GetFishingZone(uint32 zoneid);
	inline uint32 GetFishingZoneMinSkill(uint32 zoneid) { return GetFishingZone(zoneid)->MinSkill; };
	inline uint32 GetFishingZoneMaxSkill(uint32 zoneid) { return GetFishingZone(zoneid)->MaxSkill; };

	std::list<ItemPrototype*>* GetListForItemSet(uint32 setid);

  
	inline ItemPage* GetItemPage(uint32 id)
	{
		ItemPageMap::iterator itr = mItemPages.find(id);
		return (itr != mItemPages.end()) ? itr->second : NULL;
	}

	Creature * GetCreatureBySqlId(uint32 Sql_Id);
	void SetCreatureBySqlId(uint32 Sql_Id, Creature * pCreature);

	Pet * CreatePet();
	uint32 m_hiPetGuid;
	Mutex m_petlock;

	Player * CreatePlayer();
	 Mutex m_playerguidlock;
	HM_NAMESPACE::hash_map<uint32, Player*> _players;
	RWLock _playerslock;
	uint32 m_hiPlayerGuid;
	
	void AddPlayer(Player * p);//add it to global storage
	void RemovePlayer(Player *p);


	// Serialization

	void LoadQuests();
	void LoadPlayersInfo();
	void LoadCreatureNames();
	void LoadGameObjectNames();
	void SaveCreatureNames();
	void LoadItemPrototypes();
	void LoadPlayerCreateInfo();
	void LoadGuilds();
	Corpse* LoadCorpse(uint32 guid);
	void LoadGossipText();
	void LoadGraveyards();
	void LoadTeleportCoords();
	void LoadGMTickets();
	void SaveGMTicket(uint64 guid);
	void LoadAuctions();
	void LoadAuctionItems();
	void LoadSpellSkills();
	void LoadPvPAreas();
	void LoadVendors();
	void LoadTotemSpells();
	void LoadAIThreatToSpellId();
	void LoadFishingZones();
	void LoadItemPages();
	void LoadReputationModifierTable(const char * tablename, HM_NAMESPACE::hash_map<uint32, ReputationModifier*> * dmap);
	void LoadReputationModifiers();
	ReputationModifier * GetReputationModifier(uint32 entry_id, uint32 faction_id);

	void SetHighestGuids();
	uint32 GenerateLowGuid(uint32 guidhigh);
	uint32 GenerateMailID();
	
	void LoadTransporters();
	void ProcessGameobjectQuests();
   
	void GenerateTrainerSpells();
	bool AddTrainerSpell(uint32 entry, SpellEntry *pSpell);
	void LoadTrainers();
	Trainer* GetTrainer(uint32 Entry);
	void LoadCreatureProtos();
	
	void CreateGossipMenuForPlayer(GossipMenu** Location, uint64 Guid, uint32 TextID, Player* Plr); 
	void ReloadTables();

	void ReloadCreatureNames();
	void ReloadItems();
	void ReloadQuests();
	void ReloadGameObjects();

	LevelInfo * GetLevelInfo(uint32 Race, uint32 Class, uint32 Level);
	void GenerateLevelUpInfo();
	void LoadDefaultPetSpells();
	set<SpellEntry*>* GetDefaultPetSpells(uint32 Entry);
	uint32 GetPetSpellCooldown(uint32 SpellId);
	void LoadPetSpellCooldowns();
	void LoadSpellFixes();
	CreatureProto*GetCreatureProto(uint32);
	WayPointMap * GetWayPointMap(uint32 spawnid);
	void LoadSpellOverride();

	uint32 GenerateCreatureSpawnID()
	{
		m_CreatureSpawnIdMutex.Acquire();
		uint32 r = ++m_hiCreatureSpawnId;
		m_CreatureSpawnIdMutex.Release();
		return r;
	}

	uint32 GenerateGameObjectSpawnID()
	{
		m_GOSpawnIdMutex.Acquire();
		uint32 r = ++m_hiGameObjectSpawnId;
		m_GOSpawnIdMutex.Release();
		return r;
	}

	Transporter * GetTransporter(uint32 guid);
	Transporter * GetTransporterByEntry(uint32 entry);

	Charter * CreateCharter(uint32 LeaderGuid);
	Charter * GetCharter(uint32 CharterId);
	void RemoveCharter(Charter *);
	void LoadGuildCharters();
	Charter * GetCharterByName(string &charter_name);
	Charter * GetCharterByItemGuid(uint64 guid);

	inline ItemPrototypeMap::iterator BeginItemPrototype() { return mItemPrototypes.begin(); }
	inline ItemPrototypeMap::iterator EndItemPrototype() { return mItemPrototypes.end(); }

	inline CreatureNameMap::iterator BeginCreatureInfo() { return mCreatureNames.begin(); }
	inline CreatureNameMap::iterator EndCreatureInfo() { return mCreatureNames.end(); }

protected:
	RWLock playernamelock;
	uint32 m_mailid;
	// highest GUIDs, used for creating new objects
	Mutex m_guidGenMutex;
	uint32 m_hiItemGuid;
	uint32 m_hiContainerGuid;
	uint32 m_hiGroupId;
	uint32 m_hiCharterId;
	RWLock m_charterLock;
	HM_NAMESPACE::hash_map<uint32, ReputationModifier*> m_reputation_faction;
	HM_NAMESPACE::hash_map<uint32, ReputationModifier*> m_reputation_creature;

	HM_NAMESPACE::hash_map<uint32, Charter*> m_charters;
	

	Transporter ** m_transporters;
	uint32 TransportersCount;
	HM_NAMESPACE::hash_map<uint32,CreatureProto*> m_creatureproto;
	HM_NAMESPACE::hash_map<uint32,PlayerInfo*> m_playersinfo;
	
	HM_NAMESPACE::hash_map<uint32,WayPointMap*> m_waypoints;//stored by spawnid
	uint32 m_hiCreatureSpawnId;
	
	Mutex m_CreatureSpawnIdMutex;
	Mutex m_GOSpawnIdMutex;

	uint32 m_hiGameObjectSpawnId;
	
	///// Object Tables ////
	// These tables are modified as creatures are created and destroyed in the world

	// Group List
	GroupSet			mGroupSet;

	// Map of all item types in the game
	ItemPrototypeMap	mItemPrototypes;

	// map entry to a creature name
	CreatureNameMap	 mCreatureNames;

	//Map entry to a gameobject query name
	GameObjectNameMap   mGameObjectNames;

	// Map of all starting infos needed for player creation
	PlayerCreateInfoMap mPlayerCreateInfo;

	// DK: Map of all Guild's
	GuildMap mGuild;

	// Map of all vendor goods
	VendorMap mVendors;

	// Maps for Gossip stuff
	GossipTextMap	   mGossipText;
	NpcToGossipTextMap  mNpcToGossipText;

	// Death Stuff
	GraveyardMap		mGraveyards;

	// Teleport Stuff
	TeleportMap			mTeleports;

	SLMap				mSpellSkills;

	//PVP Stuff
	PvPAreaMap			mPvPAreas;

	// cached fishing zones
	FishingZoneMap		 mFishingZones;
	ItemPageMap			mItemPages;

	//Corpse Collector
	CorpseCollectorMap mCorpseCollector;

	ItemSetContentMap mItemSets;

	TrainerSpellMap mNormalSpells;
	TrainerSpellMap mPetSpells;

	TrainerMap mTrainers;
	LevelInfoMap mLevelInfo;
	PetDefaultSpellMap mDefaultPetSpells;
	PetSpellCooldownMap mPetSpellCooldowns;
	CreatureSqlIdMap mCreatureSqlIds;
	Mutex CreatureSqlIdMapMutex;
};


#define objmgr ObjectMgr::getSingleton()

//void SetProgressBar(int, int, const char*);
//void ClearProgressBar();

#endif
