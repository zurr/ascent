ALTER TABLE `arenateams` DROP INDEX `a`;
ALTER TABLE `auctions` DROP INDEX `a`;
ALTER TABLE `characters` DROP INDEX `a`;
ALTER TABLE `charters` DROP INDEX `a`;
ALTER TABLE `corpses` DROP INDEX `a`;
ALTER TABLE `gm_tickets` DROP INDEX `a`;
ALTER TABLE `groups` DROP INDEX `a`;
ALTER TABLE `guild_bankitems` DROP INDEX `a`, DROP INDEX `b`, DROP INDEX `c`;
ALTER TABLE `guild_banklogs` DROP INDEX `a`, DROP INDEX `b`;
ALTER TABLE `guild_data` ADD PRIMARY KEY(`guildid`,`playerid`), DROP INDEX `a`, DROP INDEX `b`;
ALTER TABLE `guild_banktabs` DROP INDEX `a`, DROP INDEX `b`;
ALTER TABLE `guild_logs` ADD PRIMARY KEY(`log_id`,`guildid`), DROP INDEX `a`, DROP INDEX `b`;
ALTER TABLE `guild_ranks` DROP INDEX `a`, DROP INDEX `b`;
ALTER TABLE `guilds` DROP INDEX `a`;
ALTER TABLE `instances` ADD PRIMARY KEY(`id`), DROP INDEX `a`;
ALTER TABLE `mailbox` DROP INDEX `a`;
ALTER TABLE `playercooldownitems` DROP INDEX `b`, DROP INDEX `c`, DROP INDEX `d`;
ALTER TABLE `playercooldownsecurity` DROP INDEX `b`, DROP INDEX `c`;
ALTER TABLE `playeritems` DROP INDEX `a`;
ALTER TABLE `playerpets` ADD PRIMARY KEY(`ownerguid`,`petnumber`), DROP INDEX `a`, DROP INDEX `b`;
ALTER TABLE `playerpetspells` ADD PRIMARY KEY(`ownerguid`,`petnumber`), DROP INDEX `a`, DROP INDEX `b`;
ALTER TABLE `playerpetspells` DROP PRIMARY KEY, ADD PRIMARY KEY(`ownerguid`,`petnumber`,`spellid`);
ALTER TABLE `playersummonspells` ADD PRIMARY KEY(`ownerguid`,`entryid`,`spellid`), DROP INDEX `a`, DROP INDEX `b`, ADD INDEX `a`(`ownerguid`);
ALTER TABLE `questlog` DROP INDEX `a`, DROP INDEX `d`;
ALTER TABLE `social` DROP INDEX `a`, DROP INDEX `b`;
ALTER TABLE `tutorials` DROP INDEX `a`;
