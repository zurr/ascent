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

#ifndef _OPCODES_H
#define _OPCODES_H

// NOTE: All SERVER opcodes are now uint16's, as of patch 4 (3807/0.9.0)
//	   Client opcodes are still uint32 as far as we know.
//1.8.0 client opcodes

enum Opcodes {
	MSG_NULL_ACTION					 = 0,  //(0x000)
	CMSG_BOOTME						 = 1,  //(0x001)
	CMSG_DBLOOKUP					   = 2,  //(0x002)
	SMSG_DBLOOKUP					   = 3,  //(0x003)
	CMSG_QUERY_OBJECT_POSITION		  = 4,  //(0x004)
	SMSG_QUERY_OBJECT_POSITION		  = 5,  //(0x005)
	CMSG_QUERY_OBJECT_ROTATION		  = 6,  //(0x006)
	SMSG_QUERY_OBJECT_ROTATION		  = 7,  //(0x007)
	CMSG_WORLD_TELEPORT				 = 8,  //(0x008)
	CMSG_TELEPORT_TO_UNIT			   = 9,  //(0x009)
	CMSG_ZONE_MAP					   = 10,  //(0x00A)
	SMSG_ZONE_MAP					   = 11,  //(0x00B)
	CMSG_DEBUG_CHANGECELLZONE		   = 12,  //(0x00C)
	CMSG_EMBLAZON_TABARD_OBSOLETE	   = 13,  //(0x00D)
	CMSG_UNEMBLAZON_TABARD_OBSOLETE	 = 14,  //(0x00E)
	CMSG_RECHARGE					   = 15,  //(0x00F)
	CMSG_LEARN_SPELL					= 16,  //(0x010)
	CMSG_CREATEMONSTER				  = 17,  //(0x011)
	CMSG_DESTROYMONSTER				 = 18,  //(0x012)
	CMSG_CREATEITEM					 = 19,  //(0x013)
	CMSG_CREATEGAMEOBJECT			   = 20,  //(0x014)
	CMSG_MAKEMONSTERATTACKME_OBSOLETE   = 21,  //(0x015)
	CMSG_MAKEMONSTERATTACKGUID		  = 22,  //(0x016)
	CMSG_ENABLEDEBUGCOMBATLOGGING_OBSOLETE = 23,  //(0x017)
	CMSG_FORCEACTION					= 24,  //(0x018)
	CMSG_FORCEACTIONONOTHER			 = 25,  //(0x019)
	CMSG_FORCEACTIONSHOW				= 26,  //(0x01A)
	SMSG_FORCEACTIONSHOW				= 27,  //(0x01B)
	SMSG_ATTACKERSTATEUPDATEDEBUGINFO_OBSOLETE = 28,  //(0x01C)
	SMSG_DEBUGINFOSPELL_OBSOLETE		= 29,  //(0x01D)
	SMSG_DEBUGINFOSPELLMISS_OBSOLETE	= 30,  //(0x01E)
	SMSG_DEBUG_PLAYER_RANGE_OBSOLETE	= 31,  //(0x01F)
	CMSG_UNDRESSPLAYER				  = 32,  //(0x020)
	CMSG_BEASTMASTER					= 33,  //(0x021)
	CMSG_GODMODE						= 34,  //(0x022)
	SMSG_GODMODE						= 35,  //(0x023)
	CMSG_CHEAT_SETMONEY				 = 36,  //(0x024)
	CMSG_LEVEL_CHEAT					= 37,  //(0x025)
	CMSG_PET_LEVEL_CHEAT				= 38,  //(0x026)
	CMSG_LEVELUP_CHEAT_OBSOLETE		 = 39,  //(0x027)
	CMSG_COOLDOWN_CHEAT				 = 40,  //(0x028)
	CMSG_USE_SKILL_CHEAT				= 41,  //(0x029)
	CMSG_FLAG_QUEST					 = 42,  //(0x02A)
	CMSG_FLAG_QUEST_FINISH			  = 43,  //(0x02B)
	CMSG_CLEAR_QUEST					= 44,  //(0x02C)
	CMSG_SEND_EVENT					 = 45,  //(0x02D)
	CMSG_DEBUG_AISTATE				  = 46,  //(0x02E)
	SMSG_DEBUG_AISTATE				  = 47,  //(0x02F)
	CMSG_DISABLE_PVP_CHEAT			  = 48,  //(0x030)
	CMSG_ADVANCE_SPAWN_TIME			 = 49,  //(0x031)
	CMSG_PVP_PORT_OBSOLETE			  = 50,  //(0x032)
	CMSG_AUTH_SRP6_BEGIN				= 51,  //(0x033)
	CMSG_AUTH_SRP6_PROOF				= 52,  //(0x034)
	CMSG_AUTH_SRP6_RECODE			   = 53,  //(0x035)
	CMSG_CHAR_CREATE					= 54,  //(0x036)
	CMSG_CHAR_ENUM					  = 55,  //(0x037)
	CMSG_CHAR_DELETE					= 56,  //(0x038)
	SMSG_AUTH_SRP6_RESPONSE			 = 57,  //(0x039)
	SMSG_CHAR_CREATE					= 58,  //(0x03A)
	SMSG_CHAR_ENUM					  = 59,  //(0x03B)
	SMSG_CHAR_DELETE					= 60,  //(0x03C)
	CMSG_PLAYER_LOGIN				   = 61,  //(0x03D)
	SMSG_NEW_WORLD					  = 62,  //(0x03E)
	SMSG_TRANSFER_PENDING			   = 63,  //(0x03F)
	SMSG_TRANSFER_ABORTED			   = 64,  //(0x040)
	SMSG_CHARACTER_LOGIN_FAILED		 = 65,  //(0x041)
	SMSG_LOGIN_SETTIMESPEED			 = 66,  //(0x042)
	SMSG_GAMETIME_UPDATE				= 67,  //(0x043)
	CMSG_GAMETIME_SET				   = 68,  //(0x044)
	SMSG_GAMETIME_SET				   = 69,  //(0x045)
	CMSG_GAMESPEED_SET				  = 70,  //(0x046)
	SMSG_GAMESPEED_SET				  = 71,  //(0x047)
	CMSG_SERVERTIME					 = 72,  //(0x048)
	SMSG_SERVERTIME					 = 73,  //(0x049)
	CMSG_PLAYER_LOGOUT				  = 74,  //(0x04A)
	CMSG_LOGOUT_REQUEST				 = 75,  //(0x04B)
	SMSG_LOGOUT_RESPONSE				= 76,  //(0x04C)
	SMSG_LOGOUT_COMPLETE				= 77,  //(0x04D)
	CMSG_LOGOUT_CANCEL				  = 78,  //(0x04E)
	SMSG_LOGOUT_CANCEL_ACK			  = 79,  //(0x04F)
	CMSG_NAME_QUERY					 = 80,  //(0x050)
	SMSG_NAME_QUERY_RESPONSE			= 81,  //(0x051)
	CMSG_PET_NAME_QUERY				 = 82,  //(0x052)
	SMSG_PET_NAME_QUERY_RESPONSE		= 83,  //(0x053)
	CMSG_GUILD_QUERY					= 84,  //(0x054)
	SMSG_GUILD_QUERY_RESPONSE		   = 85,  //(0x055)
	CMSG_ITEM_QUERY_SINGLE			  = 86,  //(0x056)
	CMSG_ITEM_QUERY_MULTIPLE			= 87,  //(0x057)
	SMSG_ITEM_QUERY_SINGLE_RESPONSE	 = 88,  //(0x058)
	SMSG_ITEM_QUERY_MULTIPLE_RESPONSE   = 89,  //(0x059)
	CMSG_PAGE_TEXT_QUERY				= 90,  //(0x05A)
	SMSG_PAGE_TEXT_QUERY_RESPONSE	   = 91,  //(0x05B)
	CMSG_QUEST_QUERY					= 92,  //(0x05C)
	SMSG_QUEST_QUERY_RESPONSE		   = 93,  //(0x05D)
	CMSG_GAMEOBJECT_QUERY			   = 94,  //(0x05E)
	SMSG_GAMEOBJECT_QUERY_RESPONSE	  = 95,  //(0x05F)
	CMSG_CREATURE_QUERY				 = 96,  //(0x060)
	SMSG_CREATURE_QUERY_RESPONSE		= 97,  //(0x061)
	CMSG_WHO							= 98,  //(0x062)
	SMSG_WHO							= 99,  //(0x063)
	CMSG_WHOIS						  = 100,  //(0x064)
	SMSG_WHOIS						  = 101,  //(0x065)
	CMSG_FRIEND_LIST					= 102,  //(0x066)
	SMSG_FRIEND_LIST					= 103,  //(0x067)
	SMSG_FRIEND_STATUS				  = 104,  //(0x068)
	CMSG_ADD_FRIEND					 = 105,  //(0x069)
	CMSG_DEL_FRIEND					 = 106,  //(0x06A)
	SMSG_IGNORE_LIST					= 107,  //(0x06B)
	CMSG_ADD_IGNORE					 = 108,  //(0x06C)
	CMSG_DEL_IGNORE					 = 109,  //(0x06D)
	CMSG_GROUP_INVITE				   = 110,  //(0x06E)
	SMSG_GROUP_INVITE				   = 111,  //(0x06F)
	CMSG_GROUP_CANCEL				   = 112,  //(0x070)
	SMSG_GROUP_CANCEL				   = 113,  //(0x071)
	CMSG_GROUP_ACCEPT				   = 114,  //(0x072)
	CMSG_GROUP_DECLINE				  = 115,  //(0x073)
	SMSG_GROUP_DECLINE				  = 116,  //(0x074)
	CMSG_GROUP_UNINVITE				 = 117,  //(0x075)
	CMSG_GROUP_UNINVITE_GUID			= 118,  //(0x076)
	SMSG_GROUP_UNINVITE				 = 119,  //(0x077)
	CMSG_GROUP_SET_LEADER			   = 120,  //(0x078)
	SMSG_GROUP_SET_LEADER			   = 121,  //(0x079)
	CMSG_LOOT_METHOD					= 122,  //(0x07A)
	CMSG_GROUP_DISBAND				  = 123,  //(0x07B)
	SMSG_GROUP_DESTROYED				= 124,  //(0x07C)
	SMSG_GROUP_LIST					 = 125,  //(0x07D)
	SMSG_PARTY_MEMBER_STATS			 = 126,  //(0x07E)
	SMSG_PARTY_COMMAND_RESULT		   = 127,  //(0x07F)
	UMSG_UPDATE_GROUP_MEMBERS		   = 128,  //(0x080)
	CMSG_GUILD_CREATE				   = 129,  //(0x081)
	CMSG_GUILD_INVITE				   = 130,  //(0x082)
	SMSG_GUILD_INVITE				   = 131,  //(0x083)
	CMSG_GUILD_ACCEPT				   = 132,  //(0x084)
	CMSG_GUILD_DECLINE				  = 133,  //(0x085)
	SMSG_GUILD_DECLINE				  = 134,  //(0x086)
	CMSG_GUILD_INFO					 = 135,  //(0x087)
	SMSG_GUILD_INFO					 = 136,  //(0x088)
	CMSG_GUILD_ROSTER				   = 137,  //(0x089)
	SMSG_GUILD_ROSTER				   = 138,  //(0x08A)
	CMSG_GUILD_PROMOTE				  = 139,  //(0x08B)
	CMSG_GUILD_DEMOTE				   = 140,  //(0x08C)
	CMSG_GUILD_LEAVE					= 141,  //(0x08D)
	CMSG_GUILD_REMOVE				   = 142,  //(0x08E)
	CMSG_GUILD_DISBAND				  = 143,  //(0x08F)
	CMSG_GUILD_LEADER				   = 144,  //(0x090)
	CMSG_GUILD_MOTD					 = 145,  //(0x091)
	SMSG_GUILD_EVENT					= 146,  //(0x092)
	SMSG_GUILD_COMMAND_RESULT		   = 147,  //(0x093)
	UMSG_UPDATE_GUILD				   = 148,  //(0x094)
	CMSG_MESSAGECHAT					= 149,  //(0x095)
	SMSG_MESSAGECHAT					= 150,  //(0x096)
	CMSG_JOIN_CHANNEL				   = 151,  //(0x097)
	CMSG_LEAVE_CHANNEL				  = 152,  //(0x098)
	SMSG_CHANNEL_NOTIFY				 = 153,  //(0x099)
	CMSG_CHANNEL_LIST				   = 154,  //(0x09A)
	SMSG_CHANNEL_LIST				   = 155,  //(0x09B)
	CMSG_CHANNEL_PASSWORD			   = 156,  //(0x09C)
	CMSG_CHANNEL_SET_OWNER			  = 157,  //(0x09D)
	CMSG_CHANNEL_OWNER				  = 158,  //(0x09E)
	CMSG_CHANNEL_MODERATOR			  = 159,  //(0x09F)
	CMSG_CHANNEL_UNMODERATOR			= 160,  //(0x0A0)
	CMSG_CHANNEL_MUTE				   = 161,  //(0x0A1)
	CMSG_CHANNEL_UNMUTE				 = 162,  //(0x0A2)
	CMSG_CHANNEL_INVITE				 = 163,  //(0x0A3)
	CMSG_CHANNEL_KICK				   = 164,  //(0x0A4)
	CMSG_CHANNEL_BAN					= 165,  //(0x0A5)
	CMSG_CHANNEL_UNBAN				  = 166,  //(0x0A6)
	CMSG_CHANNEL_ANNOUNCEMENTS		  = 167,  //(0x0A7)
	CMSG_CHANNEL_MODERATE			   = 168,  //(0x0A8)
	SMSG_UPDATE_OBJECT				  = 169,  //(0x0A9)
	SMSG_DESTROY_OBJECT				 = 170,  //(0x0AA)
	CMSG_USE_ITEM					   = 171,  //(0x0AB)
	CMSG_OPEN_ITEM					  = 172,  //(0x0AC)
	CMSG_READ_ITEM					  = 173,  //(0x0AD)
	SMSG_READ_ITEM_OK				   = 174,  //(0x0AE)
	SMSG_READ_ITEM_FAILED			   = 175,  //(0x0AF)
	SMSG_ITEM_COOLDOWN				  = 176,  //(0x0B0)
	CMSG_GAMEOBJ_USE					= 177,  //(0x0B1)
	CMSG_GAMEOBJ_CHAIR_USE_OBSOLETE	 = 178,  //(0x0B2)
	SMSG_GAMEOBJECT_CUSTOM_ANIM		 = 179,  //(0x0B3)
	CMSG_AREATRIGGER					= 180,  //(0x0B4)
	MSG_MOVE_START_FORWARD			  = 181,  //(0x0B5)
	MSG_MOVE_START_BACKWARD			 = 182,  //(0x0B6)
	MSG_MOVE_STOP					   = 183,  //(0x0B7)
	MSG_MOVE_START_STRAFE_LEFT		  = 184,  //(0x0B8)
	MSG_MOVE_START_STRAFE_RIGHT		 = 185,  //(0x0B9)
	MSG_MOVE_STOP_STRAFE				= 186,  //(0x0BA)
	MSG_MOVE_JUMP					   = 187,  //(0x0BB)
	MSG_MOVE_START_TURN_LEFT			= 188,  //(0x0BC)
	MSG_MOVE_START_TURN_RIGHT		   = 189,  //(0x0BD)
	MSG_MOVE_STOP_TURN				  = 190,  //(0x0BE)
	MSG_MOVE_START_PITCH_UP			 = 191,  //(0x0BF)
	MSG_MOVE_START_PITCH_DOWN		   = 192,  //(0x0C0)
	MSG_MOVE_STOP_PITCH				 = 193,  //(0x0C1)
	MSG_MOVE_SET_RUN_MODE			   = 194,  //(0x0C2)
	MSG_MOVE_SET_WALK_MODE			  = 195,  //(0x0C3)
	MSG_MOVE_TOGGLE_LOGGING			 = 196,  //(0x0C4)
	MSG_MOVE_TELEPORT				   = 197,  //(0x0C5)
	MSG_MOVE_TELEPORT_CHEAT			 = 198,  //(0x0C6)
	MSG_MOVE_TELEPORT_ACK			   = 199,  //(0x0C7)
	MSG_MOVE_TOGGLE_FALL_LOGGING		= 200,  //(0x0C8)
	MSG_MOVE_FALL_LAND				  = 201,  //(0x0C9)
	MSG_MOVE_START_SWIM				 = 202,  //(0x0CA)
	MSG_MOVE_STOP_SWIM				  = 203,  //(0x0CB)
	MSG_MOVE_SET_RUN_SPEED_CHEAT		= 204,  //(0x0CC)
	MSG_MOVE_SET_RUN_SPEED			  = 205,  //(0x0CD)
	MSG_MOVE_SET_RUN_BACK_SPEED_CHEAT   = 206,  //(0x0CE)
	MSG_MOVE_SET_RUN_BACK_SPEED		 = 207,  //(0x0CF)
	MSG_MOVE_SET_WALK_SPEED_CHEAT	   = 208,  //(0x0D0)
	MSG_MOVE_SET_WALK_SPEED			 = 209,  //(0x0D1)
	MSG_MOVE_SET_SWIM_SPEED_CHEAT	   = 210,  //(0x0D2)
	MSG_MOVE_SET_SWIM_SPEED			 = 211,  //(0x0D3)
	MSG_MOVE_SET_SWIM_BACK_SPEED_CHEAT  = 212,  //(0x0D4)
	MSG_MOVE_SET_SWIM_BACK_SPEED		= 213,  //(0x0D5)
	MSG_MOVE_SET_ALL_SPEED_CHEAT		= 214,  //(0x0D6)
	MSG_MOVE_SET_TURN_RATE_CHEAT		= 215,  //(0x0D7)
	MSG_MOVE_SET_TURN_RATE			  = 216,  //(0x0D8)
	MSG_MOVE_TOGGLE_COLLISION_CHEAT	 = 217,  //(0x0D9)
	MSG_MOVE_SET_FACING				 = 218,  //(0x0DA)
	MSG_MOVE_SET_PITCH				  = 219,  //(0x0DB)
	MSG_MOVE_WORLDPORT_ACK			  = 220,  //(0x0DC)
	SMSG_MONSTER_MOVE				   = 221,  //(0x0DD)
	SMSG_MOVE_WATER_WALK				= 222,  //(0x0DE)
	SMSG_MOVE_LAND_WALK				 = 223,  //(0x0DF)
	MSG_MOVE_SET_RAW_POSITION_ACK	   = 224,  //(0x0E0)
	CMSG_MOVE_SET_RAW_POSITION		  = 225,  //(0x0E1)
	SMSG_FORCE_RUN_SPEED_CHANGE		 = 226,  //(0x0E2)
	CMSG_FORCE_RUN_SPEED_CHANGE_ACK	 = 227,  //(0x0E3)
	SMSG_FORCE_RUN_BACK_SPEED_CHANGE	= 228,  //(0x0E4)
	CMSG_FORCE_RUN_BACK_SPEED_CHANGE_ACK = 229,  //(0x0E5)
	SMSG_FORCE_SWIM_SPEED_CHANGE		= 230,  //(0x0E6)
	CMSG_FORCE_SWIM_SPEED_CHANGE_ACK	= 231,  //(0x0E7)
	SMSG_FORCE_MOVE_ROOT				= 232,  //(0x0E8)
	CMSG_FORCE_MOVE_ROOT_ACK			= 233,  //(0x0E9)
	SMSG_FORCE_MOVE_UNROOT			  = 234,  //(0x0EA)
	CMSG_FORCE_MOVE_UNROOT_ACK		  = 235,  //(0x0EB)
	MSG_MOVE_ROOT					   = 236,  //(0x0EC)
	MSG_MOVE_UNROOT					 = 237,  //(0x0ED)
	MSG_MOVE_HEARTBEAT				  = 238,  //(0x0EE)
	SMSG_MOVE_KNOCK_BACK				= 239,  //(0x0EF)
	CMSG_MOVE_KNOCK_BACK_ACK			= 240,  //(0x0F0)
	MSG_MOVE_KNOCK_BACK				 = 241,  //(0x0F1)
	SMSG_MOVE_FEATHER_FALL			  = 242,  //(0x0F2)
	SMSG_MOVE_NORMAL_FALL			   = 243,  //(0x0F3)
	SMSG_MOVE_SET_HOVER				 = 244,  //(0x0F4)
	SMSG_MOVE_UNSET_HOVER			   = 245,  //(0x0F5)
	CMSG_MOVE_HOVER_ACK				 = 246,  //(0x0F6)
	MSG_MOVE_HOVER					  = 247,  //(0x0F7)
	CMSG_TRIGGER_CINEMATIC_CHEAT		= 248,  //(0x0F8)
	CMSG_OPENING_CINEMATIC			  = 249,  //(0x0F9)
	SMSG_TRIGGER_CINEMATIC			  = 250,  //(0x0FA)
	CMSG_NEXT_CINEMATIC_CAMERA		  = 251,  //(0x0FB)
	CMSG_COMPLETE_CINEMATIC			 = 252,  //(0x0FC)
	SMSG_TUTORIAL_FLAGS				 = 253,  //(0x0FD)
	CMSG_TUTORIAL_FLAG				  = 254,  //(0x0FE)
	CMSG_TUTORIAL_CLEAR				 = 255,  //(0x0FF)
	CMSG_TUTORIAL_RESET				 = 256,  //(0x100)
	CMSG_STANDSTATECHANGE			   = 257,  //(0x101)
	CMSG_EMOTE						  = 258,  //(0x102)
	SMSG_EMOTE						  = 259,  //(0x103)
	CMSG_TEXT_EMOTE					 = 260,  //(0x104)
	SMSG_TEXT_EMOTE					 = 261,  //(0x105)
	CMSG_AUTOEQUIP_GROUND_ITEM		  = 262,  //(0x106)
	CMSG_AUTOSTORE_GROUND_ITEM		  = 263,  //(0x107)
	CMSG_AUTOSTORE_LOOT_ITEM			= 264,  //(0x108)
	CMSG_STORE_LOOT_IN_SLOT			 = 265,  //(0x109)
	CMSG_AUTOEQUIP_ITEM				 = 266,  //(0x10A)
	CMSG_AUTOSTORE_BAG_ITEM			 = 267,  //(0x10B)
	CMSG_SWAP_ITEM					  = 268,  //(0x10C)
	CMSG_SWAP_INV_ITEM				  = 269,  //(0x10D)
	CMSG_SPLIT_ITEM					 = 270,  //(0x10E)
	CMSG_PICKUP_ITEM					= 271,  //(0x10F)
	CMSG_DROP_ITEM					  = 272,  //(0x110)
	CMSG_DESTROYITEM					= 273,  //(0x111)
	SMSG_INVENTORY_CHANGE_FAILURE	   = 274,  //(0x112)
	SMSG_OPEN_CONTAINER				 = 275,  //(0x113)
	CMSG_INSPECT						= 276,  //(0x114)
	SMSG_INSPECT						= 277,  //(0x115)
	CMSG_INITIATE_TRADE				 = 278,  //(0x116)
	CMSG_BEGIN_TRADE					= 279,  //(0x117)
	CMSG_BUSY_TRADE					 = 280,  //(0x118)
	CMSG_IGNORE_TRADE				   = 281,  //(0x119)
	CMSG_ACCEPT_TRADE				   = 282,  //(0x11A)
	CMSG_UNACCEPT_TRADE				 = 283,  //(0x11B)
	CMSG_CANCEL_TRADE				   = 284,  //(0x11C)
	CMSG_SET_TRADE_ITEM				 = 285,  //(0x11D)
	CMSG_CLEAR_TRADE_ITEM			   = 286,  //(0x11E)
	CMSG_SET_TRADE_GOLD				 = 287,  //(0x11F)
	SMSG_TRADE_STATUS				   = 288,  //(0x120)
	SMSG_TRADE_STATUS_EXTENDED		  = 289,  //(0x121)
	SMSG_INITIALIZE_FACTIONS			= 290,  //(0x122)
	SMSG_SET_FACTION_VISIBLE			= 291,  //(0x123)
	SMSG_SET_FACTION_STANDING		   = 292,  //(0x124)
	CMSG_SET_FACTION_ATWAR			  = 293,  //(0x125)
	CMSG_SET_FACTION_CHEAT			  = 294,  //(0x126)
	SMSG_SET_PROFICIENCY				= 295,  //(0x127)
	CMSG_SET_ACTION_BUTTON			  = 296,  //(0x128)
	SMSG_ACTION_BUTTONS				 = 297,  //(0x129)
	SMSG_INITIAL_SPELLS				 = 298,  //(0x12A)
	SMSG_LEARNED_SPELL				  = 299,  //(0x12B)
	SMSG_SUPERCEDED_SPELL			   = 300,  //(0x12C)
	CMSG_NEW_SPELL_SLOT				 = 301,  //(0x12D)
	CMSG_CAST_SPELL					 = 302,  //(0x12E)
	CMSG_CANCEL_CAST					= 303,  //(0x12F)
	SMSG_CAST_RESULT					= 304,  //(0x130)
	SMSG_SPELL_START					= 305,  //(0x131)
	SMSG_SPELL_GO					   = 306,  //(0x132)
	SMSG_SPELL_FAILURE				  = 307,  //(0x133)
	SMSG_SPELL_COOLDOWN				 = 308,  //(0x134)
	SMSG_COOLDOWN_EVENT				 = 309,  //(0x135)
	CMSG_CANCEL_AURA					= 310,  //(0x136)
	SMSG_UPDATE_AURA_DURATION		   = 311,  //(0x137)
	SMSG_PET_CAST_FAILED				= 312,  //(0x138)
	MSG_CHANNEL_START				   = 313,  //(0x139)
	MSG_CHANNEL_UPDATE				  = 314,  //(0x13A)
	CMSG_CANCEL_CHANNELLING			 = 315,  //(0x13B)
	SMSG_AI_REACTION					= 316,  //(0x13C)
	CMSG_SET_SELECTION				  = 317,  //(0x13D)
	CMSG_SET_TARGET_OBSOLETE			= 318,  //(0x13E)
	CMSG_UNUSED						 = 319,  //(0x13F)
	CMSG_UNUSED2						= 320,  //(0x140)
	CMSG_ATTACKSWING					= 321,  //(0x141)
	CMSG_ATTACKSTOP					 = 322,  //(0x142)
	SMSG_ATTACKSTART					= 323,  //(0x143)
	SMSG_ATTACKSTOP					 = 324,  //(0x144)
	SMSG_ATTACKSWING_NOTINRANGE		 = 325,  //(0x145)
	SMSG_ATTACKSWING_BADFACING		  = 326,  //(0x146)
	SMSG_ATTACKSWING_NOTSTANDING		= 327,  //(0x147)
	SMSG_ATTACKSWING_DEADTARGET		 = 328,  //(0x148)
	SMSG_ATTACKSWING_CANT_ATTACK		= 329,  //(0x149)
	SMSG_ATTACKERSTATEUPDATE			= 330,  //(0x14A)
	SMSG_VICTIMSTATEUPDATE_OBSOLETE	 = 331,  //(0x14B)
	SMSG_DAMAGE_DONE_OBSOLETE		   = 332,  //(0x14C)
	SMSG_DAMAGE_TAKEN_OBSOLETE		  = 333,  //(0x14D)
	SMSG_CANCEL_COMBAT				  = 334,  //(0x14E)
	SMSG_PLAYER_COMBAT_XP_GAIN_OBSOLETE = 335,  //(0x14F)
	SMSG_HEALSPELL_ON_PLAYER_OBSOLETE   = 336,  //(0x150)
	SMSG_HEALSPELL_ON_PLAYERS_PET_OBSOLETE = 337,  //(0x151)
	CMSG_SHEATHE_OBSOLETE			   = 338,  //(0x152)
	CMSG_SAVE_PLAYER					= 339,  //(0x153)
	CMSG_SETDEATHBINDPOINT			  = 340,  //(0x154)
	SMSG_BINDPOINTUPDATE				= 341,  //(0x155)
	CMSG_GETDEATHBINDZONE			   = 342,  //(0x156)
	SMSG_BINDZONEREPLY				  = 343,  //(0x157)
	SMSG_PLAYERBOUND					= 344,  //(0x158)
	SMSG_DEATH_NOTIFY_OBSOLETE		  = 345,  //(0x159)
	CMSG_REPOP_REQUEST				  = 346,  //(0x15A)
	SMSG_RESURRECT_REQUEST			  = 347,  //(0x15B)
	CMSG_RESURRECT_RESPONSE			 = 348,  //(0x15C)
	CMSG_LOOT						   = 349,  //(0x15D)
	CMSG_LOOT_MONEY					 = 350,  //(0x15E)
	CMSG_LOOT_RELEASE				   = 351,  //(0x15F)
	SMSG_LOOT_RESPONSE				  = 352,  //(0x160)
	SMSG_LOOT_RELEASE_RESPONSE		  = 353,  //(0x161)
	SMSG_LOOT_REMOVED				   = 354,  //(0x162)
	SMSG_LOOT_MONEY_NOTIFY			  = 355,  //(0x163)
	SMSG_LOOT_ITEM_NOTIFY			   = 356,  //(0x164)
	SMSG_LOOT_CLEAR_MONEY			   = 357,  //(0x165)
	SMSG_ITEM_PUSH_RESULT			   = 358,  //(0x166)
	SMSG_DUEL_REQUESTED				 = 359,  //(0x167)
	SMSG_DUEL_OUTOFBOUNDS			   = 360,  //(0x168)
	SMSG_DUEL_INBOUNDS				  = 361,  //(0x169)
	SMSG_DUEL_COMPLETE				  = 362,  //(0x16A)
	SMSG_DUEL_WINNER					= 363,  //(0x16B)
	CMSG_DUEL_ACCEPTED				  = 364,  //(0x16C)
	CMSG_DUEL_CANCELLED				 = 365,  //(0x16D)
	SMSG_MOUNTRESULT					= 366,  //(0x16E)
	SMSG_DISMOUNTRESULT				 = 367,  //(0x16F)
	SMSG_PUREMOUNT_CANCELLED_OBSOLETE   = 368,  //(0x170)
	CMSG_MOUNTSPECIAL_ANIM			  = 369,  //(0x171)
	SMSG_MOUNTSPECIAL_ANIM			  = 370,  //(0x172)
	SMSG_PET_TAME_FAILURE			   = 371,  //(0x173)
	CMSG_PET_SET_ACTION				 = 372,  //(0x174)
	CMSG_PET_ACTION					 = 373,  //(0x175)
	CMSG_PET_ABANDON					= 374,  //(0x176)
	CMSG_PET_RENAME					 = 375,  //(0x177)
	SMSG_PET_NAME_INVALID			   = 376,  //(0x178)
	SMSG_PET_SPELLS					 = 377,  //(0x179)
	SMSG_PET_MODE					   = 378,  //(0x17A)
	CMSG_GOSSIP_HELLO				   = 379,  //(0x17B)
	CMSG_GOSSIP_SELECT_OPTION		   = 380,  //(0x17C)
	SMSG_GOSSIP_MESSAGE				 = 381,  //(0x17D)
	SMSG_GOSSIP_COMPLETE				= 382,  //(0x17E)
	CMSG_NPC_TEXT_QUERY				 = 383,  //(0x17F)
	SMSG_NPC_TEXT_UPDATE				= 384,  //(0x180)
	SMSG_NPC_WONT_TALK				  = 385,  //(0x181)
	CMSG_QUESTGIVER_STATUS_QUERY		= 386,  //(0x182)
	SMSG_QUESTGIVER_STATUS			  = 387,  //(0x183)
	CMSG_QUESTGIVER_HELLO			   = 388,  //(0x184)
	SMSG_QUESTGIVER_QUEST_LIST		  = 389,  //(0x185)
	CMSG_QUESTGIVER_QUERY_QUEST		 = 390,  //(0x186)
	CMSG_QUESTGIVER_QUEST_AUTOLAUNCH	= 391,  //(0x187)
	SMSG_QUESTGIVER_QUEST_DETAILS	   = 392,  //(0x188)
	CMSG_QUESTGIVER_ACCEPT_QUEST		= 393,  //(0x189)
	CMSG_QUESTGIVER_COMPLETE_QUEST	  = 394,  //(0x18A)
	SMSG_QUESTGIVER_REQUEST_ITEMS	   = 395,  //(0x18B)
	CMSG_QUESTGIVER_REQUEST_REWARD	  = 396,  //(0x18C)
	SMSG_QUESTGIVER_OFFER_REWARD		= 397,  //(0x18D)
	CMSG_QUESTGIVER_CHOOSE_REWARD	   = 398,  //(0x18E)
	SMSG_QUESTGIVER_QUEST_INVALID	   = 399,  //(0x18F)
	CMSG_QUESTGIVER_CANCEL			  = 400,  //(0x190)
	SMSG_QUESTGIVER_QUEST_COMPLETE	  = 401,  //(0x191)
	SMSG_QUESTGIVER_QUEST_FAILED		= 402,  //(0x192)
	CMSG_QUESTLOG_SWAP_QUEST			= 403,  //(0x193)
	CMSG_QUESTLOG_REMOVE_QUEST		  = 404,  //(0x194)
	SMSG_QUESTLOG_FULL				  = 405,  //(0x195)
	SMSG_QUESTUPDATE_FAILED			 = 406,  //(0x196)
	SMSG_QUESTUPDATE_FAILEDTIMER		= 407,  //(0x197)
	SMSG_QUESTUPDATE_COMPLETE		   = 408,  //(0x198)
	SMSG_QUESTUPDATE_ADD_KILL		   = 409,  //(0x199)
	SMSG_QUESTUPDATE_ADD_ITEM		   = 410,  //(0x19A)
	CMSG_QUEST_CONFIRM_ACCEPT		   = 411,  //(0x19B)
	SMSG_QUEST_CONFIRM_ACCEPT		   = 412,  //(0x19C)
	CMSG_PUSHQUESTTOPARTY			   = 413,  //(0x19D)
	CMSG_LIST_INVENTORY				 = 414,  //(0x19E)
	SMSG_LIST_INVENTORY				 = 415,  //(0x19F)
	CMSG_SELL_ITEM					  = 416,  //(0x1A0)
	SMSG_SELL_ITEM					  = 417,  //(0x1A1)
	CMSG_BUY_ITEM					   = 418,  //(0x1A2)
	CMSG_BUY_ITEM_IN_SLOT			   = 419,  //(0x1A3)
	SMSG_BUY_ITEM					   = 420,  //(0x1A4)
	SMSG_BUY_FAILED					 = 421,  //(0x1A5)
	CMSG_TAXICLEARALLNODES			  = 422,  //(0x1A6)
	CMSG_TAXIENABLEALLNODES			 = 423,  //(0x1A7)
	CMSG_TAXISHOWNODES				  = 424,  //(0x1A8)
	SMSG_SHOWTAXINODES				  = 425,  //(0x1A9)
	CMSG_TAXINODE_STATUS_QUERY		  = 426,  //(0x1AA)
	SMSG_TAXINODE_STATUS				= 427,  //(0x1AB)
	CMSG_TAXIQUERYAVAILABLENODES		= 428,  //(0x1AC)
	CMSG_ACTIVATETAXI				   = 429,  //(0x1AD)
	SMSG_ACTIVATETAXIREPLY			  = 430,  //(0x1AE)
	SMSG_NEW_TAXI_PATH				  = 431,  //(0x1AF)
	CMSG_TRAINER_LIST				   = 432,  //(0x1B0)
	SMSG_TRAINER_LIST				   = 433,  //(0x1B1)
	CMSG_TRAINER_BUY_SPELL			  = 434,  //(0x1B2)
	SMSG_TRAINER_BUY_SUCCEEDED		  = 435,  //(0x1B3)
	SMSG_TRAINER_BUY_FAILED			 = 436,  //(0x1B4)
	CMSG_BINDER_ACTIVATE				= 437,  //(0x1B5)
	SMSG_PLAYERBINDERROR				= 438,  //(0x1B6)
	CMSG_BANKER_ACTIVATE				= 439,  //(0x1B7)
	SMSG_SHOW_BANK					  = 440,  //(0x1B8)
	CMSG_BUY_BANK_SLOT				  = 441,  //(0x1B9)
	SMSG_BUY_BANK_SLOT_RESULT		   = 442,  //(0x1BA)
	CMSG_PETITION_SHOWLIST			  = 443,  //(0x1BB)
	SMSG_PETITION_SHOWLIST			  = 444,  //(0x1BC)
	CMSG_PETITION_BUY				   = 445,  //(0x1BD)
	CMSG_PETITION_SHOW_SIGNATURES	   = 446,  //(0x1BE)
	SMSG_PETITION_SHOW_SIGNATURES	   = 447,  //(0x1BF)
	CMSG_PETITION_SIGN				  = 448,  //(0x1C0)
	SMSG_PETITION_SIGN_RESULTS		  = 449,  //(0x1C1)
	MSG_PETITION_DECLINE				= 450,  //(0x1C2)
	CMSG_OFFER_PETITION				 = 451,  //(0x1C3)
	CMSG_TURN_IN_PETITION			   = 452,  //(0x1C4)
	SMSG_TURN_IN_PETITION_RESULTS	   = 453,  //(0x1C5)
	CMSG_PETITION_QUERY				 = 454,  //(0x1C6)
	SMSG_PETITION_QUERY_RESPONSE		= 455,  //(0x1C7)
	SMSG_FISH_NOT_HOOKED				= 456,  //(0x1C8)
	SMSG_FISH_ESCAPED				   = 457,  //(0x1C9)
	CMSG_BUG							= 458,  //(0x1CA)
	SMSG_NOTIFICATION				   = 459,  //(0x1CB)
	CMSG_PLAYED_TIME					= 460,  //(0x1CC)
	SMSG_PLAYED_TIME					= 461,  //(0x1CD)
	CMSG_QUERY_TIME					 = 462,  //(0x1CE)
	SMSG_QUERY_TIME_RESPONSE			= 463,  //(0x1CF)
	SMSG_LOG_XPGAIN					 = 464,  //(0x1D0)
	MSG_SPLIT_MONEY					 = 465,  //(0x1D1)
	CMSG_RECLAIM_CORPSE				 = 466,  //(0x1D2)
	CMSG_WRAP_ITEM					  = 467,  //(0x1D3)
	SMSG_LEVELUP_INFO				   = 468,  //(0x1D4)
	MSG_MINIMAP_PING					= 469,  //(0x1D5)
	SMSG_RESISTLOG					  = 470,  //(0x1D6)
	SMSG_ENCHANTMENTLOG				 = 471,  //(0x1D7)
	CMSG_SET_SKILL_CHEAT				= 472,  //(0x1D8)
	SMSG_START_MIRROR_TIMER			 = 473,  //(0x1D9)
	SMSG_PAUSE_MIRROR_TIMER			 = 474,  //(0x1DA)
	SMSG_STOP_MIRROR_TIMER			  = 475,  //(0x1DB)
	CMSG_PING						   = 476,  //(0x1DC)
	SMSG_PONG						   = 477,  //(0x1DD)
	SMSG_CLEAR_COOLDOWN				 = 478,  //(0x1DE)
	SMSG_GAMEOBJECT_PAGETEXT			= 479,  //(0x1DF)
	CMSG_SETSHEATHED					= 480,  //(0x1E0)
	SMSG_COOLDOWN_CHEAT				 = 481,  //(0x1E1)
	SMSG_SPELL_DELAYED				  = 482,  //(0x1E2)
	CMSG_PLAYER_MACRO_OBSOLETE		  = 483,  //(0x1E3)
	SMSG_PLAYER_MACRO_OBSOLETE		  = 484,  //(0x1E4)
	CMSG_GHOST						  = 485,  //(0x1E5)
	CMSG_GM_INVIS					   = 486,  //(0x1E6)
	SMSG_INVALID_PROMOTION_CODE		 = 487,  //(0x1E7)
	MSG_GM_BIND_OTHER				   = 488,  //(0x1E8)
	MSG_GM_SUMMON					   = 489,  //(0x1E9)
	SMSG_ITEM_TIME_UPDATE			   = 490,  //(0x1EA)
	SMSG_ITEM_ENCHANT_TIME_UPDATE	   = 491,  //(0x1EB)
	SMSG_AUTH_CHALLENGE				 = 492,  //(0x1EC)
	CMSG_AUTH_SESSION				   = 493,  //(0x1ED)
	SMSG_AUTH_RESPONSE				  = 494,  //(0x1EE)
	MSG_GM_SHOWLABEL					= 495,  //(0x1EF)
	MSG_ADD_DYNAMIC_TARGET_OBSOLETE	 = 496,  //(0x1F0)
	MSG_SAVE_GUILD_EMBLEM			   = 497,  //(0x1F1)
	MSG_TABARDVENDOR_ACTIVATE		   = 498,  //(0x1F2)
	SMSG_PLAY_SPELL_VISUAL			  = 499,  //(0x1F3)
	CMSG_ZONEUPDATE					 = 500,  //(0x1F4)
	SMSG_PARTYKILLLOG				   = 501,  //(0x1F5)
	SMSG_COMPRESSED_UPDATE_OBJECT	   = 502,  //(0x1F6)
	SMSG_OBSOLETE					   = 503,  //(0x1F7)
	SMSG_EXPLORATION_EXPERIENCE		 = 504,  //(0x1F8)
	CMSG_GM_SET_SECURITY_GROUP		  = 505,  //(0x1F9)
	CMSG_GM_NUKE						= 506,  //(0x1FA)
	MSG_RANDOM_ROLL					 = 507,  //(0x1FB)
	SMSG_ENVIRONMENTALDAMAGELOG		 = 508,  //(0x1FC)
	CMSG_RWHOIS						 = 509,  //(0x1FD)
	SMSG_RWHOIS						 = 510,  //(0x1FE)
	MSG_LOOKING_FOR_GROUP			   = 511,  //(0x1FF)
	CMSG_SET_LOOKING_FOR_GROUP		  = 512,  //(0x200)
	CMSG_UNLEARN_SPELL				  = 513,  //(0x201)
	CMSG_UNLEARN_SKILL				  = 514,  //(0x202)
	SMSG_REMOVED_SPELL				  = 515,  //(0x203)
	CMSG_DECHARGE					   = 516,  //(0x204)
	CMSG_GMTICKET_CREATE				= 517,  //(0x205)
	SMSG_GMTICKET_CREATE				= 518,  //(0x206)
	CMSG_GMTICKET_UPDATETEXT			= 519,  //(0x207)
	SMSG_GMTICKET_UPDATETEXT			= 520,  //(0x208)
	SMSG_ACCOUNT_DATA_MD5			   = 521,  //(0x209)
	CMSG_REQUEST_ACCOUNT_DATA		   = 522,  //(0x20A)
	CMSG_UPDATE_ACCOUNT_DATA			= 523,  //(0x20B)
	SMSG_UPDATE_ACCOUNT_DATA			= 524,  //(0x20C)
	SMSG_CLEAR_FAR_SIGHT_IMMEDIATE	  = 525,  //(0x20D)
	SMSG_POWERGAINLOG_OBSOLETE		  = 526,  //(0x20E)
	CMSG_GM_TEACH					   = 527,  //(0x20F)
	CMSG_GM_CREATE_ITEM_TARGET		  = 528,  //(0x210)
	CMSG_GMTICKET_GETTICKET			 = 529,  //(0x211)
	SMSG_GMTICKET_GETTICKET			 = 530,  //(0x212)
	CMSG_UNLEARN_TALENTS				= 531,  //(0x213)
	SMSG_GAMEOBJECT_SPAWN_ANIM		  = 532,  //(0x214)
	SMSG_GAMEOBJECT_DESPAWN_ANIM		= 533,  //(0x215)
	MSG_CORPSE_QUERY					= 534,  //(0x216)
	CMSG_GMTICKET_DELETETICKET		  = 535,  //(0x217)
	SMSG_GMTICKET_DELETETICKET		  = 536,  //(0x218)
	SMSG_CHAT_WRONG_FACTION			 = 537,  //(0x219)
	CMSG_GMTICKET_SYSTEMSTATUS		  = 538,  //(0x21A)
	SMSG_GMTICKET_SYSTEMSTATUS		  = 539,  //(0x21B)
	CMSG_SPIRIT_HEALER_ACTIVATE		 = 540,  //(0x21C)
	CMSG_SET_STAT_CHEAT				 = 541,  //(0x21D)
	SMSG_SET_REST_START				 = 542,  //(0x21E)
	CMSG_SKILL_BUY_STEP				 = 543,  //(0x21F)
	CMSG_SKILL_BUY_RANK				 = 544,  //(0x220)
	CMSG_XP_CHEAT					   = 545,  //(0x221)
	SMSG_SPIRIT_HEALER_CONFIRM		  = 546,  //(0x222)
	CMSG_CHARACTER_POINT_CHEAT		  = 547,  //(0x223)
	SMSG_GOSSIP_POI					 = 548,  //(0x224)
	CMSG_CHAT_IGNORED				   = 549,  //(0x225)
	CMSG_GM_VISION					  = 550,  //(0x226)
	CMSG_SERVER_COMMAND				 = 551,  //(0x227)
	CMSG_GM_SILENCE					 = 552,  //(0x228)
	CMSG_GM_REVEALTO					= 553,  //(0x229)
	CMSG_GM_RESURRECT				   = 554,  //(0x22A)
	CMSG_GM_SUMMONMOB				   = 555,  //(0x22B)
	CMSG_GM_MOVECORPSE				  = 556,  //(0x22C)
	CMSG_GM_FREEZE					  = 557,  //(0x22D)
	CMSG_GM_UBERINVIS				   = 558,  //(0x22E)
	CMSG_GM_REQUEST_PLAYER_INFO		 = 559,  //(0x22F)
	SMSG_GM_PLAYER_INFO				 = 560,  //(0x230)
	CMSG_GUILD_RANK					 = 561,  //(0x231)
	CMSG_GUILD_ADD_RANK				 = 562,  //(0x232)
	CMSG_GUILD_DEL_RANK				 = 563,  //(0x233)
	CMSG_GUILD_SET_PUBLIC_NOTE		  = 564,  //(0x234)
	CMSG_GUILD_SET_OFFICER_NOTE		 = 565,  //(0x235)
	SMSG_LOGIN_VERIFY_WORLD			 = 566,  //(0x236)
	CMSG_CLEAR_EXPLORATION			  = 567,  //(0x237)
	CMSG_SEND_MAIL					  = 568,  //(0x238)
	SMSG_SEND_MAIL_RESULT			   = 569,  //(0x239)
	CMSG_GET_MAIL_LIST				  = 570,  //(0x23A)
	SMSG_MAIL_LIST_RESULT			   = 571,  //(0x23B)
	CMSG_BATTLEFIELD_LIST			   = 572,  //(0x23C)
	SMSG_BATTLEFIELD_LIST			   = 573,  //(0x23D)
	CMSG_BATTLEFIELD_JOIN			   = 574,  //(0x23E)
	SMSG_BATTLEFIELD_WIN				= 575,  //(0x23F)
	SMSG_BATTLEFIELD_LOSE			   = 576,  //(0x240)
	CMSG_TAXICLEARNODE				  = 577,  //(0x241)
	CMSG_TAXIENABLENODE				 = 578,  //(0x242)
	CMSG_ITEM_TEXT_QUERY				= 579,  //(0x243)
	SMSG_ITEM_TEXT_QUERY_RESPONSE	   = 580,  //(0x244)
	CMSG_MAIL_TAKE_MONEY				= 581,  //(0x245)
	CMSG_MAIL_TAKE_ITEM				 = 582,  //(0x246)
	CMSG_MAIL_MARK_AS_READ			  = 583,  //(0x247)
	CMSG_MAIL_RETURN_TO_SENDER		  = 584,  //(0x248)
	CMSG_MAIL_DELETE					= 585,  //(0x249)
	CMSG_MAIL_CREATE_TEXT_ITEM		  = 586,  //(0x24A)
	SMSG_SPELLLOGMISS				   = 587,  //(0x24B)
	SMSG_SPELLLOGEXECUTE				= 588,  //(0x24C)
	SMSG_DEBUGAURAPROC				  = 589,  //(0x24D)
	SMSG_PERIODICAURALOG				= 590,  //(0x24E)
	SMSG_SPELLDAMAGESHIELD			  = 591,  //(0x24F)
	SMSG_SPELLNONMELEEDAMAGELOG		 = 592,  //(0x250)
	CMSG_LEARN_TALENT				   = 593,  //(0x251)
	SMSG_RESURRECT_FAILED			   = 594,  //(0x252)
	CMSG_TOGGLE_PVP					 = 595,  //(0x253)
	SMSG_ZONE_UNDER_ATTACK			  = 596,  //(0x254)
	MSG_AUCTION_HELLO				   = 597,  //(0x255)
	CMSG_AUCTION_SELL_ITEM			  = 598,  //(0x256)
	CMSG_AUCTION_REMOVE_ITEM			= 599,  //(0x257)
	CMSG_AUCTION_LIST_ITEMS			 = 600,  //(0x258)
	CMSG_AUCTION_LIST_OWNER_ITEMS	   = 601,  //(0x259)
	CMSG_AUCTION_PLACE_BID			  = 602,  //(0x25A)
	SMSG_AUCTION_COMMAND_RESULT		 = 603,  //(0x25B)
	SMSG_AUCTION_LIST_RESULT			= 604,  //(0x25C)
	SMSG_AUCTION_OWNER_LIST_RESULT	  = 605,  //(0x25D)
	SMSG_AUCTION_BIDDER_NOTIFICATION	= 606,  //(0x25E)
	SMSG_AUCTION_OWNER_NOTIFICATION	 = 607,  //(0x25F)
	SMSG_PROCRESIST					 = 608,  //(0x260)
	SMSG_STANDSTATE_CHANGE_FAILURE	  = 609,  //(0x261)
	SMSG_DISPEL_FAILED				  = 610,  //(0x262)
	SMSG_SPELLORDAMAGE_IMMUNE		   = 611,  //(0x263)
	CMSG_AUCTION_LIST_BIDDER_ITEMS	  = 612,  //(0x264)
	SMSG_AUCTION_BIDDER_LIST_RESULT	 = 613,  //(0x265)
	SMSG_SET_FLAT_SPELL_MODIFIER		= 614,  //(0x266)
	SMSG_SET_PCT_SPELL_MODIFIER		 = 615,  //(0x267)
	CMSG_SET_AMMO					   = 616,  //(0x268)
	SMSG_CORPSE_RECLAIM_DELAY		   = 617,  //(0x269)
	CMSG_SET_ACTIVE_MOVER			   = 618,  //(0x26A)
	CMSG_PET_CANCEL_AURA				= 619,  //(0x26B)
	CMSG_PLAYER_AI_CHEAT				= 620,  //(0x26C)
	CMSG_CANCEL_AUTO_REPEAT_SPELL	   = 621,  //(0x26D)
	MSG_GM_ACCOUNT_ONLINE			   = 622,  //(0x26E)
	MSG_LIST_STABLED_PETS			   = 623,  //(0x26F)
	CMSG_STABLE_PET					 = 624,  //(0x270)
	CMSG_UNSTABLE_PET				   = 625,  //(0x271)
	CMSG_BUY_STABLE_SLOT				= 626,  //(0x272)
	SMSG_STABLE_RESULT				  = 627,  //(0x273)
	CMSG_STABLE_REVIVE_PET			  = 628,  //(0x274)
	CMSG_STABLE_SWAP_PET				= 629,  //(0x275)
	MSG_QUEST_PUSH_RESULT			   = 630,  //(0x276)
	SMSG_PLAY_MUSIC					 = 631,  //(0x277)
	SMSG_PLAY_OBJECT_SOUND			  = 632,  //(0x278)
	CMSG_REQUEST_PET_INFO			   = 633,  //(0x279)
	CMSG_FAR_SIGHT					  = 634,  //(0x27A)
	SMSG_SPELLDISPELLOG				 = 635,  //(0x27B)
	SMSG_DAMAGE_CALC_LOG				= 636,  //(0x27C)
	CMSG_ENABLE_DAMAGE_LOG			  = 637,  //(0x27D)
	CMSG_GROUP_CHANGE_SUB_GROUP		 = 638,  //(0x27E)
	CMSG_REQUEST_PARTY_MEMBER_STATS	 = 639,  //(0x27F)
	CMSG_GROUP_SWAP_SUB_GROUP		   = 640,  //(0x280)
	CMSG_RESET_FACTION_CHEAT			= 641,  //(0x281)
	CMSG_AUTOSTORE_BANK_ITEM			= 642,  //(0x282)
	CMSG_AUTOBANK_ITEM				  = 643,  //(0x283)
	MSG_QUERY_NEXT_MAIL_TIME			= 644,  //(0x284)
	SMSG_RECEIVED_MAIL				  = 645,  //(0x285)
	SMSG_RAID_GROUP_ONLY				= 646,  //(0x286)
	CMSG_SET_DURABILITY_CHEAT		   = 647,  //(0x287)
	CMSG_SET_PVP_RANK_CHEAT			 = 648,  //(0x288)
	CMSG_ADD_PVP_MEDAL_CHEAT			= 649,  //(0x289)
	CMSG_DEL_PVP_MEDAL_CHEAT			= 650,  //(0x28A)
	CMSG_SET_PVP_TITLE				  = 651,  //(0x28B)
	SMSG_PVP_CREDIT					 = 652,  //(0x28C)
	SMSG_AUCTION_REMOVED_NOTIFICATION   = 653,  //(0x28D)
	CMSG_GROUP_RAID_CONVERT			 = 654,  //(0x28E)
	CMSG_GROUP_ASSISTANT_LEADER		 = 655,  //(0x28F)
	CMSG_BUYBACK_ITEM				   = 656,  //(0x290)
	SMSG_SERVER_MESSAGE				 = 657,  //(0x291)
	CMSG_MEETINGSTONE_JOIN			  = 658,  //(0x292)
	CMSG_MEETINGSTONE_LEAVE			 = 659,  //(0x293)
	CMSG_MEETINGSTONE_CHEAT			 = 660,  //(0x294)
	SMSG_MEETINGSTONE_SETQUEUE		  = 661,  //(0x295)
	CMSG_MEETINGSTONE_INFO			  = 662,  //(0x296)
	SMSG_MEETINGSTONE_COMPLETE		  = 663,  //(0x297)
	SMSG_MEETINGSTONE_IN_PROGRESS	   = 664,  //(0x298)
	SMSG_MEETINGSTONE_MEMBER_ADDED	  = 665,  //(0x299)
	CMSG_GMTICKETSYSTEM_TOGGLE		  = 666,  //(0x29A)
	CMSG_CANCEL_GROWTH_AURA			 = 667,  //(0x29B)
	SMSG_CANCEL_AUTO_REPEAT			 = 668,  //(0x29C)
	SMSG_STANDSTATE_CHANGE_ACK		  = 669,  //(0x29D)
	SMSG_LOOT_ALL_PASSED				= 670,  //(0x29E)
	SMSG_LOOT_ROLL_WON				  = 671,  //(0x29F)
	CMSG_LOOT_ROLL					  = 672,  //(0x2A0)
	SMSG_LOOT_START_ROLL				= 673,  //(0x2A1)
	SMSG_LOOT_ROLL					  = 674,  //(0x2A2)
	CMSG_LOOT_MASTER_GIVE			   = 675,  //(0x2A3)
	SMSG_LOOT_MASTER_LIST			   = 676,  //(0x2A4)
	SMSG_SET_FORCED_REACTIONS		   = 677,  //(0x2A5)
	SMSG_SPELL_FAILED_OTHER			 = 678,  //(0x2A6)
	SMSG_GAMEOBJECT_RESET_STATE		 = 679,  //(0x2A7)
	CMSG_REPAIR_ITEM					= 680,  //(0x2A8)
	SMSG_CHAT_PLAYER_NOT_FOUND		  = 681,  //(0x2A9)
	MSG_TALENT_WIPE_CONFIRM			 = 682,  //(0x2AA)
	SMSG_SUMMON_REQUEST				 = 683,  //(0x2AB)
	CMSG_SUMMON_RESPONSE				= 684,  //(0x2AC)
	MSG_MOVE_TOGGLE_GRAVITY_CHEAT	   = 685,  //(0x2AD)
	SMSG_MONSTER_MOVE_TRANSPORT		 = 686,  //(0x2AE)
	SMSG_PET_BROKEN					 = 687,  //(0x2AF)
	MSG_MOVE_FEATHER_FALL			   = 688,  //(0x2B0)
	MSG_MOVE_WATER_WALK				 = 689,  //(0x2B1)
	CMSG_SERVER_BROADCAST			   = 690,  //(0x2B2)
	CMSG_SELF_RES					   = 691,  //(0x2B3)
	SMSG_FEIGN_DEATH_RESISTED		   = 692,  //(0x2B4)
	CMSG_RUN_SCRIPT					 = 693,  //(0x2B5)
	SMSG_SCRIPT_MESSAGE				 = 694,  //(0x2B6)
	SMSG_DUEL_COUNTDOWN				 = 695,  //(0x2B7)
	SMSG_AREA_TRIGGER_MESSAGE		   = 696,  //(0x2B8)
	CMSG_TOGGLE_HELM					= 697,  //(0x2B9)
	CMSG_TOGGLE_CLOAK				   = 698,  //(0x2BA)
	SMSG_MEETINGSTONE_JOINFAILED		= 699,  //(0x2BB)
	SMSG_PLAYER_SKINNED				 = 700,  //(0x2BC)
	SMSG_DURABILITY_DAMAGE_DEATH		= 701,  //(0x2BD)
	CMSG_SET_EXPLORATION				= 702,  //(0x2BE)
	CMSG_SET_ACTIONBAR_TOGGLES		  = 703,  //(0x2BF)
	UMSG_DELETE_GUILD_CHARTER		   = 704,  //(0x2C0)
	MSG_PETITION_RENAME				 = 705,  //(0x2C1)
	SMSG_INIT_WORLD_STATES			  = 706,  //(0x2C2)
	SMSG_UPDATE_WORLD_STATE			 = 707,  //(0x2C3)
	CMSG_ITEM_NAME_QUERY				= 708,  //(0x2C4)
	SMSG_ITEM_NAME_QUERY_RESPONSE	   = 709,  //(0x2C5)
	SMSG_PET_ACTION_FEEDBACK			= 710,  //(0x2C6)
	CMSG_CHAR_RENAME					= 711,  //(0x2C7)
	SMSG_CHAR_RENAME					= 712,  //(0x2C8)
	CMSG_MOVE_SPLINE_DONE			   = 713,  //(0x2C9)
	CMSG_MOVE_FALL_RESET				= 714,  //(0x2CA)
	SMSG_INSTANCE_SAVE_CREATED		  = 715,  //(0x2CB)
	SMSG_RAID_INSTANCE_INFO			 = 716,  //(0x2CC)
	CMSG_REQUEST_RAID_INFO			  = 717,  //(0x2CD)
	CMSG_MOVE_TIME_SKIPPED			  = 718,  //(0x2CE)
	CMSG_MOVE_FEATHER_FALL_ACK		  = 719,  //(0x2CF)
	CMSG_MOVE_WATER_WALK_ACK			= 720,  //(0x2D0)
	CMSG_MOVE_NOT_ACTIVE_MOVER		  = 721,  //(0x2D1)
	SMSG_PLAY_SOUND					 = 722,  //(0x2D2)
	CMSG_BATTLEFIELD_STATUS			 = 723,  //(0x2D3)
	SMSG_BATTLEFIELD_STATUS			 = 724,  //(0x2D4)
	CMSG_BATTLEFIELD_PORT			   = 725,  //(0x2D5)
	MSG_INSPECT_HONOR_STATS			 = 726,  //(0x2D6)
	CMSG_BATTLEMASTER_HELLO			 = 727,  //(0x2D7)
	CMSG_MOVE_START_SWIM_CHEAT		  = 728,  //(0x2D8)
	CMSG_MOVE_STOP_SWIM_CHEAT		   = 729,  //(0x2D9)
	SMSG_FORCE_WALK_SPEED_CHANGE		= 730,  //(0x2DA)
	CMSG_FORCE_WALK_SPEED_CHANGE_ACK	= 731,  //(0x2DB)
	SMSG_FORCE_SWIM_BACK_SPEED_CHANGE   = 732,  //(0x2DC)
	CMSG_FORCE_SWIM_BACK_SPEED_CHANGE_ACK = 733,  //(0x2DD)
	SMSG_FORCE_TURN_RATE_CHANGE		 = 734,  //(0x2DE)
	CMSG_FORCE_TURN_RATE_CHANGE_ACK	 = 735,  //(0x2DF)
	MSG_PVP_LOG_DATA					= 736,  //(0x2E0)
	CMSG_LEAVE_BATTLEFIELD			  = 737,  //(0x2E1)
	CMSG_AREA_SPIRIT_HEALER_QUERY	   = 738,  //(0x2E2)
	CMSG_AREA_SPIRIT_HEALER_QUEUE	   = 739,  //(0x2E3)
	SMSG_AREA_SPIRIT_HEALER_TIME		= 740,  //(0x2E4)
	CMSG_GM_UNTEACH					 = 741,  //(0x2E5)
	SMSG_WARDEN_DATA					= 742,  //(0x2E6)
	CMSG_WARDEN_DATA					= 743,  //(0x2E7)
	SMSG_GROUP_JOINED_BATTLEGROUND	  = 744,  //(0x2E8)
	MSG_BATTLEGROUND_PLAYER_POSITIONS   = 745,  //(0x2E9)
	CMSG_PET_STOP_ATTACK				= 746,  //(0x2EA)
	SMSG_BINDER_CONFIRM				 = 747,  //(0x2EB)
	SMSG_BATTLEGROUND_PLAYER_JOINED	 = 748,  //(0x2EC)
	SMSG_BATTLEGROUND_PLAYER_LEFT	   = 749,  //(0x2ED)
	CMSG_BATTLEMASTER_JOIN			  = 750,  //(0x2EF)
	SMSG_ADDON_INFO					 = 751,  //(0x2EF)
	CMSG_PET_UNLEARN					= 754,  //(0x2F2)
	SMSG_PET_UNLEARN_CONFIRM			= 755,  //(0x2F3)
	SMSG_WEATHER						= 756,  //(0x2F4)
	CMSG_PET_SPELL_AUTOCAST			 = 757,  //(0x2F5)
	SMSG_PARTY_MEMBER_STATS_FULL		= 758,  //(0x2F6)
	SMSG_PLAY_TIME_WARNING			  = 759,  //(0x2F7)
	SMSG_MINIGAME_SETUP				 = 758,  //(0x2F6)
	SMSG_MINIGAME_STATE				 = 759,  //(0x2F7)
	CMSG_MINIGAME_MOVE				  = 760,  //(0x2F8)
	SMSG_MINIGAME_MOVE_FAILED		   = 761,  //(0x2F9)
	SMSG_PET_TAME_UNK				   = 763,  //(0x2FB)
	CMSG_SET_GUILD_INFORMATION		  = 764,  //(0x2FC)
	SMSG_UNKNOWN_PET					= 772,  //(0x304)
	CMSG_ACTIVATE_MULTIPLE_TAXI			= 786,	//(0x312)
	CMSG_SET_FACTION_INACTIVE		   = 791,  //(0x317)
	CMSG_SET_WATCHED_FACTION_INDEX	  = 792,  //(0x318)
	CMSG_UNKNOWN_1					  = 793,  //(0x319)
	SMSG_UNKNOWN_DEMON				  = 794,  //(0x31A)
	CMSG_RESET_INSTANCE				 = 797,  //(0x31D)
	SMSG_RESET_INSTANCE				 = 798,  //(0x31E)
	SMSG_INSTANCE_SAVE				  = 800,  //(0x320)
	MSG_GROUP_SET_PLAYER_ICON		   = 801,  //(0x321)
	CMSG_RAID_READYCHECK				= 802,  //(0x322)
	SMSG_PET_ACTION_SOUND			   = 804,  //(0x324)
	SMSG_PET_DISMISS_SOUND			  = 805,  //(0x325)
	CMSG_DUNGEON_DIFFICULTY			 = 809,  //(0x329)
	SMSG_INSTANCE_RESET_ACTIVATE		= 811,  //(0x32B)
	SMSG_PVP_CAPTURE_STATE_MSG		  = 826,  //(0x33A)
	SMSG_BROADCAST_MSG				  = 829,  //(0x33D)
	SMSG_MOVE_SET_FLY				   = 835,  //(0x343)
	SMSG_MOVE_SET_UNFLY				 = 836,  //(0x344)
	CMSG_MOVE_SET_FLY_ACK			   = 837,  //(0x345)
	CMSG_MOVE_FLY_START_AND_END		 = 838,  //(0x346)
	CMSG_SOCKET_GEMS					= 839,  //(0x347)
	CMSG_ARENA_JOIN					 = 856,  //(0x358) - *Named*
	CMSG_FLY_PITCH_UP_Z				 = 857,  //(0x359)
	CMSG_FLY_PITCH_DOWN_AFTER_UP		= 858,  //(0x35A)
	CMSG_REALMLIST_CONNECTION_DROPPED   = 860,  //(0x35C)
	CMSG_ENABLE_AUTOJOIN				= 861,  //(0x35D) - *Named*
	CMSG_DISABLE_AUTOJOIN			   = 862,  //(0x35E) - *Named*
	CMSG_ENABLE_AUTOADD_MEMBERS		 = 863,  //(0x35F) - *Named*
	CMSG_DISABLE_AUTOADD_MEMBERS		= 864,  //(0x360) - *Named* 
	CMSG_CLEAR_LOOKING_FOR_GROUP_STATE  = 867,  //(0x363) - *Named*
	CMSG_SET_LOOKING_FOR_MORE		   = 869,  //(0x365) - *Named*
	CMSG_SET_LOOKING_FOR_GROUP_COMMENT  = 870,  //(0x366) - *Named*
	SMSG_SET_VISIBLE_RANK				= 883, //(0x373)
	CMSG_SET_VISIBLE_RANK			   = 884,  //(0x374)
	CMSG_DISMOUNT					 = 885,  //(0x0375)
	SMSG_SPIRIT_HEALER_POS				= 888,  //(0x378)
	CMSG_CANCEL_TEMPORARY_ENCHANTMENT   = 889,  //(0x379)
	SMSG_MOVE_SET_FLY_SPEED			 = 897,  //(0x381)
	CMSG_MOVE_SET_FLY_SPEED_ACK		 = 898,  //(0x382)
	SMSG_FLIGHT_SPLINE_SYNC			 = 904,  //(0x388)
	SMSG_MOVE_UNLOCK_MOVEMENT		   = 912,  //(0x390)
	CMSG_MOVE_UNLOCK_MOVEMENT_ACK	   = 913,  //(0x391)
	SMSG_SET_COMBO_POINTS				= 925,  //(0x39D)
	SMSG_PET_LEARNT_SPELL				= 932,  //(0x3A4)
	SMSG_TARGET_CAST_RESULT				= 934,	//(0x3A6)

