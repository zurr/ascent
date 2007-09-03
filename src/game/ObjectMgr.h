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

#ifndef _OBJECTMGR_H
#define _OBJECTMGR_H
#include "../shared/Threading/RWLock.h"

inline bool FindXinYString(std::string& x, std::string& y)
{
	return y.find(x) != std::string::npos;
}

enum SpellSpecialType
{
	NOTHING =0,
	SEAL    =1,
	BLESSING=2,
	CURSE   =3,
	ASPECT  =4,
	STING   =5,
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

struct FishingZoneEntry
{
	uint32 ZoneID;
	uint32 MinSkill;
	uint32 MaxSkill;
};

struct ZoneGuardEntry
{
	uint32 ZoneID;
	uint32 HordeEntry;
	uint32 AllianceEntry;
};

struct ItemPage
{
    uint32 id;
	char * text;
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
	uint32 TeachingLine;
	uint32 SpellType;
	SpellEntry* pTrainingSpell;
	int32 RequiredClass;
	//these values are required only for new trainer system
	uint32	TeachingSpellID; //this is actually castspell
	uint32	DeleteSpell;
	uint32	RequiredSpell;
	uint32	RequiredSkillLine;
	uint32	RequiredSkillLineValue;
	uint32	IsProfession;
	uint32	Cost;
	uint32	RequiredLevel;
	uint32	SpellRank;
	bool	CheckProfCount;
	uint32	RealTeachingSpellID; //well yes, just too keep the compatibility with old system we capt the misleading name...
};

struct Trainer
{
	uint32 SpellCount;
	TrainerSpell** SpellList;
	char*	TalkMessage;
	//these values are required only for new trainer system
	bool	IsSimpleTrainer; //simple trainers are the ones that use traners_defs and trainer_spells lists
	char*	TrainMsg;
	char*	NoTrainMsg;
	uint32	RequiredClass;
	uint32	Req_rep;
	int32	Req_rep_value;
	uint32	Req_lvl;
	uint32	TrainerType;
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


struct LevelInfo
{
	uint32 HP;
	uint32 Mana;
	uint32 Stat[5];
	uint32 XPToNextLevel;
};

struct ReputationMod
{
	uint32 faction[2];
	int32 value;
	uint32 replimit;
};

struct InstanceReputationMod
{
	uint32 mapid;
	uint32 mob_rep_reward;
	uint32 mob_rep_limit;
	uint32 boss_rep_reward;
	uint32 boss_rep_limit;
	uint32 faction[2];
};

struct ReputationModifier
{
	uint32 entry;
	vector<ReputationMod> mods;
};

struct InstanceReputationModifier
{
	uint32 mapid;
	vector<InstanceReputationMod> mods;
};

struct NpcMonsterSay
{
	float Chance;
	uint32 Language;
	uint32 Type;
	const char * MonsterName;

	uint32 TextCount;
	const char ** Texts;
};

enum MONSTER_SAY_EVENTS
{
	MONSTER_SAY_EVENT_ENTER_COMBAT		= 0,
	MONSTER_SAY_EVENT_RANDOM_WAYPOINT,
	MONSTER_SAY_EVENT_CALL_HELP,
	NUM_MONSTER_SAY_EVENTS,
};

enum AREATABLE_FLAGS
{
    AREA_CITY_AREA          = 0x20,
    AREA_NEUTRAL_AREA       = 0x40,
    AREA_PVP_ARENA          = 0x80,
    AREA_CITY               = 0x200,
    AREA_SANCTUARY          = 0x800,
    AREA_ISLAND             = 0x1000,
    AREA_PVP_OBJECTIVE_AREA = 0x8000,
};

enum AREATABLE_CATEGORY
{
    AREAC_CONTESTED          = 0,
    AREAC_ALLIANCE_TERRITORY = 2,
    AREAC_HORDE_TERRITORY    = 4,
    AREAC_SANCTUARY          = 6,
};

#define MAX_PREDEFINED_NEXTLEVELXP 70
static const uint32 NextLevelXp[MAX_PREDEFINED_NEXTLEVELXP]= {
	400,	900,	1400,	2100,	2800,	3600,	4500,	5400,	6500,	7600,
	8800,	10100,	11400,	12900,	14400,	16000,	17700,	19400,	21300,	23200,
	25200,	27300,	29400,	31700,	34000,	36400,	38900,	41400,	44300,	47400,
	50800,	54500,	58600,	62800,	67100,	71600,	76100,	80800,	85700,	90700,
	95800,	101000,	106300,	111800,	117500,	123200,	129100,	135100,	141200,	147500,
	153900,	160400,	167100,	173900,	180800,	187900,	195000,	202300,	209800,	217000,
	574700,	614400,	650300,	682300,	710200,	734100,	753700,	768900,	779700,	800100};

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
	Charter(uint32 id, uint32 leader) : CharterId(id), LeaderGuid(leader)
	{
		SignatureCount = 0;
		memset(Signatures, 0, sizeof(Signatures));
		ItemGuid = 0;
	}
	
