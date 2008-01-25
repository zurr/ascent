CREATE TABLE IF NOT EXISTS `creature_timed_emotes` (
  `spawnid` int(10) unsigned NOT NULL DEFAULT '0' ,
  `rowid` int(10) unsigned NOT NULL DEFAULT '0' ,
  `type` tinyint(1) unsigned NOT NULL DEFAULT '1' ,
  `value` int(10) unsigned NOT NULL DEFAULT '0' ,
  `msg` text ,
  `msg_type` int(10) unsigned NOT NULL DEFAULT '0' ,
  `msg_lang` int(10) unsigned NOT NULL DEFAULT '0' ,
  `expire_after` int(10) unsigned NOT NULL DEFAULT '0' ,
  PRIMARY KEY (`spawnid`,`rowid`)
);
