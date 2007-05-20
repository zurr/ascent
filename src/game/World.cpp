/****************************************************************************
 *
 * Main World System
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

#include "StdAfx.h"

initialiseSingleton( World );


World::World()
{
    m_playerLimit = 0;
    m_allowMovement = true;
    m_gmTicketSystem = true;

    reqGmClient = false;
    GmClientChannel = "";

    m_UpdateDistance = 0.0f;
    m_StartTime = 0;
    eventholder = new EventableObjectHolder(-1);

    mQueueUpdateInterval = 10000;
    sendRevisionOnJoin = Config.MainConfig.GetBoolDefault("SendBuildOnJoin", false);
    MapPath = Config.MainConfig.GetStringDefault("MapPath", "maps");
    UnloadMapFiles = Config.MainConfig.GetBoolDefault("UnloadMapFiles", true);
    BreathingEnabled = Config.MainConfig.GetBoolDefault("EnableBreathing", true);
    SpeedhackProtection = Config.MainConfig.GetBoolDefault("SpeedhackProtection", false);
    SendStatsOnJoin = Config.MainConfig.GetBoolDefault("SendStatsOnJoin", true);
    compression_threshold = Config.MainConfig.GetIntDefault("CompressionThreshold", 1000);
    PeakSessionCount = 0;
    mInWorldPlayerCount = 0;
    mAcceptedConnections = 0;
    HordePlayers = 0;
    AlliancePlayers = 0;
    m_isOwnInstance = true;
}

World::~World()
{
    sLog.outString("  Saving players to DB...");
    for(SessionMap::iterator i=m_sessions.begin();i!=m_sessions.end();i++)
    {
        (i->second)->LogoutPlayer(true);
    
    }

    sLog.outString("  Deleting world packet logger...");
    delete WorldLog::getSingletonPtr();

    sLog.outString("  Deleting Social Manager...");
    delete SocialMgr::getSingletonPtr();

    sLog.outString("  Deleting Object Manager...");
    delete ObjectMgr::getSingletonPtr();
    
    sLog.outString("  Deleting Loot Manager...");        
    delete LootMgr::getSingletonPtr();
    
    sLog.outString("  Deleting LFG Manager...");    
    delete LfgMgr::getSingletonPtr();

    sLog.outString("  Deleting Channel Manager...");
    delete ChannelMgr::getSingletonPtr();

    sLog.outString("  Deleting Quest Manager...");
    delete QuestMgr::getSingletonPtr();
  
    sLog.outString("  Deleting Weather Manager...");
    delete WeatherMgr::getSingletonPtr();

    sLog.outString("  Deleting Taxi Manager...");
    delete TaxiMgr::getSingletonPtr();
    
    sLog.outString("  Deleting Battleground Manager...");    
    delete BattlegroundMgr::getSingletonPtr();

    //Save all characters to db on ctrl+c
    sLog.outString("Deleting Battleground Manager...");
    delete BattlegroundMgr::getSingletonPtr();

    sLog.outString("Removing all objects and deleting WorldCreator...\n");
    delete WorldCreator::getSingletonPtr();

    sLog.outString("Deleting Thread Manager..");
    delete ThreadMgr::getSingletonPtr();

    sLog.outString("Deleting Instance Saving Management...");
    delete InstanceSavingManagement::getSingletonPtr();

    sLog.outString("Deleting Random Number Generator...");
    delete MTRand::getSingletonPtr();

    for( AreaTriggerMap::iterator i = m_AreaTrigger.begin( ); i != m_AreaTrigger.end( ); ++ i ) 
    {
        delete i->second;
    }

    for( MapInfoMap::iterator i = m_mapinfo.begin( ); i != m_mapinfo.end( ); ++ i ) 
    {
        delete i->second;
    }
	
    //eventholder = 0;
    delete eventholder;
    sLog.outString("\n  Unloading DBC files...");
    // Indexed stores, need to call correct destructor.
    delete ((FastIndexedDataStore<emoteentry>*)EmoteStore::getSingletonPtr());
    delete ((FastIndexedDataStore<SpellEntry>*)SpellStore::getSingletonPtr());
    delete ((FastIndexedDataStore<Lock>*)LockStore::getSingletonPtr());
    delete ((FastIndexedDataStore<SpellRange>*)RangeStore::getSingletonPtr());
    delete ((FastIndexedDataStore<SpellCastTime>*)CastTimeStore::getSingletonPtr());
    delete ((FastIndexedDataStore<SpellDuration>*)DurationStore::getSingletonPtr());
    delete ((FastIndexedDataStore<SpellRadius>*)RadiusStore::getSingletonPtr());
    delete ((FastIndexedDataStore<FactionTemplateDBC>*)FactionTmpStore::getSingletonPtr());
    delete ((FastIndexedDataStore<FactionDBC>*)FactionStore::getSingletonPtr());
    delete ((FastIndexedDataStore<EnchantEntry>*)EnchantStore::getSingletonPtr());
    delete ((FastIndexedDataStore<WorldMapArea>*)WorldMapAreaStore::getSingletonPtr());
    delete ((FastIndexedDataStore<AreaTable>*)AreaStore::getSingletonPtr());
    delete ((FastIndexedDataStore<skilllineentry>*)SkillLineStore::getSingletonPtr());
    delete ((FastIndexedDataStore<RandomProps>*)RandomPropStore::getSingletonPtr());
    delete ((FastIndexedDataStore<ItemSetEntry>*)ItemSetStore::getSingletonPtr());
    delete ((FastIndexedDataStore<TransportAnimation>*)TransportAnimationStore::getSingletonPtr());
    delete ((FastIndexedDataStore<AuctionHouseDBC>*)AuctionHouseStore::getSingletonPtr());
    delete ((FastIndexedDataStore<CreatureSpellDataEntry>*)CreatureSpellDataStore::getSingletonPtr());
    delete ((FastIndexedDataStore<ItemExtendedCostEntry>*)ItemExtendedCostStore::getSingletonPtr());
    delete ((FastIndexedDataStore<CreatureFamilyEntry>*)CreatureFamilyStore::getSingletonPtr());
    delete ((FastIndexedDataStore<CharClassEntry>*)CharClassStore::getSingletonPtr());
    delete ((FastIndexedDataStore<CharRaceEntry>*)CharRaceStore::getSingletonPtr());
    delete ((FastIndexedDataStore<MapEntry>*)MapStore::getSingletonPtr());
    delete ((FastIndexedDataStore<GemPropertyEntry>*)GemPropertiesStore::getSingletonPtr());

    // Non-Indexed stores
    delete WorldMapOverlayStore::getSingletonPtr();
    delete SkillStore::getSingletonPtr();
    delete TaxiPathStore::getSingletonPtr();
    delete TaxiNodeStore::getSingletonPtr();
    delete TaxiPathNodeStore::getSingletonPtr();
    delete WorldSafeLocsStore::getSingletonPtr();
    delete NameGenStore::getSingletonPtr();
    delete TalentStore::getSingletonPtr();
	
    sLog.outString("  DBC files unloaded.\n");
}


WorldSession* World::FindSession(uint32 id) const
{
    SessionMap::const_iterator itr = m_sessions.find(id);

    if(itr != m_sessions.end())
        return itr->second;
    else
        return 0;
}

void World::RemoveSession(uint32 id)
{
    SessionMap::iterator itr = m_sessions.find(id);

    if(itr != m_sessions.end())
    {
        delete itr->second;
        m_sessions.erase(itr);
    }
}

void World::AddSession(WorldSession* s)
{
    ASSERT(s);
    m_sessions[s->GetAccountId()] = s;

    if(m_sessions.size() >  PeakSessionCount)
        PeakSessionCount = m_sessions.size();
}

void World::AddGlobalSession(WorldSession *session)
{
	SessionsMutex.Acquire();
    Sessions.insert(session);
	SessionsMutex.Release();
}

void World::RemoveGlobalSession(WorldSession *session)
{
    SessionsMutex.Acquire();
    Sessions.erase(session);
	SessionsMutex.Release();
}

void BasicTaskExecutor::run()
{
    /* Set thread priority, this is a bitch for multiplatform :P */