	void SaveToDB();
	void Destroy();		 // When item is deleted.

	void AddSignature(uint32 PlayerGuid);
	void RemoveSignature(uint32 PlayerGuid);

	inline uint32 GetLeader() { return LeaderGuid; }
	inline uint32 GetID() { return CharterId; }

	inline bool IsFull() { return (SignatureCount == 9); }
};

typedef std::map<uint32, std::list<SpellEntry*>* >                  OverrideIdMap;
typedef HM_NAMESPACE::hash_map<uint32, Player*>                     PlayerStorageMap;
typedef std::list<GM_Ticket*>                                       GmTicketList;

class SERVER_DECL ObjectMgr : public Singleton < ObjectMgr >, public EventableObject
{
public:
	ObjectMgr();
	~ObjectMgr();
	void LoadCreatureWaypoints();

	// other objects
    
    // Set typedef's
	typedef std::set< Group * >                                         GroupSet;
	
    // HashMap typedef's
    typedef HM_NAMESPACE::hash_map<uint64, Item*>                       ItemMap;
	typedef HM_NAMESPACE::hash_map<uint32, CorpseData*>                 CorpseCollectorMap;
	typedef HM_NAMESPACE::hash_map<uint32, PlayerInfo*>                 PlayerNameMap;
	typedef HM_NAMESPACE::hash_map<uint32, PlayerCreateInfo*>           PlayerCreateInfoMap;
	typedef HM_NAMESPACE::hash_map<uint32, Guild*>                      GuildMap;
	typedef HM_NAMESPACE::hash_map<uint32, skilllinespell*>             SLMap;
	typedef HM_NAMESPACE::hash_map<uint32, std::vector<CreatureItem>*>  VendorMap;
	typedef HM_NAMESPACE::hash_map<uint32, Creature*>                   CreatureSqlIdMap;
    typedef HM_NAMESPACE::hash_map<uint64, Transporter*>                TransportMap;
	typedef HM_NAMESPACE::hash_map<uint32, Trainer*>                    TrainerMap;
	typedef HM_NAMESPACE::hash_map<uint32, std::vector<TrainerSpell*> > TrainerSpellMap;
    typedef HM_NAMESPACE::hash_map<uint32, ReputationModifier*>         ReputationModMap;
    typedef HM_NAMESPACE::hash_map<uint32, Corpse*>                     CorpseMap;
    
    // Map typedef's
    typedef std::map<uint32, LevelInfo*>                                LevelMap;
	typedef std::map<pair<uint32, uint32>, LevelMap* >                  LevelInfoMap;
    typedef std::map<uint32, std::list<ItemPrototype*>* >               ItemSetContentMap;
	typedef std::map<uint32, uint32>                                    NpcToGossipTextMap;
	typedef std::map<uint32, set<SpellEntry*> >                         PetDefaultSpellMap;
	typedef std::map<uint32, uint32>                                    PetSpellCooldownMap;
	typedef std::map<uint32, SpellEntry*>                               TotemSpellMap;

    // List typedef's
    typedef std::list<ThreatToSpellId*>                                 ThreadToSpellList;
    
    // object holders
	GmTicketList        GM_TicketList;
	TotemSpellMap       m_totemSpells;
	ThreadToSpellList   threatToSpells;
	OverrideIdMap       mOverrideIdMap;

