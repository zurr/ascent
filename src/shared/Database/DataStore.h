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

#ifndef __DATASTORE_H
#define __DATASTORE_H

#include "Common.h"
#include "Singleton.h"
#include "DBC.h"

template <class T>
class SERVER_DECL DataStore : public Singleton<DataStore<T> >
{
protected:
    DBC d;
public:
    DataStore(const char* filename)
    {
        d.Load(filename);
    }
    virtual ~DataStore() {}

    virtual T *LookupEntry(const uint32 row)
    {
        if(!d.IsLoaded() || (uint32)d.GetRows() < row) return NULL;
        return (T*)d.GetRow(row);
    }
    const char* LookupString(const uint32 offset)
    {
        if(!d.IsLoaded() || (uint32)d.GetDBSize() < offset) return NULL;
        return d.LookupString(offset);
    }
    uint32 GetNumRows()
    {
        return d.GetRows();
    }
};

template <class T>
class SERVER_DECL FastIndexedDataStore : public DataStore<T>
{
    T ** dataIndex;
    uint32 high;
    uint32 first;
public:
    FastIndexedDataStore(const char * filename) : DataStore<T>(filename)
    {
        std::map<uint32, void*> itemp;
        high = 0;
        first = 0;
        T * data;
        
        uint32 index;

        for(uint32 row=0;row<(uint32)DataStore<T>::d.GetRows();row++)
        {
            data = (T*)DataStore<T>::d.GetRow(row);
            index = *(int*)data;
            
            if(!first)
                first = index;

            itemp[index] = (void*)data;
            if(index > high)
                high = index;
        }

        ++high;

        // dump to index array
        dataIndex = new T*[high];
        memset(dataIndex, 0, sizeof(uint32) * high);
	std::map<uint32, void*>::iterator itr = itemp.begin();
        for(; itr != itemp.end(); ++itr)
            dataIndex[itr->first] = (T*)itr->second;
    }

    ~FastIndexedDataStore()
    {
        delete [] dataIndex;
    }

    T * LookupEntry(const uint32 row)
    {
        if(row >= high || dataIndex[row] == 0) return dataIndex[first];
        return dataIndex[row];
    }

    T *LookupRow(const uint32 row)
    {
        return (T*)DataStore<T>::d.GetRow(row);
    }
};

template <class T>
class SERVER_DECL IndexedDataStore : public DataStore<T>
{
protected:
    std::map<uint32,uint32> indexMap;
public:
    IndexedDataStore(const char* filename) : DataStore<T>(filename)
    {
        for(uint32 row=0;row<(uint32)DataStore<T>::d.GetRows();row++)
            indexMap[*(int*)DataStore<T>::d.GetRow(row)] = row;
    }
    ~IndexedDataStore() { indexMap.clear(); }

    virtual T *LookupEntry(const uint32 row)
    {
        if(!DataStore<T>::d.IsLoaded()) return NULL;
        return (T*)DataStore<T>::d.GetRow(indexMap[row]);
    }

    virtual T *LookupRow(const uint32 row)
    {
        if(!DataStore<T>::d.IsLoaded()) return NULL;
        return (T*)DataStore<T>::d.GetRow(row);
    }
};

#define defineDBCStore(name,structname) \
class SERVER_DECL name : public DataStore<structname> \
{ \
public: \
    name(const char* filename); \
    ~name(); \
}

#define implementDBCStore(name,structname) \
initialiseSingleton(name); \
initialiseSingleton(DataStore< structname >); \
name::name(const char* filename) : DataStore<structname>(filename) {} \
name::~name() {} \

#define defineIndexedDBCStore(name,structname) \
class SERVER_DECL name : public FastIndexedDataStore<structname> \
{ \
public: \
    name(const char* filename); \
    ~name(); \
}

#define implementIndexedDBCStore(name,structname) \
initialiseSingleton(name); \
initialiseSingleton(DataStore< structname >); \
name::name(const char* filename) : FastIndexedDataStore<structname>(filename) {} \
name::~name() {} 


#endif