#ifdef WIN32
    switch(priority)
    {
        case BTE_PRIORITY_LOW:
            ::SetThreadPriority( ::GetCurrentThread(), THREAD_PRIORITY_LOWEST );
            break;

        case BTW_PRIORITY_HIGH:
            ::SetThreadPriority( ::GetCurrentThread(), THREAD_PRIORITY_ABOVE_NORMAL );
            break;

        default:        // BTW_PRIORITY_MED
            ::SetThreadPriority( ::GetCurrentThread(), THREAD_PRIORITY_NORMAL );
            break;
    }
#else
    struct sched_param param;
    switch(priority)
    {
    case BTE_PRIORITY_LOW:
        param.sched_priority = 0;
        break;

    case BTW_PRIORITY_HIGH:
        param.sched_priority = 10;
        break;

    default:        // BTW_PRIORITY_MED
        param.sched_priority = 5;
        break;
    }
    pthread_setschedparam(pthread_self(), SCHED_OTHER, &param);
#endif

    // Execute the task in our new context.
    cb->execute();
}

void World::SetInitialWorldSettings()
{
    sDatabase.Execute("UPDATE characters SET online = 0 WHERE online = 1");
   
    reqGmClient = Config.MainConfig.GetBoolDefault("reqGmClient", false);
    if(!Config.MainConfig.GetString("GmClientChannel", &GmClientChannel))
    {
        GmClientChannel = "";
    }

    m_lastTick = time(NULL);

    // TODO: clean this
    time_t tiempo;
    char hour[3];
    char minute[3];
    char second[3];
    struct tm *tmPtr;
    tiempo = time(NULL);
    tmPtr = localtime(&tiempo);
    strftime( hour, 3, "%H", tmPtr );
    strftime( minute, 3, "%M", tmPtr );
    strftime( second, 3, "%S", tmPtr );
    m_gameTime = (3600*atoi(hour))+(atoi(minute)*60)+(atoi(second)); // server starts at noon

    // TODO: clean this
    // fill in emotes table
    // it appears not every emote has an animation
    mPrices[1] = 10;
    mPrices[4] = 80;
    mPrices[6] = 150;
    mPrices[8] = 200;
    mPrices[10] = 300;
    mPrices[12] = 800;
    mPrices[14] = 900;
    mPrices[16] = 1800;
    mPrices[18] = 2200;
    mPrices[20] = 2300;
    mPrices[22] = 3600;
    mPrices[24] = 4200;
    mPrices[26] = 6700;
    mPrices[28] = 7200;
    mPrices[30] = 8000;
    mPrices[32] = 11000;
    mPrices[34] = 14000;
    mPrices[36] = 16000;
    mPrices[38] = 18000;
    mPrices[40] = 20000;
    mPrices[42] = 27000;
    mPrices[44] = 32000;
    mPrices[46] = 37000;
    mPrices[48] = 42000;
    mPrices[50] = 47000;
    mPrices[52] = 52000;
    mPrices[54] = 57000;
    mPrices[56] = 62000;
    mPrices[58] = 67000;
    mPrices[60] = 72000;

    uint32 start_time = getMSTime();

    sLog.outString("  Loading DBC files...");
	new GemPropertiesStore("DBC/GemProperties.dbc");
    new SpellStore("DBC/Spell.dbc");
    new LockStore("DBC/Lock.dbc");
    new SkillStore("DBC/SkillLineAbility.dbc");
    new EmoteStore("DBC/EmotesText.dbc");
    new RangeStore("DBC/SpellRange.dbc");
    new CastTimeStore("DBC/SpellCastTimes.dbc");
    new DurationStore("DBC/SpellDuration.dbc");
    new RadiusStore("DBC/SpellRadius.dbc");
    new FactionTmpStore("DBC/FactionTemplate.dbc");
    new FactionStore("DBC/Faction.dbc");
    new EnchantStore("DBC/SpellItemEnchantment.dbc");
    new WorldMapAreaStore("DBC/WorldMapArea.dbc");
    new WorldMapOverlayStore("DBC/WorldMapOverlay.dbc");
    new AreaStore("DBC/AreaTable.dbc");
    new SkillLineStore("DBC/SkillLine.dbc");
    new RandomPropStore("DBC/ItemRandomProperties.dbc");
    new TaxiPathStore("DBC/TaxiPath.dbc");
    new TaxiNodeStore("DBC/TaxiNodes.dbc");
    new TaxiPathNodeStore("DBC/TaxiPathNode.dbc");
    new ItemSetStore("DBC/ItemSet.dbc");
    new WorldSafeLocsStore("DBC/WorldSafeLocs.dbc");
    new TransportAnimationStore("DBC/TransportAnimation.dbc");
    new AuctionHouseStore("DBC/AuctionHouse.dbc");
    new NameGenStore("DBC/NameGen.dbc");
    new TalentStore("DBC/Talent.dbc");
    new CreatureSpellDataStore("DBC/CreatureSpellData.dbc");
    new CreatureFamilyStore("DBC/CreatureFamily.dbc");
    new CharClassStore("DBC/ChrClasses.dbc");
    new CharRaceStore("DBC/ChrRaces.dbc");
    new MapStore("DBC/Map.dbc");
    new ItemExtendedCostStore("DBC/ItemExtendedCost.dbc");

    /* Convert area table ids/flags */
    DBCFile area;
    area.open("DBC/AreaTable.dbc");
    uint32 flag_, area_, zone_;
    for(uint32 i = 0; i < area.getRecordCount(); ++i)
    {
        area_ = area.getRecord(i).getUInt(0);
        flag_ = area.getRecord(i).getUInt(3);
        zone_ = area.getRecord(i).getUInt(2);

        mAreaIDToTable[flag_] = sAreaStore.LookupEntry(area_);
        if(mZoneIDToTable.find(zone_) != mZoneIDToTable.end())
        {
            if(mZoneIDToTable[zone_]->AreaFlags != 312 &&
                mAreaIDToTable[flag_]->AreaFlags == 312)
            {
                // over ride.
                mZoneIDToTable[zone_] = mAreaIDToTable[flag_];
            }
        }
        else
        {
            mZoneIDToTable[zone_] = mAreaIDToTable[flag_];
        }
    }

    new ObjectMgr;
    new ChannelMgr;
    new QuestMgr;
    new LootMgr;
    new LfgMgr;
    new WeatherMgr;
    new TaxiMgr;
    new BattlegroundMgr;
    new AddonMgr;
    new SocialMgr;
    new WorldLog;

    // grep: this only has to be done once between version updates
    // to re-fill the table.

    /*sLog.outString("Filling spell replacements table...");
    FillSpellReplacementsTable();
    sLog.outString("");*/

#define MAKE_TASK(sp, ptr) tl.AddTask(new CallbackP0<sp>(sp::getSingletonPtr(), &sp::ptr))
    // Fill the task list with jobs to do.
    TaskList tl;
    MAKE_TASK(ObjectMgr, LoadPlayerCreateInfo);
    MAKE_TASK(ObjectMgr, LoadPlayersInfo);
    MAKE_TASK(ObjectMgr, LoadItemPrototypes);
    MAKE_TASK(ObjectMgr, LoadItemPages);
    MAKE_TASK(ObjectMgr, LoadCreatureNames);
    MAKE_TASK(ObjectMgr, LoadGameObjectNames);
    MAKE_TASK(ObjectMgr, LoadCreatureProtos);
    MAKE_TASK(ObjectMgr, LoadCreatureWaypoints);
    MAKE_TASK(ObjectMgr, LoadTrainers);
    MAKE_TASK(ObjectMgr, LoadTotemSpells);
    MAKE_TASK(ObjectMgr, LoadSpellExtraData);
    MAKE_TASK(ObjectMgr, LoadSpellSkills);
    MAKE_TASK(ObjectMgr, LoadSpellFixes);
    MAKE_TASK(ObjectMgr, LoadSpellOverride);
    MAKE_TASK(ObjectMgr, LoadGossipText);
    MAKE_TASK(ObjectMgr, LoadVendors);
    MAKE_TASK(ObjectMgr, LoadAIThreatToSpellId);
    MAKE_TASK(ObjectMgr, LoadDefaultPetSpells);
    MAKE_TASK(ObjectMgr, LoadPetSpellCooldowns);
    MAKE_TASK(ObjectMgr, LoadPvPAreas);
    MAKE_TASK(World, LoadAreaTriggerInformation);
    MAKE_TASK(World, LoadMapInformation);
    MAKE_TASK(ObjectMgr, LoadFishingZones);
    MAKE_TASK(ObjectMgr, LoadTeleportCoords);
    MAKE_TASK(ObjectMgr, LoadGuildCharters);
    MAKE_TASK(ObjectMgr, LoadGraveyards);
    MAKE_TASK(ObjectMgr, LoadGMTickets);
    MAKE_TASK(SocialMgr, LoadFromDB);
    MAKE_TASK(AddonMgr, LoadFromDB);
    MAKE_TASK(ObjectMgr, GenerateLevelUpInfo);
    MAKE_TASK(ObjectMgr, SetHighestGuids);
    MAKE_TASK(QuestMgr, LoadSQLQuests);
    MAKE_TASK(ObjectMgr, LoadReputationModifiers);

#undef MAKE_TASK

    // spawn worker threads (2 * number of cpus)
    tl.spawn();

    // wait for all loading to complete.
    tl.wait();

    sLog.outString("");
    sLog.outString("Database loaded in %ums.", getMSTime() - start_time);
    sLog.outString("");

    // calling this puts all maps into our task list.
    new WorldCreator(&tl);

    // wait for the events to complete.
    tl.wait();

    // wait for them to exit, now.
    tl.kill();
    tl.waitForThreadsToExit();
    sLog.outString("");

    new InstanceSavingManagement;
    sInstanceSavingManager.LoadSavedInstances();
    //Load Corpses
    objmgr.CorpseCollectorLoad();
    
    //Updating spell.dbc--this is slow like hell due to we cant read string fields
    //dbc method will be changed in future
    sLog.outString("Processing Spells...");
    DBCFile dbc;

    dbc.open("DBC/Spell.dbc");
    uint32 cnt = dbc.getRecordCount();
    uint32 effect;
    for(uint32 x=0; x < cnt; x++)
    {
        uint32 result = 0;
        // SpellID
        uint32 spellid = dbc.getRecord(x).getUInt(0);
        // Description field
        const char* desc = dbc.getRecord(x).getString(141); 
        const char* ranktext = dbc.getRecord(x).getString(132);
        const char* nametext = dbc.getRecord(x).getString(123);

        uint32 rank = 0;
        uint32 type = 0;
        uint32 namehash = 0;

        // get spellentry
        SpellEntry * sp = sSpellStore.LookupEntry(spellid);

        // grep: this is a _very_ hacky fix for presence of mind and alike spells.
        if(sp->procFlags == 0x00015550 && sp->procCharges == 1 && sp->procChance == 100)
            sp->AuraInterruptFlags |= AURA_INTERRUPT_ON_CAST_SPELL;

        // parse rank text
        if(!sscanf(ranktext, "Rank %d", &rank))
            rank = 0;

        // hash the name
        namehash = crc32((const unsigned char*)nametext, strlen(nametext));

        // look for seal, etc in name
        if(strstr(nametext, "Seal"))
            type |= SPELL_TYPE_SEAL;

        if(strstr(nametext, "Blessing"))
            type |= SPELL_TYPE_BLESSING;

        if(strstr(nametext, "Curse"))
            type |= SPELL_TYPE_CURSE;

        if(strstr(nametext, "Aspect"))
            type |= SPELL_TYPE_ASPECT;

        if(strstr(nametext, "Sting") || strstr(nametext, "sting"))
            type |= SPELL_TYPE_STING;

        // don't break armor items!
        if(strcmp(nametext, "Armor") && strstr(nametext, "Armor") || strstr(nametext, "Demon Skin"))
            type |= SPELL_TYPE_ARMOR;

        if(strstr(nametext, "Aura"))
            type |= SPELL_TYPE_AURA;

        /*FILE * f = fopen("C:\\spells.txt", "a");
        fprintf(f, "case 0x%08X:        // %s\n", namehash, nametext);
        fclose(f);*/

        // find diminishing status
        sp->DiminishStatus = GetDiminishingGroup(namehash);

        // HACK FIX: Break roots/fear on damage.. this needs to be fixed properly!
        if(!(sp->AuraInterruptFlags & AURA_INTERRUPT_ON_ANY_DAMAGE_TAKEN))
        {
            for(uint32 z = 0; z < 3; ++z) {
                if(sp->EffectApplyAuraName[z] == SPELL_AURA_MOD_FEAR ||
                    sp->EffectApplyAuraName[z] == SPELL_AURA_MOD_ROOT)
                {
                    sp->AuraInterruptFlags |= AURA_INTERRUPT_ON_UNUSED2;
                    break;
                }
            }
        }

        // set extra properties
        sp->buffType   = type;
        sp->NameHash   = namehash;
        sp->RankNumber = rank;

        uint32 pr=sp->procFlags;
        for(uint32 y=0;y < 3; y++)
        {
            // get the effect number from the spell
            effect = dbc.getRecord(x).getUInt(64 + y); // spelleffect[0] = 64 // 2.0.1

            //spell group
            /*if(effect==SPELL_EFFECT_SUMMON_TOTEM_SLOT1||effect==SPELL_EFFECT_SUMMON_TOTEM_SLOT2||
                effect==SPELL_EFFECT_SUMMON_TOTEM_SLOT3||effect==SPELL_EFFECT_SUMMON_TOTEM_SLOT4)
            {
            
                    const char *p=desc;
                    while(p=strstr(p,"$"))
                    {
                        p++;
                        //got $  -> check if spell
                        if(*p>='0' && *p <='9')
                        {//woot this is spell id
                            uint32 tmp=atoi(p);
                            SpellEntry*s=sSpellStore.LookupEntry(tmp);
                            bool ch=false;
                            for(uint32 i=0;i<3;i++)
                                if(s->EffectTriggerSpell[i])
                                {
                                    ch=true;
                                    result=tmp;
                                    break;
                                }
                            if(ch)break;
                            result=tmp;
                            
                        }
                    
                    }
                
            }else*/
            /*if(effect==SPELL_EFFECT_ENCHANT_ITEM)//add inventory type check
            {
                result=0;
                //136--desc field
                //dirty code
                if(strstr(desc,"head"))
                    result|=(1<<INVTYPE_HEAD);
                if(strstr(desc,"leg"))
                    result|=(1<<INVTYPE_LEGS);
                if(strstr(desc,"neck"))
                    result|=(1<<INVTYPE_NECK);
                if(strstr(desc,"shoulder"))
                    result|=(1<<INVTYPE_SHOULDERS);
                if(strstr(desc,"body"))
                    result|=(1<<INVTYPE_BODY);
                if(strstr(desc,"chest"))
                    result|=((1<<INVTYPE_CHEST)|(1<<INVTYPE_ROBE));
                if(strstr(desc,"waist"))
                    result|=(1<<INVTYPE_WAIST);
                if(strstr(desc,"foot")||strstr(desc,"feet")||strstr(desc,"boot"))
                    result|=(1<<INVTYPE_FEET);
                if(strstr(desc,"wrist")||strstr(desc,"bracer"))
                    result|=(1<<INVTYPE_WRISTS);
                if(strstr(desc,"hand")||strstr(desc,"glove"))
                    result|=(1<<INVTYPE_HANDS);
                if(strstr(desc,"finger")||strstr(desc,"ring"))
                    result|=(1<<INVTYPE_FINGER);
                if(strstr(desc,"trinket"))
                    result|=(1<<INVTYPE_TRINKET);
                if(strstr(desc,"shield"))
                    result|=(1<<INVTYPE_SHIELD);
                if(strstr(desc,"cloak"))
                    result|=(1<<INVTYPE_CLOAK);
                if(strstr(desc,"robe"))
                    result|=(1<<INVTYPE_ROBE);
                //if(strstr(desc,"two")||strstr(desc,"Two"))
                //    result|=(1<<INVTYPE_2HWEAPON);<-handled in subclass
            }
            else*/
            if(effect==SPELL_EFFECT_APPLY_AURA)
            {
                uint32 aura = dbc.getRecord(x).getUInt(94+y); // 58+30+3 = 91
                if(aura == SPELL_AURA_PROC_TRIGGER_SPELL)//search for spellid in description
                {
                    const char *p=desc;
                    while(p=strstr(p,"$"))
                    {
                        p++;
                        //got $  -> check if spell
                        if(*p>='0' && *p <='9')
                        {//woot this is spell id
                        
                            result=atoi(p);
                        }                    
                    }
                    pr=0;
                    //dirty code for procs, if any1 got any better idea-> u are welcome
                    //139944 --- some magic number, it will trigger on all hits etc
                        //for seems to be smth like custom check
                    if(strstr(desc,"takes damage"))
                        pr|=PROC_ON_ANY_DAMAGE_VICTIM;
                    if(strstr(desc,"attackers when hit"))
                        pr|=PROC_ON_MELEE_ATTACK_VICTIM;
                    if(strstr(desc,"character strikes an enemy"))
                        pr|=PROC_ON_MELEE_ATTACK;
                    if(strstr(desc,"strike you with a melee attack"))
                        pr|=PROC_ON_MELEE_ATTACK_VICTIM;
                    if(strstr(desc,"target casts a spell"))
                        pr|=PROC_ON_CAST_SPELL;
                    if(strstr(desc,"Any damage caused"))
                        pr|=PROC_ON_ANY_DAMAGE_VICTIM;
                    if(strstr(desc,"The next melee attack against the caster"))
                        pr|=PROC_ON_MELEE_ATTACK_VICTIM;
                    if(strstr(desc,"when successfully hit"))
                        pr|=PROC_ON_MELEE_ATTACK ;
                    if(strstr(desc,"an enemy on hit"))
                        pr|=PROC_ON_MELEE_ATTACK;
                    if(strstr(desc,"when it hits"))
                        pr|=PROC_ON_MELEE_ATTACK;
                    if(strstr(desc,"when successfully hit"))
                        pr|=PROC_ON_MELEE_ATTACK;
                    if(strstr(desc,"on a successful hit"))
                        pr|=PROC_ON_MELEE_ATTACK;
                    if(strstr(desc,"damage to attacker on hit"))
                        pr|=PROC_ON_MELEE_ATTACK_VICTIM;
                    if(strstr(desc,"on a hit"))
                        pr|=PROC_ON_MELEE_ATTACK;
                    if(strstr(desc,"strikes you with a melee attack"))
                        pr|=PROC_ON_MELEE_ATTACK_VICTIM;
                    if(strstr(desc,"when caster takes damage"))
                        pr|=PROC_ON_ANY_DAMAGE_VICTIM;
                    if(strstr(desc,"when the caster is using melee attacks"))
                        pr|=PROC_ON_MELEE_ATTACK;
                    if(strstr(desc,"When struck in combat"))
                        pr|=PROC_ON_MELEE_ATTACK_VICTIM;
                    if(strstr(desc,"on a successful melee attack"))
                        pr|=PROC_ON_MELEE_ATTACK;
                    if(strstr(desc,"chance per attack"))
                        pr|=PROC_ON_MELEE_ATTACK;
                    if(strstr(desc,"chance per hit"))
                        pr|=PROC_ON_MELEE_ATTACK;
                    if(strstr(desc,"that strikes a party member"))
                        pr|=PROC_ON_MELEE_ATTACK_VICTIM;
                    if(strstr(desc,"when hit by a melee attack"))
                        pr|=PROC_ON_MELEE_ATTACK_VICTIM;
                    if(strstr(desc,"Your critical strikes"))
                        pr|=PROC_ON_CRIT_ATTACK;
                    if(strstr(desc,"whenever you deal ranged damage"))
                        pr|=PROC_ON_RANGED_ATTACK;
                    if(strstr(desc,"whenever you deal melee damage"))
                        pr|=PROC_ON_MELEE_ATTACK;
                    if(strstr(desc,"When struck in melee combat"))
                        pr|=PROC_ON_MELEE_ATTACK_VICTIM;
                    if(strstr(desc,"Any successful spell cast against the priest"))
                        pr|=PROC_ON_SPELL_HIT_VICTIM;
                    if(strstr(desc,"The next melee attack on the caster"))
                        pr|=PROC_ON_MELEE_ATTACK_VICTIM;
                    if(strstr(desc,"striking melee or ranged attackers"))
                        pr|=PROC_ON_MELEE_ATTACK_VICTIM|PROC_ON_RANGED_ATTACK_VICTIM;
                    if(strstr(desc,"when damaging an enemy in melee"))
                        pr|=PROC_ON_MELEE_ATTACK;
                    if(strstr(desc,"after being the victim of a critical strike"))
                        pr|=PROC_ON_CRIT_HIT_VICTIM;
                    if(strstr(desc,"On successful melee or ranged attack"))
                        pr|=PROC_ON_MELEE_ATTACK|PROC_ON_RANGED_ATTACK;
                    if(strstr(desc,"enemy that strikes you in melee"))
                        pr|=PROC_ON_MELEE_ATTACK_VICTIM;
                    if(strstr(desc,"when struck in combat"))
                        pr|=PROC_ON_MELEE_ATTACK_VICTIM;
                    if(strstr(desc,"after getting a critical strike"))
                        pr|=PROC_ON_CRIT_ATTACK;
                    if(strstr(desc,"whenever damage is dealt to you"))
                        pr|=PROC_ON_ANY_DAMAGE_VICTIM;
                    if(strstr(desc,"when ranged or melee damage is dealt"))
                        pr|=PROC_ON_MELEE_ATTACK|PROC_ON_RANGED_ATTACK;
                    if(strstr(desc,"damaging melee attacks"))
                        pr|=PROC_ON_MELEE_ATTACK;
                    if(strstr(desc,"on melee or ranged attack"))
                        pr|=PROC_ON_MELEE_ATTACK|PROC_ON_RANGED_ATTACK;
                    if(strstr(desc,"on a melee swing"))
                        pr|=PROC_ON_MELEE_ATTACK;
                    if(strstr(desc,"Chance on melee"))
                        pr|=PROC_ON_MELEE_ATTACK;
                    if(strstr(desc,"spell criticals against you"))
                        pr|=PROC_ON_SPELL_CRIT_HIT_VICTIM;
                    if(strstr(desc,"After being struck by a melee or ranged critical hit"))
                        pr|=PROC_ON_CRIT_HIT_VICTIM;
                    if(strstr(desc,"on a critical hit"))
                        pr|=PROC_ON_CRIT_ATTACK;
                    if(strstr(desc,"strikes the caster"))
                        pr|=PROC_ON_MELEE_ATTACK_VICTIM;
                    if(strstr(desc,"a spell, melee or ranged attack hits the caster"))
                        pr|=PROC_ON_ANY_DAMAGE_VICTIM;
                    if(strstr(desc,"after dealing a critical strike"))
                        pr|=PROC_ON_CRIT_ATTACK;
                    if(strstr(desc,"Each melee or ranged damage hit against the priest"))
                        pr|=PROC_ON_MELEE_ATTACK_VICTIM|PROC_ON_RANGED_ATTACK_VICTIM;                
                    if(strstr(desc, "a chance to deal additional"))
                        pr|=PROC_ON_MELEE_ATTACK;
                    if(strstr(desc, "Gives your"))
                        pr|=PROC_ON_CAST_SPECIFIC_SPELL;
                }
            }            
        }

        sp->procFlags=pr;
        //sp->dummy=result;
    }

	//fix for Predatory Strikes
	uint32 mm=(1<<(FORM_BEAR-1))|(1<<(FORM_DIREBEAR-1))|(1<<(FORM_MOONKIN-1))|(1<<(FORM_CAT-1));
	sSpellStore.LookupEntry(16972)->RequiredShapeShift = mm;
	sSpellStore.LookupEntry(16974)->RequiredShapeShift = mm;
	sSpellStore.LookupEntry(16975)->RequiredShapeShift = mm;
	
	sLog.outString( "Creating initial battlegrounds..." );
    sLog.outString("");
    sBattlegroundMgr.CreateInitialBattlegrounds();

    sLog.outString( "Starting Transport System...");
    objmgr.LoadTransporters();

    // start mail system
    new MailSystem;
    MailSystem::getSingleton().StartMailSystem();

    sLog.outString("");
    sLog.outString("  Starting Auction System...");
    new AuctionMgr;
    sAuctionMgr.LoadAuctionHouses();


    HonorHandler::PerformStartupTasks();

    TimeOut= uint32(1000* Config.MainConfig.GetIntDefault("ConnectionTimeout", 180) );
    m_queueUpdateTimer = mQueueUpdateInterval;
    
    
    if(Config.MainConfig.GetBoolDefault("MapMgr.Preloading", false))
    {
        // Load all data on each map.

        sWorldCreator.GetInstance(0, uint32(0))->LoadAllCells();
        sWorldCreator.GetInstance(1, uint32(0))->LoadAllCells();
        sWorldCreator.GetInstance(530, uint32(0))->LoadAllCells();
    }

	launch_thread(new WorldRunnable);
    if(Config.MainConfig.GetBoolDefault("Startup.BackgroundLootLoading", true))
    {
        sLog.outString("Backgrounding loot loading...");

        // loot background loading in a lower priority thread.
        launch_thread(new BasicTaskExecutor(new CallbackP0<LootMgr>(LootMgr::getSingletonPtr(), &LootMgr::LoadLoot), 
            BTE_PRIORITY_LOW));
    }
    else
    {
        sLog.outString("Loading loot in foreground...");
        lootmgr.LoadLoot();
    }
}


