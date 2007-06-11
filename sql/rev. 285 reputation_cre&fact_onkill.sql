ALTER TABLE `reputation_creature_onkill` ADD `faction1_change` int(30) NOT NULL AFTER `change_deltamax`;
ALTER TABLE `reputation_creature_onkill` ADD `change_deltamin1` int(30) NOT NULL AFTER `faction1_change`;
ALTER TABLE `reputation_creature_onkill` ADD `change_deltamax1` int(30) NOT NULL AFTER `change_deltamin1`;
ALTER TABLE `reputation_faction_onkill` ADD `faction1_change` int(30) NOT NULL AFTER `change_deltamax`;
ALTER TABLE `reputation_faction_onkill` ADD `change_deltamin1` int(30) NOT NULL AFTER `faction1_change`;
ALTER TABLE `reputation_faction_onkill` ADD `change_deltamax1` int(30) NOT NULL AFTER `change_deltamin1`;