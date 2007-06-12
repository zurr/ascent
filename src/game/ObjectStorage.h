#ifndef OBJECT_STORAGE_H
#define OBJECT_STORAGE_H

extern SQLStorage<ItemPrototype, ArrayStorageContainer<ItemPrototype> >			ItemPrototypeStorage;
extern SQLStorage<CreatureInfo, HashMapStorageContainer<CreatureInfo> >			CreatureNameStorage;
extern SQLStorage<GameObjectInfo, HashMapStorageContainer<GameObjectInfo> >		GameObjectNameStorage;
extern SQLStorage<CreatureProto, HashMapStorageContainer<CreatureProto> >		CreatureProtoStorage;

#endif