void World::Update(time_t diff)
{
    eventholder->Update(diff);
    sAuctionMgr.Update();
    _UpdateGameTime();
}


void World::SendGlobalMessage(WorldPacket *packet, WorldSession *self)
{
    SessionMap::iterator itr;
    for (itr = m_sessions.begin(); itr != m_sessions.end(); itr++)
    {
        if (itr->second->GetPlayer() &&
            itr->second->GetPlayer()->IsInWorld()
            && itr->second != self)  // dont send to self!
        {
            itr->second->SendPacket(packet);
        }
    }
}

void World::SendZoneMessage(WorldPacket *packet, uint32 zoneid, WorldSession *self)
{
    SessionMap::iterator itr;
    for (itr = m_sessions.begin(); itr != m_sessions.end(); itr++)
    {
        if (itr->second->GetPlayer() &&
            itr->second->GetPlayer()->IsInWorld()
            && itr->second != self)  // dont send to self!
        {
            if (itr->second->GetPlayer()->GetZoneId() == zoneid)
                itr->second->SendPacket(packet);
        }
    }
}

void World::SendWorldText(const char* text, WorldSession *self)
{
    WorldPacket data;

    uint32 textLen = strlen((char*)text) + 1;

    data.Initialize(SMSG_MESSAGECHAT);
    data << uint8(CHAT_MSG_SYSTEM);
    data << uint32(LANG_UNIVERSAL);
    data << uint64(0);
    data << textLen;
    data << text;
    data << uint8(0);

    SendGlobalMessage(&data, self);

    sLog.outString("> %s", text);
}

