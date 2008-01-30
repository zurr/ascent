/*

Purpuse : make modifications in dbc file for ascent project to set values that are missing for spells in able to work
Warning : Output result is changed in order to obtain ascent specific data. Do not try to use or interpret data for anything else
Warning : SpellEntry + sql_translation_table + SPELL_ENTRY must have the same structure !!!

project status : not finished yet..not tested

*/

#include <stdio.h>
#include <tchar.h>
#include <stdlib.h>
#include <assert.h>
#include <conio.h>
#include <windows.h>

#include "../../../src/shared/Database/dbcfile.h"
#include "../../../src/shared/crc32.cpp"
#include "../../../src/game/SpellNameHashes.h"
#include "../../../dep/include/mysql/mysql.h"

#include "defines.h"

DBCFile dbc;

//make this after the main
void do_fixes(TCHAR *inf);
void dump_as_sql(TCHAR *inf);
void import_from_sql();

void print_usage()
{
	printf("Usage: -dofix spell_fix.exe inf=input_spell.dbc\n");
	printf("parameters: -h output this help message\n");
	printf("parameters: -dofix start making custom fixes\n");
	printf("parameters: -conv dump DBC as sql\n");
	printf("parameters: -imp import data from an sql table(experimental)\n");
	printf("parameters: inf= specify the input filename(no spaces)\n");
}

//Spell_Fixes.exe -dofix inf=spell.dbc
//Spell_Fixes.exe -conv inf=spell.dbc
void main(int argc, TCHAR* argv[])
{
	int dotask=0;
	TCHAR file_name[300];//store the input filename
	file_name[0]=0;

	//make sure it is morron proof. Ever met a guy clicking all the next buttons in an install kit without reading 1 line ? :P
	if(argc<=1)
	{
		printf("For safety mesures this program cannot be run without parameters. Type -h for help");
		print_usage();
		exit(1);
	}
	//maybe right now we do not have a lot of options but what the hack, let's make it fancy :)
	for (int i=1; i<argc; i++)
	{
		int arglen=(int)strlen(argv[i]);
		if (strnicmp(argv[i],"-h",2)==0) print_usage();
		if (strnicmp(argv[i],"-dofix",6)==0) dotask=1;
		if (strnicmp(argv[i],"-conv",5)==0) dotask=2;
		if (strnicmp(argv[i],"-imp",4)==0) dotask=3;
		if (strnicmp(argv[i],"inf=",4)==0) strcpy(file_name,argv[i]+4);
		
	}

	//we will work with this anyway. This whole tool is about this var :P
	if(strlen(file_name)<2)
	{
		printf("This tool is designed for some special purpuse and it needs an imput file \n");
		print_usage();
		exit(1);
	}

	dbc.open(file_name);

	if(!dbc.getFieldCount())
	{
		printf("error, could not open dbc file\n");
		exit(1);
	}
	else printf("Opened DBC with %u fields and %u rows\n",(int)dbc.getFieldCount(),(int)dbc.getRecordCount());

	switch(dotask)
	{
		case 1:		do_fixes(file_name);		break;
		case 2:		dump_as_sql(file_name);		break;
		case 3:		import_from_sql();			break;
		default:								break;
	}
//	getch();
}

__int64 getfilesize(TCHAR *infname)
{
	FILE *inf=fopen(infname,"r");
	if(!inf)
	{
		printf("error ocured while opening file\n");
		exit(1);
	}
	fpos_t len;
	fseek(inf,0,SEEK_END);
	fgetpos(inf,&len);
	fclose(inf);
	return len;
}

unsigned int get_spell_row(uint32 id)
{
	//replace this later with at least a binary search
	for(unsigned int j=0;j<dbc.getRecordCount();j++)
		if(dbc.getRecord(j).getUInt(SPELL_ENTRY_Id)==id)
			return j;
	return 0xFFFFFFFF;
}

/*
void change_spell_value(int spell_id,unsigned int field, unsigned int value)
{
	uint32 row=get_spell_row(spell_id);
	if(!row)return;
	dbc.getRecord(row).SetUInt(field,value);
}

void change_spell_value(int spell_id,unsigned int field, signed int value)
{
	uint32 row=get_spell_row(spell_id);
	if(!row)return;
	dbc.getRecord(row).SetInt(field,value);
}

void change_spell_value(int spell_id,unsigned int field, float value)
{
	uint32 row=get_spell_row(spell_id);
	if(!row)return;
	dbc.getRecord(row).SetFloat(field,value);
}*/

void assign_row_to_SpellEntry(SpellEntry **spe,uint32 row)
{
	*spe = (SpellEntry *)dbc.getRecord(row).getRowStart();
}

void assign_Spell_to_SpellEntry(SpellEntry **spe,uint32 spell_id)
{
	uint32 row=get_spell_row(spell_id);
	if(row==0xFFFFFFFF)
		*spe = NULL;
	else assign_row_to_SpellEntry(spe,row);
}

void do_fixes(TCHAR *inf)
{
	if(!dbc.getFieldCount())
	{
		printf("error, could not open dbc file\n");
		exit(1);
	}
//	else printf("Opened DBC with %u fields and %u rows\n",(int)dbc.getFieldCount(),(int)dbc.getRecordCount());

	uint32 cnt = (uint32)dbc.getRecordCount();
	uint32 effect;
	uint32 All_Seal_Groups_Combined = 0;
	SpellEntry * sp;

	for(uint32 x=0; x < cnt; x++)
	{
		uint32 result = 0;
		// SpellID
//		uint32 spellid = dbc.getRecord(x).getUInt(SPELL_ENTRY_Id);
		// get spellentry
//		assign_Spell_to_SpellEntry(&sp,spellid);
		assign_row_to_SpellEntry(&sp,x);
		// Description field
		char* desc = (char*)dbc.getRecord(x).getString(SPELL_ENTRY_Description); 
		const char* ranktext = dbc.getRecord(x).getString(SPELL_ENTRY_Rank);
		const char* nametext = dbc.getRecord(x).getString(SPELL_ENTRY_Name);

		uint32 rank = 0;
		uint32 type = 0;
		uint32 namehash = 0;

		// hash the name
		//!!!!!!! representing all strings on 32 bits is dangerous. There is a chance to get same hash for a lot of strings ;)
        namehash = crc32((const unsigned char*)nametext, (unsigned int)strlen(nametext));
		sp->NameHash   = namehash; //need these set before we start processing spells

		sp->proc_interval = 0;//trigger at each event
		sp->c_is_flags = 0;

		//there are some spells that change the "damage" value of 1 effect to another : devastate = bonus first then damage
		//this is a total bullshit so remove it when spell system supports effect overwriting
		for( uint32 col1_swap = 0; col1_swap < 2 ; col1_swap++ )
			for( uint32 col2_swap = col1_swap ; col2_swap < 3 ; col2_swap++ )
				if( sp->Effect[col1_swap] == SPELL_EFFECT_WEAPON_PERCENT_DAMAGE && sp->Effect[col2_swap] == SPELL_EFFECT_DUMMYMELEE )
				{
					uint32 temp;
					float ftemp;
					temp = sp->Effect[col1_swap];			sp->Effect[col1_swap] = sp->Effect[col2_swap] ;						sp->Effect[col2_swap] = temp;
					temp = sp->EffectDieSides[col1_swap];	sp->EffectDieSides[col1_swap] = sp->EffectDieSides[col2_swap] ;		sp->EffectDieSides[col2_swap] = temp;
					temp = sp->EffectBaseDice[col1_swap];	sp->EffectBaseDice[col1_swap] = sp->EffectBaseDice[col2_swap] ;		sp->EffectBaseDice[col2_swap] = temp;
					ftemp = sp->EffectDicePerLevel[col1_swap];			sp->EffectDicePerLevel[col1_swap] = sp->EffectDicePerLevel[col2_swap] ;				sp->EffectDicePerLevel[col2_swap] = ftemp;
					ftemp = sp->EffectRealPointsPerLevel[col1_swap];	sp->EffectRealPointsPerLevel[col1_swap] = sp->EffectRealPointsPerLevel[col2_swap] ;	sp->EffectRealPointsPerLevel[col2_swap] = ftemp;
					temp = sp->EffectBasePoints[col1_swap];	sp->EffectBasePoints[col1_swap] = sp->EffectBasePoints[col2_swap] ;	sp->EffectBasePoints[col2_swap] = temp;
					temp = sp->EffectMechanic[col1_swap];	sp->EffectMechanic[col1_swap] = sp->EffectMechanic[col2_swap] ;	sp->EffectMechanic[col2_swap] = temp;
					temp = sp->EffectImplicitTargetA[col1_swap];	sp->EffectImplicitTargetA[col1_swap] = sp->EffectImplicitTargetA[col2_swap] ;	sp->EffectImplicitTargetA[col2_swap] = temp;
					temp = sp->EffectImplicitTargetB[col1_swap];	sp->EffectImplicitTargetB[col1_swap] = sp->EffectImplicitTargetB[col2_swap] ;	sp->EffectImplicitTargetB[col2_swap] = temp;
					temp = sp->EffectRadiusIndex[col1_swap];	sp->EffectRadiusIndex[col1_swap] = sp->EffectRadiusIndex[col2_swap] ;	sp->EffectRadiusIndex[col2_swap] = temp;
					temp = sp->EffectApplyAuraName[col1_swap];	sp->EffectApplyAuraName[col1_swap] = sp->EffectApplyAuraName[col2_swap] ;	sp->EffectApplyAuraName[col2_swap] = temp;
					temp = sp->EffectAmplitude[col1_swap];		sp->EffectAmplitude[col1_swap] = sp->EffectAmplitude[col2_swap] ;	sp->EffectAmplitude[col2_swap] = temp;
					ftemp = sp->Effectunknown[col1_swap];		sp->Effectunknown[col1_swap] = sp->Effectunknown[col2_swap] ;	sp->Effectunknown[col2_swap] = ftemp;
					temp = sp->EffectChainTarget[col1_swap];	sp->EffectChainTarget[col1_swap] = sp->EffectChainTarget[col2_swap] ;	sp->EffectChainTarget[col2_swap] = temp;
					temp = sp->EffectSpellGroupRelation[col1_swap];	sp->EffectSpellGroupRelation[col1_swap] = sp->EffectSpellGroupRelation[col2_swap] ;	sp->EffectSpellGroupRelation[col2_swap] = temp;
					temp = sp->EffectMiscValue[col1_swap];		sp->EffectMiscValue[col1_swap] = sp->EffectMiscValue[col2_swap] ;	sp->EffectMiscValue[col2_swap] = temp;
					temp = sp->EffectTriggerSpell[col1_swap];	sp->EffectTriggerSpell[col1_swap] = sp->EffectTriggerSpell[col2_swap] ;	sp->EffectTriggerSpell[col2_swap] = temp;
					ftemp = sp->EffectPointsPerComboPoint[col1_swap];	sp->EffectPointsPerComboPoint[col1_swap] = sp->EffectPointsPerComboPoint[col2_swap] ;	sp->EffectPointsPerComboPoint[col2_swap] = ftemp;
				}

		for(uint32 b=0;b<3;++b)
			if( sp->Attributes & ATTRIBUTES_ONLY_OUTDOORS && sp->EffectApplyAuraName[b] == SPELL_AURA_MOUNTED )
			{
				sp->Attributes &= ~ATTRIBUTES_ONLY_OUTDOORS;
			}

		// parse rank text
		if( !sscanf( ranktext, "Rank %d", (unsigned int*)&rank) )
			rank = 0;

		//seal of light 
		if( namehash == SPELL_HASH_SEAL_OF_LIGHT )			
			sp->procChance = 45;	/* this will do */

		//seal of command
		else if( namehash == SPELL_HASH_SEAL_OF_COMMAND )		
			sp->Spell_Dmg_Type = SPELL_DMG_TYPE_MAGIC;

		//judgement of command
		else if( namehash == SPELL_HASH_JUDGEMENT_OF_COMMAND )		
			sp->Spell_Dmg_Type = SPELL_DMG_TYPE_MAGIC;

		else if( namehash == SPELL_HASH_ARCANE_SHOT )		
			sp->c_is_flags |= SPELL_FLAG_IS_NOT_USING_DMG_BONUS;

		else if( namehash == SPELL_HASH_SERPENT_STING )		
			sp->c_is_flags |= SPELL_FLAG_IS_NOT_USING_DMG_BONUS;

		//Rogue: Posion time fix for 2.3
		if( strstr( nametext, "Crippling Poison") && sp->Effect[0] == 54 ) //I, II
			sp->EffectBasePoints[0] = 3599;
		if( strstr( nametext, "Mind-numbing Poison") && sp->Effect[0] == 54 ) //I,II,III
			sp->EffectBasePoints[0] = 3599;
		if( strstr( nametext, "Instant Poison") && sp->Effect[0] == 54 ) //I,II,III,IV,V,VI,VII    
			sp->EffectBasePoints[0] = 3599;
		if( strstr( nametext, "Deadly Poison") && sp->Effect[0] == 54 ) //I,II,III,IV,V,VI,VII
			sp->EffectBasePoints[0] = 3599;
		if( strstr( nametext, "Wound Poison") && sp->Effect[0] == 54 ) //I,II,III,IV,V
			sp->EffectBasePoints[0] = 3599;
		if( strstr( nametext, "Anesthetic Poison") && sp->Effect[0] == 54 ) //I
			sp->EffectBasePoints[0] = 3599;

        if( strstr( nametext, "Sharpen Blade") && sp->Effect[0] == 54 ) //All BS stones
            sp->EffectBasePoints[0] = 3599;

		//these mostly do not mix so we can use else 
        // look for seal, etc in name
        if( strstr( nametext, "Seal"))
		{
            type |= SPELL_TYPE_SEAL;
			All_Seal_Groups_Combined |= sp->SpellGroupType;
		}
        else if( strstr( nametext, "Blessing"))
            type |= SPELL_TYPE_BLESSING;
        else if( strstr( nametext, "Curse"))
            type |= SPELL_TYPE_CURSE;
        else if( strstr( nametext, "Aspect"))
            type |= SPELL_TYPE_ASPECT;
        else if( strstr( nametext, "Sting") || strstr( nametext, "sting"))
            type |= SPELL_TYPE_STING;
        // don't break armor items!
        else if(strcmp(nametext, "Armor") && strstr( nametext, "Armor") || strstr( nametext, "Demon Skin"))
            type |= SPELL_TYPE_ARMOR;
        else if( strstr( nametext, "Aura"))
            type |= SPELL_TYPE_AURA;
		else if( strstr( nametext, "Track")==nametext)
            type |= SPELL_TYPE_TRACK;
		else if( namehash == SPELL_HASH_GIFT_OF_THE_WILD || namehash == SPELL_HASH_MARK_OF_THE_WILD )
            type |= SPELL_TYPE_MARK_GIFT;
		else if( namehash == SPELL_HASH_IMMOLATION_TRAP || namehash == SPELL_HASH_FREEZING_TRAP || namehash == SPELL_HASH_FROST_TRAP || namehash == SPELL_HASH_EXPLOSIVE_TRAP || namehash == SPELL_HASH_SNAKE_TRAP )
            type |= SPELL_TYPE_HUNTER_TRAP;
		else if( namehash == SPELL_HASH_ARCANE_INTELLECT || namehash == SPELL_HASH_ARCANE_BRILLIANCE )
            type |= SPELL_TYPE_MAGE_INTEL;
		else if( namehash == SPELL_HASH_AMPLIFY_MAGIC || namehash == SPELL_HASH_DAMPEN_MAGIC )
            type |= SPELL_TYPE_MAGE_MAGI;
		else if( namehash == SPELL_HASH_FIRE_WARD || namehash == SPELL_HASH_FROST_WARD )
            type |= SPELL_TYPE_MAGE_WARDS;
		else if( namehash == SPELL_HASH_SHADOW_PROTECTION || namehash == SPELL_HASH_PRAYER_OF_SHADOW_PROTECTION )
            type |= SPELL_TYPE_PRIEST_SH_PPROT;
		else if( namehash == SPELL_HASH_WATER_SHIELD || namehash == SPELL_HASH_EARTH_SHIELD || namehash == SPELL_HASH_LIGHTNING_SHIELD )
            type |= SPELL_TYPE_SHIELD;
		else if( namehash == SPELL_HASH_POWER_WORD__FORTITUDE || namehash == SPELL_HASH_PRAYER_OF_FORTITUDE )
            type |= SPELL_TYPE_FORTITUDE;
		else if( namehash == SPELL_HASH_DIVINE_SPIRIT || namehash == SPELL_HASH_PRAYER_OF_SPIRIT )
            type |= SPELL_TYPE_SPIRIT;
//		else if( strstr( nametext, "Curse of Weakness") || strstr( nametext, "Curse of Agony") || strstr( nametext, "Curse of Recklessness") || strstr( nametext, "Curse of Tongues") || strstr( nametext, "Curse of the Elements") || strstr( nametext, "Curse of Idiocy") || strstr( nametext, "Curse of Shadow") || strstr( nametext, "Curse of Doom"))
//		else if(namehash==4129426293 || namehash==885131426 || namehash==626036062 || namehash==3551228837 || namehash==2784647472 || namehash==776142553 || namehash==3407058720 || namehash==202747424)
//		else if( strstr( nametext, "Curse of "))
//            type |= SPELL_TYPE_WARLOCK_CURSES;
		else if( strstr( nametext, "Immolate") || strstr( nametext, "Conflagrate"))
			type |= SPELL_TYPE_WARLOCK_IMMOLATE;
		else if( strstr( nametext, "Amplify Magic") || strstr( nametext, "Dampen Magic"))
			type |= SPELL_TYPE_MAGE_AMPL_DUMP;
        else if( strstr( desc, "Battle Elixir"))
            type |= SPELL_TYPE_ELIXIR_BATTLE;
        else if( strstr( desc, "Guardian Elixir"))
            type |= SPELL_TYPE_ELIXIR_GUARDIAN;
        else if( strstr( desc, "Battle and Guardian elixir"))
            type |= SPELL_TYPE_ELIXIR_FLASK;
		else if( namehash == SPELL_HASH_HUNTER_S_MARK )		// hunter's mark
			type |= SPELL_TYPE_HUNTER_MARK;
		else if( strstr( desc, "Finishing move")==desc)
			sp->c_is_flags |= SPELL_FLAG_IS_FINISHING_MOVE;
		if( IsDamagingSpell( sp ) )
			sp->c_is_flags |= SPELL_FLAG_IS_DAMAGING;
		if( IsHealingSpell( sp ) )
			sp->c_is_flags |= SPELL_FLAG_IS_HEALING;
		if( IsTargetingStealthed( sp ) )
			sp->c_is_flags |= SPELL_FLAG_IS_TARGETINGSTEALTHED;
		

		//stupid spell ranking problem
		if(sp->spellLevel==0)
		{
			uint32 new_level=0;
			if( strstr( nametext, "Apprentice "))
				new_level = 1;
			else if( strstr( nametext, "Journeyman "))
				new_level = 2;
			else if( strstr( nametext, "Expert "))
				new_level = 3;
			else if( strstr( nametext, "Artisan "))
				new_level = 4;
			else if( strstr( nametext, "Master "))
				new_level = 5;
			if(new_level!=0)
			{
				uint32 teachspell=0;
				if(sp->Effect[0]==SPELL_EFFECT_LEARN_SPELL)
					teachspell = sp->EffectTriggerSpell[0];
				else if(sp->Effect[1]==SPELL_EFFECT_LEARN_SPELL)
					teachspell = sp->EffectTriggerSpell[1];
				else if(sp->Effect[2]==SPELL_EFFECT_LEARN_SPELL)
					teachspell = sp->EffectTriggerSpell[2];
				if(teachspell)
				{
					SpellEntry *spellInfo;
					assign_Spell_to_SpellEntry(&spellInfo,teachspell);
					spellInfo->spellLevel = new_level;
					sp->spellLevel = new_level;
				}
			}
		}

		// find diminishing status
		sp->DiminishStatus = GetDiminishingGroup(namehash);
		sp->buffIndexType=0;
		switch(namehash)
		{
		case SPELL_HASH_HUNTER_S_MARK:		// Hunter's mark
			sp->buffIndexType = SPELL_TYPE_INDEX_MARK;
			break;

		case SPELL_HASH_POLYMORPH:			// Polymorph
		case SPELL_HASH_POLYMORPH__CHICKEN:	// Polymorph: Chicken
		case SPELL_HASH_POLYMORPH__PIG:		// Polymorph: Pig
		case SPELL_HASH_POLYMORPH__SHEEP:	// Polymorph: Sheep
		case SPELL_HASH_POLYMORPH__TURTLE:	// Polymorph: Turtle
			sp->buffIndexType = SPELL_TYPE_INDEX_POLYMORPH;
			break;

		case SPELL_HASH_FEAR:				// Fear
			sp->buffIndexType = SPELL_TYPE_INDEX_FEAR;
			break;

		case SPELL_HASH_SAP:				// Sap
			sp->buffIndexType = SPELL_TYPE_INDEX_SAP;
			break;

		case SPELL_HASH_SCARE_BEAST:		// Scare Beast
			sp->buffIndexType = SPELL_TYPE_INDEX_SCARE_BEAST;
			break;

		case SPELL_HASH_HIBERNATE:			// Hibernate
			sp->buffIndexType = SPELL_TYPE_INDEX_HIBERNATE;
			break;

//		removed by Zack Earth shield stacks 10 times. Current code does not support it
//		case SPELL_HASH_EARTH_SHIELD:		// Earth Shield
//			sp->buffIndexType = SPELL_TYPE_INDEX_EARTH_SHIELD;
//			break;

		case SPELL_HASH_CYCLONE:			// Cyclone
			sp->buffIndexType = SPELL_TYPE_INDEX_CYCLONE;
			break;

		case SPELL_HASH_BANISH:				// Banish
			sp->buffIndexType = SPELL_TYPE_INDEX_BANISH;
			break;
		}

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
							SpellEntry*s=sSpellStore.LookupEntryForced(tmp);
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
				if( strstr( desc,"head"))
					result|=(1<<INVTYPE_HEAD);
				if( strstr( desc,"leg"))
					result|=(1<<INVTYPE_LEGS);
				if( strstr( desc,"neck"))
					result|=(1<<INVTYPE_NECK);
				if( strstr( desc,"shoulder"))
					result|=(1<<INVTYPE_SHOULDERS);
				if( strstr( desc,"body"))
					result|=(1<<INVTYPE_BODY);
				if( strstr( desc,"chest"))
					result|=((1<<INVTYPE_CHEST)|(1<<INVTYPE_ROBE));
				if( strstr( desc,"waist"))
					result|=(1<<INVTYPE_WAIST);
				if( strstr( desc,"foot")||strstr(desc,"feet")||strstr(desc,"boot"))
					result|=(1<<INVTYPE_FEET);
				if( strstr( desc,"wrist")||strstr(desc,"bracer"))
					result|=(1<<INVTYPE_WRISTS);
				if( strstr( desc,"hand")||strstr(desc,"glove"))
					result|=(1<<INVTYPE_HANDS);
				if( strstr( desc,"finger")||strstr(desc,"ring"))
					result|=(1<<INVTYPE_FINGER);
				if( strstr( desc,"trinket"))
					result|=(1<<INVTYPE_TRINKET);
				if( strstr( desc,"shield"))
					result|=(1<<INVTYPE_SHIELD);
				if( strstr( desc,"cloak"))
					result|=(1<<INVTYPE_CLOAK);
				if( strstr( desc,"robe"))
					result|=(1<<INVTYPE_ROBE);
				//if( strstr( desc,"two")||strstr(desc,"Two"))
				//	result|=(1<<INVTYPE_2HWEAPON);<-handled in subclass
			}
			else*/
			if(effect==SPELL_EFFECT_APPLY_AURA)
			{
				uint32 aura = dbc.getRecord(x).getUInt(94+y); // 58+30+3 = 91
				if( aura == SPELL_AURA_PROC_TRIGGER_SPELL ||
					aura == SPELL_AURA_PROC_TRIGGER_DAMAGE
					)//search for spellid in description
				{
					const char *p=desc;
					while((p=strstr(p,"$")))
					{
						p++;
						//got $  -> check if spell
						if(*p>='0' && *p <='9')
						{//woot this is spell id
						
							result=atoi(p);
						}					
					}
					pr=0;

					uint32 len = (uint32)strlen(desc);
					for(uint32 i = 0; i < len; ++i)
						desc[i] = tolower(desc[i]);
					//dirty code for procs, if any1 got any better idea-> u are welcome
					//139944 --- some magic number, it will trigger on all hits etc
						//for seems to be smth like custom check
					if( strstr( desc,"your ranged criticals"))
						pr|=PROC_ON_RANGED_CRIT_ATTACK;
					if( strstr( desc,"chance on hit"))
						pr|=PROC_ON_MELEE_ATTACK;
					if( strstr( desc,"takes damage"))
						pr|=PROC_ON_ANY_DAMAGE_VICTIM;
					if( strstr( desc,"attackers when hit"))
						pr|=PROC_ON_MELEE_ATTACK_VICTIM;
					if( strstr( desc,"character strikes an enemy"))
						pr|=PROC_ON_MELEE_ATTACK;
					if( strstr( desc,"strike you with a melee attack"))
						pr|=PROC_ON_MELEE_ATTACK_VICTIM;
					if( strstr( desc,"target casts a spell"))
						pr|=PROC_ON_CAST_SPELL;
                    if( strstr( desc,"your harmful spells land"))
                        pr|=PROC_ON_CAST_SPELL;
                    if( strstr( desc,"on spell critical hit"))
                        pr|=PROC_ON_SPELL_CRIT_HIT;
                    if( strstr( desc,"spell critical strikes"))
                        pr|=PROC_ON_SPELL_CRIT_HIT;
                    if( strstr( desc,"being able to resurrect"))
                        pr|=PROC_ON_DIE;
					if( strstr( desc,"any damage caused"))
						pr|=PROC_ON_ANY_DAMAGE_VICTIM;
					if( strstr( desc,"the next melee attack against the caster"))
						pr|=PROC_ON_MELEE_ATTACK_VICTIM;
					if( strstr( desc,"when successfully hit"))
						pr|=PROC_ON_MELEE_ATTACK ;
					if( strstr( desc,"an enemy on hit"))
						pr|=PROC_ON_MELEE_ATTACK;
					if( strstr( desc,"when it hits"))
						pr|=PROC_ON_MELEE_ATTACK;
					if( strstr( desc,"when successfully hit"))
						pr|=PROC_ON_MELEE_ATTACK;
					if( strstr( desc,"on a successful hit"))
						pr|=PROC_ON_MELEE_ATTACK;
					if( strstr( desc,"damage to attacker on hit"))
						pr|=PROC_ON_MELEE_ATTACK_VICTIM;
					if( strstr( desc,"on a hit"))
						pr|=PROC_ON_MELEE_ATTACK;
					if( strstr( desc,"strikes you with a melee attack"))
						pr|=PROC_ON_MELEE_ATTACK_VICTIM;
					if( strstr( desc,"when caster takes damage"))
						pr|=PROC_ON_ANY_DAMAGE_VICTIM;
					if( strstr( desc,"when the caster is using melee attacks"))
						pr|=PROC_ON_MELEE_ATTACK;
					if( strstr( desc,"when struck in combat") || strstr(desc,"When struck in combat"))
						pr|=PROC_ON_MELEE_ATTACK_VICTIM;
					if( strstr( desc,"successful melee attack"))
						pr|=PROC_ON_MELEE_ATTACK;
					if( strstr( desc,"chance per attack"))
						pr|=PROC_ON_MELEE_ATTACK;
					if( strstr( desc,"chance per hit"))
						pr|=PROC_ON_MELEE_ATTACK;
					if( strstr( desc,"that strikes a party member"))
						pr|=PROC_ON_MELEE_ATTACK_VICTIM;
					if( strstr( desc,"when hit by a melee attack"))
						pr|=PROC_ON_MELEE_ATTACK_VICTIM;
					if( strstr( desc,"landing a melee critical strike"))
						pr|=PROC_ON_CRIT_ATTACK;
					if( strstr( desc,"your critical strikes"))
						pr|=PROC_ON_CRIT_ATTACK;
					if( strstr( desc,"whenever you deal ranged damage"))
						pr|=PROC_ON_RANGED_ATTACK;
//					if( strstr( desc,"whenever you deal melee damage"))
					if( strstr( desc,"you deal melee damage"))
						pr|=PROC_ON_MELEE_ATTACK;
					if( strstr( desc,"your melee attacks"))
						pr|=PROC_ON_MELEE_ATTACK;
					if( strstr( desc,"damage with your Sword"))
						pr|=PROC_ON_MELEE_ATTACK;
					if( strstr( desc,"when struck in melee combat"))
						pr|=PROC_ON_MELEE_ATTACK_VICTIM;
					if( strstr( desc,"any successful spell cast against the priest"))
						pr|=PROC_ON_SPELL_HIT_VICTIM;
					if( strstr( desc,"the next melee attack on the caster"))
						pr|=PROC_ON_MELEE_ATTACK_VICTIM;
					if( strstr( desc,"striking melee or ranged attackers"))
						pr|=PROC_ON_MELEE_ATTACK_VICTIM|PROC_ON_RANGED_ATTACK_VICTIM;
					if( strstr( desc,"when damaging an enemy in melee"))
						pr|=PROC_ON_MELEE_ATTACK;
					if( strstr( desc,"victim of a critical strike"))
						pr|=PROC_ON_CRIT_HIT_VICTIM;
					if( strstr( desc,"on successful melee or ranged attack"))
						pr|=PROC_ON_MELEE_ATTACK|PROC_ON_RANGED_ATTACK;
					if( strstr( desc,"enemy that strikes you in melee"))
						pr|=PROC_ON_MELEE_ATTACK_VICTIM;
					if( strstr( desc,"after getting a critical strike"))
						pr|=PROC_ON_CRIT_ATTACK;
					if( strstr( desc,"whenever damage is dealt to you"))
						pr|=PROC_ON_ANY_DAMAGE_VICTIM;
					if( strstr( desc,"when ranged or melee damage is dealt"))
						pr|=PROC_ON_MELEE_ATTACK|PROC_ON_RANGED_ATTACK;
					if( strstr( desc,"damaging melee attacks"))
						pr|=PROC_ON_MELEE_ATTACK;
					if( strstr( desc,"on melee or ranged attack"))
						pr|=PROC_ON_MELEE_ATTACK|PROC_ON_RANGED_ATTACK;
					if( strstr( desc,"on a melee swing"))
						pr|=PROC_ON_MELEE_ATTACK;
					if( strstr( desc,"Chance on melee"))
						pr|=PROC_ON_MELEE_ATTACK;
					if( strstr( desc,"spell criticals against you"))
						pr|=PROC_ON_SPELL_CRIT_HIT_VICTIM;
					if( strstr( desc,"after being struck by a melee or ranged critical hit"))
						pr|=PROC_ON_CRIT_HIT_VICTIM;
//					if( strstr( desc,"on a critical hit"))
					if( strstr( desc,"critical hit"))
						pr|=PROC_ON_CRIT_ATTACK;
					if( strstr( desc,"strikes the caster"))
						pr|=PROC_ON_MELEE_ATTACK_VICTIM;
					if( strstr( desc,"a spell, melee or ranged attack hits the caster"))
						pr|=PROC_ON_ANY_DAMAGE_VICTIM;
					if( strstr( desc,"after dealing a critical strike"))
						pr|=PROC_ON_CRIT_ATTACK;
					if( strstr( desc,"each melee or ranged damage hit against the priest"))
						pr|=PROC_ON_MELEE_ATTACK_VICTIM|PROC_ON_RANGED_ATTACK_VICTIM;				
					if( strstr( desc, "a chance to deal additional"))
						pr|=PROC_ON_MELEE_ATTACK;
					if( strstr( desc, "chance to get an extra attack"))
						pr|=PROC_ON_MELEE_ATTACK;
					if( strstr( desc, "melee attacks has"))
						pr|=PROC_ON_MELEE_ATTACK;
					if( strstr( desc, "any damage spell hits a target"))
						pr|=PROC_ON_CAST_SPELL;
					if( strstr( desc, "giving each melee attack a chance"))
						pr|=PROC_ON_MELEE_ATTACK;
					if( strstr( desc, "damage when hit"))
						pr|=PROC_ON_ANY_DAMAGE_VICTIM; //myabe melee damage ?
					if( strstr( desc, "gives your"))
					{
						if( strstr( desc, "melee"))
							pr|=PROC_ON_MELEE_ATTACK;
						else if( strstr( desc,"sinister strike, backstab, gouge and shiv"))
							pr|=PROC_ON_CAST_SPELL;
						else if( strstr( desc,"chance to daze the target"))
							pr|=PROC_ON_CAST_SPELL;
						else if( strstr( desc,"finishing moves"))
							pr|=PROC_ON_CAST_SPELL;
//						else if( strstr( desc,"shadow bolt, shadowburn, soul fire, incinerate, searing pain and conflagrate"))
//							pr|=PROC_ON_CAST_SPELL|PROC_TARGET_SELF;
						//we should find that specific spell (or group) on what we will trigger
						else pr|=PROC_ON_CAST_SPECIFIC_SPELL;
					}
					if( strstr( desc, "chance to add an additional combo") && strstr(desc, "critical") )
						pr|=PROC_ON_CRIT_ATTACK;
					else if( strstr( desc, "chance to add an additional combo"))
						pr|=PROC_ON_CAST_SPELL;
					if( strstr( desc, "victim of a melee or ranged critical strike"))
						pr|=PROC_ON_CRIT_HIT_VICTIM;
					if( strstr( desc, "getting a critical effect from"))
						pr|=PROC_ON_SPELL_CRIT_HIT_VICTIM;
					if( strstr( desc, "damaging attack is taken"))
						pr|=PROC_ON_ANY_DAMAGE_VICTIM;
					if( strstr( desc, "struck by a Stun or Immobilize"))
						pr|=PROC_ON_SPELL_HIT_VICTIM;
					if( strstr( desc, "melee critical strike"))
						pr|=PROC_ON_CRIT_ATTACK;
					if( strstr( nametext, "Bloodthirst"))
						pr|=PROC_ON_MELEE_ATTACK | PROC_TARGET_SELF;
					if( strstr( desc, "experience or honor"))
						pr|=PROC_ON_GAIN_EXPIERIENCE;
					if( strstr( desc,"your next offensive ability"))
						pr|=PROC_ON_CAST_SPELL;
					if( strstr( desc,"hit by a melee or ranged attack"))
						pr|=PROC_ON_MELEE_ATTACK_VICTIM | PROC_ON_RANGED_ATTACK_VICTIM;
					if( strstr( desc,"enemy strikes the caster"))
						pr|=PROC_ON_MELEE_ATTACK_VICTIM;
					if( strstr( desc,"melee and ranged attacks against you"))
						pr|=PROC_ON_MELEE_ATTACK_VICTIM | PROC_ON_RANGED_ATTACK_VICTIM;
					if( strstr( desc,"when a block occurs"))
						pr|=PROC_ON_BLOCK_VICTIM;
					if( strstr( desc,"dealing a critical strike from a weapon swing, spell, or ability"))
						pr|=PROC_ON_CRIT_ATTACK|PROC_ON_SPELL_CRIT_HIT;
					if( strstr( desc,"dealing a critical strike from a weapon swing, spell, or ability"))
						pr|=PROC_ON_CRIT_ATTACK|PROC_ON_SPELL_CRIT_HIT;
					if( strstr( desc,"shadow bolt critical strikes increase shadow damage"))
						pr|=PROC_ON_SPELL_CRIT_HIT;
					if( strstr( desc,"next offensive ability"))
						pr|=PROC_ON_CAST_SPELL;
					if( strstr( desc,"after being hit with a shadow or fire spell"))
						pr|=PROC_ON_SPELL_LAND_VICTIM;
					if( strstr( desc,"giving each melee attack"))
						pr|=PROC_ON_MELEE_ATTACK;
					if( strstr( desc,"each strike has"))
						pr|=PROC_ON_MELEE_ATTACK;		
					if( strstr( desc,"your Fire damage spell hits"))
						pr|=PROC_ON_CAST_SPELL;		//this happens only on hit ;)
					if( strstr( desc,"corruption, curse of agony, siphon life and seed of corruption spells also cause"))
						pr|=PROC_ON_CAST_SPELL;
					if( strstr( desc,"pain, mind flay and vampiric touch spells also cause"))
						pr|=PROC_ON_CAST_SPELL;
					if( strstr( desc,"shadow damage spells have"))
						pr|=PROC_ON_CAST_SPELL;
					if( strstr( desc,"on successful spellcast"))
						pr|=PROC_ON_CAST_SPELL;
					if( strstr( desc,"your spell criticals have"))
						pr|=PROC_ON_SPELL_CRIT_HIT | PROC_ON_SPELL_CRIT_HIT_VICTIM;
					if( strstr( desc,"after dodging their attack"))
					{
						pr|=PROC_ON_DODGE_VICTIM;
						if( strstr( desc,"add a combo point"))
							pr|=PROC_TARGET_SELF;
					}
					if( strstr( desc,"fully resisting"))
						pr|=PROC_ON_RESIST_VICTIM;
					if( strstr( desc,"Your Shadow Word: Pain, Mind Flay and Vampiric Touch spells also cause the target"))
						pr|=PROC_ON_CAST_SPELL;
					if( strstr( desc,"your melee and ranged attacks"))
						pr|=PROC_ON_MELEE_ATTACK|PROC_ON_RANGED_ATTACK;
//					if( strstr( desc,"chill effect to your Blizzard"))
//						pr|=PROC_ON_CAST_SPELL;	
					//////////////////////////////////////////////////
					//proc dmg flags
					//////////////////////////////////////////////////
					if( strstr( desc,"each attack blocked"))
						pr|=PROC_ON_BLOCK_VICTIM;
					if( strstr( desc,"into flame, causing an additional"))
						pr|=PROC_ON_MELEE_ATTACK;
					if( strstr( desc,"victim of a critical melee strike"))
						pr|=PROC_ON_CRIT_HIT_VICTIM;
					if( strstr( desc,"damage to melee attackers"))
						pr|=PROC_ON_MELEE_ATTACK;
					if( strstr( desc,"target blocks a melee attack"))
						pr|=PROC_ON_BLOCK_VICTIM;
					if( strstr( desc,"ranged and melee attacks to deal"))
						pr|=PROC_ON_MELEE_ATTACK_VICTIM | PROC_ON_RANGED_ATTACK_VICTIM;
					if( strstr( desc,"damage on hit"))
						pr|=PROC_ON_ANY_DAMAGE_VICTIM;
					if( strstr( desc,"chance on hit"))
						pr|=PROC_ON_MELEE_ATTACK;
					if( strstr( desc,"after being hit by any damaging attack"))
						pr|=PROC_ON_ANY_DAMAGE_VICTIM;
					if( strstr( desc,"striking melee or ranged attackers"))
						pr|=PROC_ON_MELEE_ATTACK_VICTIM | PROC_ON_RANGED_ATTACK_VICTIM;
					if( strstr( desc,"damage to attackers when hit"))
						pr|=PROC_ON_MELEE_ATTACK_VICTIM;
					if( strstr( desc,"striking melee attackers"))
						pr|=PROC_ON_MELEE_ATTACK_VICTIM;
					if( strstr( desc,"whenever the caster takes damage"))
						pr|=PROC_ON_ANY_DAMAGE_VICTIM;
					if( strstr( desc,"damage on every attack"))
						pr|=PROC_ON_MELEE_ATTACK | PROC_ON_RANGED_ATTACK;
					if( strstr( desc,"chance to reflect Fire spells"))
						pr|=PROC_ON_SPELL_HIT_VICTIM;
					if( strstr( desc,"hunter takes on the aspects of a hawk"))
						pr|=PROC_TARGET_SELF | PROC_ON_RANGED_ATTACK;
					if( strstr( desc,"successful auto shot attacks"))
						pr|=PROC_ON_AUTO_SHOT_HIT;
					if( strstr( desc,"after getting a critical effect from your"))
						pr=PROC_ON_SPELL_CRIT_HIT;
//					if( strstr( desc,"Your critical strikes from Fire damage"))
//						pr|=PROC_ON_SPELL_CRIT_HIT;
				}//end "if procspellaura"
				//dirty fix to remove auras that should expire on event and they are not
//				else if(sp->procCharges>0)
//				{
					//there are at least 185 spells that should loose charge uppon some event.Be prepared to add more here !
					// ! watch it cause this might conflict with our custom modified spells like : lighning shield !

					//spells like : Presence of Mind,Nature's Swiftness, Inner Focus,Amplify Curse,Coup de Grace
					//SELECT * FROM dbc_spell where proc_charges!=0 and (effect_aura_1=108 or effect_aura_2=108 and effect_aura_3=108) and description!=""
//					if(aura == SPELL_AURA_ADD_PCT_MODIFIER)
//						sp->AuraInterruptFlags |= AURA_INTERRUPT_ON_CAST_SPELL;
					//most of them probably already have these flags...not sure if we should add to all of them without checking
/*					if( strstr( desc, "melee"))
						sp->AuraInterruptFlags |= AURA_INTERRUPT_ON_START_ATTACK;
					if( strstr( desc, "ranged"))
						sp->AuraInterruptFlags |= AURA_INTERRUPT_ON_START_ATTACK;*/
//				}
			}//end "if aura"
		}//end "for each effect"
		sp->procFlags = pr;

		if( strstr( desc, "Must remain seated"))
		{
			sp->RecoveryTime = 1000;
			sp->CategoryRecoveryTime = 1000;
		}

		//////////////////////////////////////////////////////////////////////////////////////////////////////
		// procintervals
		//////////////////////////////////////////////////////////////////////////////////////////////////////
		//omg lighning shield trigger spell id's are all wrong ?
		//if you are bored you could make thiese by hand but i guess we might find other spells with this problem..and this way it's safe
		if( strstr( nametext, "Lightning Shield" ) && sp->EffectTriggerSpell[0] )
		{
			//check if we can find in the desription
			char *startofid = strstr(desc, "for $");
			if( startofid )
			{
				startofid += strlen("for $");
				sp->EffectTriggerSpell[0] = atoi( startofid ); //get new lightning shield trigger id
			}
			sp->proc_interval = 3000; //few seconds
		}
		//mage ignite talent should proc only on some chances
		else if( strstr( nametext, "Ignite") && sp->Id>=11119 && sp->Id<=12848 && sp->EffectApplyAuraName[0] == 4 )
		{
			//check if we can find in the desription
			char *startofid=strstr(desc, "an additional ");
			if(startofid)
			{
				startofid += strlen("an additional ");
				sp->EffectBasePoints[0]=atoi(startofid); //get new value. This is actually level*8 ;)
			}
			sp->Effect[0] = 6; //aura
			sp->EffectApplyAuraName[0] = 42; //force him to use procspell effect
			sp->EffectTriggerSpell[0] = 12654; //evil , but this is good for us :D
			sp->procFlags = PROC_ON_SPELL_CRIT_HIT; //add procflag here since this was not processed with the others !
		}
		// Winter's Chill handled by frost school
		else if( strstr( nametext, "Winter's Chill"))
		{
			sp->School = 4;
		}
		// Blackout handled by Shadow school
		else if( strstr( nametext, "Blackout"))
		{
			sp->School = 5;
		}
		// Shadow Weaving
		else if( strstr( nametext, "Shadow Weaving"))
		{
			sp->School = 5;
			sp->EffectApplyAuraName[0] = 42;
			sp->procChance = sp->EffectBasePoints[0] + 1;
			sp->procFlags = PROC_ON_CAST_SPECIFIC_SPELL;
		}
		//Improved Aspect of the Hawk
		else if( strstr( nametext, "Improved Aspect of the Hawk"))
			sp->EffectSpellGroupRelation[1] = 0x00100000;
		//more triggered spell ids are wrong. I think blizz is trying to outsmart us :S
		else if( strstr( nametext, "Nature's Guardian"))
		{
			sp->EffectTriggerSpell[0] = 31616;
			sp->proc_interval = 5000;
		}
		//Chain Heal all ranks %50 heal value (49 + 1)
		else if( strstr( nametext, "Chain Heal"))
		{
			sp->EffectDieSides[0] = 49;
		}
		//this starts to be an issue for trigger spell id : Deep Wounds
		else if( strstr( nametext, "Deep Wounds") && sp->EffectTriggerSpell[0])
		{
			//check if we can find in the desription
			char *startofid=strstr(desc, "over $");
			if(startofid)
			{
				startofid += strlen("over $");
				sp->EffectTriggerSpell[0] = atoi(startofid);
			}
		}
		else if( strstr( nametext, "Holy Shock"))
		{
			//check if we can find in the desription
			char *startofid=strstr(desc, "causing $");
			if(startofid)
			{
				startofid += strlen("causing $");
				sp->EffectTriggerSpell[0] = atoi(startofid);
			}
			//check if we can find in the desription
			startofid=strstr(desc, " or $");
			if(startofid)
			{
				startofid += strlen(" or $");
				sp->EffectTriggerSpell[1]=atoi(startofid);
			}
		}
		else if( strstr( nametext, "Touch of Weakness"))
		{
			//check if we can find in the desription
			char *startofid=strstr(desc, "cause $");
			if(startofid)
			{
				startofid += strlen("cause $");
				sp->EffectTriggerSpell[0] = atoi(startofid);
				sp->EffectTriggerSpell[1]=sp->EffectTriggerSpell[0]; //later versions of this spell changed to eff[1] the aura
				sp->procFlags = uint32(PROC_ON_MELEE_ATTACK_VICTIM);
			}
		}
		else if( strstr( nametext, "Firestone Passive"))
		{
			//Enchants the main hand weapon with fire, granting each attack a chance to deal $17809s1 additional fire damage.
			//check if we can find in the desription
			char * startofid=strstr(desc, "to deal $");
			if(startofid)
			{
				startofid += strlen("to deal $");
				sp->EffectTriggerSpell[0] = atoi(startofid);
				sp->EffectApplyAuraName[0] = 42;
				sp->procFlags = PROC_ON_MELEE_ATTACK;
				sp->procChance = 50;
			}
		}
		//some procs trigger at intervals
		else if( strstr( nametext, "Water Shield"))
		{
			sp->proc_interval = 3000; //few seconds
			sp->procFlags |= PROC_TARGET_SELF;
		}
		else if( strstr( nametext, "Earth Shield"))
			sp->proc_interval = 3000; //few seconds
		else if( strstr( nametext, "Shadowguard"))
			sp->proc_interval = 3000; //few seconds
		else if( strstr( nametext, "Poison Shield"))
			sp->proc_interval = 3000; //few seconds
		else if( strstr( nametext, "Infused Mushroom"))
			sp->proc_interval = 10000; //10 seconds
		else if( strstr( nametext, "Aviana's Purpose"))
			sp->proc_interval = 10000; //10 seconds
		//don't change to namehash since we are searching only a protion of the name
 		else if( strstr( nametext, "Crippling Poison"))
		{
			sp->SpellGroupType |= 16384; //some of them do have the flags but i's hard to write down those some from 130 spells
			sp->c_is_flags |= SPELL_FLAG_IS_POISON;
		}
		else if( strstr( nametext, "Mind-Numbing Poison"))
		{
			sp->SpellGroupType |= 32768; //some of them do have the flags but i's hard to write down those some from 130 spells
			sp->c_is_flags |= SPELL_FLAG_IS_POISON;
		}
		else if( strstr( nametext, "Instant Poison"))
		{
			sp->SpellGroupType |= 8192; //some of them do have the flags but i's hard to write down those some from 130 spells
			sp->c_is_flags |= SPELL_FLAG_IS_POISON;
		}
		else if( strstr( nametext, "Deadly Poison"))
		{
			sp->SpellGroupType |= 65536; //some of them do have the flags but i's hard to write down those some from 130 spells
			sp->c_is_flags |= SPELL_FLAG_IS_POISON;
		}
		else if( strstr( nametext, "Wound Poison"))
		{
			sp->SpellGroupType |= 268435456; //some of them do have the flags but i's hard to write down those some from 130 spells
			sp->c_is_flags |= SPELL_FLAG_IS_POISON;
		}

		//warlock - shadow bolt
		if( sp->NameHash == SPELL_HASH_SHADOW_BOLT )
			sp->SpellGroupType |= 1; //some of them do have the flags but i's hard to write down those some from 130 spells

		//mage Ice Floes affects these spells : Cone of Cold,Cold Snap,Ice Barrier,Ice Block
		// Zack : WTF ? talents have group relations not target spells !
