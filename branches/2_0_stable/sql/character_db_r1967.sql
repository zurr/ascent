/*
MySQL Data Transfer
Source Host: localhost
Source Database: character
Target Host: localhost
Target Database: character
Date: 18/10/2007 4:01:36 PM
*/

SET FOREIGN_KEY_CHECKS=0;
-- ----------------------------
-- Table structure for account_data
-- ----------------------------
DROP TABLE IF EXISTS `account_data`;
CREATE TABLE `account_data` (
  `acct` int(30) NOT NULL,
  `accountdata0` text NOT NULL,
  `accountdata1` text NOT NULL,
  `accountdata2` text NOT NULL,
  `accountdata3` text NOT NULL,
  `accountdata4` text NOT NULL,
  `accountdata5` text NOT NULL,
  `accountdata6` text NOT NULL,
  `accountdata7` text NOT NULL,
  PRIMARY KEY (`acct`),
  UNIQUE KEY `ind` (`acct`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

-- ----------------------------
-- Table structure for arenateams
-- ----------------------------
DROP TABLE IF EXISTS `arenateams`;
CREATE TABLE `arenateams` (
  `id` int(30) NOT NULL,
  `type` int(30) NOT NULL,
  `leader` int(30) NOT NULL,
  `name` varchar(150) NOT NULL,
  `emblemstyle` int(40) NOT NULL,
  `emblemcolour` bigint(40) NOT NULL,
  `borderstyle` int(40) NOT NULL,
  `bordercolour` bigint(40) NOT NULL,
  `backgroundcolour` bigint(40) NOT NULL,
  `rating` int(30) NOT NULL,
  `data` varchar(150) NOT NULL,
  `ranking` int(30) NOT NULL,
  `player_data1` varchar(60) NOT NULL,
  `player_data2` varchar(60) NOT NULL,
  `player_data3` varchar(60) NOT NULL,
  `player_data4` varchar(60) NOT NULL,
  `player_data5` varchar(60) NOT NULL,
  `player_data6` varchar(60) NOT NULL,
  `player_data7` varchar(60) NOT NULL,
  `player_data8` varchar(60) NOT NULL,
  `player_data9` varchar(60) NOT NULL,
  `player_data10` varchar(60) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

-- ----------------------------
-- Table structure for auctions
-- ----------------------------
DROP TABLE IF EXISTS `auctions`;
CREATE TABLE `auctions` (
  `auctionId` int(11) unsigned NOT NULL AUTO_INCREMENT,
  `auctionhouse` int(11) unsigned DEFAULT NULL,
  `item` bigint(10) unsigned DEFAULT NULL,
  `owner` int(11) unsigned DEFAULT NULL,
  `buyout` int(11) unsigned DEFAULT NULL,
  `time` int(11) unsigned DEFAULT NULL,
  `bidder` int(11) unsigned DEFAULT NULL,
  `bid` int(11) unsigned DEFAULT NULL,
  `deposit` int(11) unsigned DEFAULT NULL,
  PRIMARY KEY (`auctionId`)
) ENGINE=MyISAM AUTO_INCREMENT=11 DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

-- ----------------------------
-- Table structure for banned_names
-- ----------------------------
DROP TABLE IF EXISTS `banned_names`;
CREATE TABLE `banned_names` (
  `name` varchar(30) NOT NULL
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

-- ----------------------------
-- Table structure for banned_phrases
-- ----------------------------
DROP TABLE IF EXISTS `banned_phrases`;
CREATE TABLE `banned_phrases` (
  `phrase` varchar(250) NOT NULL,
  PRIMARY KEY (`phrase`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

-- ----------------------------
-- Table structure for characters
-- ----------------------------
DROP TABLE IF EXISTS `characters`;
CREATE TABLE `characters` (
  `guid` int(10) unsigned NOT NULL DEFAULT '0',
  `acct` int(10) unsigned NOT NULL DEFAULT '0',
  `name` varchar(21) COLLATE utf8_unicode_ci NOT NULL,
  `race` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `class` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `gender` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `custom_faction` int(30) NOT NULL DEFAULT '0',
  `level` int(10) unsigned NOT NULL DEFAULT '0',
  `xp` int(10) unsigned NOT NULL DEFAULT '0',
  `exploration_data` longtext COLLATE utf8_unicode_ci NOT NULL,
  `skills` longtext COLLATE utf8_unicode_ci NOT NULL,
  `watched_faction_index` int(10) unsigned NOT NULL DEFAULT '0',
  `selected_pvp_title` int(10) unsigned NOT NULL DEFAULT '0',
  `available_pvp_titles` int(10) unsigned NOT NULL DEFAULT '0',
  `gold` int(10) unsigned NOT NULL DEFAULT '0',
  `ammo_id` int(10) unsigned NOT NULL DEFAULT '0',
  `available_prof_points` int(10) unsigned NOT NULL DEFAULT '0',
  `available_talent_points` int(10) unsigned NOT NULL DEFAULT '0',
  `current_hp` int(10) unsigned NOT NULL DEFAULT '0',
  `current_power` int(10) unsigned NOT NULL DEFAULT '0',
  `pvprank` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `bytes` int(10) unsigned NOT NULL DEFAULT '0',
  `bytes2` int(10) unsigned NOT NULL DEFAULT '0',
  `player_flags` int(10) unsigned NOT NULL DEFAULT '0',
  `player_bytes` int(10) unsigned NOT NULL DEFAULT '0',
  `positionX` float NOT NULL DEFAULT '0',
  `positionY` float NOT NULL DEFAULT '0',
  `positionZ` float NOT NULL DEFAULT '0',
  `orientation` float NOT NULL DEFAULT '0',
  `mapId` int(10) unsigned NOT NULL DEFAULT '0',
  `zoneId` int(10) unsigned NOT NULL DEFAULT '0',
  `taximask` longtext COLLATE utf8_unicode_ci NOT NULL,
  `banned` int(10) unsigned NOT NULL DEFAULT '0',
  `banReason` varchar(50) COLLATE utf8_unicode_ci NOT NULL,
  `timestamp` int(11) NOT NULL,
  `online` int(11) DEFAULT NULL,
  `bindpositionX` float NOT NULL DEFAULT '0',
  `bindpositionY` float NOT NULL DEFAULT '0',
  `bindpositionZ` float NOT NULL DEFAULT '0',
  `bindmapId` int(10) unsigned NOT NULL DEFAULT '0',
  `bindzoneId` int(10) unsigned NOT NULL DEFAULT '0',
  `isResting` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `restState` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `restTime` int(10) unsigned NOT NULL DEFAULT '0',
  `playedtime` longtext COLLATE utf8_unicode_ci NOT NULL,
  `deathstate` int(10) unsigned NOT NULL DEFAULT '0',
  `TalentResetTimes` int(10) unsigned NOT NULL DEFAULT '0',
  `first_login` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `forced_rename_pending` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `publicNote` varchar(32) COLLATE utf8_unicode_ci NOT NULL,
  `officerNote` varchar(32) COLLATE utf8_unicode_ci NOT NULL,
  `guildid` int(10) unsigned NOT NULL DEFAULT '0',
  `guildRank` int(10) unsigned NOT NULL DEFAULT '0',
  `arenaPoints` int(10) NOT NULL,
  `totalstableslots` int(10) unsigned NOT NULL DEFAULT '0',
  `instance_id` int(10) unsigned NOT NULL DEFAULT '0',
  `entrypointmap` int(10) unsigned NOT NULL DEFAULT '0',
  `entrypointx` float NOT NULL DEFAULT '0',
  `entrypointy` float NOT NULL DEFAULT '0',
  `entrypointz` float NOT NULL DEFAULT '0',
  `entrypointo` float NOT NULL DEFAULT '0',
  `entrypointinstance` int(10) unsigned NOT NULL DEFAULT '0',
  `taxi_path` int(10) unsigned NOT NULL DEFAULT '0',
  `taxi_lastnode` int(10) unsigned NOT NULL DEFAULT '0',
  `taxi_mountid` int(10) unsigned NOT NULL DEFAULT '0',
  `transporter` int(10) unsigned NOT NULL DEFAULT '0',
  `transporter_xdiff` float NOT NULL DEFAULT '0',
  `transporter_ydiff` float NOT NULL DEFAULT '0',
  `transporter_zdiff` float NOT NULL DEFAULT '0',
  `spells` longtext COLLATE utf8_unicode_ci NOT NULL,
  `deleted_spells` longtext COLLATE utf8_unicode_ci NOT NULL,
  `reputation` longtext COLLATE utf8_unicode_ci NOT NULL,
  `actions` longtext COLLATE utf8_unicode_ci NOT NULL,
  `auras` longtext COLLATE utf8_unicode_ci NOT NULL,
  `finished_quests` longtext COLLATE utf8_unicode_ci NOT NULL,
  `honorPointsToAdd` int(10) NOT NULL,
  `killsToday` int(10) unsigned NOT NULL DEFAULT '0',
  `killsYesterday` int(10) unsigned NOT NULL DEFAULT '0',
  `killsLifeTime` int(10) unsigned NOT NULL DEFAULT '0',
  `honorToday` int(10) unsigned NOT NULL DEFAULT '0',
  `honorYesterday` int(10) unsigned NOT NULL DEFAULT '0',
  `honorPoints` int(10) unsigned NOT NULL DEFAULT '0',
  `difficulty` int(10) unsigned NOT NULL DEFAULT '0',
  UNIQUE KEY `guid` (`guid`),
  KEY `acct` (`acct`),
  KEY `guildid` (`guildid`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

-- ----------------------------
-- Table structure for charters
-- ----------------------------
DROP TABLE IF EXISTS `charters`;
CREATE TABLE `charters` (
  `charterId` int(30) NOT NULL,
  `charterType` int(30) NOT NULL DEFAULT '0',
  `leaderGuid` int(20) unsigned NOT NULL DEFAULT '0',
  `guildName` varchar(32) NOT NULL DEFAULT '',
  `itemGuid` bigint(40) unsigned NOT NULL DEFAULT '0',
  `signer1` int(10) unsigned NOT NULL DEFAULT '0',
  `signer2` int(10) unsigned NOT NULL DEFAULT '0',
  `signer3` int(10) unsigned NOT NULL DEFAULT '0',
  `signer4` int(10) unsigned NOT NULL DEFAULT '0',
  `signer5` int(10) unsigned NOT NULL DEFAULT '0',
  `signer6` int(10) unsigned NOT NULL DEFAULT '0',
  `signer7` int(10) unsigned NOT NULL DEFAULT '0',
  `signer8` int(10) unsigned NOT NULL DEFAULT '0',
  `signer9` int(10) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`charterId`),
  UNIQUE KEY `leaderGuid` (`leaderGuid`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

-- ----------------------------
-- Table structure for corpses
-- ----------------------------
DROP TABLE IF EXISTS `corpses`;
CREATE TABLE `corpses` (
  `guid` int(10) unsigned NOT NULL DEFAULT '0',
  `positionX` float NOT NULL DEFAULT '0',
  `positionY` float NOT NULL DEFAULT '0',
  `positionZ` float NOT NULL DEFAULT '0',
  `orientation` float NOT NULL DEFAULT '0',
  `zoneId` int(10) unsigned NOT NULL DEFAULT '38',
  `mapId` int(10) unsigned NOT NULL DEFAULT '0',
  `instanceId` int(10) unsigned NOT NULL DEFAULT '0',
  `data` longtext COLLATE utf8_unicode_ci NOT NULL,
  PRIMARY KEY (`guid`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

-- ----------------------------
-- Table structure for gm_tickets
-- ----------------------------
DROP TABLE IF EXISTS `gm_tickets`;
CREATE TABLE `gm_tickets` (
  `guid` int(10) unsigned NOT NULL DEFAULT '0',
  `name` varchar(200) COLLATE utf8_unicode_ci NOT NULL,
  `level` int(10) unsigned NOT NULL DEFAULT '0',
  `type` int(10) unsigned NOT NULL DEFAULT '0',
  `posX` float NOT NULL DEFAULT '0',
  `posY` float NOT NULL DEFAULT '0',
  `posZ` float NOT NULL DEFAULT '0',
  `message` text COLLATE utf8_unicode_ci NOT NULL,
  `timestamp` int(10) unsigned DEFAULT NULL,
  PRIMARY KEY (`guid`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

-- ----------------------------
-- Table structure for groups
-- ----------------------------
DROP TABLE IF EXISTS `groups`;
CREATE TABLE `groups` (
  `group_id` int(30) NOT NULL,
  `group_type` tinyint(2) NOT NULL,
  `subgroup_count` tinyint(2) NOT NULL,
  `loot_method` tinyint(2) NOT NULL,
  `loot_threshold` tinyint(2) NOT NULL,
  `group1member1` int(50) NOT NULL,
  `group1member2` int(50) NOT NULL,
  `group1member3` int(50) NOT NULL,
  `group1member4` int(50) NOT NULL,
  `group1member5` int(50) NOT NULL,
  `group2member1` int(50) NOT NULL,
  `group2member2` int(50) NOT NULL,
  `group2member3` int(50) NOT NULL,
  `group2member4` int(50) NOT NULL,
  `group2member5` int(50) NOT NULL,
  `group3member1` int(50) NOT NULL,
  `group3member2` int(50) NOT NULL,
  `group3member3` int(50) NOT NULL,
  `group3member4` int(50) NOT NULL,
  `group3member5` int(50) NOT NULL,
  `group4member1` int(50) NOT NULL,
  `group4member2` int(50) NOT NULL,
  `group4member3` int(50) NOT NULL,
  `group4member4` int(50) NOT NULL,
  `group4member5` int(50) NOT NULL,
  `group5member1` int(50) NOT NULL,
  `group5member2` int(50) NOT NULL,
  `group5member3` int(50) NOT NULL,
  `group5member4` int(50) NOT NULL,
  `group5member5` int(50) NOT NULL,
  `group6member1` int(50) NOT NULL,
  `group6member2` int(50) NOT NULL,
  `group6member3` int(50) NOT NULL,
  `group6member4` int(50) NOT NULL,
  `group6member5` int(50) NOT NULL,
  `group7member1` int(50) NOT NULL,
  `group7member2` int(50) NOT NULL,
  `group7member3` int(50) NOT NULL,
  `group7member4` int(50) NOT NULL,
  `group7member5` int(50) NOT NULL,
  `group8member1` int(50) NOT NULL,
  `group8member2` int(50) NOT NULL,
  `group8member3` int(50) NOT NULL,
  `group8member4` int(50) NOT NULL,
  `group8member5` int(50) NOT NULL,
  `timestamp` int(30) NOT NULL,
  PRIMARY KEY (`group_id`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

-- ----------------------------
-- Table structure for guild_ranks
-- ----------------------------
DROP TABLE IF EXISTS `guild_ranks`;
CREATE TABLE `guild_ranks` (
  `guildId` int(10) unsigned NOT NULL DEFAULT '0',
  `rankId` int(10) unsigned NOT NULL DEFAULT '0',
  `rankName` varchar(255) COLLATE utf8_unicode_ci NOT NULL,
  `rankRights` int(10) unsigned NOT NULL DEFAULT '0'
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

-- ----------------------------
-- Table structure for guilds
-- ----------------------------
DROP TABLE IF EXISTS `guilds`;
CREATE TABLE `guilds` (
  `guildId` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `guildName` varchar(32) COLLATE utf8_unicode_ci NOT NULL,
  `leaderGuid` bigint(20) unsigned NOT NULL DEFAULT '0',
  `emblemStyle` int(10) unsigned NOT NULL DEFAULT '0',
  `emblemColor` int(10) unsigned NOT NULL DEFAULT '0',
  `borderStyle` int(10) unsigned NOT NULL DEFAULT '0',
  `borderColor` int(10) unsigned NOT NULL DEFAULT '0',
  `backgroundColor` int(10) unsigned NOT NULL DEFAULT '0',
  `guildInfo` varchar(100) COLLATE utf8_unicode_ci NOT NULL,
  `motd` varchar(100) COLLATE utf8_unicode_ci NOT NULL,
  `createdate` datetime NOT NULL DEFAULT '0000-00-00 00:00:00',
  PRIMARY KEY (`guildId`),
  UNIQUE KEY `guildId` (`guildId`)
) ENGINE=MyISAM AUTO_INCREMENT=11 DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

-- ----------------------------
-- Table structure for instances
-- ----------------------------
DROP TABLE IF EXISTS `instances`;
CREATE TABLE `instances` (
  `instanceid` int(10) unsigned NOT NULL DEFAULT '0',
  `mapid` int(10) unsigned NOT NULL DEFAULT '0',
  `npc_data` longtext COLLATE utf8_unicode_ci NOT NULL,
  `player_data` longtext COLLATE utf8_unicode_ci NOT NULL,
  `creation` int(10) unsigned NOT NULL DEFAULT '0',
  `expire` int(10) unsigned NOT NULL DEFAULT '0',
  `difficulty` int(10) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`instanceid`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

-- ----------------------------
-- Table structure for ipbans
-- ----------------------------
DROP TABLE IF EXISTS `ipbans`;
CREATE TABLE `ipbans` (
  `ip` varchar(15) COLLATE utf8_unicode_ci NOT NULL,
  `expire` int(16) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`ip`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

-- ----------------------------
-- Table structure for mailbox
-- ----------------------------
DROP TABLE IF EXISTS `mailbox`;
CREATE TABLE `mailbox` (
  `message_id` int(10) unsigned NOT NULL DEFAULT '0',
  `message_type` int(10) unsigned NOT NULL DEFAULT '0',
  `player_guid` bigint(20) unsigned NOT NULL DEFAULT '0',
  `sender_guid` bigint(20) unsigned NOT NULL DEFAULT '0',
  `subject` varchar(255) COLLATE utf8_unicode_ci NOT NULL,
  `body` longtext COLLATE utf8_unicode_ci NOT NULL,
  `money` int(10) unsigned NOT NULL DEFAULT '0',
  `attached_item_guid` bigint(20) unsigned NOT NULL DEFAULT '0',
  `cod` int(10) unsigned NOT NULL DEFAULT '0',
  `stationary` int(10) unsigned NOT NULL DEFAULT '0',
  `expiry_time` int(10) unsigned NOT NULL DEFAULT '0',
  `delivery_time` int(10) unsigned NOT NULL DEFAULT '0',
  `copy_made` int(10) unsigned NOT NULL DEFAULT '0',
  `read_flag` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `deleted_flag` tinyint(3) unsigned NOT NULL DEFAULT '0'
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

-- ----------------------------
-- Table structure for playercooldownitems
-- ----------------------------
DROP TABLE IF EXISTS `playercooldownitems`;
CREATE TABLE `playercooldownitems` (
  `OwnerGuid` int(10) unsigned NOT NULL DEFAULT '0',
  `ItemEntry` int(10) unsigned NOT NULL DEFAULT '0',
  `SpellID` int(10) unsigned NOT NULL DEFAULT '0',
  `SpellCategory` int(10) unsigned NOT NULL DEFAULT '0',
  `CooldownTimeStamp` int(10) unsigned NOT NULL DEFAULT '0',
  `Cooldown` int(10) unsigned NOT NULL DEFAULT '0'
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

-- ----------------------------
-- Table structure for playercooldownsecurity
-- ----------------------------
DROP TABLE IF EXISTS `playercooldownsecurity`;
CREATE TABLE `playercooldownsecurity` (
  `OwnerGuid` int(10) unsigned NOT NULL DEFAULT '0',
  `SpellID` int(10) unsigned NOT NULL DEFAULT '0',
  `TimeStamp` int(10) unsigned NOT NULL DEFAULT '0'
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

-- ----------------------------
-- Table structure for playeritems
-- ----------------------------
DROP TABLE IF EXISTS `playeritems`;
CREATE TABLE `playeritems` (
  `ownerguid` int(10) unsigned NOT NULL DEFAULT '0',
  `guid` bigint(20) unsigned NOT NULL DEFAULT '0',
  `entry` int(10) unsigned NOT NULL DEFAULT '0',
  `creator` int(10) unsigned NOT NULL DEFAULT '0',
  `count` int(10) unsigned NOT NULL DEFAULT '0',
  `charges` int(10) unsigned NOT NULL DEFAULT '0',
  `flags` int(10) unsigned NOT NULL DEFAULT '0',
  `randomprop` int(10) unsigned NOT NULL DEFAULT '0',
  `randomsuffix` int(30) NOT NULL,
  `itemtext` int(10) unsigned NOT NULL DEFAULT '0',
  `durability` int(10) unsigned NOT NULL DEFAULT '0',
  `containerslot` int(11) DEFAULT '-1' COMMENT 'couldnt find this being used in source',
  `slot` tinyint(4) NOT NULL DEFAULT '0',
  `enchantments` longtext COLLATE utf8_unicode_ci NOT NULL,
  PRIMARY KEY (`guid`),
  UNIQUE KEY `guid` (`guid`),
  KEY `ownerguid` (`ownerguid`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

-- ----------------------------
-- Table structure for playerpets
-- ----------------------------
DROP TABLE IF EXISTS `playerpets`;
CREATE TABLE `playerpets` (
  `ownerguid` bigint(20) NOT NULL DEFAULT '0',
  `petnumber` int(11) NOT NULL DEFAULT '0',
  `name` varchar(21) NOT NULL DEFAULT '',
  `entryid` bigint(20) NOT NULL DEFAULT '0',
  `data` longtext NOT NULL,
  `xp` int(11) NOT NULL DEFAULT '0',
  `active` tinyint(1) NOT NULL DEFAULT '0',
  `level` int(11) NOT NULL DEFAULT '0',
  `happiness` int(11) NOT NULL DEFAULT '0',
  `actionbar` varchar(200) NOT NULL DEFAULT '',
  `happinessupdate` int(11) NOT NULL DEFAULT '0',
  `summon` int(11) NOT NULL DEFAULT '0',
  `loyaltypts` int(11) NOT NULL DEFAULT '0',
  `loyaltyupdate` int(11) NOT NULL DEFAULT '0'
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

-- ----------------------------
-- Table structure for playerpetspells
-- ----------------------------
DROP TABLE IF EXISTS `playerpetspells`;
CREATE TABLE `playerpetspells` (
  `ownerguid` bigint(20) unsigned NOT NULL DEFAULT '0',
  `petnumber` int(10) unsigned NOT NULL DEFAULT '0',
  `spellid` int(10) unsigned NOT NULL DEFAULT '0',
  `flags` smallint(5) unsigned NOT NULL DEFAULT '0'
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

-- ----------------------------
-- Table structure for playersummonspells
-- ----------------------------
DROP TABLE IF EXISTS `playersummonspells`;
CREATE TABLE `playersummonspells` (
  `ownerguid` bigint(20) unsigned NOT NULL DEFAULT '0',
  `entryid` int(10) unsigned NOT NULL DEFAULT '0',
  `spellid` int(10) unsigned NOT NULL DEFAULT '0'
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

-- ----------------------------
-- Table structure for questlog
-- ----------------------------
DROP TABLE IF EXISTS `questlog`;
CREATE TABLE `questlog` (
  `index` bigint(20) unsigned NOT NULL AUTO_INCREMENT,
  `player_guid` bigint(20) unsigned NOT NULL DEFAULT '0',
  `quest_id` int(10) unsigned NOT NULL DEFAULT '0',
  `slot` int(10) unsigned NOT NULL DEFAULT '0',
  `time_left` int(10) unsigned NOT NULL DEFAULT '0',
  `explored_area1` int(10) unsigned NOT NULL DEFAULT '0',
  `explored_area2` int(10) unsigned NOT NULL DEFAULT '0',
  `explored_area3` int(10) unsigned NOT NULL DEFAULT '0',
  `explored_area4` int(10) unsigned NOT NULL DEFAULT '0',
  `mob_kill1` int(10) unsigned NOT NULL DEFAULT '0',
  `mob_kill2` int(10) unsigned NOT NULL DEFAULT '0',
  `mob_kill3` int(10) unsigned NOT NULL DEFAULT '0',
  `mob_kill4` int(10) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`index`),
  KEY `index` (`index`)
) ENGINE=MyISAM AUTO_INCREMENT=2986 DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

-- ----------------------------
-- Table structure for server_settings
-- ----------------------------
DROP TABLE IF EXISTS `server_settings`;
CREATE TABLE `server_settings` (
  `setting_id` varchar(200) NOT NULL,
  `setting_value` int(50) NOT NULL,
  PRIMARY KEY (`setting_id`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

-- ----------------------------
-- Table structure for social
-- ----------------------------
DROP TABLE IF EXISTS `social`;
CREATE TABLE `social` (
  `guid` bigint(20) unsigned NOT NULL DEFAULT '0',
  `socialguid` bigint(20) unsigned NOT NULL DEFAULT '0',
  `flags` varchar(21) COLLATE utf8_unicode_ci NOT NULL,
  `noticed` tinyint(3) unsigned NOT NULL DEFAULT '1',
  PRIMARY KEY (`guid`,`socialguid`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

-- ----------------------------
-- Table structure for tutorials
-- ----------------------------
DROP TABLE IF EXISTS `tutorials`;
CREATE TABLE `tutorials` (
  `playerId` bigint(20) unsigned NOT NULL DEFAULT '0',
  `tut0` int(10) unsigned NOT NULL DEFAULT '0',
  `tut1` int(10) unsigned NOT NULL DEFAULT '0',
  `tut2` int(10) unsigned NOT NULL DEFAULT '0',
  `tut3` int(10) unsigned NOT NULL DEFAULT '0',
  `tut4` int(10) unsigned NOT NULL DEFAULT '0',
  `tut5` int(10) unsigned NOT NULL DEFAULT '0',
  `tut6` int(10) unsigned NOT NULL DEFAULT '0',
  `tut7` int(10) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`playerId`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;