void World::SendWorldWideScreenText(const char *text, WorldSession *self)
{
    WorldPacket data;
    data.Initialize(SMSG_AREA_TRIGGER_MESSAGE);
    data << (uint32)0 << text << (uint8)0x00;
    SendGlobalMessage(&data, self);
}

void World::UpdateSessions(uint32 diff)
{
    SessionSet::iterator itr, it2;
    WorldSession *session;
    int result;
    for(itr = Sessions.begin(); itr != Sessions.end();)
    {
        session = (*itr);
        it2 = itr;
        ++itr;
        if(!session || session->GetInstance() != 0)
        {
            Sessions.erase(it2);
            continue;
        }

        if(result = session->Update(0))
        {
            if(result == 1)
            {
                // complete deletion
                DeleteSession(session);
            }
            Sessions.erase(it2);
        }
    }
}

void World::LoadMapInformation()
{
    sLog.outString("  Loading Map Information...");
    MapInfo *mapinfo;
    QueryResult *result = sDatabase.Query( "SELECT * FROM worldmap_info" );
    if( !result )
    {
        sLog.outString( "  0 Maps Info Loaded." );
        return;
    }
    do
    {
        Field *fields = result->Fetch();
        mapinfo = new MapInfo;
        mapinfo->mapid = fields[0].GetUInt32();
        mapinfo->screenid = fields[1].GetUInt32();
        mapinfo->type = fields[2].GetUInt32();
        mapinfo->playerlimit = fields[3].GetUInt32();
        mapinfo->minlevel = fields[4].GetUInt32();
        mapinfo->repopx = fields[5].GetFloat();
        mapinfo->repopy = fields[6].GetFloat();
        mapinfo->repopz = fields[7].GetFloat();
        mapinfo->repopmapid = fields[8].GetUInt32();
        mapinfo->name = fields[9].GetString();
        mapinfo->flags =  fields[10].GetUInt32();
        mapinfo->cooldown =  fields[11].GetUInt32();

        AddMapInformation(mapinfo);

    }while( result->NextRow() );
    delete result;
}