	Player* GetPlayer(const char* name, bool caseSensitive = true);
	Player* GetPlayer(uint32 guid);
	
	CorpseMap m_corpses;
	Mutex _corpseslock;
	Mutex m_corpseguidlock;
    Mutex _TransportLock;
	uint32 m_hiCorpseGuid;
	
	Item * CreateItem(uint32 entry,Player * owner);
	Item * LoadItem(uint64 guid);
  
	// Groups
	Group * GetGroupByLeader(Player *pPlayer);
	Group * GetGroupById(uint32 id);
	inline uint32 GenerateGroupId()
	{
		return ++m_hiGroupId;
	}
	void AddGroup(Group* group) { mGroupSet.insert( group ); }
	void RemoveGroup(Group* group) { mGroupSet.erase( group ); }
	void LoadGroups();

	// player names
	void AddPlayerInfo(PlayerInfo *pn);
	PlayerInfo *GetPlayerInfo(uint32 guid );
	PlayerInfo *GetPlayerInfoByName(std::string & name);
	void DeletePlayerInfo(uint32 guid);
	PlayerCreateInfo* GetPlayerCreateInfo(uint8 race, uint8 class_) const;

	// Guild
	void AddGuild(Guild *pGuild);
	uint32 GetTotalGuildCount();
	bool RemoveGuild(uint32 guildId);
	Guild* GetGuild(uint32 guildId);  
	Guild* GetGuildByLeaderGuid(uint64 leaderGuid);  
	Guild* GetGuildByGuildName(std::string guildName);

	//Corpse Stuff
	Corpse *GetCorpseByOwner(uint32 ownerguid);
	void CorpseCollectorUnload();
	void DespawnCorpse(uint64 Guid);
	void CorpseAddEventDespawn(Corpse *pCorpse);
	void DelinkPlayerCorpses(Player *pOwner);
	Corpse * CreateCorpse();
	void AddCorpse(Corpse*);
	void RemoveCorpse(Corpse*);
	Corpse * GetCorpse(uint32 corpseguid);

	uint32 GetGossipTextForNpc(uint32 ID);

	// Gm Tickets
	void AddGMTicket(GM_Ticket *ticket,bool startup);
	void remGMTicket(uint64 guid);
	GM_Ticket* GetGMTicket(uint64 guid);

	skilllinespell* GetSpellSkill(uint32 id);

	//Vendors
	std::vector<CreatureItem> *GetVendorList(uint32 entry);
	void SetVendorList(uint32 Entry, std::vector<CreatureItem>* list_);

	//Totem
	SpellEntry* GetTotemSpell(uint32 spellId);

	// AI Threat by SpellId
	int32 GetAIThreatToSpellId(uint32 spellId);

	std::list<ItemPrototype*>* GetListForItemSet(uint32 setid);

  	Creature * GetCreatureBySqlId(uint32 Sql_Id);
	void SetCreatureBySqlId(uint32 Sql_Id, Creature * pCreature);

	Pet * CreatePet();
	uint32 m_hiPetGuid;
	Mutex m_petlock;

	Player * CreatePlayer();
	 Mutex m_playerguidlock;
	PlayerStorageMap _players;
	RWLock _playerslock;
	uint32 m_hiPlayerGuid;
	
	void AddPlayer(Player * p);//add it to global storage
	void RemovePlayer(Player *p);


	// Serialization

	void LoadQuests();
	void LoadPlayersInfo();
	void LoadPlayerCreateInfo();
	void LoadGuilds();
	Corpse* LoadCorpse(uint32 guid);
	void LoadCorpses(MapMgr * mgr);
	void LoadGMTickets();
	void SaveGMTicket(uint64 guid);
	void LoadAuctions();
	void LoadAuctionItems();
	void LoadSpellSkills();
	void LoadVendors();
	void LoadTotemSpells();
	void LoadAIThreatToSpellId();
	void LoadReputationModifierTable(const char * tablename, ReputationModMap * dmap);
	void LoadReputationModifiers();
	ReputationModifier * GetReputationModifier(uint32 entry_id, uint32 faction_id);

	void SetHighestGuids();
	uint32 GenerateLowGuid(uint32 guidhigh);
	uint32 GenerateMailID();
	