	NUM_MSG_TYPES					   = 935,  //(0x3A7)
};

enum FriendsResult {
	FRIEND_DB_ERROR		 = 0x00,
	FRIEND_LIST_FULL		= 0x01,
	FRIEND_ONLINE		   = 0x02,
	FRIEND_OFFLINE		  = 0x03,
	FRIEND_NOT_FOUND		= 0x04,
	FRIEND_REMOVED		  = 0x05,
	FRIEND_ADDED_ONLINE	 = 0x06,
	FRIEND_ADDED_OFFLINE	= 0x07,
	FRIEND_ALREADY		  = 0x08,
	FRIEND_SELF			 = 0x09,
	FRIEND_ENEMY			= 0x0A,
	FRIEND_IGNORE_FULL	  = 0x0B,
	FRIEND_IGNORE_SELF	  = 0x0C,
	FRIEND_IGNORE_NOT_FOUND = 0x0D,
	FRIEND_IGNORE_ALREADY   = 0x0E,
	FRIEND_IGNORE_ADDED	 = 0x0F,
	FRIEND_IGNORE_REMOVED   = 0x10
};


/*
0 = Universal
1 = ?Orcish?
2 = Darnassian
3 = Taurahe
4 = ?
5 = ?
6 = Dwarvish
7 = Common
8 = Demonic
9 = Titan
10 = Thelassian
11 = Draconic
12 = Kalimag
13 = Gnomish
14 = Troll
*/