void World::AddMapInformation(MapInfo *mapinfo)
{
    ASSERT( mapinfo );
    ASSERT( m_mapinfo.find(mapinfo->mapid) == m_mapinfo.end() );
    m_mapinfo[mapinfo->mapid] = mapinfo;
}

MapInfo *World::GetMapInformation(uint32 mapid)
{
    MapInfoMap::iterator iter, end;
    for( iter = m_mapinfo.begin(), end = m_mapinfo.end(); iter != end; iter++ )
    {
        MapInfo *mapinfo= iter->second;
        if(mapinfo->mapid == mapid)
            return mapinfo;
    }
    return NULL;

}

std::string World::GenerateName(uint32 type)
{
    uint32 maxval = NameGenStore::getSingleton().GetNumRows();
    uint32 entry = rand()%(maxval+1); // we don't want 1..
    NameGenEntry* ne = NameGenStore::getSingleton().LookupEntry(entry);
    if(!ne)
    {
        sLog.outError("ERROR: Couldn't find NameGenStore Entry!");
        return "ERR";
    }
    const char *name = NameGenStore::getSingleton().LookupString((const uint32)ne->offsetindex);
    if(name)
        return name;
    else
        return "ERR";
}




void World::DeleteSession(WorldSession *session)
{
    // remove from big map
    m_sessions.erase(session->GetAccountId());
    // delete us
    delete session;
}

