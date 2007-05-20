SET FOREIGN_KEY_CHECKS=0;
-- ----------------------------
-- Table structure for worldmap_info
-- ----------------------------
CREATE TABLE `worldmap_info` (
  `mapid` int(10) NOT NULL default '0',
  `screenid` int(10) default '0',
  `type` int(10) default '0',
  `maxplayers` int(10) default '0',
  `minlevel` int(11) unsigned zerofill default '00000000001',
  `repopx` float default '0',
  `repopy` float default '0',
  `repopz` float default '0',
  `repopmapid` int(5) unsigned zerofill default '00000',
  `area_name` varchar(100) default '0',
  `flags` int(11) unsigned NOT NULL default '0',
  `cooldown` int(15) NOT NULL default '0',
  PRIMARY KEY  (`mapid`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

-- ----------------------------
-- Records 
-- ----------------------------
INSERT INTO `worldmap_info` VALUES ('0', '0', '0', '0', '00000000001', '0', '0', '0', '00000', 'Estern Kingdoms Continent', '1', '0');
INSERT INTO `worldmap_info` VALUES ('1', '1', '0', '0', '00000000001', '0', '0', '0', '00001', 'Kalimdor Continent', '1', '0');
INSERT INTO `worldmap_info` VALUES ('30', '30', '3', '0', '00000000001', '0', '0', '0', '00000', 'Alterac Valley', '3', '0');
INSERT INTO `worldmap_info` VALUES ('33', '204', '2', '5', '00000000001', '-229.49', '1576.35', '78.8909', '00000', 'Shadowfang Keep', '3', '0');
INSERT INTO `worldmap_info` VALUES ('34', '34', '2', '5', '00000000001', '-8769.86', '842.181', '89.8765', '00000', 'Stormwind Stockades', '3', '0');
INSERT INTO `worldmap_info` VALUES ('35', '194', '2', '5', '00000000001', '0', '0', '0', '00000', 'Stormwind Vault - UNUSED', '0', '0');
INSERT INTO `worldmap_info` VALUES ('36', '36', '2', '5', '00000000008', '-11208.4', '1673.85', '24.6245', '00000', 'DeadMines', '3', '0');
INSERT INTO `worldmap_info` VALUES ('43', '143', '2', '5', '00000000008', '-753.596', '-2212.78', '21.5403', '00001', 'Wailing Caverns', '3', '0');
INSERT INTO `worldmap_info` VALUES ('47', '188', '2', '5', '00000000001', '-4456.7', '-1655.99', '86.1095', '00001', 'Razorfen Kraul', '3', '0');
INSERT INTO `worldmap_info` VALUES ('48', '196', '2', '5', '00000000008', '4252.37', '756.974', '-23.0632', '00001', 'Blackfathom Deeps', '3', '0');
INSERT INTO `worldmap_info` VALUES ('70', '144', '2', '5', '00000000001', '-6606.48', '-3762.19', '266.91', '00000', 'Uldaman', '3', '0');
INSERT INTO `worldmap_info` VALUES ('90', '193', '2', '5', '00000000001', '-5081.82', '719.549', '260.554', '00000', 'Gnomeregan', '3', '0');
INSERT INTO `worldmap_info` VALUES ('109', '109', '2', '5', '00000000001', '-10184.9', '-3992.84', '-109.194', '00000', 'Temple of Atal\'Hakkar', '3', '0');
INSERT INTO `worldmap_info` VALUES ('129', '145', '2', '5', '00000000001', '-4666.52', '-2536.82', '86.9671', '00001', 'Razorfen Downs', '3', '0');
INSERT INTO `worldmap_info` VALUES ('189', '42', '2', '5', '00000000008', '2870.97', '-759.845', '160.333', '00000', 'Scarlet Monastery', '3', '0');
INSERT INTO `worldmap_info` VALUES ('209', '146', '2', '5', '00000000001', '-6773.49', '-2889.77', '15.1063', '00001', 'Zul\'Farrak', '3', '0');
INSERT INTO `worldmap_info` VALUES ('229', '189', '2', '10', '00000000055', '-7518.19', '-1239.13', '287.243', '00000', 'Blackrock Spire', '3', '0');
INSERT INTO `worldmap_info` VALUES ('230', '103', '2', '10', '00000000055', '-7176.63', '-937.667', '170.206', '00000', 'Blackrock Depths', '3', '0');
INSERT INTO `worldmap_info` VALUES ('249', '61', '1', '40', '00000000055', '-4768.61', '-3752.11', '53.4317', '00001', 'Onyxia\'s Lair', '3', '432000');
INSERT INTO `worldmap_info` VALUES ('269', '269', '1', '5', '00000000064', '-8770.08', '-4171.7', '-210.158', '00001', 'Opening of the Dark Portal', '9', '0');
INSERT INTO `worldmap_info` VALUES ('289', '102', '2', '5', '00000000055', '1282.05', '-2548.73', '85.3994', '00000', 'Scholomance', '3', '0');
INSERT INTO `worldmap_info` VALUES ('309', '161', '1', '20', '00000000055', '-11915.8', '-1236.43', '96.1502', '00000', 'Zul\'Gurub', '3', '259200');
INSERT INTO `worldmap_info` VALUES ('329', '101', '2', '5', '00000000055', '3237.46', '-4060.6', '112.01', '00000', 'Stratholme', '3', '0');
INSERT INTO `worldmap_info` VALUES ('349', '81', '2', '5', '00000000001', '-1431.07', '2970.64', '99.8095', '00000', 'Maraudon', '3', '0');
INSERT INTO `worldmap_info` VALUES ('369', '369', '0', '0', '00000000001', '0', '0', '0', '00000', 'Deeprun Tram', '1', '0');
INSERT INTO `worldmap_info` VALUES ('389', '195', '2', '5', '00000000008', '1818.4', '-4427.26', '-10.4478', '00001', 'Ragefire Chasm', '3', '0');
INSERT INTO `worldmap_info` VALUES ('409', '192', '1', '40', '00000000055', '-7510.31', '-1041.63', '180.912', '00000', 'Molten Core', '3', '604800');
INSERT INTO `worldmap_info` VALUES ('429', '82', '2', '5', '00000000001', '-4278.76', '1332.49', '161.214', '00001', 'Dire Maul', '3', '0');
INSERT INTO `worldmap_info` VALUES ('449', '181', '0', '0', '00000000001', '-8761.85', '848.557', '87.8052', '00000', 'Alliance Military Barracks', '1', '0');
INSERT INTO `worldmap_info` VALUES ('450', '182', '0', '0', '00000000001', '1643.36', '-4233.6', '56.1557', '00001', 'Horde Military Barracks', '1', '0');
INSERT INTO `worldmap_info` VALUES ('469', '141', '1', '40', '00000000055', '-7655.63', '-1221.93', '287.798', '00000', 'Blackwing Lair', '3', '604800');
INSERT INTO `worldmap_info` VALUES ('509', '184', '1', '20', '00000000055', '-8134.79', '1525.9', '6.1904', '00001', 'Ahn\'Qiraj Ruins', '0', '259200');
INSERT INTO `worldmap_info` VALUES ('530', '530', '0', '0', '00000000055', '0', '0', '0', '00530', 'Outlands', '9', '0');
INSERT INTO `worldmap_info` VALUES ('531', '531', '1', '40', '00000000055', '-8134.79', '1525.9', '6.1904', '00001', 'Ahn\'Qiraj Temple', '0', '259200');
INSERT INTO `worldmap_info` VALUES ('532', '532', '2', '10', '00000000068', '0', '0', '0', '00000', 'Karazahn', '3', '0');
INSERT INTO `worldmap_info` VALUES ('533', '0', '1', '40', '00000000055', '3237.46', '-4060.6', '112.01', '00000', 'Naxxramas', '3', '604800');
INSERT INTO `worldmap_info` VALUES ('534', '534', '2', '25', '00000000068', '-8178.25', '-4184.74', '-168.4', '00001', 'The Battle for Mount Hyjal', '3', '0');
INSERT INTO `worldmap_info` VALUES ('560', '560', '1', '5', '00000000064', '-8343.69', '-4057.43', '207.924', '00001', 'Caverns Of Time - Hillsbrad Past', '3', '0');
INSERT INTO `worldmap_info` VALUES ('553', '553', '2', '5', '00000000068', '3365.84', '1519.15', '179.541', '00530', 'The Botanica', '3', '0');
INSERT INTO `worldmap_info` VALUES ('545', '545', '2', '5', '00000000070', '816.87', '6935.02', '-80.5606', '00530', 'Steamvault', '3', '0');