//		if( sp->NameHash == SPELL_HASH_CONE_OF_COLD || sp->NameHash == SPELL_HASH_COLD_SNAP || sp->NameHash == SPELL_HASH_ICE_BARRIER || sp->NameHash == SPELL_HASH_ICE_BLOCK )
//			sp->EffectSpellGroupRelation[0] = 0x00200000;

/*		else if( strstr( nametext, "Anesthetic Poison"))
			sp->SpellGroupType |= 0; //not yet known ? 
		else if( strstr( nametext, "Blinding Powder"))
			sp->SpellGroupType |= 0; //not yet known ?*/
//		else if( strstr( nametext, "Illumination"))
//			sp->EffectTriggerSpell[0] = 20272;
		//sp->dummy=result;
/*		//if there is a proc spell and has 0 as charges then it's probably going to triger infinite times. Better not save these
		if(sp->procCharges==0)
			sp->procCharges=-1;*/

		//Set Silencing spells mech.
		if( sp->EffectApplyAuraName[0] == 27 || 
			sp->EffectApplyAuraName[1] == 27 ||
			sp->EffectApplyAuraName[2] == 27 )
			sp->MechanicsType = MECHANIC_SILENCED;

		//Set Stunning spells mech.
		if( sp->EffectApplyAuraName[0] == 12 || 
			sp->EffectApplyAuraName[1] == 12 ||
			sp->EffectApplyAuraName[2] == 12 )
			sp->MechanicsType = MECHANIC_STUNNED;

		//Set Fearing spells mech
		if( sp->EffectApplyAuraName[0] == 7 || 
			sp->EffectApplyAuraName[1] == 7 ||
			sp->EffectApplyAuraName[2] == 7 )
			sp->MechanicsType = MECHANIC_FLEEING;

		if( sp->proc_interval != 0 )
			sp->procFlags |= PROC_REMOVEONUSE;

		// Seal of Command - Proc Chance
		if( sp->NameHash == SPELL_HASH_SEAL_OF_COMMAND )
		{
			sp->procChance = 25;
			sp->School = SCHOOL_HOLY; //the procspells of the original seal of command have fizical school instead of holy
			sp->Spell_Dmg_Type = SPELL_DMG_TYPE_MAGIC; //heh, crazy spell uses melee/ranged/magic dmg type for 1 spell. Now which one is correct ?
		}

		//Seal of Jusice - Proc Chance
		if( sp->NameHash == SPELL_HASH_SEAL_OF_JUSTICE )
			sp->procChance = 25;

		/* Decapitate */
		if( sp->NameHash == SPELL_HASH_DECAPITATE )
			sp->procChance = 30;

		//shaman - shock, has no spellgroup.very dangerous move !
		if( sp->NameHash == SPELL_HASH_SHOCK )
			sp->SpellGroupType = 4;

		//mage - fireball. Only some of the spell has the flags 
		if( sp->NameHash == SPELL_HASH_FIREBALL )
			sp->SpellGroupType |= 1;

		if( sp->NameHash == SPELL_HASH_DIVINE_SHIELD || sp->NameHash == SPELL_HASH_DIVINE_PROTECTION || sp->NameHash == SPELL_HASH_BLESSING_OF_PROTECTION )
			sp->MechanicsType = 25;

		/* hackfix for this - FIX ME LATER - Burlex */
		if( namehash == SPELL_HASH_SEAL_FATE )
			sp->procFlags = 0;

		if(
			((sp->Attributes & ATTRIBUTES_TRIGGER_COOLDOWN) && (sp->AttributesEx & ATTRIBUTESEX_DELAY_SOME_TRIGGERS)) //rogue cold blood
			|| ((sp->Attributes & ATTRIBUTES_TRIGGER_COOLDOWN) && (!sp->AttributesEx || sp->AttributesEx & ATTRIBUTESEX_REMAIN_OOC))
			)
		{
			sp->c_is_flags |= SPELL_FLAG_IS_REQUIRECOOLDOWNUPDATE;
		}