uint32 World::GetNonGmSessionCount()
{
    uint32 total = m_sessions.size();

    SessionMap::const_iterator itr = m_sessions.begin();
    for( ; itr != m_sessions.end(); itr++ )
    {
        if( (itr->second)->HasGMPermissions() )
            total--;
    }

    return total;
}

uint32 World::AddQueuedSocket(WorldSocket* Socket)
{
    // Since we have multiple socket threads, better guard for this one,
    // we don't want heap corruption ;)
    queueMutex.Acquire();

    // Add socket to list
    mQueuedSessions.push_back(Socket);
    queueMutex.Release();
    // Return queue position
    return mQueuedSessions.size();
}

void World::RemoveQueuedSocket(WorldSocket* Socket)
{
    // Since we have multiple socket threads, better guard for this one,
    // we don't want heap corruption ;)
    queueMutex.Acquire();

    // Find socket in list
    QueueSet::iterator iter = mQueuedSessions.begin();
    for(; iter != mQueuedSessions.end(); ++iter)
    {
        if((*iter) == Socket)
        {
            // Remove from the queue and abort.
            // This will be slow (Removing from middle of a vector!) but it won't
            // get called very much, so it's not really a big deal.

            mQueuedSessions.erase(iter);
            queueMutex.Release();
            return;
        }
    }
    queueMutex.Release();
}

