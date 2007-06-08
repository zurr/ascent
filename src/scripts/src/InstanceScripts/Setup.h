#include <string>
#ifndef INSTANCE_SCRIPTS_SETUP_H
#define INSTANCE_SCRIPTS_SETUP_H

void SetupShadowfangKeep(ScriptMgr * mgr);
void SetupDeadmines(ScriptMgr * mgr);
void SetupMoltenCore(ScriptMgr * mgr);
void SetupOnyxiaAI(ScriptMgr * mgr);
void SetupZulGurub(ScriptMgr * mgr);
void SetupBotanica(ScriptMgr * mgr);
void SetupBlackwingLair(ScriptMgr * mgr);
void SetupNaxxramas(ScriptMgr * mgr);
void SetupSteamVaults(ScriptMgr * mgr);
void SetupTheEye(ScriptMgr * mgr);
void SetupManaTombs(ScriptMgr * mgr);
void SetupAuchenaiCrypts(ScriptMgr * mgr);

struct SP_AI_Spell{
	SpellEntry *info; // spell info
	char targettype; // 0-self , 1-attaking target, ....
	bool instant; // does it is instant or not?
	float perctrigger; // % of the cast of this spell in a total of 100% of the attacks
	int attackstoptimer; // stop the creature from attacking
	int soundid; // sound id in DBC
	std::string speech; // text displaied when spell was casted
};

enum
{
	TARGET_SELF,
	TARGET_VARIOUS,
	TARGET_ATTACKING,
	TARGET_DESTINATION,
	TARGET_SOURCE,

	//.....add
};

#endif
