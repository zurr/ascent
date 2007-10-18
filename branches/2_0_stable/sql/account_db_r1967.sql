/*
MySQL Data Transfer
Source Host: localhost
Source Database: character
Target Host: localhost
Target Database: character
Date: 18/10/2007 4:01:49 PM
*/

SET FOREIGN_KEY_CHECKS=0;
-- ----------------------------
-- Table structure for accounts
-- ----------------------------
DROP TABLE IF EXISTS `accounts`;
CREATE TABLE `accounts` (
  `acct` bigint(20) unsigned NOT NULL AUTO_INCREMENT,
  `login` varchar(255) COLLATE utf8_unicode_ci NOT NULL DEFAULT '',
  `password` varchar(28) COLLATE utf8_unicode_ci NOT NULL DEFAULT '',
  `gm` varchar(27) COLLATE utf8_unicode_ci NOT NULL DEFAULT '',
  `banned` int(1) unsigned NOT NULL DEFAULT '0',
  `lastlogin` timestamp NOT NULL DEFAULT '0000-00-00 00:00:00',
  `lastip` varchar(15) COLLATE utf8_unicode_ci NOT NULL DEFAULT '',
  `email` varchar(50) COLLATE utf8_unicode_ci NOT NULL DEFAULT '',
  `flags` int(11) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`acct`,`login`),
  UNIQUE KEY `acct` (`acct`)
) ENGINE=MyISAM AUTO_INCREMENT=5 DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;