uint32 World::GetQueuePos(WorldSocket* Socket)
{
    // Since we have multiple socket threads, better guard for this one,
    // we don't want heap corruption ;)
    queueMutex.Acquire();

    // Find socket in list
    QueueSet::iterator iter = mQueuedSessions.begin();
    uint32 QueuePos = 1;
    for(; iter != mQueuedSessions.end(); ++iter, ++QueuePos)
    {
        if((*iter) == Socket)
        {
            queueMutex.Release();
            // Return our queue position.
            return QueuePos;
        }
    }
    queueMutex.Release();
    // We shouldn't get here..
    return 1;
}

void World::UpdateQueuedSessions(uint32 diff)
{
    if(diff >= m_queueUpdateTimer) 
    {
        m_queueUpdateTimer = mQueueUpdateInterval;
        queueMutex.Acquire();

        if(mQueuedSessions.size() == 0)
        {
            queueMutex.Release();
            return;
        }
        
        if(m_sessions.size() < m_playerLimit)
        {
            // Yay. We can let another player in now.
            // Grab the first fucker from the queue, but guard of course, since
            // this is in a different thread again.

            QueueSet::iterator iter = mQueuedSessions.begin();
            WorldSocket * QueuedSocket = *iter;
            mQueuedSessions.erase(iter);

            // Welcome, sucker.
            if(QueuedSocket->GetSession())
            {
                QueuedSocket->GetSession()->deleteMutex.Acquire();
                QueuedSocket->Authenticate();
            }
        }

        if(mQueuedSessions.size() == 0)
        {
            queueMutex.Release();
            return;
        }

        // Update the remaining queue members.
        QueueSet::iterator iter = mQueuedSessions.begin();
        uint32 Position = 1;
        for(; iter != mQueuedSessions.end(); ++iter, ++Position)
        {
            (*iter)->UpdateQueuePosition(Position);
        }
        queueMutex.Release();
    } 
    else 
    {
        m_queueUpdateTimer -= diff;
    }
}

