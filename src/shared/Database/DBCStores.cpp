/****************************************************************************
 *
 * General Object Type File
 * Copyright (c) 2007 Team Ascent
 *
 * This work is licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 3.0
 * License. To view a copy of this license, visit
 * http://creativecommons.org/licenses/by-nc-sa/3.0/ or send a letter to Creative Commons,
 * 543 Howard Street, 5th Floor, San Francisco, California, 94105, USA.
 *
 * EXCEPT TO THE EXTENT REQUIRED BY APPLICABLE LAW, IN NO EVENT WILL LICENSOR BE LIABLE TO YOU
 * ON ANY LEGAL THEORY FOR ANY SPECIAL, INCIDENTAL, CONSEQUENTIAL, PUNITIVE OR EXEMPLARY DAMAGES
 * ARISING OUT OF THIS LICENSE OR THE USE OF THE WORK, EVEN IF LICENSOR HAS BEEN ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGES.
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

