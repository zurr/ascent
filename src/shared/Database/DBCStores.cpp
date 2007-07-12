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

#include "DBCStores.h"
#include "DataStore.h"

// You need a line like this for every DBC store. If you use createDBCStore (no Indexed), the lines will be ordered the way they are in the file
// SpellEntry is the file struct entry (for Spell.dbc here).
implementIndexedDBCStore(ItemSetStore,ItemSetEntry)
implementIndexedDBCStore(LockStore,Lock)
implementIndexedDBCStore(SpellStore,SpellEntry)
implementIndexedDBCStore(DurationStore,SpellDuration)
implementIndexedDBCStore(RangeStore,SpellRange)
implementIndexedDBCStore(EmoteStore,emoteentry)
implementIndexedDBCStore(RadiusStore,SpellRadius)
implementIndexedDBCStore(CastTimeStore,SpellCastTime)
implementIndexedDBCStore(AreaStore,AreaTable)
implementIndexedDBCStore(FactionTmpStore,FactionTemplateDBC)
implementIndexedDBCStore(RandomPropStore,RandomProps)
implementIndexedDBCStore(FactionStore,FactionDBC)
implementIndexedDBCStore(EnchantStore,EnchantEntry)
implementIndexedDBCStore(WorldMapAreaStore,WorldMapArea)
implementDBCStore(WorldMapOverlayStore,WorldMapOverlay)
implementDBCStore(SkillStore,skilllinespell)
implementIndexedDBCStore(SkillLineStore,skilllineentry)
implementDBCStore(TaxiPathStore,DBCTaxiPath)
implementDBCStore(TaxiNodeStore,DBCTaxiNode)
implementDBCStore(TaxiPathNodeStore,DBCTaxiPathNode)
implementDBCStore(WorldSafeLocsStore,GraveyardEntry)
implementIndexedDBCStore(TransportAnimationStore,TransportAnimation);
implementDBCStore(NameGenStore, NameGenEntry);
implementIndexedDBCStore(AuctionHouseStore,AuctionHouseDBC);
implementDBCStore(TalentStore, TalentEntry);
implementIndexedDBCStore(CreatureSpellDataStore, CreatureSpellDataEntry);
implementIndexedDBCStore(CreatureFamilyStore, CreatureFamilyEntry);
implementIndexedDBCStore(CharClassStore, CharClassEntry);
implementIndexedDBCStore(CharRaceStore, CharRaceEntry);
implementIndexedDBCStore(MapStore, MapEntry);
implementIndexedDBCStore(ItemExtendedCostStore,ItemExtendedCostEntry);
implementIndexedDBCStore(GemPropertiesStore,GemPropertyEntry);