	void LoadTransporters();
	void ProcessGameobjectQuests();
    void AddTransport(Transporter * pTransporter);
   
	void GenerateTrainerSpells();
	bool AddTrainerSpell(uint32 entry, SpellEntry *pSpell);
	void LoadTrainers();
	Trainer* GetTrainer(uint32 Entry);

	void LoadExtraItemStuff();
	void LoadExtraCreatureProtoStuff();
	void CreateGossipMenuForPlayer(GossipMenu** Location, uint64 Guid, uint32 TextID, Player* Plr); 

	LevelInfo * GetLevelInfo(uint32 Race, uint32 Class, uint32 Level);
	void GenerateLevelUpInfo();
	void LoadDefaultPetSpells();
	set<SpellEntry*>* GetDefaultPetSpells(uint32 Entry);
	uint32 GetPetSpellCooldown(uint32 SpellId);
	void LoadPetSpellCooldowns();
	void LoadSpellFixes();
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

	Transporter * GetTransporter(uint64 guid);
	Transporter * GetTransporterByEntry(uint32 entry);

	Charter * CreateCharter(uint32 LeaderGuid);
	Charter * GetCharter(uint32 CharterId);
	void RemoveCharter(Charter *);
	void LoadGuildCharters();
	Charter * GetCharterByName(string &charter_name);
	Charter * GetCharterByItemGuid(uint64 guid);


	typedef HM_NAMESPACE::hash_map<uint32, NpcMonsterSay*> MonsterSayMap;
	MonsterSayMap mMonsterSays[NUM_MONSTER_SAY_EVENTS];

	void HandleMonsterSayEvent(Creature * pCreature, MONSTER_SAY_EVENTS Event);
	bool HasMonsterSay(uint32 Entry, MONSTER_SAY_EVENTS Event);
	void LoadMonsterSay();

	bool HandleInstanceReputationModifiers(Player * pPlayer, Unit * pVictim);
	void LoadInstanceReputationModifiers();

	inline bool IsSpellDisabled(uint32 spellid)
	{
		if(m_disabled_spells.find(spellid) != m_disabled_spells.end())
			return true;
		return false;
	}

	void LoadDisabledSpells();
	vector<TrainerSpell*> * GetTrainerSpellsForLine(uint32 line)
	{
		TrainerSpellMap::iterator itr = mNormalSpells.find(line);
		return (itr == mNormalSpells.end()) ? 0 : &itr->second;
	}
	vector<TrainerSpell*> * GetTrainerPetSpellsForLine(uint32 line)
	{
			TrainerSpellMap::iterator itr = mPetSpells.find(line);
			return (itr == mPetSpells.end()) ? 0 : &itr->second;
	}

	inline GuildMap::iterator GetGuildsBegin() { return mGuild.begin(); }
	inline GuildMap::iterator GetGuildsEnd() { return mGuild.end(); }

protected:
	RWLock playernamelock;
	uint32 m_mailid;
	// highest GUIDs, used for creating new objects
	Mutex m_guidGenMutex;
    union
    {
	    uint32 m_hiItemGuid;
	    uint32 m_hiContainerGuid;
    };
	uint32 m_hiGroupId;
	uint32 m_hiCharterId;
	RWLock m_charterLock;

	ReputationModMap m_reputation_faction;
	ReputationModMap m_reputation_creature;
	HM_NAMESPACE::hash_map<uint32, InstanceReputationModifier*> m_reputation_instance;

	HM_NAMESPACE::hash_map<uint32, Charter*> m_charters;
	
	set<uint32> m_disabled_spells;
	set<uint32> m_disabled_trainer_spells;

	uint64 TransportersCount;
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

	// Map of all starting infos needed for player creation
	PlayerCreateInfoMap mPlayerCreateInfo;

	// DK: Map of all Guild's
	GuildMap mGuild;

	// Map of all vendor goods
	VendorMap mVendors;

	// Maps for Gossip stuff
	NpcToGossipTextMap  mNpcToGossipText;

	SLMap				mSpellSkills;

	//Corpse Collector
	CorpseCollectorMap mCorpseCollector;

    TransportMap mTransports;

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