enum NPCFlags {
	UNIT_NPC_FLAG_NONE			  = 0,	// None
	UNIT_NPC_FLAG_GOSSIP			= 1,	// Gossip/Talk (CMSG_GOSSIP_HELLO, ?)
	UNIT_NPC_FLAG_QUESTGIVER		= 2,	// Questgiver
	UNIT_NPC_FLAG_VENDOR			= 4,	// Vendor (CMSG_LIST_INVENTORY, SMSG_LIST_INVENTORY)
	UNIT_NPC_FLAG_TAXIVENDOR		= 8,	// Taxi Vendor (CMSG_TAXIQUERYAVAILABLENODES, SMSG_SHOWTAXINODES)
	UNIT_NPC_FLAG_TRAINER		   = 16,   // Trainer (CMSG_TRAINER_LIST, SMSG_TRAINER_LIST)
	UNIT_NPC_FLAG_SPIRITHEALER	  = 32,   // Spirithealer (CMSG_BINDER_ACTIVATE, ?)
	UNIT_NPC_FLAG_INNKEEPER		 = 128,  // Innkeeper Asking for Bind Point
	UNIT_NPC_FLAG_BANKER			= 256,  // Banker
	UNIT_NPC_FLAG_ARENACHARTER	  = 512,  // Purchase an arena charter.. huh?
	UNIT_NPC_FLAG_TABARDCHANGER	 = 1024, // Tabard Design Changer Flag
	UNIT_NPC_FLAG_PETITIONER		= 1536, // Guild Master
	UNIT_NPC_FLAG_TABARDVENDOR	  = 1540,
	UNIT_NPC_FLAG_BATTLEFIELDPERSON = 2048, // BattleMaster (CMSG_BATTLEMASTER_HELLO (0x02D7))
	UNIT_NPC_FLAG_AUCTIONEER		= 4096, // Auctioneer (MSG_AUCTION_HELLO)
	UNIT_NPC_FLAG_STABLEMASTER	  = 8192, // (MSG_LIST_STABLED_PETS (0x026F))
	UNIT_NPC_FLAG_ARMORER		   = 16384,// not sure
};

//enum FieldFlags {
//	0x00000008 == can't climb slopes too steep
//	0x40000000 == sheathed
//};

#endif