void World::SaveAllPlayers()
{
    if(!(ObjectMgr::getSingletonPtr() && WorldCreator::getSingletonPtr()))
        return;

    sLog.outString("Saving all players to database...");
    uint32 count = 0;
    HM_NAMESPACE::hash_map<uint32, Player*>::const_iterator itr;
        // Servers started and obviously runing. lets save all players.
    uint32 mt;
	objmgr._playerslock.AcquireReadLock();   
	for (itr = objmgr._players.begin(); itr != objmgr._players.end(); itr++)
        {
            if(itr->second->GetSession())
            {
                mt = getMSTime();
                itr->second->SaveToDB(false);
                sLog.outString("Saved player `%s` (level %u) in %ums.", itr->second->GetName(), itr->second->GetUInt32Value(UNIT_FIELD_LEVEL), getMSTime() - mt);
                ++count;
            }
        }
    objmgr._playerslock.ReleaseReadLock();
    sLog.outString("Saved %u players.", count);
}


WorldSession* World::FindSessionByName(const char * Name)//case insensetive
{
    // loop sessions, see if we can find him
    SessionMap::iterator itr = m_sessions.begin();
    for(; itr != m_sessions.end(); ++itr)
    {
      if(!stricmp(itr->second->GetAccountName().c_str(),Name))
            return itr->second;
    }
    return 0;
}

void World::BroadcastExtendedMessage(WorldSession * self, const char* str, ...)
{
    va_list ap;
    va_start(ap, str);
    char msg[1024];
    vsprintf(msg, str, ap);
    va_end(ap);
    SessionSet::iterator itr = mExtendedSessions.begin();
    WorldSession * s;
    for(; itr != mExtendedSessions.end(); )
    {
        s = *itr;
        ++itr;

        if(s->GetPlayer() /*&& s != this*/)
            s->GetPlayer()->BroadcastMessage(msg);
    }
}

void World::ShutdownClasses()
{
    sThreadMgr.Shutdown();

    sLog.outString("Deleting Addon Manager...");
    sAddonMgr.SaveToDB();
    delete AddonMgr::getSingletonPtr();

    sLog.outString("\nDeleting Auction Manager...");
    delete AuctionMgr::getSingletonPtr();
    sLog.outString("Deleting Loot Manager...");
    delete LootMgr::getSingletonPtr();

    sMailSystem.ShutdownMailSystem();
    delete MailSystem::getSingletonPtr();
    delete WorldCreator::getSingletonPtr();
    delete ScriptMgr::getSingletonPtr();
}

void World::EventDeleteBattleground(Battleground * BG)
{
    // remove the instance
    //sWorldCreator.DestroyBattlegroundInstance(BG);
    //sBattlegroundMgr.RemoveBattleground(BG->GetID());
}

void World::GetStats(uint32 * GMCount, float * AverageLatency)
{
    int gm = 0;
    int count = 0;
    int avg = 0;
    HM_NAMESPACE::hash_map<uint32, Player*>::const_iterator itr;
	objmgr._playerslock.AcquireReadLock();
    for (itr = objmgr._players.begin(); itr != objmgr._players.end(); itr++)
    {
        if(itr->second->GetSession())
        {
            count++;
            avg += itr->second->GetSession()->GetLatency();
            if(itr->second->GetSession()->GetPermissionCount())
                gm++;
        }            
    }
	objmgr._playerslock.ReleaseReadLock();

    *AverageLatency = count ? (float)((float)avg / (float)count) : 0;
    *GMCount = gm;
}

void TaskList::AddTask(CallbackBase * task)
{
    queueLock.Acquire();
    tasks.push(task);
    queueLock.Release();
}

CallbackBase * TaskList::GetTask()
{
    queueLock.Acquire();

    if(!tasks.size())
    {
        queueLock.Release();
        return 0;
    }

    CallbackBase * t = tasks.front();
    tasks.pop();
    queueLock.Release();
    return t;
}

void TaskList::spawn()
{
    running = true;
    thread_count = 0;

    uint32 threadcount;
    if(Config.MainConfig.GetBoolDefault("Startup.EnableMultithreadedLoading", true))
    {
        // get processor count
#ifndef WIN32
#if UNIX_FLAVOUR == UNIX_FLAVOUR_LINUX
        long affmask;
        sched_getaffinity(0, 4, (cpu_set_t*)&affmask);
        threadcount = (BitCount8(affmask)) * 2;
        if(threadcount > 8) threadcount = 8;
        else if(threadcount <= 0) threadcount = 1;
#else
        threadcount = 2;
#endif
#else
        SYSTEM_INFO s;
        GetSystemInfo(&s);
        threadcount = s.dwNumberOfProcessors * 2;
#endif
    }
    else
        threadcount = 1;

    sLog.outString("\n  Beginning %s server startup with %u threads.\n", (threadcount == 1) ? "progressive" : "parallel", threadcount);
    for(uint32 x = 0; x < threadcount; ++x)
        launch_thread(new TaskExecutor(this));
}

void TaskList::wait()
{
    bool has_tasks = true;
    while(has_tasks)
    {
        queueLock.Acquire();
        has_tasks = (tasks.size() > 0) ? true : false;
        queueLock.Release();
        Sleep(20);
    }
}

void TaskList::kill()
{
    running = false;
}

void TaskExecutor::run()
{
    CallbackBase * t;
    while(starter->running)
    {
        t = starter->GetTask();
        if(t)
        {
            t->execute();
            delete t;
        }
        else
            Sleep(20);
    }
}

void TaskList::waitForThreadsToExit()
{
    while(thread_count)
    {
        Sleep(20);
    }
}

void World::DeleteObject(Object * obj)
{
    switch(obj->GetTypeId())
    {
    case TYPEID_PLAYER:
        delete ((Player*)obj);
        break;
        
    case TYPEID_CORPSE:
        delete ((Corpse*)obj);
        break;

    case TYPEID_UNIT:
        if(obj->GetGUIDHigh() == HIGHGUID_PET)
            delete ((Pet*)obj);
        else
            delete ((Creature*)obj);
        break;

    case TYPEID_GAMEOBJECT:
        delete ((GameObject*)obj);
        break;
    }
}