//junk code to get me has :P 
//if(sp->Id==11267 || sp->Id==11289 || sp->Id==6409)
//	printf("!!!!!!! name %s , id %u , hash %u \n",nametext,sp->Id, namehash);
	}
	//this is so lame : shamanistic rage triggers a new spell which borrows it's stats from parent spell :S
	SpellEntry * parentsp;
	assign_Spell_to_SpellEntry(&parentsp,30823);
	SpellEntry * triggersp;
	assign_Spell_to_SpellEntry(&triggersp,30824);
	if( parentsp && triggersp ) 
		triggersp->EffectBasePoints[0] = parentsp->EffectBasePoints[0];

	assign_Spell_to_SpellEntry( &sp , 16164 );
	if( sp != NULL && sp->Id == 16164 )
		sp->procFlags = PROC_ON_SPELL_CRIT_HIT_VICTIM;

	//remove stormstrike effect 0
	assign_Spell_to_SpellEntry( &sp , 17364 );
	if( sp != NULL && sp->Id == 17364 )
		sp->Effect[0] = 0;

	//Warlock: Nether Protection
	assign_Spell_to_SpellEntry( &sp , 30299 );
	if (sp != NULL)
		sp->procChance = 10;
	assign_Spell_to_SpellEntry( &sp , 30301 );
	if (sp != NULL)
		sp->procChance = 20;
	assign_Spell_to_SpellEntry( &sp , 30302 );
	if (sp != NULL)
		sp->procChance = 30;

	//Warlock: Backlash
	assign_Spell_to_SpellEntry( &sp , 34935 );
	if (sp != NULL)
		sp->procFlags |= PROC_ON_MELEE_ATTACK_VICTIM | PROC_TARGET_SELF;
	assign_Spell_to_SpellEntry( &sp , 34938 );
	if (sp != NULL)
		sp->procFlags |= PROC_ON_MELEE_ATTACK_VICTIM | PROC_TARGET_SELF;
	assign_Spell_to_SpellEntry( &sp , 34939 );
	if (sp != NULL)
		sp->procFlags |= PROC_ON_MELEE_ATTACK_VICTIM | PROC_TARGET_SELF;
	assign_Spell_to_SpellEntry( &sp , 34936 );
	if (sp != NULL)
	{
		sp->AuraInterruptFlags |= AURA_INTERRUPT_ON_CAST_SPELL;
		sp->EffectSpellGroupRelation[0] = 1 ;
	}

	//Warlock: Demonic Knowledge
    assign_Spell_to_SpellEntry( &sp , 35691 );
    if( sp != NULL )
	{
        sp->EffectApplyAuraName[0] = 13;
		sp->EffectImplicitTargetA[0]=EFF_TARGET_SELF;
        sp->Effect[1] = 6;
        sp->EffectApplyAuraName[1] = 13;
        sp->EffectBasePoints[1] = sp->EffectBasePoints[0];
		sp->EffectImplicitTargetA[1]=EFF_TARGET_PET;
        sp->Effect[2] = SPELL_EFFECT_TRIGGER_SPELL;
        sp->EffectTriggerSpell[2] = 35696;
		sp->EffectImplicitTargetA[2]=EFF_TARGET_PET;
		sp->c_is_flags |= SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_PET_OWNER | SPELL_FLAG_IS_EXPIREING_WITH_PET;
    }
    assign_Spell_to_SpellEntry( &sp , 35692 );
    if( sp != NULL )
	{
        sp->EffectApplyAuraName[0] = 13;
		sp->EffectImplicitTargetA[0]=EFF_TARGET_SELF;
        sp->Effect[1] = 6;
        sp->EffectApplyAuraName[1] = 13;
        sp->EffectBasePoints[1] = sp->EffectBasePoints[0];
		sp->EffectImplicitTargetA[1]=EFF_TARGET_PET;
        sp->Effect[2] = SPELL_EFFECT_TRIGGER_SPELL;
        sp->EffectTriggerSpell[2] = 35696;
		sp->EffectImplicitTargetA[2]=EFF_TARGET_PET;
		sp->c_is_flags |= SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_PET_OWNER | SPELL_FLAG_IS_EXPIREING_WITH_PET;
    }
    assign_Spell_to_SpellEntry( &sp , 35693 );
    if( sp != NULL )
	{
        sp->EffectApplyAuraName[0] = 13;
		sp->EffectImplicitTargetA[0]=EFF_TARGET_SELF;
        sp->Effect[1] = 6;
        sp->EffectApplyAuraName[1] = 13;
        sp->EffectBasePoints[1] = sp->EffectBasePoints[0];
		sp->EffectImplicitTargetA[1]=EFF_TARGET_PET;
        sp->Effect[2] = SPELL_EFFECT_TRIGGER_SPELL;
        sp->EffectTriggerSpell[2] = 35696;
		sp->EffectImplicitTargetA[2]=EFF_TARGET_PET;
		sp->c_is_flags |= SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_PET_OWNER | SPELL_FLAG_IS_EXPIREING_WITH_PET;
    }
    assign_Spell_to_SpellEntry( &sp , 35696 );
    if( sp != NULL )
	{
        sp->Effect[0] = 6; //making this only for the visible effect
        sp->EffectApplyAuraName[0] = 4; //no effect here
		sp->EffectImplicitTargetA[0]=EFF_TARGET_PET;
	}

    //Priest - Holy Nova
    assign_Spell_to_SpellEntry( &sp ,  15237 );
    if( sp != NULL )
    {
        sp->Effect[1] = 64;
        sp->EffectTriggerSpell[1] = 23455;
    }
    assign_Spell_to_SpellEntry( &sp ,  15430 );
    if( sp != NULL )
    {
        sp->Effect[1] = 64;
        sp->EffectTriggerSpell[1] = 23458;
    }
    assign_Spell_to_SpellEntry( &sp ,  15431 );
    if( sp != NULL )
    {
        sp->Effect[1] = 64;
        sp->EffectTriggerSpell[1] = 23459;
    }
    assign_Spell_to_SpellEntry( &sp ,  27799 );
    if( sp != NULL )
    {
        sp->Effect[1] = 64;
        sp->EffectTriggerSpell[1] = 27803;
    }
    assign_Spell_to_SpellEntry( &sp ,  27800 );
    if( sp != NULL )
    {
        sp->Effect[1] = 64;
        sp->EffectTriggerSpell[1] = 27804;
    }
    assign_Spell_to_SpellEntry( &sp ,  27801 );
    if( sp != NULL )
    {
        sp->Effect[1] = 64;
        sp->EffectTriggerSpell[1] = 27805;
    }

	//Bloodlust targets sorounding creatures instead of us
	assign_Spell_to_SpellEntry( &sp , 2825 );
	if( sp != NULL )
	{
		sp->EffectImplicitTargetA[0]=EFF_TARGET_ALL_PARTY;
		sp->EffectImplicitTargetA[1]=EFF_TARGET_ALL_PARTY;
		sp->EffectImplicitTargetA[2]=0;
		sp->EffectImplicitTargetB[0]=0;
		sp->EffectImplicitTargetB[1]=0;
		sp->EffectImplicitTargetB[2]=0;
	}

	//Heroism targets sorounding creatures instead of us
	assign_Spell_to_SpellEntry( &sp , 32182 );
	if( sp != NULL )
	{
		sp->EffectImplicitTargetA[0]=EFF_TARGET_ALL_PARTY;
		sp->EffectImplicitTargetA[1]=EFF_TARGET_ALL_PARTY;
		sp->EffectImplicitTargetA[2]=0;
		sp->EffectImplicitTargetB[0]=0;
		sp->EffectImplicitTargetB[1]=0;
		sp->EffectImplicitTargetB[2]=0;
	}
	// Drums of war targets sorounding creatures instead of us
	assign_Spell_to_SpellEntry( &sp , 35475 );
	if( sp != NULL )
	{
		sp->EffectImplicitTargetA[0]=EFF_TARGET_ALL_PARTY;
		sp->EffectImplicitTargetA[1]=EFF_TARGET_ALL_PARTY;
		sp->EffectImplicitTargetA[2]=0;
		sp->EffectImplicitTargetB[0]=0;
		sp->EffectImplicitTargetB[1]=0;
		sp->EffectImplicitTargetB[2]=0;
	}
	// Symbol of Hope targets sorounding creatures instead of us
	assign_Spell_to_SpellEntry( &sp , 32548 );
	if( sp != NULL )
	{
		sp->EffectImplicitTargetA[0]=EFF_TARGET_ALL_PARTY;
		sp->EffectImplicitTargetA[1]=EFF_TARGET_ALL_PARTY;
		sp->EffectImplicitTargetA[2]=0;
		sp->EffectImplicitTargetB[0]=0;
		sp->EffectImplicitTargetB[1]=0;
		sp->EffectImplicitTargetB[2]=0;
	}
	// Drums of Battle targets sorounding creatures instead of us
	assign_Spell_to_SpellEntry( &sp , 35476 );
	if( sp != NULL )
	{
		sp->EffectImplicitTargetA[0]=EFF_TARGET_ALL_PARTY;
		sp->EffectImplicitTargetA[1]=EFF_TARGET_ALL_PARTY;
		sp->EffectImplicitTargetA[2]=0;
		sp->EffectImplicitTargetB[0]=0;
		sp->EffectImplicitTargetB[1]=0;
		sp->EffectImplicitTargetB[2]=0;
	}
	// Drums of Panic targets sorounding creatures instead of us
	assign_Spell_to_SpellEntry( &sp , 35474 );
	if( sp != NULL )
	{
		sp->EffectImplicitTargetA[0]=EFF_TARGET_ALL_PARTY;
		sp->EffectImplicitTargetA[1]=EFF_TARGET_ALL_PARTY;
		sp->EffectImplicitTargetA[2]=0;
		sp->EffectImplicitTargetB[0]=0;
		sp->EffectImplicitTargetB[1]=0;
		sp->EffectImplicitTargetB[2]=0;
	}
	// Drums of Restoration targets sorounding creatures instead of us
	assign_Spell_to_SpellEntry( &sp , 35478 );
	if( sp != NULL )
	{
		sp->EffectImplicitTargetA[0]=EFF_TARGET_ALL_PARTY;
		sp->EffectImplicitTargetA[1]=EFF_TARGET_ALL_PARTY;
		sp->EffectImplicitTargetA[2]=0;
		sp->EffectImplicitTargetB[0]=0;
		sp->EffectImplicitTargetB[1]=0;
		sp->EffectImplicitTargetB[2]=0;
	}
	// Drums of Speed targets sorounding creatures instead of us
	assign_Spell_to_SpellEntry( &sp , 35477 );
	if( sp != NULL )
	{
		sp->EffectImplicitTargetA[0]=EFF_TARGET_ALL_PARTY;
		sp->EffectImplicitTargetA[1]=EFF_TARGET_ALL_PARTY;
		sp->EffectImplicitTargetA[2]=0;
		sp->EffectImplicitTargetB[0]=0;
		sp->EffectImplicitTargetB[1]=0;
		sp->EffectImplicitTargetB[2]=0;
	}

	//paladin - Blessing of Light. Changed to scripted because it needs to mod target and should not influence holy nova
	assign_Spell_to_SpellEntry( &sp , 19977 );
	if( sp != NULL )
	{
		sp->EffectApplyAuraName[0] = 4;
		sp->EffectApplyAuraName[1] = 4;
	}
	assign_Spell_to_SpellEntry( &sp , 19978 );
	if( sp != NULL )
	{
		sp->EffectApplyAuraName[0] = 4;
		sp->EffectApplyAuraName[1] = 4;
	}
	assign_Spell_to_SpellEntry( &sp , 19979 );
	if( sp != NULL )
	{
		sp->EffectApplyAuraName[0] = 4;
		sp->EffectApplyAuraName[1] = 4;
	}
	assign_Spell_to_SpellEntry( &sp , 27144 );
	if( sp != NULL )
	{
		sp->EffectApplyAuraName[0] = 4;
		sp->EffectApplyAuraName[1] = 4;
	}
	assign_Spell_to_SpellEntry( &sp , 32770 );
	if( sp != NULL )
	{
		sp->EffectApplyAuraName[0] = 4;
		sp->EffectApplyAuraName[1] = 4;
	}

	//paladin ( grouping ) Healing Light
	uint32 group_relation_paladin_healing_light = 0x40000000 | 0x80000000;

	//paladin - 
	assign_Spell_to_SpellEntry( &sp , 20237 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = group_relation_paladin_healing_light;
	assign_Spell_to_SpellEntry( &sp , 20238 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = group_relation_paladin_healing_light;
	assign_Spell_to_SpellEntry( &sp , 20239 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = group_relation_paladin_healing_light;

	//paladin - Aura Mastery
	assign_Spell_to_SpellEntry( &sp , 31821 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 131072 | 67108864 | 8 | 64;

	//paladin ( grouping ) Sanctified Light
	uint32 group_relation_paladin_sanctified_light = 0x80000000;

	//paladin - Sanctified Light
	assign_Spell_to_SpellEntry( &sp , 20359 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = group_relation_paladin_sanctified_light;
	assign_Spell_to_SpellEntry( &sp , 20360 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = group_relation_paladin_sanctified_light;
	assign_Spell_to_SpellEntry( &sp , 20361 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = group_relation_paladin_sanctified_light;

	//paladin - Improved Seal of the Crusader
	assign_Spell_to_SpellEntry( &sp , 20335 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 512;
	assign_Spell_to_SpellEntry( &sp , 20336 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 512;
	assign_Spell_to_SpellEntry( &sp , 20337 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 512;
	assign_Spell_to_SpellEntry( &sp , 28852 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 512;
	assign_Spell_to_SpellEntry( &sp , 33557 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 512;

	//paladin - Light's Grace
	assign_Spell_to_SpellEntry( &sp , 31834 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 0x80000000;

	//paladin - Stoicism
	assign_Spell_to_SpellEntry( &sp , 31844 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[1] = 0xFFFFFFFF;
	assign_Spell_to_SpellEntry( &sp , 31845 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[1] = 0xFFFFFFFF;

	//paladin - Fanaticism
	assign_Spell_to_SpellEntry( &sp , 31879 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 1048576 | 524288 | 1024 | 536870912 | 524288;
	assign_Spell_to_SpellEntry( &sp , 31880 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 1048576 | 524288 | 1024 | 536870912 | 524288;
	assign_Spell_to_SpellEntry( &sp , 31881 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 1048576 | 524288 | 1024 | 536870912 | 524288;
	assign_Spell_to_SpellEntry( &sp , 31882 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 1048576 | 524288 | 1024 | 536870912 | 524288;
	assign_Spell_to_SpellEntry( &sp , 31883 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 1048576 | 524288 | 1024 | 536870912 | 524288;

	//paladin - Seal of Vengeance
	assign_Spell_to_SpellEntry( &sp , 31801 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_MELEE_ATTACK;
        sp->EffectTriggerSpell[0] = 31803;
		sp->EffectApplyAuraName[0] = 42;
	}

	//paladin - Reckoning
	assign_Spell_to_SpellEntry( &sp , 20177 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_MELEE_ATTACK_VICTIM | PROC_TARGET_SELF;
        sp->EffectTriggerSpell[0] = 32746;
	}
	assign_Spell_to_SpellEntry( &sp , 20179 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_MELEE_ATTACK_VICTIM | PROC_TARGET_SELF;
        sp->EffectTriggerSpell[0] = 32746;
	}
	assign_Spell_to_SpellEntry( &sp , 20180 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_MELEE_ATTACK_VICTIM | PROC_TARGET_SELF;
        sp->EffectTriggerSpell[0] = 32746;
	}
	assign_Spell_to_SpellEntry( &sp , 20181 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_MELEE_ATTACK_VICTIM | PROC_TARGET_SELF;
        sp->EffectTriggerSpell[0] = 32746;
	}
	assign_Spell_to_SpellEntry( &sp , 20182 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_MELEE_ATTACK_VICTIM | PROC_TARGET_SELF;
        sp->EffectTriggerSpell[0] = 32746;
	}

	//paladin - Judgement of Wisdom
	assign_Spell_to_SpellEntry( &sp , 20186 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_ANY_DAMAGE_VICTIM;
        sp->EffectTriggerSpell[0] = 20268;
	}
	assign_Spell_to_SpellEntry( &sp , 20354 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_ANY_DAMAGE_VICTIM;
        sp->EffectTriggerSpell[0] = 20352;
	}
	assign_Spell_to_SpellEntry( &sp , 20355 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_ANY_DAMAGE_VICTIM;
        sp->EffectTriggerSpell[0] = 20353;
	}
	assign_Spell_to_SpellEntry( &sp , 27164 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_ANY_DAMAGE_VICTIM;
        sp->EffectTriggerSpell[0] = 27165;
	}
	assign_Spell_to_SpellEntry( &sp , 20268 );
	if( sp != NULL )
		sp->EffectImplicitTargetA[0] = EFF_TARGET_SINGLE_ENEMY;
	assign_Spell_to_SpellEntry( &sp , 20352 );
	if( sp != NULL )
		sp->EffectImplicitTargetA[0] = EFF_TARGET_SINGLE_ENEMY;
	assign_Spell_to_SpellEntry( &sp , 20353 );
	if( sp != NULL )
		sp->EffectImplicitTargetA[0] = EFF_TARGET_SINGLE_ENEMY;
	assign_Spell_to_SpellEntry( &sp , 27165 );
	if( sp != NULL )
		sp->EffectImplicitTargetA[0] = EFF_TARGET_SINGLE_ENEMY;

	//paladin - Judgement of Light
	assign_Spell_to_SpellEntry( &sp , 20185 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_MELEE_ATTACK_VICTIM;
        sp->EffectTriggerSpell[0] = 20267;
	}
	assign_Spell_to_SpellEntry( &sp , 20344 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_MELEE_ATTACK_VICTIM;
        sp->EffectTriggerSpell[0] = 20341;
	}
	assign_Spell_to_SpellEntry( &sp , 20345 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_MELEE_ATTACK_VICTIM;
        sp->EffectTriggerSpell[0] = 20342;
	}
	assign_Spell_to_SpellEntry( &sp , 20346 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_MELEE_ATTACK_VICTIM;
        sp->EffectTriggerSpell[0] = 20343;
	}
	assign_Spell_to_SpellEntry( &sp , 27162 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_MELEE_ATTACK_VICTIM;
        sp->EffectTriggerSpell[0] = 27163;
	}
	assign_Spell_to_SpellEntry( &sp , 20267 );
	if( sp != NULL )
		sp->EffectImplicitTargetA[0] = EFF_TARGET_SINGLE_ENEMY;
	assign_Spell_to_SpellEntry( &sp , 20341 );
	if( sp != NULL )
		sp->EffectImplicitTargetA[0] = EFF_TARGET_SINGLE_ENEMY;
	assign_Spell_to_SpellEntry( &sp , 20342 );
	if( sp != NULL )
		sp->EffectImplicitTargetA[0] = EFF_TARGET_SINGLE_ENEMY;
	assign_Spell_to_SpellEntry( &sp , 20343 );
	if( sp != NULL )
		sp->EffectImplicitTargetA[0] = EFF_TARGET_SINGLE_ENEMY;
	assign_Spell_to_SpellEntry( &sp , 27163 );
	if( sp != NULL )
		sp->EffectImplicitTargetA[0] = EFF_TARGET_SINGLE_ENEMY;

	//paladin - Eye for an Eye
	assign_Spell_to_SpellEntry( &sp , 9799 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_SPELL_CRIT_HIT_VICTIM;
        sp->EffectApplyAuraName[0] = 42;
        sp->EffectTriggerSpell[0] = 25997;
	}
	assign_Spell_to_SpellEntry( &sp , 25988 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_SPELL_CRIT_HIT_VICTIM;
        sp->EffectApplyAuraName[0] = 42;
        sp->EffectTriggerSpell[0] = 25997;
	}

	//paladin - sanctified judgement
	assign_Spell_to_SpellEntry( &sp , 31876 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_CAST_SPELL;
        sp->EffectApplyAuraName[0] = 42;
        sp->EffectTriggerSpell[0] = 31930;
	}
	assign_Spell_to_SpellEntry( &sp , 31877 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_CAST_SPELL;
        sp->EffectApplyAuraName[0] = 42;
        sp->EffectTriggerSpell[0] = 31930;
	}
	assign_Spell_to_SpellEntry( &sp , 31878 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_CAST_SPELL;
        sp->EffectApplyAuraName[0] = 42;
        sp->EffectTriggerSpell[0] = 31930;
//sp->procChance=100;
	}

	//paladin - Blessed Life
	assign_Spell_to_SpellEntry( &sp , 31828 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_ANY_DAMAGE_VICTIM;
        sp->EffectTriggerSpell[0] = 31828;
	}
	assign_Spell_to_SpellEntry( &sp , 31829 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_ANY_DAMAGE_VICTIM;
        sp->EffectTriggerSpell[0] = 31828;
	}
	assign_Spell_to_SpellEntry( &sp , 31830 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_ANY_DAMAGE_VICTIM;
        sp->EffectTriggerSpell[0] = 31828;
	}

	//paladin - Light's Grace
	assign_Spell_to_SpellEntry( &sp , 31833 );
	if( sp != NULL )
		sp->procFlags = PROC_ON_CAST_SPELL;;
	assign_Spell_to_SpellEntry( &sp , 31835 );
	if( sp != NULL )
		sp->procFlags = PROC_ON_CAST_SPELL;;
	assign_Spell_to_SpellEntry( &sp , 31836 );
	if( sp != NULL )
		sp->procFlags = PROC_ON_CAST_SPELL;;

	//shaman - Lightning Overload 
	assign_Spell_to_SpellEntry( &sp , 30675 ); 
	if( sp != NULL )
	{
		sp->EffectApplyAuraName[0] = 42;//proc something (we will owerride this)
		sp->EffectTriggerSpell[0] = 39805;//proc something (we will owerride this)
		sp->procFlags = PROC_ON_CAST_SPELL;
	}
	assign_Spell_to_SpellEntry( &sp , 30678 ); 
	if( sp != NULL )
	{
		sp->EffectApplyAuraName[0] = 42;//proc something (we will owerride this)
		sp->EffectTriggerSpell[0] = 39805;//proc something (we will owerride this)
		sp->procFlags = PROC_ON_CAST_SPELL;
	}
	assign_Spell_to_SpellEntry( &sp , 30679 ); 
	if( sp != NULL )
	{
		sp->EffectApplyAuraName[0] = 42;//proc something (we will owerride this)
		sp->EffectTriggerSpell[0] = 39805;//proc something (we will owerride this)
		sp->procFlags = PROC_ON_CAST_SPELL;
	}
	assign_Spell_to_SpellEntry( &sp , 30680 ); 
	if( sp != NULL )
	{
		sp->EffectApplyAuraName[0] = 42;//proc something (we will owerride this)
		sp->EffectTriggerSpell[0] = 39805;//proc something (we will owerride this)
		sp->procFlags = PROC_ON_CAST_SPELL;
	}
	assign_Spell_to_SpellEntry( &sp , 30681 ); 
	if( sp != NULL )
	{
		sp->EffectApplyAuraName[0] = 42;//proc something (we will owerride this)
		sp->EffectTriggerSpell[0] = 39805;//proc something (we will owerride this)
		sp->procFlags = PROC_ON_CAST_SPELL;
	}

	//shaman - Purge 
	assign_Spell_to_SpellEntry( &sp , 370 ); 
	if( sp != NULL )
		sp->DispelType = DISPEL_MAGIC;
	assign_Spell_to_SpellEntry( &sp , 8012 ); 
	if( sp != NULL )
		sp->DispelType = DISPEL_MAGIC;
	assign_Spell_to_SpellEntry( &sp , 27626 ); 
	if( sp != NULL )
		sp->DispelType = DISPEL_MAGIC;
	assign_Spell_to_SpellEntry( &sp , 33625 ); 
	if( sp != NULL )
		sp->DispelType = DISPEL_MAGIC;

	//shaman - Elemental mastery 
	assign_Spell_to_SpellEntry( &sp , 16166 ); 
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 0xFFFFFFFF;//nature+fire+frost is all that shaman can do
		sp->EffectSpellGroupRelation[1] = 0xFFFFFFFF;//nature+fire+frost is all that shaman can do
	}

	//Shaman - Eye of the Storm
	assign_Spell_to_SpellEntry( &sp , 29062 );
	if( sp != NULL )
		sp->procFlags = PROC_ON_CRIT_HIT_VICTIM;
	assign_Spell_to_SpellEntry( &sp , 29064 );
	if( sp != NULL )
		sp->procFlags = PROC_ON_CRIT_HIT_VICTIM;
	assign_Spell_to_SpellEntry( &sp , 29065 );
	if( sp != NULL )
		sp->procFlags = PROC_ON_CRIT_HIT_VICTIM;

	//Shaman - Focused Casting
	assign_Spell_to_SpellEntry( &sp , 29063 );
	if( sp != NULL )
	{
	//        sp->EffectSpellGroupRelation[0] =  1 | 2 | 64 | 128 | 256;
		sp->EffectSpellGroupRelation[0] =  0xFFFFFFFF; // shaman spells. Guess that wraps them all 
	}

	//Shaman - Healing Focus
	assign_Spell_to_SpellEntry( &sp , 16181 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] =  64 | 128 | 256;
	assign_Spell_to_SpellEntry( &sp , 16230 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] =  64 | 128 | 256;
	assign_Spell_to_SpellEntry( &sp , 16232 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] =  64 | 128 | 256;
	assign_Spell_to_SpellEntry( &sp , 16233 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] =  64 | 128 | 256;
	assign_Spell_to_SpellEntry( &sp , 16234 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] =  64 | 128 | 256;

	//shaman - Improved Lightning shield 
	assign_Spell_to_SpellEntry( &sp , 16261 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 1024;
	assign_Spell_to_SpellEntry( &sp , 16290 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 1024;
	assign_Spell_to_SpellEntry( &sp , 16291 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 1024;

	//shaman - Tidal focus . Add more heal spells here if i forgot any :P
	assign_Spell_to_SpellEntry( &sp , 16179 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 64 | 128 | 256;
	assign_Spell_to_SpellEntry( &sp , 16214 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 64 | 128 | 256;
	assign_Spell_to_SpellEntry( &sp , 16215 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 64 | 128 | 256;
	assign_Spell_to_SpellEntry( &sp , 16216 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 64 | 128 | 256;
	assign_Spell_to_SpellEntry( &sp , 16217 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 64 | 128 | 256;

	//shaman ( grouping ) Enhancing Totems = Strength of Earth + Grace of Air
	uint32 group_relation_shaman_enhancing_totems = 0x00010000 | 0x00020000;

	//shaman - Enhancing Totems
	assign_Spell_to_SpellEntry( &sp , 16259 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = group_relation_shaman_enhancing_totems;
	assign_Spell_to_SpellEntry( &sp , 16295 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = group_relation_shaman_enhancing_totems;

	//shaman - Elemental Fury - ! Not finished !
	assign_Spell_to_SpellEntry( &sp , 16089 ); 
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 0xFFFFFFFF ; //damn, what other spells do there remain after that list ? Maybe later :P
		//sp->EffectSpellGroupRelation[0] = 1073741824 | 32 | 1048576 | 1 | ... ; //Searing/Magma/Fire Nova Totem effects and Fire,Frost,Nature spells
	}

	//shaman ( grouping ) Restorative Totems = Mana Spring + Healing Stream
	uint32 group_relation_shaman_restorative_totems = 0x00004000 | 0x00002000;

	//shaman - Restorative Totems
	assign_Spell_to_SpellEntry( &sp , 16259 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = group_relation_shaman_restorative_totems;
	assign_Spell_to_SpellEntry( &sp , 16205 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = group_relation_shaman_restorative_totems;
	assign_Spell_to_SpellEntry( &sp , 16206 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = group_relation_shaman_restorative_totems;
	assign_Spell_to_SpellEntry( &sp , 16207 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = group_relation_shaman_restorative_totems;
	assign_Spell_to_SpellEntry( &sp , 16208 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = group_relation_shaman_restorative_totems;
	
	//shaman - Healing Way
	assign_Spell_to_SpellEntry( &sp , 29202 ); 
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_CAST_SPELL;
		sp->EffectApplyAuraName[0] = 42;
	}
	assign_Spell_to_SpellEntry( &sp , 29205 ); 
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_CAST_SPELL;
		sp->EffectApplyAuraName[0] = 42;
	}
	assign_Spell_to_SpellEntry( &sp , 29206 ); 
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_CAST_SPELL;
		sp->EffectApplyAuraName[0] = 42;
	}

	//shaman - Elemental Devastation
	assign_Spell_to_SpellEntry( &sp , 29179 ); 
	if( sp != NULL )
		sp->procFlags = PROC_ON_SPELL_CRIT_HIT;
	assign_Spell_to_SpellEntry( &sp , 29180 ); 
	if( sp != NULL )
		sp->procFlags = PROC_ON_SPELL_CRIT_HIT;
	assign_Spell_to_SpellEntry( &sp , 30160 ); 
	if( sp != NULL )
		sp->procFlags = PROC_ON_SPELL_CRIT_HIT;

	//shaman - Ancestral healing
	assign_Spell_to_SpellEntry( &sp , 16176 ); 
	if( sp != NULL )
		sp->procFlags = PROC_ON_SPELL_CRIT_HIT;
	assign_Spell_to_SpellEntry( &sp , 16235 ); 
	if( sp != NULL )
		sp->procFlags = PROC_ON_SPELL_CRIT_HIT;
	assign_Spell_to_SpellEntry( &sp , 16240 ); 
	if( sp != NULL )
		sp->procFlags = PROC_ON_SPELL_CRIT_HIT;

	//shaman ( grouping ) Mental Quickness (missing 18 spells which have no grouping)
	uint32 group_relation_shaman_mental_quickness = 0x00000008 | 0x00000010 | 0x00000200 | 0x00000400 | 0x00080000 | 0x00100000 | 0x00400000 | 0x20000000 | 0x10000000 | 0x80000000;

	//shaman - Mental Quickness
	assign_Spell_to_SpellEntry( &sp , 30812 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = group_relation_shaman_mental_quickness;
	assign_Spell_to_SpellEntry( &sp , 30813 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = group_relation_shaman_mental_quickness;
	assign_Spell_to_SpellEntry( &sp , 30814 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = group_relation_shaman_mental_quickness;

	//shaman ( grouping ) Totems
	uint32 group_relation_shaman_totems = 0x00000008 | 0x00000010 | 0x00001000 | 0x00080000 | 0x20000000; //these are only selected flag since some totems contain more then 1 flag

	//shaman - Totemic focus
	assign_Spell_to_SpellEntry( &sp , 16173 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = group_relation_shaman_totems;
	assign_Spell_to_SpellEntry( &sp , 16222 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = group_relation_shaman_totems;
	assign_Spell_to_SpellEntry( &sp , 16223 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = group_relation_shaman_totems;
	assign_Spell_to_SpellEntry( &sp , 16224 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = group_relation_shaman_totems;
	assign_Spell_to_SpellEntry( &sp , 16225 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = group_relation_shaman_totems;

	//shaman ( grouping ) Lightning = Lightning Bolt + Chain Lightning
	uint32 group_relation_shaman_lightning = 0x00000001 | 0x00000002;

	//shaman - Call of Thunder
	assign_Spell_to_SpellEntry( &sp , 16041 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = group_relation_shaman_lightning;
	assign_Spell_to_SpellEntry( &sp , 16117 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = group_relation_shaman_lightning;
	assign_Spell_to_SpellEntry( &sp , 16118 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = group_relation_shaman_lightning;
	assign_Spell_to_SpellEntry( &sp , 16119 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = group_relation_shaman_lightning;
	assign_Spell_to_SpellEntry( &sp , 16120 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = group_relation_shaman_lightning;

    //shaman ( grouping ) Shock = Earth + Flame + Frost
	uint32 group_relation_shaman_shock = 0x00100000 | 0x10000000 | 0x80000000;

	//shaman - Convection
	assign_Spell_to_SpellEntry( &sp , 16039 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = group_relation_shaman_shock | group_relation_shaman_lightning;
	assign_Spell_to_SpellEntry( &sp , 16109 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = group_relation_shaman_shock | group_relation_shaman_lightning;
	assign_Spell_to_SpellEntry( &sp , 16110 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = group_relation_shaman_shock | group_relation_shaman_lightning;
	assign_Spell_to_SpellEntry( &sp , 16111 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = group_relation_shaman_shock | group_relation_shaman_lightning;
	assign_Spell_to_SpellEntry( &sp , 16112 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = group_relation_shaman_shock | group_relation_shaman_lightning;

    //shaman - Concussion
    assign_Spell_to_SpellEntry( &sp , 16035 );
    if( sp != NULL )
        sp->EffectSpellGroupRelation[0] = group_relation_shaman_shock | group_relation_shaman_lightning;
    assign_Spell_to_SpellEntry( &sp , 16105 );
    if( sp != NULL )
        sp->EffectSpellGroupRelation[0] = group_relation_shaman_shock | group_relation_shaman_lightning;
    assign_Spell_to_SpellEntry( &sp , 16106 );
    if( sp != NULL )
        sp->EffectSpellGroupRelation[0] = group_relation_shaman_shock | group_relation_shaman_lightning;
    assign_Spell_to_SpellEntry( &sp , 16107 );
    if( sp != NULL )
        sp->EffectSpellGroupRelation[0] = group_relation_shaman_shock | group_relation_shaman_lightning;
    assign_Spell_to_SpellEntry( &sp , 16108 );
    if( sp != NULL )
        sp->EffectSpellGroupRelation[0] = group_relation_shaman_shock | group_relation_shaman_lightning;

	//rogue ( grouping ) Elusiveness = blind + vanish
	uint32 group_relation_rogue_elusiveness = 0x00000800 | 0x01000000;

	//rogue - Elusiveness
	assign_Spell_to_SpellEntry( &sp , 13981 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = group_relation_rogue_elusiveness;
	assign_Spell_to_SpellEntry( &sp , 14066 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = group_relation_rogue_elusiveness;

	//rogue ( grouping ) Poisons
	uint32 group_relation_rogue_poisons = 0x00002000 | 0x00004000 | 0x00008000 | 0x00010000 | 0x10000000;

	//rogue - Vile Poisons
	assign_Spell_to_SpellEntry( &sp , 14168 ); 
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = group_relation_rogue_poisons;
		sp->EffectSpellGroupRelation[1] = 0x00800000; //maybe this is mixed up with 0 grouprelation ?
		sp->EffectSpellGroupRelation[2] = group_relation_rogue_poisons;
	}
	assign_Spell_to_SpellEntry( &sp , 16514 ); 
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = group_relation_rogue_poisons;
		sp->EffectSpellGroupRelation[1] = 0x00800000; //maybe this is mixed up with 0 grouprelation ?
		sp->EffectSpellGroupRelation[2] = group_relation_rogue_poisons;
	}
	assign_Spell_to_SpellEntry( &sp , 16515 ); 
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = group_relation_rogue_poisons;
		sp->EffectSpellGroupRelation[1] = 0x00800000; //maybe this is mixed up with 0 grouprelation ?
		sp->EffectSpellGroupRelation[2] = group_relation_rogue_poisons;
	}
	assign_Spell_to_SpellEntry( &sp , 16719 ); 
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = group_relation_rogue_poisons;
		sp->EffectSpellGroupRelation[1] = 0x00800000; //maybe this is mixed up with 0 grouprelation ?
		sp->EffectSpellGroupRelation[2] = group_relation_rogue_poisons;
	}
	assign_Spell_to_SpellEntry( &sp , 16720 ); 
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = group_relation_rogue_poisons;
		sp->EffectSpellGroupRelation[1] = 0x00800000; //maybe this is mixed up with 0 grouprelation ?
		sp->EffectSpellGroupRelation[2] = group_relation_rogue_poisons;
	}

	//rogue - Improved Poisons
	assign_Spell_to_SpellEntry( &sp , 14113 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = group_relation_rogue_poisons;
	assign_Spell_to_SpellEntry( &sp , 14114 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = group_relation_rogue_poisons;
	assign_Spell_to_SpellEntry( &sp , 14115 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = group_relation_rogue_poisons;
	assign_Spell_to_SpellEntry( &sp , 14116 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = group_relation_rogue_poisons;
	assign_Spell_to_SpellEntry( &sp , 14117 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = group_relation_rogue_poisons;
	assign_Spell_to_SpellEntry( &sp , 21881 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = group_relation_rogue_poisons;

	//rogue - Improved Expose Armor
	assign_Spell_to_SpellEntry( &sp , 14168 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 0x00080000;
	assign_Spell_to_SpellEntry( &sp , 14169 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 0x00080000;

	//rogue - Master Poisoner.
	assign_Spell_to_SpellEntry( &sp , 31226 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = group_relation_rogue_poisons;
	assign_Spell_to_SpellEntry( &sp , 31227 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = group_relation_rogue_poisons;

	//rogue - Find Weakness.
	assign_Spell_to_SpellEntry( &sp , 31233 ); 
	if( sp != NULL )
		sp->procFlags = PROC_ON_CAST_SPELL;
	assign_Spell_to_SpellEntry( &sp , 31239 ); 
	if( sp != NULL )
		sp->procFlags = PROC_ON_CAST_SPELL;
	assign_Spell_to_SpellEntry( &sp , 31240 ); 
	if( sp != NULL )
		sp->procFlags = PROC_ON_CAST_SPELL;
	assign_Spell_to_SpellEntry( &sp , 31241 ); 
	if( sp != NULL )
		sp->procFlags = PROC_ON_CAST_SPELL;
	assign_Spell_to_SpellEntry( &sp , 31242 ); 
	if( sp != NULL )
		sp->procFlags = PROC_ON_CAST_SPELL;

	//rogue ( grouping ) Find Weakness
	uint32 group_relation_rogue_find_weakness = 0x00000008 | 0x00000010 | 0x00000100 | 0x00100000 | 0x00800000 | 0x04000000 | 0x20000000;

	//rogue - Find Weakness. The effect
	assign_Spell_to_SpellEntry( &sp , 31234 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = group_relation_rogue_find_weakness;
	assign_Spell_to_SpellEntry( &sp , 31235 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = group_relation_rogue_find_weakness;
	assign_Spell_to_SpellEntry( &sp , 31236 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = group_relation_rogue_find_weakness;
	assign_Spell_to_SpellEntry( &sp , 31237 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = group_relation_rogue_find_weakness;
	assign_Spell_to_SpellEntry( &sp , 31238 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = group_relation_rogue_find_weakness;
	
	//rogue - Camouflage.
	assign_Spell_to_SpellEntry( &sp , 13975 ); 
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 4194304;
		sp->EffectMiscValue[0] = SMT_SPELL_VALUE;
		sp->EffectSpellGroupRelation[1] = 4194304;
	}
	assign_Spell_to_SpellEntry( &sp , 14062 ); 
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 4194304;
		sp->EffectMiscValue[0] = SMT_SPELL_VALUE;
		sp->EffectSpellGroupRelation[1] = 4194304;
	}
	assign_Spell_to_SpellEntry( &sp , 14063 ); 
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 4194304;
		sp->EffectMiscValue[0] = SMT_SPELL_VALUE;
		sp->EffectSpellGroupRelation[1] = 4194304;
	}
	assign_Spell_to_SpellEntry( &sp , 14064 ); 
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 4194304;
		sp->EffectMiscValue[0] = SMT_SPELL_VALUE;
		sp->EffectSpellGroupRelation[1] = 4194304;
	}
	assign_Spell_to_SpellEntry( &sp , 14065 ); 
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 4194304;
		sp->EffectMiscValue[0] = SMT_SPELL_VALUE;
		sp->EffectSpellGroupRelation[1] = 4194304;
	}

	//rogue - Mace Specialization.
	assign_Spell_to_SpellEntry( &sp , 13709 ); 
	if( sp != NULL )
		sp->procFlags = PROC_ON_MELEE_ATTACK;
	assign_Spell_to_SpellEntry( &sp , 13800 ); 
	if( sp != NULL )
		sp->procFlags = PROC_ON_MELEE_ATTACK;
	assign_Spell_to_SpellEntry( &sp , 13801 ); 
	if( sp != NULL )
		sp->procFlags = PROC_ON_MELEE_ATTACK;
	assign_Spell_to_SpellEntry( &sp , 13802 ); 
	if( sp != NULL )
		sp->procFlags = PROC_ON_MELEE_ATTACK;
	assign_Spell_to_SpellEntry( &sp , 13803 ); 
	if( sp != NULL )
		sp->procFlags = PROC_ON_MELEE_ATTACK;

	//rogue - Dirty Tricks 
	assign_Spell_to_SpellEntry( &sp , 14076 ); 
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 16777216 | 128; // blind + sap
		sp->EffectSpellGroupRelation[1] = 16777216 | 128; // blind + sap
	}
	assign_Spell_to_SpellEntry( &sp , 14094 ); 
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 16777216 | 128; // blind + sap
		sp->EffectSpellGroupRelation[1] = 16777216 | 128; // blind + sap
	}

	//rogue - Dirty Deeds
	assign_Spell_to_SpellEntry( &sp , 14082 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 1024 | 256; // Cheap Shot + Garrote
	assign_Spell_to_SpellEntry( &sp , 14083 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 1024 | 256; // Cheap Shot + Garrote


	//rogue ( grouping ) Shadowstep
	uint32 group_relation_rogue_shadow_step = 0;
	group_relation_rogue_shadow_step |= 512;//rogue - ambush (only a part of the whole group since it would affect other spells too)
	group_relation_rogue_shadow_step |= 4;//rogue - Backstab (only a part of the whole group since it would affect other spells too)
	group_relation_rogue_shadow_step |= 256;//Garrote
	group_relation_rogue_shadow_step |= 536870912 | 16 | 8 | 8389120 | 41943040 | 33554432 | 32 | 67108864 | 64 | 128 ;

	//rogue - Shadowstep
	assign_Spell_to_SpellEntry( &sp , 36563 ); 
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[1] = group_relation_rogue_shadow_step;
		sp->EffectMiscValue[1] = SMT_SPELL_VALUE;
	}

	//rogue ( grouping ) Lethality
	uint32 group_relation_rogue_lethality = 0;
	group_relation_rogue_lethality |= 2;//rogue - Sinister Strike (only a part of the whole group since it would affect other spells too)
	group_relation_rogue_lethality |= 4;//rogue - backstab (only a part of the whole group since it would affect other spells too)
	group_relation_rogue_lethality |= 8;//rogue - Gouge (only a part of the whole group since it would affect other spells too)
	group_relation_rogue_lethality |= 33554432UL;//rogue - Hemorrhage (only a part of the whole group since it would affect other spells too)
	group_relation_rogue_lethality |= 536870912UL;//rogue - Shiv (only a part of the whole group since it would affect other spells too)
	group_relation_rogue_lethality |= 1073741824UL;//rogue - Ghostly Strike (only a part of the whole group since it would affect other spells too)

	//rogue Lethality
	assign_Spell_to_SpellEntry( &sp , 14128 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = group_relation_rogue_lethality;
	assign_Spell_to_SpellEntry( &sp , 14132 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = group_relation_rogue_lethality;
	assign_Spell_to_SpellEntry( &sp , 14135 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = group_relation_rogue_lethality;
	assign_Spell_to_SpellEntry( &sp , 14136 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = group_relation_rogue_lethality;
	assign_Spell_to_SpellEntry( &sp , 14137 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = group_relation_rogue_lethality;

	//rogue - Endurance 
	assign_Spell_to_SpellEntry( &sp , 13742 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 64 | 32;	//Sprint + Evasion
	assign_Spell_to_SpellEntry( &sp , 13872 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 64 | 32;	//Sprint + Evasion

     //priest - Holy Concentration
     assign_Spell_to_SpellEntry( &sp , 34753 );
     if (sp != NULL)
          sp->procFlags = PROC_ON_CAST_SPELL;
     assign_Spell_to_SpellEntry( &sp , 34859 );
     if (sp != NULL)
	     sp->procFlags = PROC_ON_CAST_SPELL;
     assign_Spell_to_SpellEntry( &sp , 34860 );
     if (sp != NULL)
          sp->procFlags = PROC_ON_CAST_SPELL;
     assign_Spell_to_SpellEntry( &sp , 34754 );
     if (sp != NULL)
     {
          //sp->EffectSpellGroupRelation[0] = 2048 | 4096;
     }

	//priest - Focused Mind 
	assign_Spell_to_SpellEntry( &sp , 33213 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 8192 | 131072 | 8388608; // Mind Blast + Mind Control + Mind Flay
	assign_Spell_to_SpellEntry( &sp , 33214 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 8192 | 131072 | 8388608; // Mind Blast + Mind Control + Mind Flay
	assign_Spell_to_SpellEntry( &sp , 33215 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 8192 | 131072 | 8388608; // Mind Blast + Mind Control + Mind Flay
	//Priest: Blessed Recovery
	assign_Spell_to_SpellEntry( &sp , 27811);
	if(sp != NULL)
	{
		sp->EffectTriggerSpell[0] = 27813;
		sp->procFlags = PROC_ON_CRIT_HIT_VICTIM;
	}
	assign_Spell_to_SpellEntry( &sp , 27815);
	if(sp != NULL)
	{
		sp->EffectTriggerSpell[0] = 27817;
		sp->procFlags = PROC_ON_CRIT_HIT_VICTIM;
	}
	assign_Spell_to_SpellEntry( &sp , 27816);
	if(sp != NULL)
	{
		sp->EffectTriggerSpell[0] = 27818;
		sp->procFlags = PROC_ON_CRIT_HIT_VICTIM;
	}
	//priest- Blessed Resilience
	assign_Spell_to_SpellEntry( &sp , 33142 );
	if( sp != NULL )
		sp->procFlags = PROC_ON_CRIT_HIT_VICTIM;
	assign_Spell_to_SpellEntry( &sp , 33145 );
	if( sp != NULL )
		sp->procFlags = PROC_ON_CRIT_HIT_VICTIM;
	assign_Spell_to_SpellEntry( &sp , 33146 );
	if( sp != NULL )
		sp->procFlags = PROC_ON_CRIT_HIT_VICTIM;

	//priest- Focused Will
	assign_Spell_to_SpellEntry( &sp , 45234 );
	if( sp != NULL )
		sp->procFlags = PROC_ON_CRIT_HIT_VICTIM;
	assign_Spell_to_SpellEntry( &sp , 45243 );
	if( sp != NULL )
		sp->procFlags = PROC_ON_CRIT_HIT_VICTIM;
	assign_Spell_to_SpellEntry( &sp , 45244 );
	if( sp != NULL )
		sp->procFlags = PROC_ON_CRIT_HIT_VICTIM;

	//priest - Improved Divine Spirit 
	assign_Spell_to_SpellEntry( &sp , 33174 ); 
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 32 ;
		sp->EffectSpellGroupRelation[1] = 32 ;
	}
	assign_Spell_to_SpellEntry( &sp , 33182 ); 
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 32 ;
		sp->EffectSpellGroupRelation[1] = 32 ;
	}

	//priest - Empowered Healing 
	assign_Spell_to_SpellEntry( &sp , 33158 ); 
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 4096 ;
		sp->EffectSpellGroupRelation[1] = 2048 ;
	}
	assign_Spell_to_SpellEntry( &sp , 33159 ); 
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 4096 ;
		sp->EffectSpellGroupRelation[1] = 2048 ;
	}
	assign_Spell_to_SpellEntry( &sp , 33160 ); 
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 4096 ;
		sp->EffectSpellGroupRelation[1] = 2048 ;
	}
	assign_Spell_to_SpellEntry( &sp , 33161 ); 
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 4096 ;
		sp->EffectSpellGroupRelation[1] = 2048 ;
	}
	assign_Spell_to_SpellEntry( &sp , 33162 ); 
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 4096 ;
		sp->EffectSpellGroupRelation[1] = 2048 ;
	}

	//priest - Force of Will 
	assign_Spell_to_SpellEntry( &sp , 18544 ); 
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 4194304 | 128 | 32768 | 8192 | 16 | 1048576 | 8388608 | 2097152 | 67108864;
		sp->EffectSpellGroupRelation[1] = 4194304 | 128 | 32768 | 8192 | 16 | 1048576 | 8388608 | 2097152 | 67108864;
		sp->EffectSpellGroupRelation[2] = 0 ;	//1-2 mod the same ?
		sp->EffectSpellGroupRelation_high[0] = 2 | 1024 ;
		sp->EffectSpellGroupRelation_high[1] = 2 | 1024 ;
	}
	assign_Spell_to_SpellEntry( &sp , 18547 ); 
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 4194304 | 128 | 32768 | 8192 | 16 | 1048576 | 8388608 | 2097152 | 67108864;
		sp->EffectSpellGroupRelation[1] = 4194304 | 128 | 32768 | 8192 | 16 | 1048576 | 8388608 | 2097152 | 67108864;
		sp->EffectSpellGroupRelation[2] = 0 ;	//1-2 mod the same ?
		sp->EffectSpellGroupRelation_high[0] = 2 | 1024 ;
		sp->EffectSpellGroupRelation_high[1] = 2 | 1024 ;
	}
	assign_Spell_to_SpellEntry( &sp , 18548 ); 
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 4194304 | 128 | 32768 | 8192 | 16 | 1048576 | 8388608 | 2097152 | 67108864;
		sp->EffectSpellGroupRelation[1] = 4194304 | 128 | 32768 | 8192 | 16 | 1048576 | 8388608 | 2097152 | 67108864 ;
		sp->EffectSpellGroupRelation[2] = 0 ;	//1-2 mod the same ?
		sp->EffectSpellGroupRelation_high[0] = 2 | 1024 ;
		sp->EffectSpellGroupRelation_high[1] = 2 | 1024 ;
	}
	assign_Spell_to_SpellEntry( &sp , 18549 ); 
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 4194304 | 128 | 32768 | 8192 | 16 | 1048576 | 8388608 | 2097152 | 67108864;
		sp->EffectSpellGroupRelation[1] = 4194304 | 128 | 32768 | 8192 | 16 | 1048576 | 8388608 | 2097152 | 67108864;
		sp->EffectSpellGroupRelation[2] = 0;	//1-2 mod the same ?
		sp->EffectSpellGroupRelation_high[0] = 2 | 1024 ;
		sp->EffectSpellGroupRelation_high[1] = 2 | 1024 ;
	}
	assign_Spell_to_SpellEntry( &sp , 18550 ); 
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 4194304 | 128 | 32768 | 8192 | 16 | 1048576 | 8388608 | 2097152 | 67108864;
		sp->EffectSpellGroupRelation[1] = 4194304 | 128 | 32768 | 8192 | 16 | 1048576 | 8388608 | 2097152 | 67108864;
		sp->EffectSpellGroupRelation[2] = 0;	//1-2 mod the same ?
		sp->EffectSpellGroupRelation_high[0] = 2 | 1024;
		sp->EffectSpellGroupRelation_high[1] = 2 | 1024;
	}

	//Priest: Shadowguard
	assign_Spell_to_SpellEntry( &sp , 18137 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_SPELL_HIT_VICTIM | PROC_ON_RANGED_ATTACK_VICTIM | PROC_ON_MELEE_ATTACK_VICTIM;
		sp->proc_interval = 3000; //every 3 seconds
		sp->EffectTriggerSpell[0] = 28377;
	}
	assign_Spell_to_SpellEntry( &sp , 19308 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_SPELL_HIT_VICTIM | PROC_ON_RANGED_ATTACK_VICTIM | PROC_ON_MELEE_ATTACK_VICTIM;
		sp->proc_interval = 3000; //every 3 seconds
		sp->EffectTriggerSpell[0] = 28378;
	}
	assign_Spell_to_SpellEntry( &sp , 19309 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_SPELL_HIT_VICTIM | PROC_ON_RANGED_ATTACK_VICTIM | PROC_ON_MELEE_ATTACK_VICTIM;
		sp->proc_interval = 3000; //every 3 seconds
		sp->EffectTriggerSpell[0] = 28379;
	}
	assign_Spell_to_SpellEntry( &sp , 19310 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_SPELL_HIT_VICTIM | PROC_ON_RANGED_ATTACK_VICTIM | PROC_ON_MELEE_ATTACK_VICTIM;
		sp->proc_interval = 3000; //every 3 seconds
		sp->EffectTriggerSpell[0] = 28380;
	}
	assign_Spell_to_SpellEntry( &sp , 19311 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_SPELL_HIT_VICTIM | PROC_ON_RANGED_ATTACK_VICTIM | PROC_ON_MELEE_ATTACK_VICTIM;
		sp->proc_interval = 3000; //every 3 seconds
		sp->EffectTriggerSpell[0] = 28381;
	}
	assign_Spell_to_SpellEntry( &sp , 19312 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_SPELL_HIT_VICTIM | PROC_ON_RANGED_ATTACK_VICTIM | PROC_ON_MELEE_ATTACK_VICTIM;
		sp->proc_interval = 3000; //every 3 seconds
		sp->EffectTriggerSpell[0] = 28382;
	}
	assign_Spell_to_SpellEntry( &sp , 25477 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_SPELL_HIT_VICTIM | PROC_ON_RANGED_ATTACK_VICTIM | PROC_ON_MELEE_ATTACK_VICTIM;
		sp->proc_interval = 3000; //every 3 seconds
		sp->EffectTriggerSpell[0] = 28385;
	}

	//priest - Absolution 
	assign_Spell_to_SpellEntry( &sp , 33167 ); 
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 2147483648UL;
		sp->EffectSpellGroupRelation_high[0] = 1 | 128;
	}
	assign_Spell_to_SpellEntry( &sp , 33171 ); 
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 2147483648UL;
		sp->EffectSpellGroupRelation_high[0] = 1 | 128;
	}
	assign_Spell_to_SpellEntry( &sp , 33172 ); 
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 2147483648UL;
		sp->EffectSpellGroupRelation_high[0] = 1 | 128;
	}

	//priest - Mental Agility - all instant spells. I wonder if it conflicts with any other spells 
	assign_Spell_to_SpellEntry( &sp , 14520 ); 
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 2147483648UL | 65536 | 67108864UL | 4 | 1 | 64 | 32 | 4194304UL | 32768 | 8388608UL | 8 | 16384 | 2 | 256 | 16777216UL | 2097152UL | 33554432UL;
		sp->EffectSpellGroupRelation_high[0] = 1 | 64 | 2;
	}
	assign_Spell_to_SpellEntry( &sp , 14780 ); 
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 2147483648UL | 65536 | 67108864UL | 4 | 1 | 64 | 32 | 4194304UL | 32768 | 8388608UL | 8 | 16384 | 2 | 256 | 16777216UL | 2097152UL | 33554432UL;
		sp->EffectSpellGroupRelation_high[0] = 1 | 64 | 2;
	}
	assign_Spell_to_SpellEntry( &sp , 14781 ); 
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 2147483648UL | 65536 | 67108864UL | 4 | 1 | 64 | 32 | 4194304UL | 32768 | 8388608UL | 8 | 16384 | 2 | 256 | 16777216UL | 2097152UL | 33554432UL;
		sp->EffectSpellGroupRelation_high[0] = 1 | 64 | 2;
	}
	assign_Spell_to_SpellEntry( &sp , 14782 ); 
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 2147483648UL | 65536 | 67108864UL | 4 | 1 | 64 | 32 | 4194304UL | 32768 | 8388608UL | 8 | 16384 | 2 | 256 | 16777216UL | 2097152UL | 33554432UL;
		sp->EffectSpellGroupRelation_high[0] = 1 | 64 | 2;
	}
	assign_Spell_to_SpellEntry( &sp , 14783 ); 
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 2147483648UL | 65536 | 67108864UL | 4 | 1 | 64 | 32 | 4194304UL | 32768 | 8388608UL | 8 | 16384 | 2 | 256 | 16777216UL | 2097152UL | 33554432UL;
		sp->EffectSpellGroupRelation_high[0] = 1 | 64 | 2;
	}

	//priest - Focused Power
	assign_Spell_to_SpellEntry( &sp , 33186 ); 
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 2147483648UL;
		sp->EffectSpellGroupRelation[1] = 128 | 8192 | 2147483648UL;
	}
	assign_Spell_to_SpellEntry( &sp , 33190 ); 
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 2147483648UL;
		sp->EffectSpellGroupRelation[1] = 128 | 8192 | 2147483648UL;
	}

	//priest - Shadow Reach 
	assign_Spell_to_SpellEntry( &sp , 17322 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 32768 | 65536 | 4 | 8192 | 16 | 8388608 | 2147483648UL;
	assign_Spell_to_SpellEntry( &sp , 17323 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 32768 | 65536 | 4 | 8192 | 16 | 8388608 | 2147483648UL;

	//priest - Shadow Focus 
	assign_Spell_to_SpellEntry( &sp , 15260 ); 
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 65536 | 67108864 | 131072 | 2147483648UL | 32768 | 8192 | 16 | 4 | 8388608 | 16384 | 256 | 33554432;
		sp->EffectSpellGroupRelation_high[0] = 64 | 2 | 1024;
	}
	assign_Spell_to_SpellEntry( &sp , 15327 ); 
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 65536 | 67108864 | 131072 | 2147483648UL | 32768 | 8192 | 16 | 4 | 8388608 | 16384 | 256 | 33554432;
		sp->EffectSpellGroupRelation_high[0] = 64 | 2 | 1024;
	}
	assign_Spell_to_SpellEntry( &sp , 15328 ); 
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 65536 | 67108864 | 131072 | 2147483648UL | 32768 | 8192 | 16 | 4 | 8388608 | 16384 | 256 | 33554432;
		sp->EffectSpellGroupRelation_high[0] = 64 | 2 | 1024;
	}
	assign_Spell_to_SpellEntry( &sp , 15329 ); 
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 65536 | 67108864 | 131072 | 2147483648UL | 32768 | 8192 | 16 | 4 | 8388608 | 16384 | 256 | 33554432;
		sp->EffectSpellGroupRelation_high[0] = 64 | 2 | 1024;
	}
	assign_Spell_to_SpellEntry( &sp , 15330 ); 
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 65536 | 67108864 | 131072 | 2147483648UL | 32768 | 8192 | 16 | 4 | 8388608 | 16384 | 256 | 33554432;
		sp->EffectSpellGroupRelation_high[0] = 64 | 2 | 1024;
	}

	//Paladin: Seal of Wisdom
	uint32 procchance = 0;
	assign_Spell_to_SpellEntry( &sp , 27116 );
	if( sp != NULL )
		procchance = sp->procChance;
	assign_Spell_to_SpellEntry( &sp , 20166 );
	if( sp != NULL )
		sp->procChance = procchance;
	assign_Spell_to_SpellEntry( &sp , 20356 );
	if( sp != NULL )
		sp->procChance = procchance;
	assign_Spell_to_SpellEntry( &sp , 20357 );
	if( sp != NULL )
		sp->procChance = procchance;
	assign_Spell_to_SpellEntry( &sp , 27166 );
	if( sp != NULL )
		sp->procChance = procchance;

	//Druid: Natural Perfection
	assign_Spell_to_SpellEntry( &sp , 33881 );
	if ( sp != NULL )
		sp->procFlags = PROC_ON_CRIT_HIT_VICTIM;
	assign_Spell_to_SpellEntry( &sp , 33882 );
	if ( sp != NULL )
		sp->procFlags = PROC_ON_CRIT_HIT_VICTIM;
	assign_Spell_to_SpellEntry( &sp , 33883 );
	if ( sp != NULL )
		sp->procFlags = PROC_ON_CRIT_HIT_VICTIM;

	//Druid: Frenzied Regeneration
	assign_Spell_to_SpellEntry( &sp , 22842 );
	if( sp != NULL )
	{
		sp->Effect[0] = 6;
		sp->EffectApplyAuraName[0] = 23;
		sp->EffectTriggerSpell[0] = 22845;
	}
	assign_Spell_to_SpellEntry( &sp , 22895 );
	if( sp != NULL )
	{
		sp->Effect[0] = 6;
		sp->EffectApplyAuraName[0] = 23;
		sp->EffectTriggerSpell[0] = 22845;
	}
	assign_Spell_to_SpellEntry( &sp , 22896 );
	if( sp != NULL )
	{
		sp->Effect[0] = 6;
		sp->EffectApplyAuraName[0] = 23;
		sp->EffectTriggerSpell[0] = 22845;
	}
	assign_Spell_to_SpellEntry( &sp , 26999 );
	if( sp != NULL )
	{
		sp->Effect[0] = 6;
		sp->EffectApplyAuraName[0] = 23;
		sp->EffectTriggerSpell[0] = 22845;
	}

	//Druid - Ferocity.
	assign_Spell_to_SpellEntry( &sp , 16934 ); 
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 2048;
		sp->EffectSpellGroupRelation_high[0] = 1048576 | 64;
		sp->EffectSpellGroupRelation[1] = 4096;
		sp->EffectSpellGroupRelation_high[1] = 1024;
	}
	assign_Spell_to_SpellEntry( &sp , 16935 ); 
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 2048;
		sp->EffectSpellGroupRelation_high[0] = 1048576 | 64;
		sp->EffectSpellGroupRelation[1] = 4096;
		sp->EffectSpellGroupRelation_high[1] = 1024;
	}
	assign_Spell_to_SpellEntry( &sp , 16936 ); 
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 2048;
		sp->EffectSpellGroupRelation_high[0] = 1048576 | 64;
		sp->EffectSpellGroupRelation[1] = 4096;
		sp->EffectSpellGroupRelation_high[1] = 1024;
	}
	assign_Spell_to_SpellEntry( &sp , 16937 ); 
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 2048;
		sp->EffectSpellGroupRelation_high[0] = 1048576 | 64;
		sp->EffectSpellGroupRelation[1] = 4096;
		sp->EffectSpellGroupRelation_high[1] = 1024;
	}
	assign_Spell_to_SpellEntry( &sp , 16938 ); 
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 2048;
		sp->EffectSpellGroupRelation_high[0] = 1048576 | 64;
		sp->EffectSpellGroupRelation[1] = 4096;
		sp->EffectSpellGroupRelation_high[1] = 1024;
	}

	//Druid - Focused Starlight
	assign_Spell_to_SpellEntry( &sp , 35363 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 1 | 4;
	assign_Spell_to_SpellEntry( &sp , 35364 ); 
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 1 | 4;

	//Druid - Celestial Focus
	assign_Spell_to_SpellEntry( &sp , 16850 ); 
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_CAST_SPELL;
		sp->EffectSpellGroupRelation[1] = 1;
	}
	assign_Spell_to_SpellEntry( &sp , 16923 ); 
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_CAST_SPELL;
		sp->EffectSpellGroupRelation[1] = 1;
	}
	assign_Spell_to_SpellEntry( &sp , 16924 ); 
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_CAST_SPELL;
		sp->EffectSpellGroupRelation[1] = 1;
	}

	//Druid - Feral Aggression. Blizz made a mistake here ?
	assign_Spell_to_SpellEntry( &sp , 16858 ); 
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 8;
		sp->EffectSpellGroupRelation[1] = 8388608;
	}
	assign_Spell_to_SpellEntry( &sp , 16859 ); 
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 8;
		sp->EffectSpellGroupRelation[1] = 8388608;
	}
	assign_Spell_to_SpellEntry( &sp , 16860 ); 
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 8;
		sp->EffectSpellGroupRelation[1] = 8388608;
	}
	assign_Spell_to_SpellEntry( &sp , 16861 ); 
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 8;
		sp->EffectSpellGroupRelation[1] = 8388608;
	}
	assign_Spell_to_SpellEntry( &sp , 16862 ); 
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 8;
		sp->EffectSpellGroupRelation[1] = 8388608;
	}

	//paladin - seal of blood
	assign_Spell_to_SpellEntry( &sp , 31892 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_MELEE_ATTACK;
		sp->EffectApplyAuraName[0] = 42;
		sp->EffectTriggerSpell[0] = 31893;
	}
	assign_Spell_to_SpellEntry( &sp , 38008 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_MELEE_ATTACK;
		sp->EffectApplyAuraName[0] = 42;
		sp->EffectTriggerSpell[0] = 31893;
	}

	//paladin - Spiritual Attunement 
	assign_Spell_to_SpellEntry( &sp , 31785 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_SPELL_HIT_VICTIM | PROC_TARGET_SELF ;
		sp->EffectApplyAuraName[0] = 42;
		sp->EffectTriggerSpell[0] = 31786;
	}
	assign_Spell_to_SpellEntry( &sp , 33776 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_SPELL_HIT_VICTIM | PROC_TARGET_SELF;
		sp->EffectApplyAuraName[0] = 42;
		sp->EffectTriggerSpell[0] = 31786;
	}

	//Druid: Leader of the Pack
	assign_Spell_to_SpellEntry( &sp , 24932 );
	if( sp != NULL )
	{
		sp->Effect[1] = 0;
		sp->Effect[2] = 0; //removing strange effects.
	}

	//Druid: Improved Leader of the Pack
	assign_Spell_to_SpellEntry( &sp , 34299 );
	if( sp != NULL )
		sp->proc_interval = 6000;//6 secs

	//fix for the right Enchant ID for Enchant Cloak - Major Resistance
	assign_Spell_to_SpellEntry( &sp , 27962 );
	if( sp != NULL )
		sp->EffectMiscValue[0] = 2998;
	assign_Spell_to_SpellEntry( &sp , 36285 );
	if( sp != NULL )
		sp->EffectMiscValue[0] = 2998;

	//muhaha, rewriting Retaliation spell as old one :D
	assign_Spell_to_SpellEntry( &sp , 20230 );
	if( sp != NULL )
	{
		sp->Effect[0] = 6; //aura
		sp->EffectApplyAuraName[0] = 42; //force him to use procspell effect
		sp->EffectTriggerSpell[0] = 22858; //evil , but this is good for us :D
		sp->procFlags = PROC_ON_MELEE_ATTACK_VICTIM; //add procflag here since this was not processed with the others !
	}

	//"bloodthirst" new version is ok but old version is wrong from now on :(
	assign_Spell_to_SpellEntry( &sp , 23881 );
	if( sp != NULL )
	{
		sp->Effect[1] = 64; //cast on us, it is good
		sp->EffectTriggerSpell[1] = 23885; //evil , but this is good for us :D
	}
	assign_Spell_to_SpellEntry( &sp , 23892 );
	if( sp != NULL )
	{
		sp->Effect[1] = 64;
		sp->EffectTriggerSpell[1] = 23886; //evil , but this is good for us :D
	}
	assign_Spell_to_SpellEntry( &sp , 23893 );
	if( sp != NULL )
	{
		sp->Effect[1] = 64; //
		sp->EffectTriggerSpell[1] = 23887; //evil , but this is good for us :D
	}
	assign_Spell_to_SpellEntry( &sp , 23894 );
	if( sp != NULL )
	{
		sp->Effect[1] = 64; //
		sp->EffectTriggerSpell[1] = 23888; //evil , but this is good for us :D
	}
	assign_Spell_to_SpellEntry( &sp , 25251 );
	if( sp != NULL )
	{
		sp->Effect[1] = 64; //aura
		sp->EffectTriggerSpell[1] = 25252; //evil , but this is good for us :D
	}
	assign_Spell_to_SpellEntry( &sp , 30335 );
	if( sp != NULL )
	{
		sp->Effect[1] = 64; //aura
		sp->EffectTriggerSpell[1] = 30339; //evil , but this is good for us :D
	}

	// Hunter - Master Tactician
	assign_Spell_to_SpellEntry( &sp , 34506 );
	if( sp != NULL )
		sp->procFlags = PROC_ON_RANGED_ATTACK | PROC_TARGET_SELF;
	assign_Spell_to_SpellEntry( &sp , 34507 );
	if( sp != NULL )
		sp->procFlags = PROC_ON_RANGED_ATTACK | PROC_TARGET_SELF;
	assign_Spell_to_SpellEntry( &sp , 34508 );
	if( sp != NULL )
		sp->procFlags = PROC_ON_RANGED_ATTACK | PROC_TARGET_SELF;
	assign_Spell_to_SpellEntry( &sp , 34838 );
	if( sp != NULL )
		sp->procFlags = PROC_ON_RANGED_ATTACK | PROC_TARGET_SELF;
	assign_Spell_to_SpellEntry( &sp , 34839 );
	if( sp != NULL )
		sp->procFlags = PROC_ON_RANGED_ATTACK | PROC_TARGET_SELF;

	// Hunter - Spirit Bond
	assign_Spell_to_SpellEntry( &sp , 19578 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_PET_OWNER | SPELL_FLAG_IS_EXPIREING_WITH_PET;
		sp->Effect[0] = SPELL_EFFECT_TRIGGER_SPELL;
		sp->EffectTriggerSpell[0] = 19579;
	}
	assign_Spell_to_SpellEntry( &sp , 20895 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_PET_OWNER | SPELL_FLAG_IS_EXPIREING_WITH_PET;
		sp->Effect[0] = SPELL_EFFECT_TRIGGER_SPELL;
		sp->EffectTriggerSpell[0] = 24529;
	}
	assign_Spell_to_SpellEntry( &sp , 19579 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_EXPIREING_WITH_PET;
		sp->Effect[1] = SPELL_EFFECT_APPLY_AURA; //we should do the same for player too as we did for pet
		sp->EffectApplyAuraName[1] = sp->EffectApplyAuraName[0];
		sp->EffectImplicitTargetA[1] = EFF_TARGET_PET;
		sp->EffectBasePoints[1] = sp->EffectBasePoints[0];
		sp->EffectAmplitude[1] = sp->EffectAmplitude[0];
		sp->EffectDieSides[1] = sp->EffectDieSides[0];
	}
	assign_Spell_to_SpellEntry( &sp , 24529 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_EXPIREING_WITH_PET;
		sp->Effect[1] = SPELL_EFFECT_APPLY_AURA; //we should do the same for player too as we did for pet
		sp->EffectApplyAuraName[1] = sp->EffectApplyAuraName[0];
		sp->EffectImplicitTargetA[1] = EFF_TARGET_PET;
		sp->EffectBasePoints[1] = sp->EffectBasePoints[0];
		sp->EffectAmplitude[1] = sp->EffectAmplitude[0];
		sp->EffectDieSides[1] = sp->EffectDieSides[0];
	}

	// Hunter - Animal Handler
	assign_Spell_to_SpellEntry( &sp , 34453 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_ON_PET;
		sp->EffectApplyAuraName[1] = SPELL_AURA_MOD_HIT_CHANCE;
		sp->EffectImplicitTargetA[1] = EFF_TARGET_PET;
	}
	assign_Spell_to_SpellEntry( &sp , 34454 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_ON_PET;
		sp->EffectApplyAuraName[1] = SPELL_AURA_MOD_HIT_CHANCE;
		sp->EffectImplicitTargetA[1] = EFF_TARGET_PET;
	}

	// Hunter - Catlike Reflexes
	assign_Spell_to_SpellEntry( &sp , 34462 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_ON_PET;
		sp->EffectApplyAuraName[1] = sp->EffectApplyAuraName[0];
		sp->EffectImplicitTargetA[1] = EFF_TARGET_PET;
	}
	assign_Spell_to_SpellEntry( &sp , 34464 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_ON_PET;
		sp->EffectApplyAuraName[1] = sp->EffectApplyAuraName[0];
		sp->EffectImplicitTargetA[1] = EFF_TARGET_PET;
	}
	assign_Spell_to_SpellEntry( &sp , 34465 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_ON_PET;
		sp->EffectApplyAuraName[1] = sp->EffectApplyAuraName[0];
		sp->EffectImplicitTargetA[1] = EFF_TARGET_PET;
	}

	// Hunter - Serpent's Swiftness
	assign_Spell_to_SpellEntry( &sp , 34466 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_ON_PET ;
		sp->EffectApplyAuraName[1] = SPELL_AURA_MOD_HASTE;
		sp->EffectImplicitTargetA[1] = EFF_TARGET_PET;
	}
	assign_Spell_to_SpellEntry( &sp , 34467 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_ON_PET ;
		sp->EffectApplyAuraName[1] = SPELL_AURA_MOD_HASTE;
		sp->EffectImplicitTargetA[1] = EFF_TARGET_PET;
	}
	assign_Spell_to_SpellEntry( &sp , 34468 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_ON_PET ;
		sp->EffectApplyAuraName[1] = SPELL_AURA_MOD_HASTE;
		sp->EffectImplicitTargetA[1] = EFF_TARGET_PET;
	}
	assign_Spell_to_SpellEntry( &sp , 34469 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_ON_PET ;
		sp->EffectApplyAuraName[1] = SPELL_AURA_MOD_HASTE;
		sp->EffectImplicitTargetA[1] = EFF_TARGET_PET;
	}
	assign_Spell_to_SpellEntry( &sp , 34470 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_ON_PET ;
		sp->EffectApplyAuraName[1] = SPELL_AURA_MOD_HASTE;
		sp->EffectImplicitTargetA[1] = EFF_TARGET_PET;
	}

	// Hunter - Ferocious Inspiration
	assign_Spell_to_SpellEntry( &sp , 34455 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_ON_PET;
		sp->EffectApplyAuraName[0] = SPELL_AURA_PROC_TRIGGER_SPELL;
		sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
		sp->EffectTriggerSpell[0] = 34456;
		sp->procFlags = PROC_ON_SPELL_CRIT_HIT | PROC_TARGET_SELF; //maybe target master ?
		sp->Effect[1] = 0; //remove this
	}
	assign_Spell_to_SpellEntry( &sp , 34459 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_ON_PET;
		sp->EffectApplyAuraName[0] = SPELL_AURA_PROC_TRIGGER_SPELL;
		sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
		sp->EffectTriggerSpell[0] = 34456;
		sp->procFlags = PROC_ON_SPELL_CRIT_HIT | PROC_TARGET_SELF; 
		sp->Effect[1] = 0; //remove this
	}
	assign_Spell_to_SpellEntry( &sp , 34460 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_ON_PET;
		sp->EffectApplyAuraName[0] = SPELL_AURA_PROC_TRIGGER_SPELL;
		sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
		sp->EffectTriggerSpell[0] = 34456;
		sp->procFlags = PROC_ON_SPELL_CRIT_HIT | PROC_TARGET_SELF;
		sp->Effect[1] = 0; //remove this
	}

	// Hunter - Focused Fire
	assign_Spell_to_SpellEntry( &sp , 35029 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_PET_OWNER | SPELL_FLAG_IS_EXPIREING_WITH_PET;
		sp->Effect[0] = SPELL_EFFECT_TRIGGER_SPELL;
		sp->EffectTriggerSpell[0] = 35060;
		sp->EffectSpellGroupRelation_high[1] = 2048;
	}
	assign_Spell_to_SpellEntry( &sp , 35030 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_PET_OWNER | SPELL_FLAG_IS_EXPIREING_WITH_PET;
		sp->Effect[0] = SPELL_EFFECT_TRIGGER_SPELL;
		sp->EffectTriggerSpell[0] = 35061;
		sp->EffectSpellGroupRelation_high[1] = 2048;
	}
	assign_Spell_to_SpellEntry( &sp , 35060 );
	if( sp != NULL )
		sp->c_is_flags |= SPELL_FLAG_IS_EXPIREING_WITH_PET;
	assign_Spell_to_SpellEntry( &sp , 35061 );
	if( sp != NULL )
		sp->c_is_flags |= SPELL_FLAG_IS_EXPIREING_WITH_PET;

	// Hunter - Thick Hide
	assign_Spell_to_SpellEntry( &sp , 19609 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_PET_OWNER ;
		sp->EffectApplyAuraName[0] = SPELL_AURA_MOD_RESISTANCE; //we do not support armor rating for pets yet !
		sp->EffectBasePoints[0] *= 10; //just give it a little juice :P
		sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
	}
	assign_Spell_to_SpellEntry( &sp , 19610 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_PET_OWNER ;
		sp->EffectApplyAuraName[0] = SPELL_AURA_MOD_RESISTANCE; //we do not support armor rating for pets yet !
		sp->EffectBasePoints[0] *= 10; //just give it a little juice :P
		sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
	}
	assign_Spell_to_SpellEntry( &sp , 19612 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_PET_OWNER ;
		sp->EffectApplyAuraName[0] = SPELL_AURA_MOD_RESISTANCE; //we do not support armor rating for pets yet !
		sp->EffectBasePoints[0] *= 10; //just give it a little juice :P
		sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
	}

	// Hunter - Ferocity
	assign_Spell_to_SpellEntry( &sp , 19612 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_ON_PET;
		sp->EffectApplyAuraName[0] = SPELL_AURA_MOD_CRIT_PERCENT;
		sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
	}
	assign_Spell_to_SpellEntry( &sp , 19599 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_ON_PET;
		sp->EffectApplyAuraName[0] = SPELL_AURA_MOD_CRIT_PERCENT;
		sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
	}
	assign_Spell_to_SpellEntry( &sp , 19600 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_ON_PET;
		sp->EffectApplyAuraName[0] = SPELL_AURA_MOD_CRIT_PERCENT;
		sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
	}
	assign_Spell_to_SpellEntry( &sp , 19601 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_ON_PET;
		sp->EffectApplyAuraName[0] = SPELL_AURA_MOD_CRIT_PERCENT;
		sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
	}
	assign_Spell_to_SpellEntry( &sp , 19602 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_ON_PET;
		sp->EffectApplyAuraName[0] = SPELL_AURA_MOD_CRIT_PERCENT;
		sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
	}

	// Hunter - Bestial Swiftness
	assign_Spell_to_SpellEntry( &sp , 19596 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_ON_PET;
		sp->EffectApplyAuraName[0] = SPELL_AURA_MOD_INCREASE_SPEED; 
		sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
	}

	// Hunter - Endurance Training
	assign_Spell_to_SpellEntry( &sp , 19583 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_PET_OWNER ;
		sp->EffectApplyAuraName[0] = SPELL_AURA_MOD_INCREASE_HEALTH_PERCENT;
		sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
	}
	assign_Spell_to_SpellEntry( &sp , 19584 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_PET_OWNER ;
		sp->EffectApplyAuraName[0] = SPELL_AURA_MOD_INCREASE_HEALTH_PERCENT;
		sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
	}
	assign_Spell_to_SpellEntry( &sp , 19585 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_PET_OWNER ;
		sp->EffectApplyAuraName[0] = SPELL_AURA_MOD_INCREASE_HEALTH_PERCENT;
		sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
	}
	assign_Spell_to_SpellEntry( &sp , 19586 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_PET_OWNER ;
		sp->EffectApplyAuraName[0] = SPELL_AURA_MOD_INCREASE_HEALTH_PERCENT;
		sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
	}
	assign_Spell_to_SpellEntry( &sp , 19587 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_PET_OWNER ;
		sp->EffectApplyAuraName[0] = SPELL_AURA_MOD_INCREASE_HEALTH_PERCENT;
		sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
	}

	// Hunter - Thrill of the Hunt
	assign_Spell_to_SpellEntry( &sp , 34497 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_SPELL_CRIT_HIT | PROC_TARGET_SELF;
		sp->procChance = sp->EffectBasePoints[0]+1;
		sp->EffectApplyAuraName[0] = 42;
		sp->EffectTriggerSpell[0] = 34720;
	}
	assign_Spell_to_SpellEntry( &sp , 34498 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_SPELL_CRIT_HIT | PROC_TARGET_SELF;
		sp->procChance = sp->EffectBasePoints[0]+1;
		sp->EffectApplyAuraName[0] = 42;
		sp->EffectTriggerSpell[0] = 34720;
	}
	assign_Spell_to_SpellEntry( &sp , 34499 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_SPELL_CRIT_HIT | PROC_TARGET_SELF;
		sp->procChance = sp->EffectBasePoints[0]+1;
		sp->EffectApplyAuraName[0] = 42;
		sp->EffectTriggerSpell[0] = 34720;
	}

	// Hunter - Expose Weakness
	assign_Spell_to_SpellEntry( &sp , 34500 );
	if( sp != NULL )
		sp->procFlags = PROC_ON_RANGED_CRIT_ATTACK;
	assign_Spell_to_SpellEntry( &sp , 34502 );
	if( sp != NULL )
		sp->procFlags = PROC_ON_RANGED_CRIT_ATTACK;
	assign_Spell_to_SpellEntry( &sp , 34503 );
	if( sp != NULL )
		sp->procFlags = PROC_ON_RANGED_CRIT_ATTACK;

	// Hunter - Hawk Eye
	assign_Spell_to_SpellEntry( &sp , 19498 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 1 | 4;
	assign_Spell_to_SpellEntry( &sp , 19499 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 1 | 4;
	assign_Spell_to_SpellEntry( &sp , 19500 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 1 | 4;

	//Hunter - Frenzy
	assign_Spell_to_SpellEntry( &sp , 19621 );
	if( sp != NULL )
	{
		sp->EffectApplyAuraName[0] = SPELL_AURA_PROC_TRIGGER_SPELL;
		sp->EffectTriggerSpell[0] = 19615;
		sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
		sp->procChance = sp->EffectBasePoints[0];
		sp->procFlags = PROC_ON_CRIT_ATTACK;
		sp->c_is_flags |= SPELL_FLAG_IS_EXPIREING_WITH_PET | SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_ON_PET | PROC_TARGET_SELF;
	}
	assign_Spell_to_SpellEntry( &sp , 19622 );
	if( sp != NULL )
	{
		sp->EffectApplyAuraName[0] = SPELL_AURA_PROC_TRIGGER_SPELL;
		sp->EffectTriggerSpell[0] = 19615;
		sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
		sp->procChance = sp->EffectBasePoints[0];
		sp->procFlags = PROC_ON_CRIT_ATTACK;
		sp->c_is_flags |= SPELL_FLAG_IS_EXPIREING_WITH_PET | SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_ON_PET | PROC_TARGET_SELF;
	}
	assign_Spell_to_SpellEntry( &sp , 19623 );
	if( sp != NULL )
	{
		sp->EffectApplyAuraName[0] = SPELL_AURA_PROC_TRIGGER_SPELL;
		sp->EffectTriggerSpell[0] = 19615;
		sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
		sp->procChance = sp->EffectBasePoints[0];
		sp->procFlags = PROC_ON_CRIT_ATTACK;
		sp->c_is_flags |= SPELL_FLAG_IS_EXPIREING_WITH_PET | SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_ON_PET | PROC_TARGET_SELF;
	}
	assign_Spell_to_SpellEntry( &sp , 19624 );
	if( sp != NULL )
	{
		sp->EffectApplyAuraName[0] = SPELL_AURA_PROC_TRIGGER_SPELL;
		sp->EffectTriggerSpell[0] = 19615;
		sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
		sp->procChance = sp->EffectBasePoints[0];
		sp->procFlags = PROC_ON_CRIT_ATTACK;
		sp->c_is_flags |= SPELL_FLAG_IS_EXPIREING_WITH_PET | SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_ON_PET | PROC_TARGET_SELF;
	}
	assign_Spell_to_SpellEntry( &sp , 19625 );
	if( sp != NULL )
	{
		sp->EffectApplyAuraName[0] = SPELL_AURA_PROC_TRIGGER_SPELL;
		sp->EffectTriggerSpell[0] = 19615;
		sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
		sp->procChance = sp->EffectBasePoints[0];
		sp->procFlags = PROC_ON_CRIT_ATTACK;
		sp->c_is_flags |= SPELL_FLAG_IS_EXPIREING_WITH_PET | SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_ON_PET | PROC_TARGET_SELF;
	}

	//Hunter -  Unleashed Fury
	assign_Spell_to_SpellEntry( &sp , 19616 );
	if( sp != NULL )
	{
		sp->EffectApplyAuraName[0] = SPELL_AURA_MOD_DAMAGE_PERCENT_DONE;
		sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
		sp->EffectMiscValue[0] = 1; //tweekign melee dmg
		sp->c_is_flags |= SPELL_FLAG_IS_EXPIREING_WITH_PET | SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_ON_PET;
	}
	assign_Spell_to_SpellEntry( &sp , 19617 );
	if( sp != NULL )
	{
		sp->EffectApplyAuraName[0] = SPELL_AURA_MOD_DAMAGE_PERCENT_DONE;
		sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
		sp->EffectMiscValue[0] = 1; //tweekign melee dmg
		sp->c_is_flags |= SPELL_FLAG_IS_EXPIREING_WITH_PET | SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_ON_PET;
	}
	assign_Spell_to_SpellEntry( &sp , 19618 );
	if( sp != NULL )
	{
		sp->EffectApplyAuraName[0] = SPELL_AURA_MOD_DAMAGE_PERCENT_DONE;
		sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
		sp->EffectMiscValue[0] = 1; //tweekign melee dmg
		sp->c_is_flags |= SPELL_FLAG_IS_EXPIREING_WITH_PET | SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_ON_PET;
	}
	assign_Spell_to_SpellEntry( &sp , 19619 );
	if( sp != NULL )
	{
		sp->EffectApplyAuraName[0] = SPELL_AURA_MOD_DAMAGE_PERCENT_DONE;
		sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
		sp->EffectMiscValue[0] = 1; //tweekign melee dmg
		sp->c_is_flags |= SPELL_FLAG_IS_EXPIREING_WITH_PET | SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_ON_PET;
	}
	assign_Spell_to_SpellEntry( &sp , 19620 );
	if( sp != NULL )
	{
		sp->EffectApplyAuraName[0] = SPELL_AURA_MOD_DAMAGE_PERCENT_DONE;
		sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
		sp->EffectMiscValue[0] = 1; //tweekign melee dmg
		sp->c_is_flags |= SPELL_FLAG_IS_EXPIREING_WITH_PET | SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_ON_PET;
	}

	//Hunter : Pathfinding
	assign_Spell_to_SpellEntry( &sp , 19559 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 2097152;
		sp->EffectMiscValue[0] = SMT_SPELL_VALUE;
	}
	assign_Spell_to_SpellEntry( &sp , 19560 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 2097152;
		sp->EffectMiscValue[0] = SMT_SPELL_VALUE;
	}
	assign_Spell_to_SpellEntry( &sp , 5131 );
	if( sp != NULL )
		sp->SpellGroupType = 2097152;
	assign_Spell_to_SpellEntry( &sp , 13160 );
	if( sp != NULL )
		sp->SpellGroupType = 2097152;

	//Hunter : Rapid Killing - might need to add honor trigger too here. I'm guessing you receive Xp too so i'm avoiding double proc
	assign_Spell_to_SpellEntry( &sp , 34948 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_GAIN_EXPIERIENCE | PROC_TARGET_SELF;
		sp->EffectSpellGroupRelation[1] = 32;
	}
	assign_Spell_to_SpellEntry( &sp , 34949 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_GAIN_EXPIERIENCE | PROC_TARGET_SELF;
		sp->EffectSpellGroupRelation[1] = 32;
	}
	//Hunter : Rapid Killing - PROC
	assign_Spell_to_SpellEntry( &sp , 35098 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 131072 | 2048 | 1;
	assign_Spell_to_SpellEntry( &sp , 35099 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 131072 | 2048 | 1;

	//Hunter : Improved Stings
	assign_Spell_to_SpellEntry( &sp , 19464 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 16384 | 65536;
		sp->EffectSpellGroupRelation[1] = 65536;
		sp->EffectSpellGroupRelation[2] = 16384 | 65536 | 32768;
	}
	assign_Spell_to_SpellEntry( &sp , 19465 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 16384 | 65536;
		sp->EffectSpellGroupRelation[1] = 65536;
		sp->EffectSpellGroupRelation[2] = 16384 | 65536 | 32768;
	}
	assign_Spell_to_SpellEntry( &sp , 19466 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 16384 | 65536;
		sp->EffectSpellGroupRelation[1] = 65536;
		sp->EffectSpellGroupRelation[2] = 16384 | 65536 | 32768;
	}
	assign_Spell_to_SpellEntry( &sp , 19467 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 16384 | 65536;
		sp->EffectSpellGroupRelation[1] = 65536;
		sp->EffectSpellGroupRelation[2] = 16384 | 65536 | 32768;
	}
	assign_Spell_to_SpellEntry( &sp , 19468 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 16384 | 65536;
		sp->EffectSpellGroupRelation[1] = 65536;
		sp->EffectSpellGroupRelation[2] = 16384 | 65536 | 32768;
	}

	//we need to adress this somehow : shot
	assign_Spell_to_SpellEntry( &sp , 3018 );
	if( sp != NULL )
		sp->SpellGroupType = 4;

	//Hunter : Mortal Shots
	assign_Spell_to_SpellEntry( &sp , 19485 );
	if( sp != NULL )
//		sp->EffectSpellGroupRelation[0] = 4 | 1 | 4096 | 2048 | 512 | 65536 | 131072 | 262144;
		sp->EffectSpellGroupRelation[0] = 4 ; //simple shot
	assign_Spell_to_SpellEntry( &sp , 19487 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 4 ; //simple shot
	assign_Spell_to_SpellEntry( &sp , 19488 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 4 ; //simple shot
	assign_Spell_to_SpellEntry( &sp , 19489 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 4 ; //simple shot
	assign_Spell_to_SpellEntry( &sp , 19490 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 4 ; //simple shot

	//Hunter : Improved Barrage
	assign_Spell_to_SpellEntry( &sp , 35104 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 4096;
		sp->EffectSpellGroupRelation[1] = 8192;
	}
	assign_Spell_to_SpellEntry( &sp , 35110 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 4096;
		sp->EffectSpellGroupRelation[1] = 8192;
	}
	assign_Spell_to_SpellEntry( &sp , 35111 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 4096;
		sp->EffectSpellGroupRelation[1] = 8192;
	}

	//Hunter : Clever Traps
	assign_Spell_to_SpellEntry( &sp , 19239 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 16 | 8;
		sp->EffectSpellGroupRelation[0] = 4;
		sp->EffectSpellGroupRelation[0] = 128;
	}
	assign_Spell_to_SpellEntry( &sp , 19245 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 16 | 8;
		sp->EffectSpellGroupRelation[0] = 4;
		sp->EffectSpellGroupRelation[0] = 128;
	}

	//Hunter : Resourcefulness
	assign_Spell_to_SpellEntry( &sp , 34491 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 128 | 2 | 64; 
		sp->EffectSpellGroupRelation[1] = 128;
	}
	assign_Spell_to_SpellEntry( &sp , 34492 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 128 | 2 | 64; 
		sp->EffectSpellGroupRelation[1] = 128;
	}
	assign_Spell_to_SpellEntry( &sp , 34493 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 128 | 2 | 64; 
		sp->EffectSpellGroupRelation[1] = 128;
	}

	/*
	//Hunter : Entrapment
	assign_Spell_to_SpellEntry( &sp , 19184 );
	if( sp != NULL )
		sp->procFlags = PROC_ON_CAST_SPELL;
	assign_Spell_to_SpellEntry( &sp , 19387 );
	if( sp != NULL )
		sp->procFlags = PROC_ON_CAST_SPELL;
	assign_Spell_to_SpellEntry( &sp , 19388 );
	if( sp != NULL )
		sp->procFlags = PROC_ON_CAST_SPELL;
	*/

	//general wand shoot. Needs a group that is not used for mage and priest
	assign_Spell_to_SpellEntry( &sp , 5019 );
	if( sp != NULL )
		sp->SpellGroupType = 134217728;
	
	//Mage - Wand Specialization. Not the forst thing we messed up. Blizz uses attack as magic and wandds as weapons :S
	assign_Spell_to_SpellEntry( &sp , 6057 );
	if( sp != NULL )
	{
		sp->EffectApplyAuraName[0] = SPELL_AURA_ADD_PCT_MODIFIER;
		sp->EffectMiscValue[0] = SMT_SPELL_VALUE;
		sp->EffectSpellGroupRelation[0] = 134217728;
	}
	assign_Spell_to_SpellEntry( &sp , 6085 );
	if( sp != NULL )
	{
		sp->EffectApplyAuraName[0] = SPELL_AURA_ADD_PCT_MODIFIER;
		sp->EffectMiscValue[0] = SMT_SPELL_VALUE;
		sp->EffectSpellGroupRelation[0] = 134217728;
	}
	//Priest - Wand Specialization
	assign_Spell_to_SpellEntry( &sp , 14524 );
	if( sp != NULL )
	{
		sp->EffectApplyAuraName[0] = SPELL_AURA_ADD_PCT_MODIFIER;
		sp->EffectMiscValue[0] = SMT_SPELL_VALUE;
		sp->EffectSpellGroupRelation[0] = 134217728;
	}
	assign_Spell_to_SpellEntry( &sp , 14525 );
	if( sp != NULL )
	{
		sp->EffectApplyAuraName[0] = SPELL_AURA_ADD_PCT_MODIFIER;
		sp->EffectMiscValue[0] = SMT_SPELL_VALUE;
		sp->EffectSpellGroupRelation[0] = 134217728;
	}
	assign_Spell_to_SpellEntry( &sp , 14526 );
	if( sp != NULL )
	{
		sp->EffectApplyAuraName[0] = SPELL_AURA_ADD_PCT_MODIFIER;
		sp->EffectMiscValue[0] = SMT_SPELL_VALUE;
		sp->EffectSpellGroupRelation[0] = 134217728;
	}
	assign_Spell_to_SpellEntry( &sp , 14527 );
	if( sp != NULL )
	{
		sp->EffectApplyAuraName[0] = SPELL_AURA_ADD_PCT_MODIFIER;
		sp->EffectMiscValue[0] = SMT_SPELL_VALUE;
		sp->EffectSpellGroupRelation[0] = 134217728;
	}
	assign_Spell_to_SpellEntry( &sp , 14528 );
	if( sp != NULL )
	{
		sp->EffectApplyAuraName[0] = SPELL_AURA_ADD_PCT_MODIFIER;
		sp->EffectMiscValue[0] = SMT_SPELL_VALUE;
		sp->EffectSpellGroupRelation[0] = 134217728;
	}

	//Mage - Spell Power
	assign_Spell_to_SpellEntry( &sp , 35578 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 0xFFFFFFFF;
		sp->EffectSpellGroupRelation_high[0] = 0xFFFFFFFF;
	}
	assign_Spell_to_SpellEntry( &sp , 35581 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 0xFFFFFFFF;
		sp->EffectSpellGroupRelation_high[0] = 0xFFFFFFFF;
	}

	//Mage - Frost Channeling
	assign_Spell_to_SpellEntry( &sp , 11160 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 524288 | 131072  ;
		sp->EffectSpellGroupRelation_high[0] = 1;
	}
	assign_Spell_to_SpellEntry( &sp , 12518 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 524288 | 131072  ;
		sp->EffectSpellGroupRelation_high[0] = 1;
	}
	assign_Spell_to_SpellEntry( &sp , 12519 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 524288 | 131072  ;
		sp->EffectSpellGroupRelation_high[0] = 1;
	}

	//Mage - Elemental Precision
	assign_Spell_to_SpellEntry( &sp , 29438 );
	if( sp != NULL )
	{
		sp->EffectApplyAuraName[0] = SPELL_AURA_ADD_PCT_MODIFIER;
		sp->EffectSpellGroupRelation[0] = 8388608 | 2 | 16 | 4 | 1573376 | 524288 | 8 | 131072 | 262144 | 4194304 | 1 ;
		sp->EffectSpellGroupRelation_high[0] = 64 | 128 | 1;
		sp->EffectMiscValue[0] = SMT_COST;
	}
	assign_Spell_to_SpellEntry( &sp , 29439 );
	if( sp != NULL )
	{
		sp->EffectApplyAuraName[0] = SPELL_AURA_ADD_PCT_MODIFIER;
		sp->EffectSpellGroupRelation[0] = 8388608 | 2 | 16 | 4 | 1573376 | 524288 | 8 | 131072 | 262144 | 4194304 | 1 ;
		sp->EffectSpellGroupRelation_high[0] = 64 | 128 | 1;
		sp->EffectMiscValue[0] = SMT_COST;
	}
	assign_Spell_to_SpellEntry( &sp , 29440 );
	if( sp != NULL )
	{
		sp->EffectApplyAuraName[0] = SPELL_AURA_ADD_PCT_MODIFIER;
		sp->EffectSpellGroupRelation[0] = 8388608 | 2 | 16 | 4 | 1573376 | 524288 | 8 | 131072 | 262144 | 4194304 | 1 ;
		sp->EffectSpellGroupRelation_high[0] = 64 | 128 | 1;
		sp->EffectMiscValue[0] = SMT_COST;
	}

	//Mage - Arcane Concentration
	assign_Spell_to_SpellEntry( &sp , 11213 );
	if( sp != NULL )
		sp->procFlags = PROC_ON_CAST_SPELL | PROC_TARGET_SELF;
	assign_Spell_to_SpellEntry( &sp , 12574 );
	if( sp != NULL )
		sp->procFlags = PROC_ON_CAST_SPELL | PROC_TARGET_SELF;
	assign_Spell_to_SpellEntry( &sp , 12575 );
	if( sp != NULL )
		sp->procFlags = PROC_ON_CAST_SPELL | PROC_TARGET_SELF;
	assign_Spell_to_SpellEntry( &sp , 12576 );
	if( sp != NULL )
		sp->procFlags = PROC_ON_CAST_SPELL | PROC_TARGET_SELF;
	assign_Spell_to_SpellEntry( &sp , 12577 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_CAST_SPELL | PROC_TARGET_SELF;
//sp->procChance = 100;
	}

	//Mage - ClearCasting Effect
	assign_Spell_to_SpellEntry( &sp , 12536 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 0xFFFFFFFF;		//!!!this will set value multiple times but we do not care since it is 100%
		sp->EffectSpellGroupRelation_high[0] = 0xFFFFFFFF;	//!!!this will set value multiple times but we do not care since it is 100%
	}	

	//Mage - Arcane Blast
	assign_Spell_to_SpellEntry( &sp , 30451 );
	if( sp != NULL )
	{
		sp->EffectApplyAuraName[1] = 42;
		sp->EffectTriggerSpell[1] = 36032;
		sp->procFlags = PROC_ON_CAST_SPECIFIC_SPELL;
	}

	//Mage - Magic Attunement
	assign_Spell_to_SpellEntry( &sp , 11247 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 8192;
	assign_Spell_to_SpellEntry( &sp , 12606 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 8192; //strange lvl 2 was working 

	//Mage:Arcane Blast proc spell
	assign_Spell_to_SpellEntry( &sp , 36032 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 536870912;
		sp->EffectSpellGroupRelation[1] = 536870912;
	}

	// general - clearcasting
	assign_Spell_to_SpellEntry( &sp , 12536 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 0xFFFFFFFF; //all possible spells we can affect
	assign_Spell_to_SpellEntry( &sp , 16246 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 0xFFFFFFFF; //all possible spells we can affect
	assign_Spell_to_SpellEntry( &sp , 16870 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 0xFFFFFFFF; //all possible spells we can affect
	assign_Spell_to_SpellEntry( &sp , 34754 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 0xFFFFFFFF; //all possible spells we can affect

	// Shaman - Storm Reach
	assign_Spell_to_SpellEntry( &sp , 28999 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 2 | 1;
	assign_Spell_to_SpellEntry( &sp , 29000 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 2 | 1;

	//rogue - Seal Fate
	assign_Spell_to_SpellEntry( &sp , 14186 );
	if( sp != NULL ) 
	{
		sp->procFlags = PROC_ON_CRIT_ATTACK;
		sp->procChance = 20;
	}
	assign_Spell_to_SpellEntry( &sp , 14190 );
	if( sp != NULL ) 
	{
		sp->procFlags = PROC_ON_CRIT_ATTACK;
		sp->procChance = 40;
	}
	assign_Spell_to_SpellEntry( &sp , 14193 );
	if( sp != NULL ) 
	{
		sp->procFlags = PROC_ON_CRIT_ATTACK;
		sp->procChance = 60;
	}
	assign_Spell_to_SpellEntry( &sp , 14194 );
	if( sp != NULL ) 
	{
		sp->procFlags = PROC_ON_CRIT_ATTACK;
		sp->procChance = 80;
	}
	assign_Spell_to_SpellEntry( &sp , 14195 );
	if( sp != NULL ) 
	{
		sp->procFlags = PROC_ON_CRIT_ATTACK;
		sp->procChance = 100;
	}

	//druid Savage Fury
	assign_Spell_to_SpellEntry( &sp , 16998);
	if( sp != NULL ) 
	{
		sp->EffectSpellGroupRelation_high[0] |= 1024;
		//sp->EffectSpellGroupRelation_high[1] |= 1024;
		//sp->EffectSpellGroupRelation_high[2] |= 1024;
	}
	assign_Spell_to_SpellEntry( &sp , 16999);
	if( sp != NULL ) 
	{
		sp->EffectSpellGroupRelation_high[0] |= 1024;
		//sp->EffectSpellGroupRelation_high[1] |= 1024;
		//sp->EffectSpellGroupRelation_high[2] |= 1024;
	}

	//druid - Blood Frenzy
	assign_Spell_to_SpellEntry( &sp , 16954 );
	if( sp != NULL ) 
	{
		sp->procFlags = PROC_ON_CRIT_ATTACK;
		sp->procChance = 100;
	}
	assign_Spell_to_SpellEntry( &sp , 16952 );
	if( sp != NULL ) 
	{
		sp->procFlags = PROC_ON_CRIT_ATTACK;
		sp->procChance = 50;
	}

	//druid - Primal Fury
	assign_Spell_to_SpellEntry( &sp , 16961 );
	if( sp != NULL ) 
	{
		sp->procFlags = PROC_ON_CRIT_ATTACK;
		sp->procChance = 100;
	}
	assign_Spell_to_SpellEntry( &sp , 16958 );
	if( sp != NULL ) 
	{
		sp->procFlags = PROC_ON_CRIT_ATTACK;
		sp->procChance = 50;
	}

	//druid - Intensity
	assign_Spell_to_SpellEntry( &sp , 17106 );
	if( sp != NULL )
	{
	   sp->EffectApplyAuraName[1] = 42;
	   sp->procFlags = PROC_ON_CAST_SPELL;
	}
	assign_Spell_to_SpellEntry( &sp , 17107 );
	if( sp != NULL )
	{
		sp->EffectApplyAuraName[1] = 42;
		 sp->procFlags = PROC_ON_CAST_SPELL;
	}
	assign_Spell_to_SpellEntry( &sp , 17108 );
	if( sp != NULL )
	{
		sp->EffectApplyAuraName[1] = 42;
		sp->procFlags = PROC_ON_CAST_SPELL;
	}

    //Improved Sprint
	assign_Spell_to_SpellEntry( &sp , 13743 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_CAST_SPELL;
		sp->procChance = 50;
	}
	assign_Spell_to_SpellEntry( &sp , 13875 );
	if( sp != NULL )
	{
		sp->procChance = 100;
		sp->procFlags = PROC_ON_CAST_SPELL;
	}

	//warlock -  Seed of Corruption
	assign_Spell_to_SpellEntry( &sp , 27243 );
	if( sp != NULL )
	{
		sp->EffectApplyAuraName[1] = SPELL_AURA_PROC_TRIGGER_SPELL;
		sp->EffectTriggerSpell[1] = 27285;
		sp->procFlags = PROC_ON_SPELL_HIT_VICTIM | PROC_ON_DIE;
		sp->procChance = 100;
	}

	//warlock -  soul link
	assign_Spell_to_SpellEntry( &sp , 19028 );
	if( sp != NULL )
	{
		//this is for the trigger effect
		sp->Effect[0]=6;
		sp->EffectApplyAuraName[0] = SPELL_AURA_PROC_TRIGGER_SPELL;
		sp->EffectTriggerSpell[0] = 25228;
		sp->EffectImplicitTargetA[0] = EFF_TARGET_SELF;
		sp->procFlags = PROC_ON_ANY_DAMAGE_VICTIM ;
		//this is for the extra 5% dmg for caster and pet
		sp->Effect[1]=6;
		sp->EffectApplyAuraName[1]=79;
		sp->EffectBasePoints[1]=4; //4+1=5
		sp->EffectImplicitTargetA[1] = EFF_TARGET_SELF ;
		sp->EffectImplicitTargetB[1] = EFF_TARGET_PET ;
		sp->c_is_flags |= SPELL_FLAG_IS_EXPIREING_WITH_PET;
	}
	assign_Spell_to_SpellEntry( &sp , 25228 );
	if( sp != NULL )
	{
		sp->Effect[0]=3;	
		sp->Effect[1]=0;//disable this part to not have multiple effects
	}

	//warlock: Demonic Aegis
	assign_Spell_to_SpellEntry( &sp , 30143 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation_high[0] = 32;
	assign_Spell_to_SpellEntry( &sp , 30144 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation_high[0] = 32;
	assign_Spell_to_SpellEntry( &sp , 30145 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation_high[0] = 32;

	//warlock: Nightfall
	assign_Spell_to_SpellEntry( &sp , 18094 );
	if( sp != NULL )
	{
		sp->EffectApplyAuraName[0] = 42;
		sp->EffectTriggerSpell[0] = 17941;
		sp->procFlags = PROC_ON_CAST_SPELL;
		sp->procChance = 2;
	}
	assign_Spell_to_SpellEntry( &sp , 18095 );
	if( sp != NULL )
	{
		sp->EffectApplyAuraName[0] = 42;
		sp->EffectTriggerSpell[0] = 17941;
		sp->procFlags = PROC_ON_CAST_SPELL;
		sp->procChance = 4;
	}

	//warlock: Contagion
	assign_Spell_to_SpellEntry( &sp , 30060 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 1024 | 2;
		sp->EffectSpellGroupRelation_high[0] = 32784;
		sp->EffectSpellGroupRelation[1] = 1024 | 2;
		sp->EffectSpellGroupRelation_high[1] = 32784;
		sp->EffectSpellGroupRelation[2] = 2 | 8 | 32768 | 2147483648UL | 1024 | 16384 | 262144UL | 16 | 524288UL | 4194304UL;
	}
	assign_Spell_to_SpellEntry( &sp , 30061 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 1024 | 2;
		sp->EffectSpellGroupRelation_high[0] = 32784;
		sp->EffectSpellGroupRelation[1] = 1024 | 2;
		sp->EffectSpellGroupRelation_high[1] = 32784;
		sp->EffectSpellGroupRelation[2] = 2 | 8 | 32768 | 2147483648UL | 1024 | 16384 | 262144UL | 16 | 524288UL | 4194304UL;
	}
	assign_Spell_to_SpellEntry( &sp , 30062 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 1024 | 2;
		sp->EffectSpellGroupRelation_high[0] = 32784;
		sp->EffectSpellGroupRelation[1] = 1024 | 2;
		sp->EffectSpellGroupRelation_high[1] = 32784;
		sp->EffectSpellGroupRelation[2] = 2 | 8 | 32768 | 2147483648UL | 1024 | 16384 | 262144UL | 16 | 524288UL | 4194304UL;
	}
	assign_Spell_to_SpellEntry( &sp , 30063 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 1024 | 2;
		sp->EffectSpellGroupRelation_high[0] = 32784;
		sp->EffectSpellGroupRelation[1] = 1024 | 2;
		sp->EffectSpellGroupRelation_high[1] = 32784;
		sp->EffectSpellGroupRelation[2] = 2 | 8 | 32768 | 2147483648UL | 1024 | 16384 | 262144UL | 16 | 524288UL | 4194304UL;
	}
	assign_Spell_to_SpellEntry( &sp , 30064 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 1024 | 2;
		sp->EffectSpellGroupRelation_high[0] = 32784;
		sp->EffectSpellGroupRelation[1] = 1024 | 2;
		sp->EffectSpellGroupRelation_high[1] = 32784;
		sp->EffectSpellGroupRelation[2] = 2 | 8 | 32768 | 2147483648UL | 1024 | 16384 | 262144UL | 16 | 524288UL | 4194304UL;
	}

	//warlock: Malediction
	assign_Spell_to_SpellEntry( &sp , 32477 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 2097152;
		sp->EffectSpellGroupRelation_high[0] = 512;
	}
	assign_Spell_to_SpellEntry( &sp , 32483 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 2097152;
		sp->EffectSpellGroupRelation_high[0] = 512;
	}
	assign_Spell_to_SpellEntry( &sp , 32484 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 2097152;
		sp->EffectSpellGroupRelation_high[0] = 512;
	}

	//warlock: Improved Searing Pain
	assign_Spell_to_SpellEntry( &sp , 17927 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 256;
	assign_Spell_to_SpellEntry( &sp , 17929 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 256;
	assign_Spell_to_SpellEntry( &sp , 17930 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 256;

	//warlock: Empowered Corruption
	assign_Spell_to_SpellEntry( &sp , 32381 );
	if( sp != NULL )
	{
		sp->EffectBasePoints[0] *= 6;
		sp->EffectSpellGroupRelation[0] = 2;
	}
	assign_Spell_to_SpellEntry( &sp , 32382 );
	if( sp != NULL )
	{
		sp->EffectBasePoints[0] *= 6;
		sp->EffectSpellGroupRelation[0] = 2;
	}
	assign_Spell_to_SpellEntry( &sp , 32383 );
	if( sp != NULL )
	{
		sp->EffectBasePoints[0] *= 6;
		sp->EffectSpellGroupRelation[0] = 2;
	}

	//warlock: Improved Enslave Demon
	assign_Spell_to_SpellEntry( &sp , 18821 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 2048;
		sp->EffectMiscValue[0]=SMT_SPELL_VALUE_PCT;
		sp->EffectBasePoints[0] = -(sp->EffectBasePoints[0]+2);
//		sp->EffectSpellGroupRelation[1] = 2048; //we do not handle this misc type yet anyway. Removed it just as a reminder
		sp->EffectSpellGroupRelation[2] = 2048;
	}
	assign_Spell_to_SpellEntry( &sp , 18822 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 2048;
		sp->EffectSpellGroupRelation[1] = 2048;
		sp->EffectSpellGroupRelation[2] = 2048;
	}

	//warlock: Devastation
	assign_Spell_to_SpellEntry( &sp , 18130 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 4 | 1 | 64 | 256 | 32 | 128 | 512; //destruction spells
	assign_Spell_to_SpellEntry( &sp , 18131 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 4 | 1 | 64 | 256 | 32 | 128 | 512;
	assign_Spell_to_SpellEntry( &sp , 18132 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 4 | 1 | 64 | 256 | 32 | 128 | 512;
	assign_Spell_to_SpellEntry( &sp , 18133 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 4 | 1 | 64 | 256 | 32 | 128 | 512;
	assign_Spell_to_SpellEntry( &sp , 18134 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 4 | 1 | 64 | 256 | 32 | 128 | 512;

	//warlock - Shadow Mastery
	assign_Spell_to_SpellEntry( &sp , 18271 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] =  2147483648UL | 4194304 | 1 | 2 | 16384 | 1024 | 8 | 262144 | 524288 | 2147483648UL | 16777216UL | 128 | 16 | 32768;
		sp->EffectSpellGroupRelation[1] =  2147483648UL | 4194304 | 1 | 2 | 16384 | 1024 | 8 | 262144 | 524288 | 2147483648UL | 16777216UL | 128 | 16 | 32768;
	}
	assign_Spell_to_SpellEntry( &sp , 18272 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] =  2147483648UL | 4194304 | 1 | 2 | 16384 | 1024 | 8 | 262144 | 524288 | 2147483648UL | 16777216UL | 128 | 16 | 32768;
		sp->EffectSpellGroupRelation[1] =  2147483648UL | 4194304 | 1 | 2 | 16384 | 1024 | 8 | 262144 | 524288 | 2147483648UL | 16777216UL | 128 | 16 | 32768;
	}
	assign_Spell_to_SpellEntry( &sp , 18273 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] =  2147483648UL | 4194304 | 1 | 2 | 16384 | 1024 | 8 | 262144 | 524288 | 2147483648UL | 16777216UL | 128 | 16 | 32768;
		sp->EffectSpellGroupRelation[1] =  2147483648UL | 4194304 | 1 | 2 | 16384 | 1024 | 8 | 262144 | 524288 | 2147483648UL | 16777216UL | 128 | 16 | 32768;
	}
	assign_Spell_to_SpellEntry( &sp , 18274 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] =  2147483648UL | 4194304 | 1 | 2 | 16384 | 1024 | 8 | 262144 | 524288 | 2147483648UL | 16777216UL | 128 | 16 | 32768;
		sp->EffectSpellGroupRelation[1] =  2147483648UL | 4194304 | 1 | 2 | 16384 | 1024 | 8 | 262144 | 524288 | 2147483648UL | 16777216UL | 128 | 16 | 32768;
	}
	assign_Spell_to_SpellEntry( &sp , 18275 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] =  2147483648UL | 4194304 | 1 | 2 | 16384 | 1024 | 8 | 262144 | 524288 | 2147483648UL | 16777216UL | 128 | 16 | 32768;
		sp->EffectSpellGroupRelation[1] =  2147483648UL | 4194304 | 1 | 2 | 16384 | 1024 | 8 | 262144 | 524288 | 2147483648UL | 16777216UL | 128 | 16 | 32768;
	}

	//mage: Arcane Power
	assign_Spell_to_SpellEntry( &sp , 12042 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[1] = 1 | 8192 | 4194304 | 8388608 | 262144 | 131072 | 536870912 | 524352 | 4 | 4096 | 2 | 2048 | 16;
		sp->EffectSpellGroupRelation[2] = 1 | 8192 | 4194304 | 8388608 | 262144 | 131072 | 536870912 | 524352 | 4 | 4096 | 2 | 2048 | 16;
	}

	//mage : Improved Blink
	assign_Spell_to_SpellEntry( &sp , 31569 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 65536;
	assign_Spell_to_SpellEntry( &sp , 31570 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 65536;

	//mage : Empowered Arcane Missiles
	assign_Spell_to_SpellEntry( &sp , 31579 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 2097152;
		sp->EffectBasePoints[0] *= 5; //heh B thinks he is smart by adding this to description ? If it doesn;t work std then it still needs to made by hand
		sp->EffectSpellGroupRelation[1] = 2048;
	}
	assign_Spell_to_SpellEntry( &sp , 31582 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 2097152;
		sp->EffectBasePoints[0] *= 5; //heh B thinks he is smart by adding this to description ? If it doesn;t work std then it still needs to made by hand
		sp->EffectSpellGroupRelation[1] = 2048;
	}
	assign_Spell_to_SpellEntry( &sp , 31583 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 2097152; //damage
		sp->EffectBasePoints[0] *= 5; //heh B thinks he is smart by adding this to description ? If it doesn;t work std then it still needs to made by hand
		sp->EffectSpellGroupRelation[1] = 2048; //cost
	}

	//mage : Empowered Fireball
	assign_Spell_to_SpellEntry( &sp , 31656 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 1;
	assign_Spell_to_SpellEntry( &sp , 31657 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 1;
	assign_Spell_to_SpellEntry( &sp , 31658 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 1;
	assign_Spell_to_SpellEntry( &sp , 31659 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 1;
	assign_Spell_to_SpellEntry( &sp , 31660 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 1;

	//mage : Ice Floes
	assign_Spell_to_SpellEntry( &sp , 31670 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 512;
		sp->EffectSpellGroupRelation_high[0] = 4 | 1;
	}
	assign_Spell_to_SpellEntry( &sp , 31672 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 512;
		sp->EffectSpellGroupRelation_high[0] = 4 | 1;
	}

	//mage : Empowered Frostbolt
	assign_Spell_to_SpellEntry( &sp , 31682 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 32;
		sp->EffectSpellGroupRelation[1] = 32;
	}
	assign_Spell_to_SpellEntry( &sp , 31683 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 32;
		sp->EffectSpellGroupRelation[1] = 32;
	}
	assign_Spell_to_SpellEntry( &sp , 31684 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 32;
		sp->EffectSpellGroupRelation[1] = 32;
	}
	assign_Spell_to_SpellEntry( &sp , 31685 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 32;
		sp->EffectSpellGroupRelation[1] = 32;
	}
	assign_Spell_to_SpellEntry( &sp , 31686 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 32;
		sp->EffectSpellGroupRelation[1] = 32;
	}

	//Mage - Ice Shards
	assign_Spell_to_SpellEntry( &sp , 11207 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] =  524288 | 131072;
	assign_Spell_to_SpellEntry( &sp , 12672 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] =  524288 | 131072;
	assign_Spell_to_SpellEntry( &sp , 15047 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] =  524288 | 131072;
	assign_Spell_to_SpellEntry( &sp , 15052 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] =  524288 | 131072;
	assign_Spell_to_SpellEntry( &sp , 15053 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] =  524288 | 131072;

	//Mage - Improved Blizzard
	assign_Spell_to_SpellEntry( &sp , 11185 );
	if( sp != NULL )
	{    
		sp->EffectApplyAuraName[0] = 42;
		sp->EffectTriggerSpell[0] = 12484;
		sp->procFlags = PROC_ON_CAST_SPELL;
	}
	assign_Spell_to_SpellEntry( &sp , 12487 );
	if( sp != NULL )
	{
		sp->EffectApplyAuraName[0] = 42;
		sp->EffectTriggerSpell[0] = 12485;
		sp->procFlags = PROC_ON_CAST_SPELL;
	}
	assign_Spell_to_SpellEntry( &sp , 12488 );
	if( sp != NULL )
	{
		sp->EffectApplyAuraName[0] = 42;
		sp->EffectTriggerSpell[0] = 12486;
		sp->procFlags = PROC_ON_CAST_SPELL;
	}

	//mage: Fire Power
	assign_Spell_to_SpellEntry( &sp , 11124 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 8388608 | 16 | 2 | 4 | 8 | 262144 | 4194304 | 1;
		sp->EffectSpellGroupRelation[1] = 8388608 | 16 | 2 | 4 | 8 | 262144 | 4194304 | 1;
	}
	assign_Spell_to_SpellEntry( &sp , 12398 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 8388608 | 16 | 2 | 4 | 8 | 262144 | 4194304 | 1;
		sp->EffectSpellGroupRelation[1] = 8388608 | 16 | 2 | 4 | 8 | 262144 | 4194304 | 1;
	}
	assign_Spell_to_SpellEntry( &sp , 12399 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 8388608 | 16 | 2 | 4 | 8 | 262144 | 4194304 | 1;
		sp->EffectSpellGroupRelation[1] = 8388608 | 16 | 2 | 4 | 8 | 262144 | 4194304 | 1;
	}
	assign_Spell_to_SpellEntry( &sp , 12400 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 8388608 | 16 | 2 | 4 | 8 | 262144 | 4194304 | 1;
		sp->EffectSpellGroupRelation[1] = 8388608 | 16 | 2 | 4 | 8 | 262144 | 4194304 | 1;
	}
	assign_Spell_to_SpellEntry( &sp , 12378 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 8388608 | 16 | 2 | 4 | 8 | 262144 | 4194304 | 1;
		sp->EffectSpellGroupRelation[1] = 8388608 | 16 | 2 | 4 | 8 | 262144 | 4194304 | 1;
	}
	
	//mage - Burning Soul
	assign_Spell_to_SpellEntry( &sp , 11083 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 8388608 | 16 | 2 | 4 | 8 | 262144 | 4194304 | 1;
	assign_Spell_to_SpellEntry( &sp , 12351 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 8388608 | 16 | 2 | 4 | 8 | 262144 | 4194304 | 1;

	//mage - Combustion
	assign_Spell_to_SpellEntry( &sp , 11129 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_CAST_SPELL | PROC_ON_SPELL_CRIT_HIT | PROC_TARGET_SELF;
		sp->procCharges = 0;
		sp->c_is_flags |= SPELL_FLAG_IS_REQUIRECOOLDOWNUPDATE;
	}
	assign_Spell_to_SpellEntry( &sp , 28682 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 8388608 | 16 | 2 | 4 | 4194304 | 1;

	//mage - Empowered Fireball
	assign_Spell_to_SpellEntry( &sp , 31656 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 1;
	assign_Spell_to_SpellEntry( &sp , 31657 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 1;
	assign_Spell_to_SpellEntry( &sp , 31658 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 1;
	assign_Spell_to_SpellEntry( &sp , 31659 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 1;
	assign_Spell_to_SpellEntry( &sp , 31660 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 1;

	//mage - Empowered Frostbolt
	assign_Spell_to_SpellEntry( &sp , 31682 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 32;
		sp->EffectSpellGroupRelation[1] = 32;
	}
	assign_Spell_to_SpellEntry( &sp , 31683 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 32;
		sp->EffectSpellGroupRelation[1] = 32;
	}
	assign_Spell_to_SpellEntry( &sp , 31684 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 32;
		sp->EffectSpellGroupRelation[1] = 32;
	}
	assign_Spell_to_SpellEntry( &sp , 31685 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 32;
		sp->EffectSpellGroupRelation[1] = 32;
	}
	assign_Spell_to_SpellEntry( &sp , 31686 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 32;
		sp->EffectSpellGroupRelation[1] = 32;
	}

	//mage - Master of Elements
	assign_Spell_to_SpellEntry( &sp , 29074 );
	if( sp != NULL )
	{
		sp->EffectApplyAuraName[0] = 42;
		sp->EffectTriggerSpell[0] = 29077;
		sp->procFlags = uint32(PROC_ON_SPELL_CRIT_HIT|PROC_TARGET_SELF);
		sp->procChance = 100;
	}
	assign_Spell_to_SpellEntry( &sp , 29075 );
	if( sp != NULL )
	{
		sp->EffectApplyAuraName[0] = 42;
		sp->EffectTriggerSpell[0] = 29077;
		sp->procFlags = uint32(PROC_ON_SPELL_CRIT_HIT|PROC_TARGET_SELF);
		sp->procChance = 100;
	}
	assign_Spell_to_SpellEntry( &sp , 29076 );
	if( sp != NULL )
	{
		sp->EffectApplyAuraName[0] = 42;
		sp->EffectTriggerSpell[0] = 29077;
		sp->procFlags = uint32(PROC_ON_SPELL_CRIT_HIT|PROC_TARGET_SELF);
		sp->procChance = 100;
	}

	//mage: Blazing Speed
	assign_Spell_to_SpellEntry( &sp , 31641 ); 
	if( sp != NULL )
		sp->EffectTriggerSpell[0] = 31643;
	assign_Spell_to_SpellEntry( &sp , 31642 );
	if( sp != NULL )
		sp->EffectTriggerSpell[0] = 31643;

	//mage talent "frostbyte". we make it to be dummy
	assign_Spell_to_SpellEntry( &sp , 11071 );
	if( sp != NULL )
		sp->EffectApplyAuraName[0]=4;
	assign_Spell_to_SpellEntry( &sp , 12496 );
	if( sp != NULL )
		sp->EffectApplyAuraName[0]=4;
	assign_Spell_to_SpellEntry( &sp , 12497 );
	if( sp != NULL )
		sp->EffectApplyAuraName[0]=4;
	//rogue-shiv -> add 1 combo point
	assign_Spell_to_SpellEntry( &sp , 5938 );
	if( sp != NULL )
		sp->Effect[1]=80;

	//warlock - Improved Curse of Weakness
	assign_Spell_to_SpellEntry( &sp , 18179 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 32768;
	assign_Spell_to_SpellEntry( &sp , 18180 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 32768;

	//warlock - Amplify Curse
	assign_Spell_to_SpellEntry( &sp , 18288 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 1024;
		sp->EffectSpellGroupRelation_high[0] = 2;
		sp->EffectSpellGroupRelation[1] = 4194304;
	}

	//warlock - Improved Howl of Terror
	assign_Spell_to_SpellEntry( &sp , 30054 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation_high[0] = 8;
	assign_Spell_to_SpellEntry( &sp , 30057 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation_high[0] = 8;

	//warlock - Emberstorm
	assign_Spell_to_SpellEntry( &sp , 17954 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 32 | 64 | 4 | 1048576 | 256 | 512 | 1;
	assign_Spell_to_SpellEntry( &sp , 17955 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 32 | 64 | 4 | 1048576 | 256 | 512 | 1;
	assign_Spell_to_SpellEntry( &sp , 17956 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 32 | 64 | 4 | 1048576 | 256 | 512 | 1;
	assign_Spell_to_SpellEntry( &sp , 17957 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 32 | 64 | 4 | 1048576 | 256 | 512 | 1;
	assign_Spell_to_SpellEntry( &sp , 17958 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 32 | 64 | 4 | 1048576 | 256 | 512 | 1;

	//warlock - Shadow and Flame
	assign_Spell_to_SpellEntry( &sp , 30288 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 1;
		sp->EffectSpellGroupRelation_high[0] = 64;
	}
	assign_Spell_to_SpellEntry( &sp , 30289 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 1;
		sp->EffectSpellGroupRelation_high[0] = 64;
	}
	assign_Spell_to_SpellEntry( &sp , 30290 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 1;
		sp->EffectSpellGroupRelation_high[0] = 64;
	}
	assign_Spell_to_SpellEntry( &sp , 30291 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 1;
		sp->EffectSpellGroupRelation_high[0] = 64;
	}
	assign_Spell_to_SpellEntry( &sp , 30292 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 1;
		sp->EffectSpellGroupRelation_high[0] = 64;
	}

	//warlock - Ruin
	assign_Spell_to_SpellEntry( &sp , 17959 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 1 | 4 | 32 | 64 | 128 | 256 | 512 ;
		sp->EffectSpellGroupRelation_high[0] = 128 | 4096 ;
	}

	//warlock - destructive reach 
	assign_Spell_to_SpellEntry( &sp , 17917 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 1 | 4 | 32 | 64 | 128 | 256 | 512 ;
		sp->EffectSpellGroupRelation_high[0] = 128 | 4096 ;
		sp->EffectSpellGroupRelation[1] = 1 | 4 | 32 | 64 | 128 | 256 | 512 ;
		sp->EffectSpellGroupRelation_high[1] = 128 | 4096 ;
	}
	assign_Spell_to_SpellEntry( &sp , 17918 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 1 | 4 | 32 | 64 | 128 | 256 | 512 ;
		sp->EffectSpellGroupRelation_high[0] = 128 | 4096 ;
		sp->EffectSpellGroupRelation[1] = 1 | 4 | 32 | 64 | 128 | 256 | 512 ;
		sp->EffectSpellGroupRelation_high[1] = 128 | 4096 ;
	}

	//warlock - Cataclysm 
	assign_Spell_to_SpellEntry( &sp , 17778 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 1 | 4 | 32 | 64 | 128 | 256 | 512 ;
		sp->EffectSpellGroupRelation_high[0] = 128 | 4096 ;
		sp->EffectSpellGroupRelation[1] = 1 | 4 | 32 | 64 | 128 | 256 | 512 ;
		sp->EffectSpellGroupRelation_high[1] = 128 | 4096 ;
	}
	assign_Spell_to_SpellEntry( &sp , 17779 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 1 | 4 | 32 | 64 | 128 | 256 | 512 ;
		sp->EffectSpellGroupRelation_high[0] = 128 | 4096 ;
		sp->EffectSpellGroupRelation[1] = 1 | 4 | 32 | 64 | 128 | 256 | 512 ;
		sp->EffectSpellGroupRelation_high[1] = 128 | 4096 ;
	}
	assign_Spell_to_SpellEntry( &sp , 17780 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 1 | 4 | 32 | 64 | 128 | 256 | 512 ;
		sp->EffectSpellGroupRelation_high[0] = 128 | 4096 ;
		sp->EffectSpellGroupRelation[1] = 1 | 4 | 32 | 64 | 128 | 256 | 512 ;
		sp->EffectSpellGroupRelation_high[1] = 128 | 4096 ;
	}
	assign_Spell_to_SpellEntry( &sp , 17781 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 1 | 4 | 32 | 64 | 128 | 256 | 512 ;
		sp->EffectSpellGroupRelation_high[0] = 128 | 4096 ;
		sp->EffectSpellGroupRelation[1] = 1 | 4 | 32 | 64 | 128 | 256 | 512 ;
		sp->EffectSpellGroupRelation_high[1] = 128 | 4096 ;
	}
	assign_Spell_to_SpellEntry( &sp , 17782 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 1 | 4 | 32 | 64 | 128 | 256 | 512 ;
		sp->EffectSpellGroupRelation_high[0] = 128 | 4096 ;
		sp->EffectSpellGroupRelation[1] = 1 | 4 | 32 | 64 | 128 | 256 | 512 ;
		sp->EffectSpellGroupRelation_high[1] = 128 | 4096 ;
	}

	//warlock - Intensity 
	assign_Spell_to_SpellEntry( &sp , 18135 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 1 | 4 | 32 | 64 | 128 | 256 | 512 ;
		sp->EffectSpellGroupRelation_high[0] = 128 | 4096 ;
		sp->EffectSpellGroupRelation[1] = 1 | 4 | 32 | 64 | 128 | 256 | 512 ;
		sp->EffectSpellGroupRelation_high[1] = 128 | 4096 ;
	}
	assign_Spell_to_SpellEntry( &sp , 18136 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 1 | 4 | 32 | 64 | 128 | 256 | 512 ;
		sp->EffectSpellGroupRelation_high[0] = 128 | 4096 ;
		sp->EffectSpellGroupRelation[1] = 1 | 4 | 32 | 64 | 128 | 256 | 512 ;
		sp->EffectSpellGroupRelation_high[1] = 128 | 4096 ;
	}

	//warlock - Suppression
	assign_Spell_to_SpellEntry( &sp , 18174 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 2 | 8 | 32768 | 2147483648UL | 1024 | 16384 | 262144 | 16 | 524288 | 4194304; //affliction spell
	assign_Spell_to_SpellEntry( &sp , 18175 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 2 | 8 | 32768 | 2147483648UL | 1024 | 16384 | 262144 | 16 | 524288 | 4194304;
	assign_Spell_to_SpellEntry( &sp , 18176 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 2 | 8 | 32768 | 2147483648UL | 1024 | 16384 | 262144 | 16 | 524288 | 4194304;
	assign_Spell_to_SpellEntry( &sp , 18177 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 2 | 8 | 32768 | 2147483648UL | 1024 | 16384 | 262144 | 16 | 524288 | 4194304;
	assign_Spell_to_SpellEntry( &sp , 18178 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 2 | 8 | 32768 | 2147483648UL | 1024 | 16384 | 262144 | 16 | 524288 | 4194304;

	//warlock - Improved Curse of Agony
	assign_Spell_to_SpellEntry( &sp , 18827 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 1024;
	assign_Spell_to_SpellEntry( &sp , 18829 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 1024;

	//warlock - Grim Reach
	assign_Spell_to_SpellEntry( &sp , 18218 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 2 | 8 | 32768 | 2147483648UL | 1024 | 16384 | 262144 | 16 | 524288 | 4194304;
	assign_Spell_to_SpellEntry( &sp , 18219 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 2 | 8 | 32768 | 2147483648UL | 1024 | 16384 | 262144 | 16 | 524288 | 4194304;

	//warlock - Demonic Sacrifice
	assign_Spell_to_SpellEntry( &sp , 18789 );
	if( sp != NULL )
		sp->c_is_flags |= SPELL_FLAG_IS_EXPIREING_ON_PET;
	assign_Spell_to_SpellEntry( &sp , 18790 );
	if( sp != NULL )
		sp->c_is_flags |= SPELL_FLAG_IS_EXPIREING_ON_PET;
	assign_Spell_to_SpellEntry( &sp , 18791 );
	if( sp != NULL )
		sp->c_is_flags |= SPELL_FLAG_IS_EXPIREING_ON_PET;
	assign_Spell_to_SpellEntry( &sp , 18792 );
	if( sp != NULL )
		sp->c_is_flags |= SPELL_FLAG_IS_EXPIREING_ON_PET;
	assign_Spell_to_SpellEntry( &sp , 35701 );
	if( sp != NULL )
		sp->c_is_flags |= SPELL_FLAG_IS_EXPIREING_ON_PET;
	

	//warlock - Improved Imp
	assign_Spell_to_SpellEntry( &sp , 18694 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_ON_PET ;
		sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
	}
	assign_Spell_to_SpellEntry( &sp , 18695 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_ON_PET ;
		sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
	}
	assign_Spell_to_SpellEntry( &sp , 18696 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_ON_PET ;
		sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
	}

	//warlock - Improved Voidwalker
	assign_Spell_to_SpellEntry( &sp , 18705 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_ON_PET ;
		sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
	}
	assign_Spell_to_SpellEntry( &sp , 18706 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_ON_PET ;
		sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
	}
	assign_Spell_to_SpellEntry( &sp , 18707 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_ON_PET ;
		sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
	}

	//warlock - Improved Succubus
	assign_Spell_to_SpellEntry( &sp , 18754 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_ON_PET ;
		sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
		sp->EffectImplicitTargetA[1] = EFF_TARGET_PET;
	}
	assign_Spell_to_SpellEntry( &sp , 18755 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_ON_PET ;
		sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
		sp->EffectImplicitTargetA[1] = EFF_TARGET_PET;
	}
	assign_Spell_to_SpellEntry( &sp , 18756 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_ON_PET ;
		sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
		sp->EffectImplicitTargetA[1] = EFF_TARGET_PET;
	}

	//warlock - Fel Intellect
	assign_Spell_to_SpellEntry( &sp , 18731 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_ON_PET ;
		sp->EffectApplyAuraName[0] = SPELL_AURA_MOD_PERCENT_STAT;
		sp->EffectMiscValue[0] = 3;
		sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
	}
	assign_Spell_to_SpellEntry( &sp , 18743 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_ON_PET ;
		sp->EffectApplyAuraName[0] = SPELL_AURA_MOD_PERCENT_STAT;
		sp->EffectMiscValue[0] = 3;
		sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
	}
	assign_Spell_to_SpellEntry( &sp , 18744 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_ON_PET ;
		sp->EffectApplyAuraName[0] = SPELL_AURA_MOD_PERCENT_STAT;
		sp->EffectMiscValue[0] = 3;
		sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
	}

	//warlock - Fel Stamina
	assign_Spell_to_SpellEntry( &sp , 18748 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_ON_PET ;
		sp->EffectApplyAuraName[0] = SPELL_AURA_MOD_PERCENT_STAT;
		sp->EffectMiscValue[0] = 2;
		sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
	}
	assign_Spell_to_SpellEntry( &sp , 18749 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_ON_PET ;
		sp->EffectApplyAuraName[0] = SPELL_AURA_MOD_PERCENT_STAT;
		sp->EffectMiscValue[0] = 2;
		sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
	}
	assign_Spell_to_SpellEntry( &sp , 18750 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_ON_PET ;
		sp->EffectApplyAuraName[0] = SPELL_AURA_MOD_PERCENT_STAT;
		sp->EffectMiscValue[0] = 2;
		sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
	}

	//warlock - Demonic Tactics
	assign_Spell_to_SpellEntry( &sp , 30242 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_ON_PET ;
		sp->EffectApplyAuraName[0] = SPELL_AURA_ADD_PCT_MODIFIER;
		sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
		//this is required since blizz uses spells for melee attacks while we use fixed functions
		sp->Effect[1] = SPELL_EFFECT_APPLY_AURA;
		sp->EffectApplyAuraName[1] = SPELL_AURA_MOD_DAMAGE_PERCENT_DONE;
		sp->EffectImplicitTargetA[1] = EFF_TARGET_PET;
		sp->EffectMiscValue[1] = SCHOOL_NORMAL;
		sp->EffectBasePoints[1] = sp->EffectBasePoints[0] ;
	}

	//warlock - Unholy Power
	assign_Spell_to_SpellEntry( &sp , 18769 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_ON_PET ;
		sp->EffectApplyAuraName[0] = SPELL_AURA_ADD_PCT_MODIFIER;
		sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
		sp->EffectSpellGroupRelation[0] = 4096;
		//this is required since blizz uses spells for melee attacks while we use fixed functions
		sp->Effect[1] = SPELL_EFFECT_APPLY_AURA;
		sp->EffectApplyAuraName[1] = SPELL_AURA_MOD_DAMAGE_PERCENT_DONE;
		sp->EffectImplicitTargetA[1] = EFF_TARGET_PET;
		sp->EffectMiscValue[1] = SCHOOL_NORMAL;
		sp->EffectBasePoints[1] = sp->EffectBasePoints[0] ;
	}
	assign_Spell_to_SpellEntry( &sp , 18770 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_ON_PET ;
		sp->EffectApplyAuraName[0] = SPELL_AURA_ADD_PCT_MODIFIER;
		sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
		sp->EffectSpellGroupRelation[0] = 4096;
		//this is required since blizz uses spells for melee attacks while we use fixed functions
		sp->Effect[1] = SPELL_EFFECT_APPLY_AURA;
		sp->EffectApplyAuraName[1] = SPELL_AURA_MOD_DAMAGE_PERCENT_DONE;
		sp->EffectImplicitTargetA[1] = EFF_TARGET_PET;
		sp->EffectMiscValue[1] = SCHOOL_NORMAL;
		sp->EffectBasePoints[1] = sp->EffectBasePoints[0] ;
	}
	assign_Spell_to_SpellEntry( &sp , 18771 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_ON_PET ;
		sp->EffectApplyAuraName[0] = SPELL_AURA_ADD_PCT_MODIFIER;
		sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
		sp->EffectSpellGroupRelation[0] = 4096;
		//this is required since blizz uses spells for melee attacks while we use fixed functions
		sp->Effect[1] = SPELL_EFFECT_APPLY_AURA;
		sp->EffectApplyAuraName[1] = SPELL_AURA_MOD_DAMAGE_PERCENT_DONE;
		sp->EffectImplicitTargetA[1] = EFF_TARGET_PET;
		sp->EffectMiscValue[1] = SCHOOL_NORMAL;
		sp->EffectBasePoints[1] = sp->EffectBasePoints[0] ;
	}
	assign_Spell_to_SpellEntry( &sp , 18772 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_ON_PET ;
		sp->EffectApplyAuraName[0] = SPELL_AURA_ADD_PCT_MODIFIER;
		sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
		sp->EffectSpellGroupRelation[0] = 4096;
		//this is required since blizz uses spells for melee attacks while we use fixed functions
		sp->Effect[1] = SPELL_EFFECT_APPLY_AURA;
		sp->EffectApplyAuraName[1] = SPELL_AURA_MOD_DAMAGE_PERCENT_DONE;
		sp->EffectImplicitTargetA[1] = EFF_TARGET_PET;
		sp->EffectMiscValue[1] = SCHOOL_NORMAL;
		sp->EffectBasePoints[1] = sp->EffectBasePoints[0] ;
	}
	assign_Spell_to_SpellEntry( &sp , 18773 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_ON_PET ;
		sp->EffectApplyAuraName[0] = SPELL_AURA_ADD_PCT_MODIFIER;
		sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
		sp->EffectSpellGroupRelation[0] = 4096;
		//this is required since blizz uses spells for melee attacks while we use fixed functions
		sp->Effect[1] = SPELL_EFFECT_APPLY_AURA;
		sp->EffectApplyAuraName[1] = SPELL_AURA_MOD_DAMAGE_PERCENT_DONE;
		sp->EffectImplicitTargetA[1] = EFF_TARGET_PET;
		sp->EffectMiscValue[1] = SCHOOL_NORMAL;
		sp->EffectBasePoints[1] = sp->EffectBasePoints[0] ;
	}

	//warlock - Master Demonologist - 25 spells here
	assign_Spell_to_SpellEntry( &sp , 23785 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_ON_PET | SPELL_FLAG_IS_EXPIREING_WITH_PET;
		sp->Effect[0] = SPELL_EFFECT_TRIGGER_SPELL;
		sp->EffectTriggerSpell[0] = 23784;
	}
	assign_Spell_to_SpellEntry( &sp , 23822 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_ON_PET | SPELL_FLAG_IS_EXPIREING_WITH_PET;
		sp->Effect[0] = SPELL_EFFECT_TRIGGER_SPELL;
		sp->EffectTriggerSpell[0] = 23830;
	}
	assign_Spell_to_SpellEntry( &sp , 23823 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_ON_PET | SPELL_FLAG_IS_EXPIREING_WITH_PET;
		sp->Effect[0] = SPELL_EFFECT_TRIGGER_SPELL;
		sp->EffectTriggerSpell[0] = 23831;
	}
	assign_Spell_to_SpellEntry( &sp , 23824 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_ON_PET | SPELL_FLAG_IS_EXPIREING_WITH_PET;
		sp->Effect[0] = SPELL_EFFECT_TRIGGER_SPELL;
		sp->EffectTriggerSpell[0] = 23832;
	}
	assign_Spell_to_SpellEntry( &sp , 23825 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_CASTED_ON_PET_SUMMON_ON_PET | SPELL_FLAG_IS_EXPIREING_WITH_PET;
		sp->Effect[0] = SPELL_EFFECT_TRIGGER_SPELL;
		sp->EffectTriggerSpell[0] = 35708;
	}
	//and the rest
	assign_Spell_to_SpellEntry( &sp , 23784 );
	if( sp != NULL )
		sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
	assign_Spell_to_SpellEntry( &sp , 23830 );
	if( sp != NULL )
		sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
	assign_Spell_to_SpellEntry( &sp , 23831 );
	if( sp != NULL )
		sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
	assign_Spell_to_SpellEntry( &sp , 23832 );
	if( sp != NULL )
		sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
	assign_Spell_to_SpellEntry( &sp , 35708 );
	if( sp != NULL )
		sp->EffectImplicitTargetA[0] = EFF_TARGET_PET;
	assign_Spell_to_SpellEntry( &sp , 23759 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_EXPIREING_WITH_PET;
		sp->Effect[0] = SPELL_EFFECT_APPLY_AURA;
	}
	assign_Spell_to_SpellEntry( &sp , 23760 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_EXPIREING_WITH_PET;
		sp->Effect[0] = SPELL_EFFECT_APPLY_AURA;
	}
	assign_Spell_to_SpellEntry( &sp , 23761 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_EXPIREING_WITH_PET;
		sp->Effect[0] = SPELL_EFFECT_APPLY_AURA;
	}
	assign_Spell_to_SpellEntry( &sp , 23762 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_EXPIREING_WITH_PET;
		sp->Effect[0] = SPELL_EFFECT_APPLY_AURA;
	}
	assign_Spell_to_SpellEntry( &sp , 23826 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_EXPIREING_WITH_PET;
		sp->Effect[0] = SPELL_EFFECT_APPLY_AURA;
	}
	assign_Spell_to_SpellEntry( &sp , 23827 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_EXPIREING_WITH_PET;
		sp->Effect[0] = SPELL_EFFECT_APPLY_AURA;
	}
	assign_Spell_to_SpellEntry( &sp , 23828 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_EXPIREING_WITH_PET;
		sp->Effect[0] = SPELL_EFFECT_APPLY_AURA;
	}
	assign_Spell_to_SpellEntry( &sp , 23829 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_EXPIREING_WITH_PET;
		sp->Effect[0] = SPELL_EFFECT_APPLY_AURA;
	}
	for(uint32 i=23833;i<=23844;i++)
	{
		assign_Spell_to_SpellEntry( &sp , i );
		if( sp != NULL )
		{
			sp->c_is_flags |= SPELL_FLAG_IS_EXPIREING_WITH_PET;
			sp->Effect[0] = SPELL_EFFECT_APPLY_AURA;
		}
	}
	assign_Spell_to_SpellEntry( &sp , 35702 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_EXPIREING_WITH_PET;
		sp->Effect[0] = SPELL_EFFECT_APPLY_AURA;
		sp->Effect[1] = 0; //hacks, we are handling this in another way
	}
	assign_Spell_to_SpellEntry( &sp , 35703 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_EXPIREING_WITH_PET;
		sp->Effect[0] = SPELL_EFFECT_APPLY_AURA;
		sp->Effect[1] = 0; //hacks, we are handling this in another way
	}
	assign_Spell_to_SpellEntry( &sp , 35704 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_EXPIREING_WITH_PET;
		sp->Effect[0] = SPELL_EFFECT_APPLY_AURA;
		sp->Effect[1] = 0; //hacks, we are handling this in another way
	}
	assign_Spell_to_SpellEntry( &sp , 35705 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_EXPIREING_WITH_PET;
		sp->Effect[0] = SPELL_EFFECT_APPLY_AURA;
		sp->Effect[1] = 0; //hacks, we are handling this in another way
	}
	assign_Spell_to_SpellEntry( &sp , 35706 );
	if( sp != NULL )
	{
		sp->c_is_flags |= SPELL_FLAG_IS_EXPIREING_WITH_PET;
		sp->Effect[0] = SPELL_EFFECT_APPLY_AURA;
		sp->Effect[1] = 0; //hacks, we are handling this in another way
	}
	//warlock - Improved Healthstone
	assign_Spell_to_SpellEntry( &sp , 18692 );
	if( sp != NULL )
	{
		sp->EffectApplyAuraName[0] = SPELL_AURA_ADD_PCT_MODIFIER;
		sp->EffectMiscValue[0] = SMT_SPELL_VALUE;
		sp->EffectImplicitTargetA[0] = EFF_TARGET_SELF;
		sp->EffectSpellGroupRelation[0] = 65536;
	}
	assign_Spell_to_SpellEntry( &sp , 18693 );
	if( sp != NULL )
	{
		sp->EffectApplyAuraName[0] = SPELL_AURA_ADD_PCT_MODIFIER;
		sp->EffectMiscValue[0] = SMT_SPELL_VALUE;
		sp->EffectImplicitTargetA[0] = EFF_TARGET_SELF;
		sp->EffectSpellGroupRelation[0] = 65536;
	}

	//warlock - Unstable Affliction
	assign_Spell_to_SpellEntry( &sp , 30108 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_PRE_DISPELL_AURA_VICTIM ;
		sp->procChance = 100;
		sp->Effect[1] = SPELL_EFFECT_APPLY_AURA;
		sp->EffectApplyAuraName[1] = SPELL_AURA_PROC_TRIGGER_SPELL;
		sp->EffectTriggerSpell[1] = 43523;
		sp->EffectImplicitTargetA[1] = EFF_TARGET_SINGLE_FRIEND;
	}
	assign_Spell_to_SpellEntry( &sp , 30404 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_PRE_DISPELL_AURA_VICTIM ;
		sp->procChance = 100;
		sp->Effect[1] = SPELL_EFFECT_APPLY_AURA;
		sp->EffectApplyAuraName[1] = SPELL_AURA_PROC_TRIGGER_SPELL;
		sp->EffectTriggerSpell[1] = 43523;
		sp->EffectImplicitTargetA[1] = EFF_TARGET_SINGLE_FRIEND;
	}
	assign_Spell_to_SpellEntry( &sp , 30405 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_PRE_DISPELL_AURA_VICTIM ;
		sp->procChance = 100;
		sp->Effect[1] = SPELL_EFFECT_APPLY_AURA;
		sp->EffectApplyAuraName[1] = SPELL_AURA_PROC_TRIGGER_SPELL;
		sp->EffectTriggerSpell[1] = 43523;
		sp->EffectImplicitTargetA[1] = EFF_TARGET_SINGLE_FRIEND;
	}

	//warlock - Improved Drain Soul
	assign_Spell_to_SpellEntry( &sp , 18213 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_TARGET_DIE | PROC_TARGET_SELF;
		sp->procChance = 100;
		sp->Effect[0] = SPELL_EFFECT_APPLY_AURA;
		sp->EffectApplyAuraName[0] = SPELL_AURA_PROC_TRIGGER_SPELL;
		sp->EffectTriggerSpell[0] = 18371;
		sp->EffectImplicitTargetA[0] = EFF_TARGET_SELF;
		sp->EffectSpellGroupRelation[1] = 2 | 8 | 32768 | 2147483648UL | 1024 | 16384 | 262144 | 16 | 524288 | 4194304;
		sp->Effect[2] = 0 ; //remove this effect
	}
	assign_Spell_to_SpellEntry( &sp , 18372 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_TARGET_DIE | PROC_TARGET_SELF;
		sp->procChance = 100;
		sp->Effect[0] = SPELL_EFFECT_APPLY_AURA;
		sp->EffectApplyAuraName[0] = SPELL_AURA_PROC_TRIGGER_SPELL;
		sp->EffectTriggerSpell[0] = 18371;
		sp->EffectImplicitTargetA[0] = EFF_TARGET_SELF;
		sp->EffectSpellGroupRelation[1] = 2 | 8 | 32768 | 2147483648UL | 1024 | 16384 | 262144 | 16 | 524288 | 4194304;
		sp->Effect[2] = 0 ; //remove this effect
	}

	//warlock - Shadow Embrace
	assign_Spell_to_SpellEntry( &sp , 32385 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_CAST_SPELL;
		sp->Effect[1] = 0 ; //remove this effect ? Maybe remove the other one :P xD
	}
	assign_Spell_to_SpellEntry( &sp , 32387 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_CAST_SPELL;
		sp->Effect[1] = 0 ; //remove this effect ? Maybe remove the other one :P xD
	}
	assign_Spell_to_SpellEntry( &sp , 32392 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_CAST_SPELL;
		sp->Effect[1] = 0 ; //remove this effect ? Maybe remove the other one :P xD
	}
	assign_Spell_to_SpellEntry( &sp , 32393 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_CAST_SPELL;
		sp->Effect[1] = 0 ; //remove this effect ? Maybe remove the other one :P xD
	}
	assign_Spell_to_SpellEntry( &sp , 32394 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_CAST_SPELL;
		sp->Effect[1] = 0 ; //remove this effect ? Maybe remove the other one :P xD
	}

	//warlock - Bane
	assign_Spell_to_SpellEntry( &sp , 17788 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 1 | 4;
		sp->EffectSpellGroupRelation_high[1] = 128;
	}
	assign_Spell_to_SpellEntry( &sp , 17789 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 1 | 4;
		sp->EffectSpellGroupRelation_high[1] = 128;
	}
	assign_Spell_to_SpellEntry( &sp , 17790 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 1 | 4;
		sp->EffectSpellGroupRelation_high[1] = 128;
	}
	assign_Spell_to_SpellEntry( &sp , 17791 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 1 | 4;
		sp->EffectSpellGroupRelation_high[1] = 128;
	}
	assign_Spell_to_SpellEntry( &sp , 17792 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 1 | 4;
		sp->EffectSpellGroupRelation_high[1] = 128;
	}

	//warlock - soul leech
	assign_Spell_to_SpellEntry( &sp , 30293 );
	if( sp != NULL )
	{
		sp->Effect[0] = 6; //aura
		sp->EffectApplyAuraName[0] = 42;
		sp->EffectTriggerSpell[0] = 30294;
		sp->procFlags = uint32(PROC_ON_CAST_SPELL|PROC_TARGET_SELF);
	}
	assign_Spell_to_SpellEntry( &sp , 30295 );
	if( sp != NULL )
	{
		sp->Effect[0] = 6; //aura
		sp->EffectApplyAuraName[0] = 42;
		sp->EffectTriggerSpell[0] = 30294;
		sp->procFlags = uint32(PROC_ON_CAST_SPELL|PROC_TARGET_SELF);
	}
	assign_Spell_to_SpellEntry( &sp , 30296 );
	if( sp != NULL )
	{
		sp->Effect[0] = 6; //aura
		sp->EffectApplyAuraName[0] = 42;
		sp->EffectTriggerSpell[0] = 30294;
		sp->procFlags = uint32(PROC_ON_CAST_SPELL|PROC_TARGET_SELF);
	}

	//warlock - Pyroclasm
	assign_Spell_to_SpellEntry( &sp , 18073 );
	if( sp != NULL )
	{
		sp->Effect[0] = 0; //delete this owerride effect :P
		sp->EffectTriggerSpell[1] = 18093; //trigger spell was wrong :P
		sp->procFlags = PROC_ON_ANY_HOSTILE_ACTION;
		sp->procChance = 13; //god, save us from fixed values !
	}
	assign_Spell_to_SpellEntry( &sp , 18096 );
	if( sp != NULL )
	{
		sp->Effect[0] = 0; //delete this owerride effect :P
		sp->EffectTriggerSpell[1] = 18093; //trigger spell was wrong :P
		sp->procFlags = PROC_ON_ANY_HOSTILE_ACTION;
		sp->procChance = 26; //god, save us from fixed values !
	}

	//Mage - improved scorch
	assign_Spell_to_SpellEntry( &sp , 11095 );
	if( sp != NULL )
	{
		sp->EffectApplyAuraName[0] = 42; 
		sp->procChance =33;
		sp->procFlags = PROC_ON_CAST_SPELL;
	}
	assign_Spell_to_SpellEntry( &sp , 12872 );
	if( sp != NULL )
	{
		sp->EffectApplyAuraName[0] = 42; 
		sp->procChance =66;
		sp->procFlags = PROC_ON_CAST_SPELL;
	}
	assign_Spell_to_SpellEntry( &sp , 12873 );
	if( sp != NULL )
	{
		sp->EffectApplyAuraName[0] = 42; 
		sp->procChance =100;
		sp->procFlags = PROC_ON_CAST_SPELL;
	}

	//Nature's Grasp
	assign_Spell_to_SpellEntry( &sp , 16689 );
	if( sp != NULL )
	{
		sp->Effect[0] = 6; 
		sp->EffectApplyAuraName[0] = 42; 
		sp->EffectTriggerSpell[0] = 339; 
		sp->Effect[1] = 0; 
		sp->procFlags = PROC_ON_MELEE_ATTACK_VICTIM | PROC_REMOVEONUSE;
		sp->AuraInterruptFlags = 0; //we remove it on proc or timeout
	}
	assign_Spell_to_SpellEntry( &sp , 16810 );
	if( sp != NULL )
	{
		sp->Effect[0] = 6; 
		sp->EffectApplyAuraName[0] = 42; 
		sp->EffectTriggerSpell[0] = 1062; 
		sp->Effect[1] = 0; 
		sp->procFlags = PROC_ON_MELEE_ATTACK_VICTIM | PROC_REMOVEONUSE;
		sp->AuraInterruptFlags = 0; //we remove it on proc or timeout
	}
	assign_Spell_to_SpellEntry( &sp , 16811 );
	if( sp != NULL )
	{
		sp->Effect[0] = 6; 
		sp->EffectApplyAuraName[0] = 42; 
		sp->EffectTriggerSpell[0] = 5195; 
		sp->Effect[1] = 0; 
		sp->procFlags = PROC_ON_MELEE_ATTACK_VICTIM | PROC_REMOVEONUSE;
		sp->AuraInterruptFlags = 0; //we remove it on proc or timeout
	}
	assign_Spell_to_SpellEntry( &sp , 16812 );
	if( sp != NULL )
	{
		sp->Effect[0] = 6; 
		sp->EffectApplyAuraName[0] = 42; 
		sp->EffectTriggerSpell[0] = 5196; 
		sp->Effect[1] = 0; 
		sp->procFlags = PROC_ON_MELEE_ATTACK_VICTIM | PROC_REMOVEONUSE;
		sp->AuraInterruptFlags = 0; //we remove it on proc or timeout
	}
	assign_Spell_to_SpellEntry( &sp , 16813 );
	if( sp != NULL )
	{
		sp->Effect[0] = 6; 
		sp->EffectApplyAuraName[0] = 42; 
		sp->EffectTriggerSpell[0] = 9852; 
		sp->Effect[1] = 0; 
		sp->procFlags = PROC_ON_MELEE_ATTACK_VICTIM | PROC_REMOVEONUSE;
		sp->AuraInterruptFlags = 0; //we remove it on proc or timeout
	}
	assign_Spell_to_SpellEntry( &sp , 17329 );
	if( sp != NULL )
	{
		sp->Effect[0] = 6; 
		sp->EffectApplyAuraName[0] = 42; 
		sp->EffectTriggerSpell[0] = 9853; 
		sp->Effect[1] = 0; 
		sp->procFlags = PROC_ON_MELEE_ATTACK_VICTIM | PROC_REMOVEONUSE;
		sp->AuraInterruptFlags = 0; //we remove it on proc or timeout
	}
	assign_Spell_to_SpellEntry( &sp , 27009 );
	if( sp != NULL )
	{
		sp->Effect[0] = 6; 
		sp->EffectApplyAuraName[0] = 42; 
		sp->EffectTriggerSpell[0] = 26989; 
		sp->Effect[1] = 0; 
		sp->procFlags = PROC_ON_MELEE_ATTACK_VICTIM | PROC_REMOVEONUSE;
		sp->AuraInterruptFlags = 0; //we remove it on proc or timeout
	}

	//wrath of air totem targets sorounding creatures instead of us
	assign_Spell_to_SpellEntry( &sp , 2895 );
	if( sp != NULL )
	{
		sp->EffectImplicitTargetA[0]=EFF_TARGET_SELF;
		sp->EffectImplicitTargetA[1]=EFF_TARGET_SELF;
		sp->EffectImplicitTargetA[2]=0;
		sp->EffectImplicitTargetB[0]=0;
		sp->EffectImplicitTargetB[1]=0;
		sp->EffectImplicitTargetB[2]=0;
	}

	//-----BLEED effects----- (setting mech to BLEED)
	//garrot
	assign_Spell_to_SpellEntry( &sp , 14179 );
	if( sp != NULL )
		sp->MechanicsType = MECHANIC_BLEEDING;
	assign_Spell_to_SpellEntry( &sp , 8631 );
	if( sp != NULL )
		sp->MechanicsType = MECHANIC_BLEEDING;
	assign_Spell_to_SpellEntry( &sp , 8632 );
	if( sp != NULL )
		sp->MechanicsType = MECHANIC_BLEEDING;
	assign_Spell_to_SpellEntry( &sp , 8633 );
	if( sp != NULL )
		sp->MechanicsType = MECHANIC_BLEEDING;
	assign_Spell_to_SpellEntry( &sp , 11289 );
	if( sp != NULL )
		sp->MechanicsType = MECHANIC_BLEEDING;
	assign_Spell_to_SpellEntry( &sp , 11290 );
	if( sp != NULL )
		sp->MechanicsType = MECHANIC_BLEEDING;
	assign_Spell_to_SpellEntry( &sp , 26839 );
	if( sp != NULL )
		sp->MechanicsType = MECHANIC_BLEEDING;
	assign_Spell_to_SpellEntry( &sp , 26884 );
	if( sp != NULL )
		sp->MechanicsType = MECHANIC_BLEEDING;

	//pounce
	assign_Spell_to_SpellEntry( &sp , 9007 );
	if( sp != NULL )
		sp->MechanicsType = MECHANIC_BLEEDING;
	assign_Spell_to_SpellEntry( &sp , 9824 );
	if( sp != NULL )
		sp->MechanicsType = MECHANIC_BLEEDING;
	assign_Spell_to_SpellEntry( &sp , 9826 );
	if( sp != NULL )
		sp->MechanicsType = MECHANIC_BLEEDING;
	assign_Spell_to_SpellEntry( &sp , 27007 );
	if( sp != NULL )
		sp->MechanicsType = MECHANIC_BLEEDING;

	//rend
	assign_Spell_to_SpellEntry( &sp , 772 );
	if( sp != NULL )
		sp->MechanicsType = MECHANIC_BLEEDING;
	assign_Spell_to_SpellEntry( &sp , 6546 );
	if( sp != NULL )
		sp->MechanicsType = MECHANIC_BLEEDING;
	assign_Spell_to_SpellEntry( &sp , 6547 );
	if( sp != NULL )
		sp->MechanicsType = MECHANIC_BLEEDING;
	assign_Spell_to_SpellEntry( &sp , 6548 );
	if( sp != NULL )
		sp->MechanicsType = MECHANIC_BLEEDING;
	assign_Spell_to_SpellEntry( &sp , 11572 );
	if( sp != NULL )
		sp->MechanicsType = MECHANIC_BLEEDING;
	assign_Spell_to_SpellEntry( &sp , 11573 );
	if( sp != NULL )
		sp->MechanicsType = MECHANIC_BLEEDING;
	assign_Spell_to_SpellEntry( &sp , 11574 );
	if( sp != NULL )
		sp->MechanicsType = MECHANIC_BLEEDING;
	assign_Spell_to_SpellEntry( &sp , 25208 );
	if( sp != NULL )
		sp->MechanicsType = MECHANIC_BLEEDING;

	//rip
	assign_Spell_to_SpellEntry( &sp , 1079 );
	if( sp != NULL )
		sp->MechanicsType = MECHANIC_BLEEDING;
	assign_Spell_to_SpellEntry( &sp , 9492 );
	if( sp != NULL )
		sp->MechanicsType = MECHANIC_BLEEDING;
	assign_Spell_to_SpellEntry( &sp , 9493 );
	if( sp != NULL )
		sp->MechanicsType = MECHANIC_BLEEDING;
	assign_Spell_to_SpellEntry( &sp , 9752 );
	if( sp != NULL )
		sp->MechanicsType = MECHANIC_BLEEDING;
	assign_Spell_to_SpellEntry( &sp , 9894 );
	if( sp != NULL )
		sp->MechanicsType = MECHANIC_BLEEDING;
	assign_Spell_to_SpellEntry( &sp , 9896 );
	if( sp != NULL )
		sp->MechanicsType = MECHANIC_BLEEDING;
	assign_Spell_to_SpellEntry( &sp , 27008 );
	if( sp != NULL )
		sp->MechanicsType = MECHANIC_BLEEDING;

	//rupture
	assign_Spell_to_SpellEntry( &sp , 1943 );
	if( sp != NULL )
		sp->MechanicsType = MECHANIC_BLEEDING;
	assign_Spell_to_SpellEntry( &sp , 8639 );
	if( sp != NULL )
		sp->MechanicsType = MECHANIC_BLEEDING;
	assign_Spell_to_SpellEntry( &sp , 8640 );
	if( sp != NULL )
		sp->MechanicsType = MECHANIC_BLEEDING;
	assign_Spell_to_SpellEntry( &sp , 11273 );
	if( sp != NULL )
		sp->MechanicsType = MECHANIC_BLEEDING;
	assign_Spell_to_SpellEntry( &sp , 11274 );
	if( sp != NULL )
		sp->MechanicsType = MECHANIC_BLEEDING;
	assign_Spell_to_SpellEntry( &sp , 11275 );
	if( sp != NULL )
		sp->MechanicsType = MECHANIC_BLEEDING;
	assign_Spell_to_SpellEntry( &sp , 26867 );
	if( sp != NULL )
		sp->MechanicsType = MECHANIC_BLEEDING;

	//rake
	assign_Spell_to_SpellEntry( &sp , 1822 );
	if( sp != NULL )
		sp->MechanicsType = MECHANIC_BLEEDING;
	assign_Spell_to_SpellEntry( &sp , 1823 );
	if( sp != NULL )
		sp->MechanicsType = MECHANIC_BLEEDING;
	assign_Spell_to_SpellEntry( &sp , 1824 );
	if( sp != NULL )
		sp->MechanicsType = MECHANIC_BLEEDING;
	assign_Spell_to_SpellEntry( &sp , 9904 );
	if( sp != NULL )
		sp->MechanicsType = MECHANIC_BLEEDING;
	assign_Spell_to_SpellEntry( &sp , 27003 );
	if( sp != NULL )
		sp->MechanicsType = MECHANIC_BLEEDING;

	//lacerate
	assign_Spell_to_SpellEntry( &sp , 33745 );
	if( sp != NULL )
		sp->MechanicsType = MECHANIC_BLEEDING;

	//deuid - Pounce Bleed
	assign_Spell_to_SpellEntry( &sp , 9007 );
	if( sp != NULL )
		sp->DurationIndex = 18000;
	assign_Spell_to_SpellEntry( &sp , 9824 );
	if( sp != NULL )
		sp->DurationIndex = 18000;
	assign_Spell_to_SpellEntry( &sp , 9826 );
	if( sp != NULL )
		sp->DurationIndex = 18000;
	assign_Spell_to_SpellEntry( &sp , 27007 );
	if( sp != NULL )
		sp->DurationIndex = 18000;

	//Druid: Natural Shapeshifter
	assign_Spell_to_SpellEntry( &sp , 16833 );
	if( sp != NULL )
		sp->DurationIndex = 0;
	assign_Spell_to_SpellEntry( &sp , 16834 );
	if( sp != NULL )
		sp->DurationIndex = 0;
	assign_Spell_to_SpellEntry( &sp , 16835 );
	if( sp != NULL )
		sp->DurationIndex = 0;

	//Priest: Shadow Power
	assign_Spell_to_SpellEntry( &sp , 15310 );
	if( sp != NULL )
	{
		uint32 group = sp->EffectSpellGroupRelation[0];
		assign_Spell_to_SpellEntry( &sp , 33221 );
		if( sp != NULL )
			sp->EffectSpellGroupRelation[0] = group;
		assign_Spell_to_SpellEntry( &sp , 33222 );
		if( sp != NULL )
			sp->EffectSpellGroupRelation[0] = group;
		assign_Spell_to_SpellEntry( &sp , 33223 );
		if( sp != NULL )
			sp->EffectSpellGroupRelation[0] = group;
		assign_Spell_to_SpellEntry( &sp , 33224 );
		if( sp != NULL )
			sp->EffectSpellGroupRelation[0] = group;
		assign_Spell_to_SpellEntry( &sp , 33225 );
		if( sp != NULL )
			sp->EffectSpellGroupRelation[0] = group;
	}

	//Relentless Strikes
	assign_Spell_to_SpellEntry( &sp , 14179 );
	if( sp != NULL )
	{
		sp->EffectApplyAuraName[0] = 42;//proc spell
		sp->procFlags = PROC_ON_CAST_SPELL;
		sp->EffectBasePoints[1] = 20; //client showes 20% chance but whe do not have it ? :O
	}
	//priest - surge of light
	assign_Spell_to_SpellEntry( &sp , 33150 );
	if( sp != NULL )
		sp->procFlags = uint32(PROC_ON_SPELL_CRIT_HIT_VICTIM | PROC_TARGET_SELF);
	assign_Spell_to_SpellEntry( &sp , 33154 );
	if( sp != NULL )
		sp->procFlags = uint32(PROC_ON_SPELL_CRIT_HIT_VICTIM | PROC_TARGET_SELF);
	assign_Spell_to_SpellEntry( &sp , 33151 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 128;
		sp->EffectSpellGroupRelation[1] = 128;
		sp->EffectSpellGroupRelation[2] = 128;
		sp->AuraInterruptFlags = AURA_INTERRUPT_ON_CAST_SPELL;
	}

	//Seal of Justice -lowered proc chance (experimental values !)
	assign_Spell_to_SpellEntry( &sp , 20164 );
	if( sp != NULL )
		sp->procChance = 20;
	assign_Spell_to_SpellEntry( &sp , 31895 );
	if( sp != NULL )
		sp->procChance = 20;

	//make Berserking a simple spell 
	assign_Spell_to_SpellEntry( &sp , 20554 );
	if( sp != NULL )
	{
		sp->Effect[0] = SPELL_EFFECT_TRIGGER_SPELL;
		sp->EffectTriggerSpell[0] = 26635;
	}
	assign_Spell_to_SpellEntry( &sp , 26296 );
	if( sp != NULL )
	{
		sp->Effect[0] = SPELL_EFFECT_TRIGGER_SPELL;
		sp->EffectTriggerSpell[0] = 26635;
	}
	assign_Spell_to_SpellEntry( &sp , 26297 );
	if( sp != NULL )
	{
		sp->Effect[0] = SPELL_EFFECT_TRIGGER_SPELL;
		sp->EffectTriggerSpell[0] = 26635;
	}

	//rogue - intiative
	assign_Spell_to_SpellEntry( &sp , 13976 );
	if( sp != NULL )
	{
		sp->EffectApplyAuraName[0] = 42;
		sp->procFlags = uint32(PROC_ON_CAST_SPELL|PROC_TARGET_SELF);
	}
	assign_Spell_to_SpellEntry( &sp , 13979 );
	if( sp != NULL )
	{
		sp->EffectApplyAuraName[0] = 42;
		sp->procFlags = uint32(PROC_ON_CAST_SPELL|PROC_TARGET_SELF);
	}
	assign_Spell_to_SpellEntry( &sp , 13980 );
	if( sp != NULL )
	{
		sp->EffectApplyAuraName[0] = 42;
		sp->procFlags = uint32(PROC_ON_CAST_SPELL|PROC_TARGET_SELF);
	}

	//this an on equip item spell(2824) :  ice arrow(29501)
	assign_Spell_to_SpellEntry( &sp , 29501 );
	if( sp != NULL )
	{
		sp->procChance = 30;//some say it is triggered every now and then
		sp->procFlags = PROC_ON_RANGED_ATTACK;
	}

	//warrior - second wind should trigger on self
	assign_Spell_to_SpellEntry( &sp , 29841 );
	if( sp != NULL )
		sp->procFlags |= PROC_TARGET_SELF;
	assign_Spell_to_SpellEntry( &sp , 29842 );
	if( sp != NULL )
		sp->procFlags |= PROC_TARGET_SELF;

	//warrior - berserker rage is missing 1 effect = regenerate rage
	assign_Spell_to_SpellEntry( &sp , 18499 );
	if( sp != NULL )
	{
		sp->Effect[2] = 6;
		sp->EffectApplyAuraName[2] = 42;
		sp->EffectTriggerSpell[2] = 9174; //not sure if this is the one. In my time this adds 30 rage
		sp->procFlags = PROC_ON_ANY_DAMAGE_VICTIM | PROC_TARGET_SELF;
	}

	//warrior - improved berserker rage
	assign_Spell_to_SpellEntry( &sp , 20500 );
	if( sp != NULL )
		sp->procFlags = PROC_ON_ANY_DAMAGE_VICTIM | PROC_TARGET_SELF;
	assign_Spell_to_SpellEntry( &sp , 20501 );
	if( sp != NULL )
		sp->procFlags = PROC_ON_ANY_DAMAGE_VICTIM | PROC_TARGET_SELF;

	//warrior - berserker rage is missing 1 effect = regenerate rage
	assign_Spell_to_SpellEntry( &sp , 18499 );
	if( sp != NULL )
	{
		sp->Effect[2] = 6;
		sp->EffectApplyAuraName[2] = 42;
		sp->EffectTriggerSpell[2] = 9174; //not sure if this is the one. In my time this adds 30 rage
		sp->procFlags = PROC_ON_ANY_DAMAGE_VICTIM | PROC_TARGET_SELF;
	}

	//warrior - Blood Frenzy
	assign_Spell_to_SpellEntry( &sp , 29836 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_CAST_SPELL;
		sp->EffectApplyAuraName[0] = 42; // who uses 109 aura anyway ?
	}
	assign_Spell_to_SpellEntry( &sp , 29859 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_CAST_SPELL;
		sp->EffectApplyAuraName[0] = 42; // who uses 109 aura anyway ?
	}

	//warrior - Rampage
	assign_Spell_to_SpellEntry( &sp , 30030 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_MELEE_ATTACK | PROC_TARGET_SELF;
		sp->EffectTriggerSpell[0] = sp->EffectTriggerSpell[1];
	}
	assign_Spell_to_SpellEntry( &sp , 30033 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_MELEE_ATTACK | PROC_TARGET_SELF;
		sp->EffectTriggerSpell[0] = sp->EffectTriggerSpell[1];
	}

	//warrior - Unbridled Wrath
	assign_Spell_to_SpellEntry( &sp , 12322 );
	if( sp != NULL )
		sp->procFlags = PROC_ON_MELEE_ATTACK | PROC_TARGET_SELF;
	assign_Spell_to_SpellEntry( &sp , 12999 );
	if( sp != NULL )
		sp->procFlags = PROC_ON_MELEE_ATTACK | PROC_TARGET_SELF;
	assign_Spell_to_SpellEntry( &sp , 13000 );
	if( sp != NULL )
		sp->procFlags = PROC_ON_MELEE_ATTACK | PROC_TARGET_SELF;
	assign_Spell_to_SpellEntry( &sp , 13001 );
	if( sp != NULL )
		sp->procFlags = PROC_ON_MELEE_ATTACK | PROC_TARGET_SELF;
	assign_Spell_to_SpellEntry( &sp , 13002 );
	if( sp != NULL )
		sp->procFlags = PROC_ON_MELEE_ATTACK | PROC_TARGET_SELF;

	//warrior - Commanding Presence
	assign_Spell_to_SpellEntry( &sp , 12318 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 65536 ;
		sp->EffectSpellGroupRelation_high[0] = 128;
	}
	assign_Spell_to_SpellEntry( &sp , 12857 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 65536 ;
		sp->EffectSpellGroupRelation_high[0] = 128;
	}
	assign_Spell_to_SpellEntry( &sp , 12858 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 65536 ;
		sp->EffectSpellGroupRelation_high[0] = 128;
	}
	assign_Spell_to_SpellEntry( &sp , 12860 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 65536 ;
		sp->EffectSpellGroupRelation_high[0] = 128;
	}
	assign_Spell_to_SpellEntry( &sp , 12861 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 65536 ;
		sp->EffectSpellGroupRelation_high[0] = 128;
	}

	//warrior - Booming Voice
	assign_Spell_to_SpellEntry( &sp , 12321 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 65536 | 131072;
		sp->EffectSpellGroupRelation_high[0] = 128;
		sp->EffectSpellGroupRelation[1] = 65536 | 131072;
		sp->EffectSpellGroupRelation_high[1] = 128;
	}
	assign_Spell_to_SpellEntry( &sp , 12835 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 65536 | 131072;
		sp->EffectSpellGroupRelation_high[0] = 128;
		sp->EffectSpellGroupRelation[1] = 65536 | 131072;
		sp->EffectSpellGroupRelation_high[1] = 128;
	}
	assign_Spell_to_SpellEntry( &sp , 12836 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 65536 | 131072;
		sp->EffectSpellGroupRelation_high[0] = 128;
		sp->EffectSpellGroupRelation[1] = 65536 | 131072;
		sp->EffectSpellGroupRelation_high[1] = 128;
	}
	assign_Spell_to_SpellEntry( &sp , 12837 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 65536 | 131072;
		sp->EffectSpellGroupRelation_high[0] = 128;
		sp->EffectSpellGroupRelation[1] = 65536 | 131072;
		sp->EffectSpellGroupRelation_high[1] = 128;
	}
	assign_Spell_to_SpellEntry( &sp , 12838 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 65536 | 131072;
		sp->EffectSpellGroupRelation_high[0] = 128;
		sp->EffectSpellGroupRelation[1] = 65536 | 131072;
		sp->EffectSpellGroupRelation_high[1] = 128;
	}

	//warrior - Improved Intercept
	assign_Spell_to_SpellEntry( &sp , 29888 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 1073741824;
	assign_Spell_to_SpellEntry( &sp , 29889 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 1073741824;

	//warrior - Improved Mortal Strike
	assign_Spell_to_SpellEntry( &sp , 35446 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 33554432;
	assign_Spell_to_SpellEntry( &sp , 35448 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 33554432;
	assign_Spell_to_SpellEntry( &sp , 35449 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 33554432;
	assign_Spell_to_SpellEntry( &sp , 35450 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 33554432;
	assign_Spell_to_SpellEntry( &sp , 35451 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 33554432;

	//warrior - Focused Rage
	assign_Spell_to_SpellEntry( &sp , 29787 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 8 | 1 | 4 | 32 | 2 | 4194304 | 536870912 | 2097152 | 128 | 134217728 | 1073741824UL | 2048 | 64 | 1024 | 33554432;
		sp->EffectSpellGroupRelation_high[0] = 8 | 256 | 64;
	}
	assign_Spell_to_SpellEntry( &sp , 29790 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 8 | 1 | 4 | 32 | 2 | 4194304 | 536870912 | 2097152 | 128 | 134217728 | 1073741824UL | 2048 | 64 | 1024 | 33554432;
		sp->EffectSpellGroupRelation_high[0] = 8 | 256 | 64;
	}
	assign_Spell_to_SpellEntry( &sp , 29792 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 8 | 1 | 4 | 32 | 2 | 4194304 | 536870912 | 2097152 | 128 | 134217728 | 1073741824UL | 2048 | 64 | 1024 | 33554432;
		sp->EffectSpellGroupRelation_high[0] = 8 | 256 | 64;
	}

	//warrior - impale
	assign_Spell_to_SpellEntry( &sp , 16493 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 8 | 1 | 4 | 32 | 2 | 4194304 | 536870912 | 2097152 | 128 | 134217728 | 1073741824 | 2048 | 64 | 1024;
	assign_Spell_to_SpellEntry( &sp , 16494 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 8 | 1 | 4 | 32 | 2 | 4194304 | 536870912 | 2097152 | 128 | 134217728 | 1073741824 | 2048 | 64 | 1024;

	//warrior - Improved Whirlwind
	assign_Spell_to_SpellEntry( &sp , 29721 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation_high[0] = 4;
	assign_Spell_to_SpellEntry( &sp , 29776 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation_high[0] = 4;

	//warrior - Improved Commanding Shout
	assign_Spell_to_SpellEntry( &sp , 38408 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation_high[0] = 128;

	// priest - Reflective Shield
	assign_Spell_to_SpellEntry( &sp , 33201 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_ABSORB;
		sp->EffectApplyAuraName[0] = 42;
		sp->EffectTriggerSpell[0] = 33619; //!! WRONG spell, we will make direct dmg here
	}
	assign_Spell_to_SpellEntry( &sp , 33202 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_ABSORB;
		sp->EffectApplyAuraName[0] = 42;
		sp->EffectTriggerSpell[0] = 33619; //!! WRONG spell, we will make direct dmg here
	}
	assign_Spell_to_SpellEntry( &sp , 33203 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_ABSORB;
		sp->EffectApplyAuraName[0] = 42;
		sp->EffectTriggerSpell[0] = 33619; //!! WRONG spell, we will make direct dmg here
	}
	assign_Spell_to_SpellEntry( &sp , 33204 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_ABSORB;
		sp->EffectApplyAuraName[0] = 42;
		sp->EffectTriggerSpell[0] = 33619; //!! WRONG spell, we will make direct dmg here
	}
	assign_Spell_to_SpellEntry( &sp , 33205 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_ABSORB;
		sp->EffectApplyAuraName[0] = 42;
		sp->EffectTriggerSpell[0] = 33619; //!! WRONG spell, we will make direct dmg here
	}

	// mage - Frost Warding
	assign_Spell_to_SpellEntry( &sp , 11189 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 33554432;
	assign_Spell_to_SpellEntry( &sp , 28332 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 33554432;

	//Gnomish Poultryizer trinket - Poultryizer
	assign_Spell_to_SpellEntry( &sp , 30507 );
	if( sp != NULL )
	{
		sp->Effect[0] = 6;
		sp->EffectApplyAuraName[0] = 56;
	}

	// paladin - benediction
	assign_Spell_to_SpellEntry( &sp , 20101 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = All_Seal_Groups_Combined | 8388608;
	assign_Spell_to_SpellEntry( &sp , 20102 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = All_Seal_Groups_Combined | 8388608;
	assign_Spell_to_SpellEntry( &sp , 20103 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = All_Seal_Groups_Combined | 8388608;
	assign_Spell_to_SpellEntry( &sp , 20104 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = All_Seal_Groups_Combined | 8388608;
	assign_Spell_to_SpellEntry( &sp , 20105 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = All_Seal_Groups_Combined | 8388608;

	/*// paladin - Improved Hammer of Justice
	uint32 Hammer_of_Justice_group=0;
	tassign_Spell_to_SpellEntry( &sp , 853 ); //Hammer of Justice
	if(tsp)
		Hammer_of_Justice_group = tsp->SpellGroupType;
	assign_Spell_to_SpellEntry( &sp , 20487 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = Hammer_of_Justice_group;
	assign_Spell_to_SpellEntry( &sp , 20488 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = Hammer_of_Justice_group;
	assign_Spell_to_SpellEntry( &sp , 20489 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = Hammer_of_Justice_group;
	assign_Spell_to_SpellEntry( &sp , 24188 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = Hammer_of_Justice_group; 
	*/

	/*// paladin - Improved Judgement
	assign_Spell_to_SpellEntry( &sp , 25956 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = judgement_group;
	assign_Spell_to_SpellEntry( &sp , 25957 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = judgement_group;
	*/

	// paladin - Improved Sanctity Aura
	assign_Spell_to_SpellEntry( &sp , 31869 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 67108864;
		sp->EffectMiscValue[0] = SMT_SPELL_VALUE;
	}
	assign_Spell_to_SpellEntry( &sp , 31870 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 67108864;
		sp->EffectMiscValue[0] = SMT_SPELL_VALUE;
	}
    assign_Spell_to_SpellEntry( &sp , 20608 ); //Reincarnation
    if( sp != NULL )
    {
        for(uint32 i=0;i<8;i++)
        {
            if(sp->Reagent[i])
            {
                sp->Reagent[i] = 0;
                sp->ReagentCount[i] = 0;
            }
        }
    }

	// druid - Tree of Life
	assign_Spell_to_SpellEntry( &sp , 5420 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[2] = 32 | 64 | 16; //for the mana cost tweak
		sp->EffectSpellGroupRelation_high[2] = 2 | 4096 | 128 | 524288 ;
	}

	// druid - Shredding Attacks
	assign_Spell_to_SpellEntry( &sp , 16966 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 32768;
		sp->EffectSpellGroupRelation_high[1] = 256;
	}
	assign_Spell_to_SpellEntry( &sp , 16968 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 32768;
		sp->EffectSpellGroupRelation_high[1] = 256;
	}

	// druid - Naturalist
	assign_Spell_to_SpellEntry( &sp , 17069 );
	if( sp != NULL )
	{
		sp->EffectApplyAuraName[1]=SPELL_AURA_MOD_DAMAGE_PERCENT_DONE;
		sp->EffectMiscValue[1] = 1;  
	}
	assign_Spell_to_SpellEntry( &sp , 17070 );
	if( sp != NULL )
	{
		sp->EffectApplyAuraName[1]=SPELL_AURA_MOD_DAMAGE_PERCENT_DONE;
		sp->EffectMiscValue[1] = 1; 
	}
	assign_Spell_to_SpellEntry( &sp , 17071 );
	if( sp != NULL )
	{
		sp->EffectApplyAuraName[1]=SPELL_AURA_MOD_DAMAGE_PERCENT_DONE;
		sp->EffectMiscValue[1] = 1; 
	}
	assign_Spell_to_SpellEntry( &sp , 17072 );
	if( sp != NULL )
	{
		sp->EffectApplyAuraName[1]=SPELL_AURA_MOD_DAMAGE_PERCENT_DONE;
		sp->EffectMiscValue[1] = 1;
	}
	assign_Spell_to_SpellEntry( &sp , 17073 );
	if( sp != NULL )
	{
		sp->EffectApplyAuraName[1]=SPELL_AURA_MOD_DAMAGE_PERCENT_DONE;
		sp->EffectMiscValue[1] = 1; 
	}

	// druid - Gift of Nature
	assign_Spell_to_SpellEntry( &sp , 17104 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 268435456 | 32 | 64 | 16 | 128 | 268435456;
		sp->EffectMiscValue[0] = SMT_SPELL_VALUE;
		sp->EffectSpellGroupRelation[1] = 0;
		sp->EffectSpellGroupRelation_high[0] |= 16;
	}
	assign_Spell_to_SpellEntry( &sp , 24943 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 268435456 | 32 | 64 | 16 | 128 | 268435456;
		sp->EffectMiscValue[0] = SMT_SPELL_VALUE;
		sp->EffectSpellGroupRelation[1] = 0;
		sp->EffectSpellGroupRelation_high[0] |= 16;
	}
	assign_Spell_to_SpellEntry( &sp , 24944 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 268435456 | 32 | 64 | 16 | 128 | 268435456;
		sp->EffectMiscValue[0] = SMT_SPELL_VALUE;
		sp->EffectSpellGroupRelation[1] = 0;
		sp->EffectSpellGroupRelation_high[0] |= 16;
	}
	assign_Spell_to_SpellEntry( &sp , 24945 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 268435456 | 32 | 64 | 16 | 128 | 268435456;
		sp->EffectMiscValue[0] = SMT_SPELL_VALUE;
		sp->EffectSpellGroupRelation[1] = 0;
		sp->EffectSpellGroupRelation_high[0] |= 16;
	}
	assign_Spell_to_SpellEntry( &sp , 24946 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 268435456 | 32 | 64 | 16 | 128 | 268435456;
		sp->EffectMiscValue[0] = SMT_SPELL_VALUE;
		sp->EffectSpellGroupRelation[1] = 0;
		sp->EffectSpellGroupRelation_high[0] |= 16;
	}

	// druid - Empowered Touch
	assign_Spell_to_SpellEntry( &sp , 33879 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 32;
	assign_Spell_to_SpellEntry( &sp , 33880 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 32;

	// druid - Empowered Rejuvenation
	assign_Spell_to_SpellEntry( &sp , 33886 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 64 | 16 | 128;
	assign_Spell_to_SpellEntry( &sp , 33887 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 64 | 16 | 128;
	assign_Spell_to_SpellEntry( &sp , 33888 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 64 | 16 | 128;
	assign_Spell_to_SpellEntry( &sp , 33889 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 64 | 16 | 128;
	assign_Spell_to_SpellEntry( &sp , 33890 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 64 | 16 | 128;

	// druid - Wrath of cenarius
	assign_Spell_to_SpellEntry( &sp , 33603 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 4;
		sp->EffectSpellGroupRelation[1] = 1;
	}
	assign_Spell_to_SpellEntry( &sp , 33604 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 4;
		sp->EffectSpellGroupRelation[1] = 1;
	}
	assign_Spell_to_SpellEntry( &sp , 33605 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 4;
		sp->EffectSpellGroupRelation[1] = 1;
	}
	assign_Spell_to_SpellEntry( &sp , 33606 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 4;
		sp->EffectSpellGroupRelation[1] = 1;
	}
	assign_Spell_to_SpellEntry( &sp , 33607 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 4;
		sp->EffectSpellGroupRelation[1] = 1;
	}

	// druid - Nature's Grace
	assign_Spell_to_SpellEntry( &sp , 16880 );
	if( sp != NULL )
		sp->procFlags = PROC_ON_SPELL_CRIT_HIT;

	assign_Spell_to_SpellEntry( &sp , 16886 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 0xFFFFFFFF; //all spells, too bad not all spells have grouping flags :S
		sp->procCharges = 0;
	}

	// druid - Starlight Wrath
	assign_Spell_to_SpellEntry( &sp , 16814 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 1 | 4;
	assign_Spell_to_SpellEntry( &sp , 16815 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 1 | 4;
	assign_Spell_to_SpellEntry( &sp , 16816 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 1 | 4;
	assign_Spell_to_SpellEntry( &sp , 16817 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 1 | 4;
	assign_Spell_to_SpellEntry( &sp , 16818 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 1 | 4;

	// Druid: Omen of Clarity
	assign_Spell_to_SpellEntry( &sp , 16864 );
	if( sp != NULL )
	{
		sp->procChance=100; //procchance dynamic. 3ppm
		sp->procFlags = PROC_ON_MELEE_ATTACK | PROC_ON_CRIT_ATTACK;
	}

	//---------ITEMS-----------------

	//Solarian's Sapphire
	assign_Spell_to_SpellEntry( &sp , 37536);
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 65536;

	//Totem of the Pulsing Earth
	assign_Spell_to_SpellEntry( &sp , 37740);
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 1;

	//Totem of the Maelstrom
	assign_Spell_to_SpellEntry( &sp , 37738 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 64;

	//Totem of Living Water
	assign_Spell_to_SpellEntry( &sp , 43752 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 256;

	//Totem of Healing Rains
	assign_Spell_to_SpellEntry( &sp , 38322 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 256;

	//Totem of Lightning
	assign_Spell_to_SpellEntry( &sp , 33696 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 1;

	//Everbloom Idol
	assign_Spell_to_SpellEntry( &sp , 33693 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 32768;

	//Idol of the Avian Heart
	assign_Spell_to_SpellEntry( &sp , 38321 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 32;

	//Idol of the Crescent Goddess
	assign_Spell_to_SpellEntry( &sp , 37737 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 64;

	//Idol of the Avenger
	assign_Spell_to_SpellEntry( &sp , 37760 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation[0] = 1;

	//Energized 
	assign_Spell_to_SpellEntry( &sp , 43750 ); 
	if( sp != NULL )
		sp->procFlags = PROC_ON_CAST_SPELL;

	//Spell Haste Trinket
	assign_Spell_to_SpellEntry( &sp , 33297 ); 
	if( sp != NULL )
		sp->procFlags = PROC_ON_CAST_SPELL | PROC_TARGET_SELF;

	// Band of the Eternal Sage
	assign_Spell_to_SpellEntry( &sp , 35083 );
	if( sp != NULL )
		sp->procFlags = PROC_ON_CAST_SPELL;

	// Band of the Eternal Restorer 
	assign_Spell_to_SpellEntry( &sp , 35086 );
	if( sp != NULL )
		sp->procFlags = PROC_ON_CAST_SPELL;


	//Item Set: Thunderheart Harness
	assign_Spell_to_SpellEntry( &sp , 38447 );if( sp != NULL )
	{
		sp->EffectSpellGroupRelation_high[0] |=1024;
		sp->EffectSpellGroupRelation_high[1] |=64;
	}
	assign_Spell_to_SpellEntry( &sp , 38416 );if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] |=8388608;
		sp->EffectSpellGroupRelation_high[0] |=1048576;
		sp->EffectSpellGroupRelation[1] |=8388608;
	}

	//Item Set: Thunderheart Regalia
	assign_Spell_to_SpellEntry( &sp , 38414 );if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] |=2;
	}
	assign_Spell_to_SpellEntry( &sp , 38415 );if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] |=4;
	}

	//Item Set: Thunderheart Raiment
	assign_Spell_to_SpellEntry( &sp , 38417 );if( sp != NULL )
	{
		sp->EffectSpellGroupRelation_high[0] |=2;
	}
	assign_Spell_to_SpellEntry( &sp , 38420 );if( sp != NULL )
	{
		sp->EffectSpellGroupRelation_high[0] |=32;
	}

	//Item Set: Nordrassil Harness
	assign_Spell_to_SpellEntry( &sp , 37333 );if( sp != NULL )
	{
		sp->EffectSpellGroupRelation_high[1] |=256;
		sp->EffectSpellGroupRelation[0] |=32768;
	}

	//Item Set: Nordrassil Raiment
	assign_Spell_to_SpellEntry( &sp , 37313 );if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] |=64;
	}
	assign_Spell_to_SpellEntry( &sp , 37314 );if( sp != NULL )
	{
		sp->EffectSpellGroupRelation_high[0] |=16;
	}

	//Item Set: Malorne Raiment
	assign_Spell_to_SpellEntry( &sp , 37292 );if( sp != NULL )
	{
		sp->EffectSpellGroupRelation_high[0] |=524288;
	}

	//Item Set: Malorne Regalia
	assign_Spell_to_SpellEntry( &sp , 37297 );if( sp != NULL )
	{
		sp->EffectSpellGroupRelation_high[0] |=4096;
	}

	//Item Set: Malorne Harness
	assign_Spell_to_SpellEntry( &sp , 37306 );if( sp != NULL )
	{
		sp->procChance = 4;
		sp->procFlags = PROC_ON_MELEE_ATTACK;
	}
	assign_Spell_to_SpellEntry( &sp , 37311 );if( sp != NULL )
	{
		sp->procChance = 4;
		sp->procFlags = PROC_ON_MELEE_ATTACK;
	}

	//Item Set: Slayer's Armor
	assign_Spell_to_SpellEntry( &sp , 38388 );if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] |=262144;
	}
	assign_Spell_to_SpellEntry( &sp , 38389 );if( sp != NULL )
	{
		sp->EffectSpellGroupRelation_high[0] |= 2 | 4;
		sp->EffectSpellGroupRelation[0] |=8388612 |8388610 |41943040;
	}

	//Item Set: Deathmantle
	assign_Spell_to_SpellEntry( &sp , 37170 );if( sp != NULL )
	{
		sp->procChance = 4;
		sp->procFlags = PROC_ON_MELEE_ATTACK;
	}

	//Item Set: Netherblade
	assign_Spell_to_SpellEntry( &sp , 37167 );if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] |=262144;
	}
	assign_Spell_to_SpellEntry( &sp , 37168 );if( sp != NULL )
	{
		sp->procChance = 15;
		//sp->procFlags = PROC_ON_CAST_SPELL; Need new flag - PROC_ON_FINISH_MOVE;
	}

	//Item Set: Tempest Regalia
	assign_Spell_to_SpellEntry( &sp , 38396 );if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 67108864;
	}
	assign_Spell_to_SpellEntry( &sp , 38397 );if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 1572896 | 1 | 2048;
	}

	//Item Set: Tirisfal Regalia
	assign_Spell_to_SpellEntry( &sp , 37441 );if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 536870912;
		sp->EffectSpellGroupRelation[1] = 536870912;
	}
	assign_Spell_to_SpellEntry( &sp , 37443 );if( sp != NULL )
	{
		sp->procFlags = PROC_ON_SPELL_CRIT_HIT;
	}

	//Item Set: Aldor Regalia
	assign_Spell_to_SpellEntry( &sp , 37438 );if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 1572896 | 1;
	}
	assign_Spell_to_SpellEntry( &sp , 37439 );if( sp != NULL )
	{
		sp->EffectSpellGroupRelation_high[0] = 32;
		sp->EffectSpellGroupRelation_high[1] = 64;
		//sp->EffectSpellGroupRelation_high[2] = 64; NEED DBC 2.3.2 :P
	}

	//Item Set: Absolution Regalia
	assign_Spell_to_SpellEntry( &sp , 38413 );if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 32768;
	}
	assign_Spell_to_SpellEntry( &sp , 38412 );if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 8192;
	}

	//Item Set: Vestments of Absolution
	assign_Spell_to_SpellEntry( &sp , 38410 );if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 512;
	}
	assign_Spell_to_SpellEntry( &sp , 38411 );if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 4096 ;
	}

	//Item Set: Avatar Raiment
	assign_Spell_to_SpellEntry( &sp , 26171 );if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 64;
	}

	//Item Set: Avatar Regalia
	assign_Spell_to_SpellEntry( &sp , 37600 );if( sp != NULL )
	{
		sp->procFlags = PROC_ON_CAST_SPELL;
		sp->procChance = 6;
	}

	//Item Set: Incarnate Raiment
	assign_Spell_to_SpellEntry( &sp , 37568 );if( sp != NULL )
	{
		sp->procFlags = PROC_ON_CAST_SPELL;
	}
	assign_Spell_to_SpellEntry( &sp , 37565 );if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 4096;
	}

	//Item Set: Incarnate Regalia
	assign_Spell_to_SpellEntry( &sp , 37570 );if( sp != NULL )
	{
		sp->EffectSpellGroupRelation_high[1] = 256;
	}
	assign_Spell_to_SpellEntry( &sp , 37571 );if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 128;
		sp->EffectSpellGroupRelation[1] = 8388608;
	}

	//Item Set: Malefic Raiment
	assign_Spell_to_SpellEntry( &sp , 38393 );if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 1;
		sp->EffectSpellGroupRelation_high[0] = 64;
	}

	//Item Set: Voidheart Raiment
	assign_Spell_to_SpellEntry( &sp , 37377 );if( sp != NULL )
	{
		sp->Effect[0] = 6;
		sp->EffectApplyAuraName[0] = SPELL_AURA_PROC_TRIGGER_SPELL;
		sp->procChance = 5;
		sp->procFlags = PROC_ON_CAST_SPELL;
		sp->proc_interval = 20;
		sp->EffectTriggerSpell[0] = 37379;
	}
	assign_Spell_to_SpellEntry( &sp , 39437 );if( sp != NULL )
	{
		sp->Effect[0] = 6;
		sp->EffectApplyAuraName[0] = SPELL_AURA_PROC_TRIGGER_SPELL;
		sp->procChance = 5;
		sp->procFlags = PROC_ON_CAST_SPELL;
		sp->proc_interval = 20;
		sp->EffectTriggerSpell[0] = 37378;
	}
	assign_Spell_to_SpellEntry( &sp , 37380 );if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 2|4;
	}

	//Item Set: Gronnstalker's Armor
	assign_Spell_to_SpellEntry( &sp , 38392 );if( sp != NULL )
	{
		sp->EffectSpellGroupRelation_high[0] = 1;
	}

	//Item Set: Rift Stalker Armor
	assign_Spell_to_SpellEntry( &sp , 37505 );if( sp != NULL )
	{
		sp->EffectSpellGroupRelation_high[0] = 1;
	}

	//Item Set: Demon Stalker Armor
	assign_Spell_to_SpellEntry( &sp , 37484 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 256;
	}
	assign_Spell_to_SpellEntry( &sp , 37485 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 4096;
	}

	//Item Set: Skyshatter Harness
	assign_Spell_to_SpellEntry( &sp , 38429 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 1048576 | 268435456 | 2147483648UL;
	}

	//Item Set: Skyshatter Raiment
	assign_Spell_to_SpellEntry( &sp , 38434 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 256;
	}
	assign_Spell_to_SpellEntry( &sp , 38435 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 256;
	}

	//Item Set: Skyshatter Regalia
	assign_Spell_to_SpellEntry( &sp , 38436 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 1;
	}

	//Item Set: Cataclysm Raiment
	assign_Spell_to_SpellEntry( &sp , 37225 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 128;
	}
	assign_Spell_to_SpellEntry( &sp , 37227 );
	if( sp != NULL )
	{
		sp->proc_interval = 60000;
		sp->procChance = 100;
		sp->procFlags = PROC_ON_SPELL_CRIT_HIT;
	}
	assign_Spell_to_SpellEntry( &sp , 39950 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 64;
	}

	//Item Set: Cataclysm Regalia
	assign_Spell_to_SpellEntry( &sp , 37228 );
	if( sp != NULL )
	{
		sp->procChance = 7;
		sp->procFlags = PROC_ON_CAST_SPELL;
	}
	assign_Spell_to_SpellEntry( &sp , 37234 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 128;
	}
	assign_Spell_to_SpellEntry( &sp , 37237 );
	if( sp != NULL )
	{
		sp->procChance = 25;
		sp->procFlags = PROC_ON_SPELL_CRIT_HIT;
	}

	//Item Set: Cataclysm Harness
	assign_Spell_to_SpellEntry( &sp , 37239 );
	if( sp != NULL )
	{
		sp->procChance = 2;
		sp->procFlags = PROC_ON_MELEE_ATTACK;
	}
	assign_Spell_to_SpellEntry( &sp , 37240 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 128;
	}
	assign_Spell_to_SpellEntry( &sp , 37241 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation_high[0] = 512;
	}

	//Item Set: Cyclone Raiment
	assign_Spell_to_SpellEntry( &sp , 37210 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 524288;
	}
	assign_Spell_to_SpellEntry( &sp , 37211 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation_high[0] = 524288;
	}

	//Item Set: Cyclone Harness
	assign_Spell_to_SpellEntry( &sp , 37224 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation_high[0] = 16;
	}
	assign_Spell_to_SpellEntry( &sp , 37223 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 536870912UL;
	}

	//Item Set: Cyclone Regalia
	assign_Spell_to_SpellEntry( &sp , 37212 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation_high[0] = 256;
	}
	assign_Spell_to_SpellEntry( &sp , 37213 );
	if( sp != NULL )
	{
		sp->procChance = 11;
		sp->procFlags = PROC_ON_SPELL_CRIT_HIT;
	}

	//Item Set: Lightbringer Armor
	assign_Spell_to_SpellEntry( &sp , 38421 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation_high[0] = 4096;
	}
	assign_Spell_to_SpellEntry( &sp , 38422 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 32;
	}

	//Item Set: Lightbringer Battlegear
	assign_Spell_to_SpellEntry( &sp , 38427 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_MELEE_ATTACK;
		sp->procChance = 20;
	}
	assign_Spell_to_SpellEntry( &sp , 38424 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 128;
	}

	//Item Set: Lightbringer Raiment
	assign_Spell_to_SpellEntry( &sp , 38426 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 2147483648UL;
	}
	assign_Spell_to_SpellEntry( &sp , 38425 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 1073741824UL;
	}

	//Item Set: Crystalforge Armor
	assign_Spell_to_SpellEntry( &sp , 37190 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 8;
	}
	assign_Spell_to_SpellEntry( &sp , 37191 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_CAST_SPELL;
	}

	//Item Set: Crystalforge Battlegear
	assign_Spell_to_SpellEntry( &sp , 37190 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 1024 | 524288 | 1048576 | 536870912UL;
		sp->EffectSpellGroupRelation_high[0] = 1|520;
	}
	assign_Spell_to_SpellEntry( &sp , 37195 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_CAST_SPELL;
		sp->procChance = 6;
	}

	//Item Set: Crystalforge Raiment
	assign_Spell_to_SpellEntry( &sp , 37189 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_SPELL_CRIT_HIT;
		sp->proc_interval = 60000;
	}
	assign_Spell_to_SpellEntry( &sp , 43837 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 2147483648UL;
	}
	assign_Spell_to_SpellEntry( &sp , 37188 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_CAST_SPELL;
	}

	//Item Set: Justicar Raiment
	assign_Spell_to_SpellEntry( &sp , 37182 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation_high[0] = 1;
	}
	assign_Spell_to_SpellEntry( &sp , 37183 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation_high[0] = 256;
	}

	//Item Set: Justicar Armor
	assign_Spell_to_SpellEntry( &sp , 37184 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation_high[0] = 1024;
		sp->EffectSpellGroupRelation[0] = 134217728;
		sp->EffectSpellGroupRelation_high[1] = 8;
	}
	assign_Spell_to_SpellEntry( &sp , 37185 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation_high[0] = 64;
	}

	//Item Set: Justicar Battlegear
	assign_Spell_to_SpellEntry( &sp , 37186 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 536870912UL;
	}
	assign_Spell_to_SpellEntry( &sp , 37187 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation_high[0] = 520;
	}

	//Item Set: Onslaught Battlegear
	assign_Spell_to_SpellEntry( &sp , 38399 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 33554432UL;
		sp->EffectSpellGroupRelation_high[0] = 1024;
	}
	assign_Spell_to_SpellEntry( &sp , 38398 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 536870912UL;
	}

	//Item Set: Onslaught Armor
	assign_Spell_to_SpellEntry( &sp , 38408 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation_high[0] = 128;
	}
	assign_Spell_to_SpellEntry( &sp , 38407 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation_high[0] = 513;
	}

	//Item Set: Destroyer Armor
	assign_Spell_to_SpellEntry( &sp , 37525 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_MELEE_ATTACK_VICTIM;
		sp->procChance = 7;
	}

	//Item Set: Destroyer Battlegear
	assign_Spell_to_SpellEntry( &sp , 37528 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_CAST_SPELL;
		sp->procChance = 100;
	}
	assign_Spell_to_SpellEntry( &sp , 37535 );
	if( sp != NULL )
	{
		sp->EffectSpellGroupRelation[0] = 33554432;
		sp->EffectSpellGroupRelation_high[0] = 1024;
	}

	//Item Set: Warbringer Armor
	assign_Spell_to_SpellEntry( &sp , 37516 );
	if( sp != NULL )
	{
		sp->procFlags = PROC_ON_CAST_SPELL;
		sp->procChance = 100;
	}

	//Item Set: Warbringer Battlegear
	assign_Spell_to_SpellEntry( &sp , 37518 );
	if( sp != NULL )
		sp->EffectSpellGroupRelation_high[0] = 4;

	//all Drums 
	assign_Spell_to_SpellEntry( &sp , 35474 );
	if( sp != NULL )
		sp->RequiredShapeShift = 0;
	assign_Spell_to_SpellEntry( &sp , 35475 );
	if( sp != NULL )
		sp->RequiredShapeShift = 0;
	assign_Spell_to_SpellEntry( &sp , 35476 );
	if( sp != NULL )
		sp->RequiredShapeShift = 0;
	assign_Spell_to_SpellEntry( &sp , 35477 );
	if( sp != NULL )
		sp->RequiredShapeShift = 0;
	assign_Spell_to_SpellEntry( &sp , 35478 );
	if( sp != NULL )
		sp->RequiredShapeShift = 0;

	//Paladin - Improved Lay on Hands
	assign_Spell_to_SpellEntry( &sp , 20234 );
	if( sp != NULL )
		sp->procFlags = PROC_ON_CAST_SPELL;
	assign_Spell_to_SpellEntry( &sp , 20235 );
	if( sp != NULL )
		sp->procFlags = PROC_ON_CAST_SPELL;


	//fix for Predatory Strikes
	uint32 mm = (1<<(FORM_BEAR-1))|(1<<(FORM_DIREBEAR-1))|(1<<(FORM_MOONKIN-1))|(1<<(FORM_CAT-1));

	assign_Spell_to_SpellEntry( &sp , 16972 );
	if( sp != NULL )
		sp->RequiredShapeShift = mm;
	assign_Spell_to_SpellEntry( &sp , 16974 );
	if( sp != NULL )
		sp->RequiredShapeShift = mm;
	assign_Spell_to_SpellEntry( &sp , 16975 );
	if( sp != NULL )
		sp->RequiredShapeShift = mm;

	assign_Spell_to_SpellEntry( &sp , 20134 );
	if( sp != NULL )
		sp->procChance = 50;

	/* aspect of the pack - change to AA */
	assign_Spell_to_SpellEntry( &sp , 13159 );
	if( sp != NULL )
		sp->Effect[0] = SPELL_EFFECT_APPLY_AREA_AURA;
	assign_Spell_to_SpellEntry( &sp , 13159 );
	if( sp != NULL )
		sp->Effect[1] = SPELL_EFFECT_APPLY_AREA_AURA;

	/* aspect of the cheetah - add proc flags */
	assign_Spell_to_SpellEntry( &sp , 5118 );
	if( sp != NULL )
		sp->procFlags = 139944;
	
	/* thrown - add a 1.6 second cooldown */
	const static uint32 thrown_spells[] = {SPELL_RANGED_GENERAL,SPELL_RANGED_THROW,SPELL_RANGED_WAND, 26679, 27084, 29436, 37074, 41182, 41346, 0};
	for(i = 0; thrown_spells[i] != 0; ++i)
	{
		assign_Spell_to_SpellEntry( &sp , thrown_spells[i]);
		if(sp->RecoveryTime==0 && sp->StartRecoveryTime == 0)
			sp->RecoveryTime = 1600;
	}

	/* wands - s5019 - change Spell_Dmg_Type from magic to ranged (3) */
	assign_Spell_to_SpellEntry( &sp , SPELL_RANGED_WAND );
	if( sp != NULL )
		sp->Spell_Dmg_Type = SPELL_DMG_TYPE_RANGED;

	dbc.DumpBufferToFile(inf);
}

void dump_as_sql(TCHAR *inf)
{
	if(SPELL_COLUMN_COUNT!=dbc.getFieldCount())
	{
		printf("error,column counts do not match update code for latest dbc format\n");
		exit(1);
	}

	printf("will start dumping data into sql file (we will drop not required fields!)\n");

	FILE *fsql=fopen("dbc_spell.sql","w");

	//i really don;t know what are these for xD
	fprintf(fsql,"%s","/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;\n");
	fprintf(fsql,"%s","/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;\n");
	fprintf(fsql,"%s","/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;\n");
	fprintf(fsql,"%s","/*!40101 SET NAMES utf8 */;\n");
	fprintf(fsql,"%s","/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;\n");
	fprintf(fsql,"%s","/*!40103 SET TIME_ZONE='+00:00' */;\n");
	fprintf(fsql,"%s","/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;\n");
	fprintf(fsql,"%s","/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;\n");
	fprintf(fsql,"%s","/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;\n");
	fprintf(fsql,"%s","/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;\n");

	fprintf(fsql,"\n\n");

	//drop table if already exist
	fprintf(fsql,"%s","DROP TABLE IF EXISTS `dbc_spell`;\n");

	//create the table
	fprintf(fsql,"%s","CREATE TABLE dbc_spell (\n");

	for(int i=0;i<SPELL_COLUMN_COUNT;i++)
		if(sql_translation_table[i][2][0]=='0')
		{
			if(sql_translation_table[i][0][0]=='u')
				fprintf(fsql,"\t `%s` INT (11) UNSIGNED DEFAULT '0' NOT NULL,\n",sql_translation_table[i][1]);
			else if(sql_translation_table[i][0][0]=='i')
				fprintf(fsql,"\t `%s` INT (11) DEFAULT '0' NOT NULL,\n",sql_translation_table[i][1]);
			else if(sql_translation_table[i][0][0]=='f')
				fprintf(fsql,"\t `%s` FLOAT DEFAULT '0' NOT NULL,\n",sql_translation_table[i][1]);
			else if(sql_translation_table[i][0][0]=='s')
				fprintf(fsql,"\t `%s` VARCHAR(2000),\n",sql_translation_table[i][1]);
		}

	fprintf(fsql,"%s","PRIMARY KEY(id), UNIQUE(id), INDEX(id));\n");

	fprintf(fsql,"\n\n");

	fprintf(fsql,"LOCK TABLES `dbc_spell` WRITE;\n");
	fprintf(fsql,"/*!40000 ALTER TABLE `dbc_spell` DISABLE KEYS */;\n");

	fprintf(fsql,"\n\n");

	//start dumping the data from the DBC

	char tstr[2000];
	for(unsigned int j=0;j<dbc.getRecordCount();j++)
	{
		//we start a new insert block
		if((j % SQL_INSERTS_PER_QUERY) == 0)
		{
			fprintf(fsql,"%s","REPLACE INTO dbc_spell (");
			for(int i=0;i<SPELL_COLUMN_COUNT-1;i++)
				if(sql_translation_table[i][2][0]=='0')
					fprintf(fsql,"`%s`,",sql_translation_table[i][1]);
			fprintf(fsql,"`%s`) values \n",sql_translation_table[SPELL_COLUMN_COUNT-1][1]);
			fprintf(fsql," (");
		}
		else
			fprintf(fsql,",(");
		for(int i=0;i<SPELL_COLUMN_COUNT;i++)
			if(sql_translation_table[i][2][0]=='0')
			{
				if(i!=0)
					fprintf(fsql,",");
				if(sql_translation_table[i][0][0]=='u')
					fprintf(fsql,"%u",dbc.getRecord(j).getUInt(i));
				else if(sql_translation_table[i][0][0]=='i')
					fprintf(fsql,"%d",dbc.getRecord(j).getInt(i));
				else if(sql_translation_table[i][0][0]=='f')
					fprintf(fsql,"%f",dbc.getRecord(j).getFloat(i));
				else if(sql_translation_table[i][0][0]=='s')
				{
					const char *dstr=dbc.getRecord(j).getString(i);
					int otherindex=0;
					for(unsigned int k=0;k<=strlen(dstr);k++)
					{
						if(dstr[k]=='\'' || dstr[k]=='"')
						{
							tstr[otherindex++] = '\\';
							tstr[otherindex++] = dstr[k];
						}
						else if (dstr[k] == '\n' )
						{
							tstr[otherindex++] = '\\';
							tstr[otherindex++] = 'n';
						}
						else if (dstr[k] == '\r' )
						{
							tstr[otherindex++] = '\\';
							tstr[otherindex++] = 'r';
						}
						else
							tstr[otherindex++] = dstr[k];
					}
					fprintf(fsql,"\"%s\"",tstr);
				}
			}
		//we need to end an insert block
		if(((j+1) % SQL_INSERTS_PER_QUERY)==0)
			fprintf(fsql,");\n");
		else fprintf(fsql,")\n");
	}
	fprintf(fsql,";");

	fprintf(fsql,"\n\n");

	fprintf(fsql,"/*!40000 ALTER TABLE `dbc_spell` ENABLE KEYS */;\n");
	fprintf(fsql,"UNLOCK TABLES;\n");

	fclose(fsql);
}


//this is not tested yet !!!
void import_from_sql()
{
	TCHAR mHostname[50];strcpy(mHostname,"");
	TCHAR mUsername[50];strcpy(mUsername,"");
	TCHAR mPassword[50];strcpy(mPassword,"");
	TCHAR mDatabaseName[50];strcpy(mDatabaseName,"");
	TCHAR mTableName[50];strcpy(mTableName,"");
	uint32 mPort=3306;
	TCHAR strbuffer[500];
	//read sql connection data
	FILE *inif=fopen("mysql_con.ini","r");
	if(!inif)
	{
		printf("error,Could not open mysql ini file\n");
		exit(1);
	}
	while (!feof(inif))
	{
		fgets (strbuffer , 100 , inif);
		if (strnicmp(strbuffer,"Hostname=",strlen("Hostname="))==0) strcpy(mHostname,strbuffer+strlen("Hostname="));
		else if (strnicmp(strbuffer,"Username=",strlen("Username="))==0) strcpy(mUsername,strbuffer+strlen("Username="));
		else if (strnicmp(strbuffer,"Password=",strlen("Password="))==0) strcpy(mPassword,strbuffer+strlen("Password="));
		else if (strnicmp(strbuffer,"DatabaseName=",strlen("DatabaseName="))==0) strcpy(mDatabaseName,strbuffer+strlen("DatabaseName="));
		else if (strnicmp(strbuffer,"TableName=",strlen("TableName="))==0) strcpy(mTableName,strbuffer+strlen("TableName="));
		else if (strnicmp(strbuffer,"Port=",strlen("Port="))==0) mPort=atoi(strbuffer+strlen("Port="));
	}
	fclose(inif);

	//establish mysql connection
	MYSQL* Descriptor = mysql_init(NULL);
	if(mysql_options(Descriptor, MYSQL_SET_CHARSET_NAME, "utf8"))
		printf("sql: Could not set utf8 character set [!!]");
	MYSQL *con = mysql_real_connect(Descriptor, mHostname,
		mUsername, mPassword, "", mPort, NULL, 0);
	if(con == NULL)
	{
		printf("sql: Connection failed. Reason was `%s`", mysql_error(Descriptor));
		return;
	}
	
	if(mysql_select_db(con, mDatabaseName))
	{
		printf("sql: Select of Database %s failed due to `%s`", mDatabaseName,mysql_error(con));
		return;
	}

	//start to get the lines we have in our dbc from the mysql table
	for(unsigned int j=0;j<dbc.getRecordCount();j++)
	{
		uint32 spell_id=dbc.getRecord(j).getUInt(SPELL_ENTRY_Id);

		//get the eqivalent of this dbc spell from mysql
		sprintf(strbuffer,"SELECT * FROM `%s` where id='%d'",mTableName,spell_id);
		int result = mysql_query(con, strbuffer);
		if(result > 0)
		{
			printf("Sql query failed due to [%s]", mysql_error(con));
			exit(1);
		}
        MYSQL_RES * Result = mysql_store_result(con);
        uint32 RowCount = (uint32)mysql_affected_rows(con);
        uint32 FieldCount = mysql_field_count(con);
        if(!RowCount || !FieldCount)
		{
			//empty result means we did not have this spell in sql
            mysql_free_result(Result); 
			continue;
		}
	    MYSQL_ROW row = mysql_fetch_row(Result);
		//now to set the values
		int *dbc_row; //dbc row does contain diferent data types but we don;t interpret values just simply set data
		int col_shifter=0;
		dbc_row=(int *)dbc.getRecord(j).getRowStart();
		for(int i=0;i<SPELL_COLUMN_COUNT;i++)
			if(sql_translation_table[i][2][0]=='0' && sql_translation_table[i][0][0]!='s')
				dbc_row[i] = (int)atoi(row[col_shifter++]); 
		mysql_free_result(Result); 
	}
}
