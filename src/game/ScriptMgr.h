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

#ifndef SCRIPTMGR_H
#define SCRIPTMGR_H

#define SCRIPTLIB_VERSION_MAJOR 3
#define SCRIPTLIB_VERSION_MINOR 0
#define MAKE_SCRIPT_VERSION(major, minor) (uint32)(((uint16)major << 16) | ((uint16)minor))
#define SCRIPT_MODULE void*
#define ADD_CREATURE_FACTORY_FUNCTION(cl) static CreatureAIScript * Create(Creature * c) { return new cl(c); }
#define ADD_GOSSIP_FACTORY_FUNCTION(cl) static GossipScript * Create() { return new cl; }

class Spell;
class Aura;
class Creature;
class CreatureAIScript;
class GossipScript;
class GameObjectAIScript;
class ScriptMgr;
struct ItemPrototype;
class QuestLogEntry;

/* Factory Imports (from script lib) */
typedef CreatureAIScript*(*exp_create_creature_ai)(Creature * pCreature);
typedef GameObjectAIScript*(*exp_create_gameobject_ai)(GameObject * pGameObject);
typedef GossipScript*(*exp_create_gossip_script)();
typedef bool(*exp_handle_dummy_spell)(uint32 i, Spell * pSpell);
typedef bool(*exp_handle_dummy_aura)(uint32 i, Aura * pAura, bool apply);
typedef void(*exp_script_register)(ScriptMgr * mgr);

typedef uint32(*exp_get_version)();

/* Hashmap typedefs */
typedef HM_NAMESPACE::hash_map<uint32, exp_create_creature_ai> CreatureCreateMap;
typedef HM_NAMESPACE::hash_map<uint32, exp_create_gameobject_ai> GameObjectCreateMap;
typedef HM_NAMESPACE::hash_map<uint32, exp_create_gossip_script> GossipCreateMap;
typedef HM_NAMESPACE::hash_map<uint32, exp_handle_dummy_aura> HandleDummyAuraMap;
typedef HM_NAMESPACE::hash_map<uint32, exp_handle_dummy_spell> HandleDummySpellMap;
typedef list<SCRIPT_MODULE> LibraryHandleMap;

class SERVER_DECL ScriptMgr : public Singleton<ScriptMgr>
{
public:

    void LoadScripts();
    void UnloadScripts();

    CreatureAIScript * CreateAIScriptClassForEntry(Creature* pCreature);
    GameObjectAIScript * CreateAIScriptClassForGameObject(uint32 uEntryId, GameObject* pGameObject);

    bool CallScriptedDummySpell(uint32 uSpellId, uint32 i, Spell* pSpell);
    bool CallScriptedDummyAura( uint32 uSpellId, uint32 i, Aura* pAura, bool apply);

    GossipScript* GetGossipScript(uint32 uEntryId);

    void register_creature_script(uint32 entry, exp_create_creature_ai callback);
    void register_gameobject_script(uint32 entry, exp_create_gameobject_ai callback);
    void register_gossip_script(uint32 entry, exp_create_gossip_script callback);
    void register_dummy_aura(uint32 entry, exp_handle_dummy_aura callback);
    void register_dummy_spell(uint32 entry, exp_handle_dummy_spell callback);

protected:
    CreatureCreateMap _creatures;
    GameObjectCreateMap _gameobjects;
    GossipCreateMap _gossips;
    HandleDummyAuraMap _auras;
    HandleDummySpellMap _spells;
    LibraryHandleMap _handles;
};

class SERVER_DECL CreatureAIScript
{
public:
    CreatureAIScript(Creature* creature);
    virtual ~CreatureAIScript() {};

    virtual void OnCombatStart(Unit* mTarget) {}
    virtual void OnCombatStop(Unit* mTarget) {}
    virtual void OnDamageTaken(Unit* mAttacker, float fAmount) {}
    virtual void OnCastSpell(uint32 iSpellId) {}
    virtual void OnTargetParried(Unit* mTarget) {}
    virtual void OnTargetDodged(Unit* mTarget) {}
    virtual void OnTargetBlocked(Unit* mTarget, int32 iAmount) {}
    virtual void OnTargetCritHit(Unit* mTarget, float fAmount) {}
    virtual void OnTargetDied(Unit* mTarget) {}
    virtual void OnParried(Unit* mTarget) {}
    virtual void OnDodged(Unit* mTarget) {}
    virtual void OnBlocked(Unit* mTarget, int32 iAmount) {}
    virtual void OnCritHit(Unit* mTarget, float fAmount) {}
    virtual void OnHit(Unit* mTarget, float fAmount) {}
    virtual void OnDied(Unit *mKiller) {}
    virtual void OnAssistTargetDied(Unit* mAssistTarget) {}
    virtual void OnFear(Unit* mFeared, uint32 iSpellId) {}
    virtual void OnFlee(Unit* mFlee) {}
    virtual void OnCallForHelp() {}
    virtual void OnLoad() {}
    virtual void OnReachWP(uint32 iWaypointId, bool bForwards) {}
    virtual void OnLootTaken(Player* pPlayer, ItemPrototype *pItemPrototype) {}
    virtual void AIUpdate() {}

    void RegisterAIUpdateEvent(uint32 frequency);
    void RemoveAIUpdateEvent();

    virtual void Destroy() {}
    Creature* GetUnit() { return _unit; }

protected:
    Creature* _unit;
};

class SERVER_DECL GameObjectAIScript
{
public:
    GameObjectAIScript(GameObject* goinstance);
    virtual ~GameObjectAIScript() {}

    virtual void OnCreate() {}
    virtual void OnSpawn() {}
    virtual void OnDespawn() {}
    virtual void OnLootTaken(Player * pLooter, ItemPrototype *pItemInfo) {}
    virtual void OnActivate(Player * pPlayer) {}
    virtual void AIUpdate() {}
    virtual void Destroy() {}

    void RegisterAIUpdateEvent(uint32 frequency);

protected:

    GameObject* _gameobject;
};

class SERVER_DECL GossipScript
{
public:
    GossipScript();
    virtual ~GossipScript() {} 

    virtual void GossipHello(Creature* pCreature, Player* Plr, bool AutoSend);
    virtual void GossipSelectOption(Creature* pCreature, Player* Plr, uint32 Id, uint32 IntId);
    virtual void GossipEnd(Creature* pCreature, Player* Plr);
    virtual void Destroy();

    bool AutoCreated;
};

class SERVER_DECL QuestScript
{
public:
    QuestScript(QuestLogEntry* qle);
    virtual ~QuestScript() {};

    virtual void OnQuestStart(Player* mTarget) {}
    virtual void OnQuestComplete(Player* mTarget) {}
    virtual void OnQuestCancel(Player* mTarget) {}
    virtual void OnGameObjectActivate(uint32 entry, Player* mTarget) {}
    virtual void OnCreatureKill(uint32 entry, Player* mTarget) {}
    virtual void OnExploreArea(uint32 areaId, Player* mTarget) {}
    virtual void OnPlayerItemPickup(uint32 itemId, uint32 totalCount, Player* mTarget) {}
    virtual void EventUpdate() {};

    void RegisterQuestEvent(uint32 frequency);
    void RemoveQuestEvent();

    virtual void Destroy() {}

protected:
    QuestLogEntry *_qLogEntry;
};

#define sScriptMgr ScriptMgr::getSingleton()

#endif
