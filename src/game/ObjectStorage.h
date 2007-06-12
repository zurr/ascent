#ifndef OBJECT_STORAGE_H
#define OBJECT_STORAGE_H

extern const char * ItemPrototypeFormat;
extern const char * CreatureNameFormat;
extern const char * GameObjectNameFormat;
extern const char * CreatureProtoFormat;
extern const char * AreaTriggerFormat;
extern const char * ItemPageFormat;
extern const char * NpcTextFormat;
extern const char * QuestFormat;
extern const char * VendorFormat;

extern SQLStorage<ItemPrototype, ArrayStorageContainer<ItemPrototype> >			ItemPrototypeStorage;
extern SQLStorage<CreatureInfo, HashMapStorageContainer<CreatureInfo> >			CreatureNameStorage;
extern SQLStorage<GameObjectInfo, HashMapStorageContainer<GameObjectInfo> >		GameObjectNameStorage;
extern SQLStorage<CreatureProto, HashMapStorageContainer<CreatureProto> >		CreatureProtoStorage;

#endif