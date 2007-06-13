#ifndef STORAGE_H_
#define STORAGE_H_

/** Base iterator class, returned by MakeIterator() functions.
 */
template<class T>
class StorageContainerIterator
{
protected:
	/** Currently referenced object
	 */
	T * Pointer;
public:
	/** Returns the currently stored object
	 */
	inline T * Get() { return Pointer; }

	/** Sets the current object to P
	 */
	inline void Set(T * P) { Pointer = P; }

	/** Are we at the end of the storage container?
	 */
	inline bool AtEnd() { return (Pointer == 0); }

	/** Virtual function to increment to the next element
	 */
	virtual bool Inc() = 0;

	/** Virtual function to destroy the iterator
	 */
	virtual void Destruct() = 0;
};

template<class T>
class ArrayStorageContainer
{
public:
	/** This is where the magic happens :P
	 */
	T ** _array;

	/** Maximum possible entry 
	 */
	uint32 _max;

	/** Returns an iterator currently referencing the start of the container
	 */
	StorageContainerIterator<T> * MakeIterator();

	/** Creates the array with specified maximum
	 */
	void Setup(uint32 Max)
	{
		_array = new T*[Max];
		_max = Max;
		memset(_array, 0, sizeof(T*) * Max);
	}
	
	/** Sets up the array with a different maximum
	 */
	void Resetup(uint32 Max)
	{
		if(Max <= _max)
			return;		// no need to realloc

        T ** a = new T*[Max];
		memcpy(a, _array, sizeof(T*) * _max);
		delete [] _array;
		_array = a;
		_max = Max;
	}

	/** Frees the container array and all elements inside it
	 */
	~ArrayStorageContainer()
	{
		for(int i = 0; i < _max; ++i)
			if(_array[i] != 0)
				delete _array[i];

		delete [] _array;
	}

	/** Allocates entry Entry in the array and sets the pointer, and returns
	 * the allocated memory.
	 */
	T * AllocateEntry(uint32 Entry)
	{
		if(Entry > _max || _array[Entry] != 0)
			return reinterpret_cast<T*>(0);

		_array[Entry] = new T;
		return _array[Entry];
	}

	/** Deallocates the entry Entry in the array and sets the pointer to null.
	 */
	bool DeallocateEntry(uint32 Entry)
	{
		if(Entry > _max || _array[Entry] == 0)
			return false;

		delete _array[Entry];
		_array[Entry] = 0;
		return true;
	}

	/** Looks up entry Entry and returns the pointer if it is existant, otherwise null.
	 */
	T * LookupEntry(uint32 Entry)
	{
		if(Entry > _max)
			return reinterpret_cast<T*>(0);
		else
			return _array[Entry];
	}

	/** Sets the pointer to entry Entry to Pointer, and if it already exists frees the existing
	 * element.
	 */
	bool SetEntry(uint32 Entry, T * Pointer)
	{
		if(Entry > _max)
			return false;

		if(_array[Entry] != 0)
			delete _array[Entry];

		_array[Entry] = Pointer;
		return true;
	}

	/** Returns the current pointer if it exists, otherwise allocates it.
	 */
	T * LookupEntryAllocate(uint32 Entry)
	{
		T * ret = LookupEntry(Entry);
		if(!ret)
			ret = AllocateEntry(Entry);
		return ret;
	}
};

template<class T>
class HashMapStorageContainer
{
public:
	typename HM_NAMESPACE::hash_map<uint32, T*> _map;

	/** Returns an iterator currently referencing the start of the container
	 */
	StorageContainerIterator<T> * MakeIterator();

	/** Frees the container array and all elements inside it
	 */
	~HashMapStorageContainer()
	{
		for(typename HM_NAMESPACE::hash_map<uint32, T*>::iterator itr = _map.begin(); itr != _map.end(); ++itr)
			delete itr->second;
	}

	/** Creates the array with specified maximum
	 */
	void Setup(uint32 Max)
	{

	}

	void Resetup(uint32 Max)
	{

	}

	/** Allocates entry Entry in the array and sets the pointer, and returns
	 * the allocated memory.
	 */
	T * AllocateEntry(uint32 Entry)
	{
		if(_map.find(Entry) != _map.end())
			return reinterpret_cast<T*>(0);
		T * n = new T;
		_map.insert( make_pair( Entry, n ) );
		return n;
	}

	/** Deallocates the entry Entry in the array and sets the pointer to null.
	 */
	bool DeallocateEntry(uint32 Entry)
	{
		typename HM_NAMESPACE::hash_map<uint32, T*>::iterator itr = _map.find(Entry);
		if(itr == _map.end())
			return false;

		delete itr->second;
		_map.erase(itr);
		return true;
	}


	T * LookupEntry(uint32 Entry)
	{
		typename HM_NAMESPACE::hash_map<uint32, T*>::iterator itr = _map.find(Entry);
		if(itr == _map.end())
			return reinterpret_cast<T*>(0);
		return itr->second;
	}

	/** Sets the pointer to entry Entry to Pointer, and if it already exists frees the existing
	 * element.
	 */
	bool SetEntry(uint32 Entry, T * Pointer)
	{
		typename HM_NAMESPACE::hash_map<uint32, T*>::iterator itr = _map.find(Entry);
		if(itr == _map.end())
		{
			_map.insert( make_pair( Entry, Pointer ) );
			return true;
		}

		delete itr->second;
		itr->second = Pointer;
		return true;
	}

	/** Returns the current pointer if it exists, otherwise allocates it.
	 */
	T * LookupEntryAllocate(uint32 Entry)
	{
		T * ret = LookupEntry(Entry);
		if(!ret)
			ret = AllocateEntry(Entry);
		return ret;
	}
};

template<class T>
class ArrayStorageIterator : public StorageContainerIterator<T>
{
	ArrayStorageContainer<T> * Source;
	uint32 MyIndex;
public:

	/** Increments the iterator
	*/
	bool Inc()
	{
		GetNextElement();
		if(StorageContainerIterator<T>::Pointer != 0)
			return true;
		else
			return false;
	}

	/** Frees the memory occupied by this iterator
	*/
	void Destruct()
	{
		delete this;
	}

	/** Constructor
	*/
	ArrayStorageIterator(ArrayStorageContainer<T> * S) : StorageContainerIterator<T>(), Source(S), MyIndex(0)
	{
		GetNextElement();
	}

	/** Sets the next element pointer, or to 0 if we reached the end
	*/
	void GetNextElement()
	{
		while(MyIndex < Source->_max)
		{
			if(Source->_array[MyIndex] != 0)
			{
				StorageContainerIterator<T>::Set(Source->_array[MyIndex]);
				++MyIndex;
				return;
			}
			++MyIndex;
		}
		// reached the end of the array
		StorageContainerIterator<T>::Set(reinterpret_cast<T*>(0));
	}
};

template<class T>
class HashMapStorageIterator : public StorageContainerIterator<T>
{
	HashMapStorageContainer<T> * Source;
	typename HM_NAMESPACE::hash_map<uint32, T*>::iterator itr;
public:

	/** Constructor
	*/
	HashMapStorageIterator(HashMapStorageContainer<T> * S) : StorageContainerIterator<T>(), Source(S)
	{
		itr = S->_map.begin();
		if(itr == S->_map.end())
			StorageContainerIterator<T>::Set(0);
		else
			StorageContainerIterator<T>::Set(itr->second);
	}

	/** Gets the next element, or if we reached the end sets it to 0
	*/
	void GetNextElement()
	{
		++itr;
		if(itr == Source->_map.end())
			StorageContainerIterator<T>::Set(0);
		else
			StorageContainerIterator<T>::Set(itr->second);
	}

	/** Returns true if we're not at the end, otherwise false.
	*/
	bool Inc()
	{
		GetNextElement();
		if(StorageContainerIterator<T>::Pointer != 0)
			return true;
		else
			return false;
	}

	/** Frees the memory occupied by this iterator
	*/
	void Destruct()
	{
		delete this;
	}
};

template<class T>
StorageContainerIterator<T> * ArrayStorageContainer<T>::MakeIterator()
{
	return new ArrayStorageIterator<T>(this);
}

template<class T>
StorageContainerIterator<T> * HashMapStorageContainer<T>::MakeIterator()
{
	return new HashMapStorageIterator<T>(this);
}

template<class T, class StorageType>
class Storage
{
protected:
	StorageType _storage;
	char * _indexName;
	char * _formatString;
public:
	
	inline char * GetIndexName() { return _indexName; }
	inline char * GetFormatString() { return _formatString; }

	/** False constructor to fool compiler
	 */
	Storage() {}
	~Storage() {}

	/** Makes an iterator, w00t!
	 */
	StorageContainerIterator<T> * MakeIterator()
	{
		return _storage.MakeIterator();
	}

	/** Calls the storage container lookup function.
	 */
	T * LookupEntry(uint32 Entry)
	{
		return _storage.LookupEntry(Entry);
	}

	/** Reloads the content in this container.
	 */
    	virtual void Reload() = 0;

	/** Loads the container using the specified name and format string
	 */
	virtual void Load(const char * IndexName, const char * FormatString)
	{
		_indexName = strdup(IndexName);
		_formatString = strdup(FormatString);
	}

	/** Frees the duplicated strings and all entries inside the storage container
	 */
	virtual void Cleanup()
	{
        StorageContainerIterator<T> * itr = _storage.MakeIterator();
		while(!itr->AtEnd())
		{
			FreeBlock(itr->Get());
			if(!itr->Inc())
				break;
		}
		itr->Destruct();

		free(_indexName);
		free(_formatString);
	}

	/** Frees any string elements inside blocks. 
	 */
	void FreeBlock(T * Allocated)
	{
		char * p = _formatString;
		char * structpointer = (char*)Allocated;
		for(; *p != 0; ++p)
		{
			switch(*p)
			{
			case 's':		// string is the only one we have to actually do anything for here
				free((void*)(*(uint32*)structpointer));
				structpointer += sizeof(char*);
				break;

			case 'u':
			case 'i':
			case 'f':
				structpointer += sizeof(uint32);
				break;
				
			case 'h':
				structpointer += sizeof(uint16);
				break;

			case 'c':
				structpointer += sizeof(uint8);
				break;
			}
		}
	}
};

template<class T, class StorageType>
class SQLStorage : public Storage<T, StorageType>
{
public:
	SQLStorage() : Storage<T, StorageType>() {}
	~SQLStorage() {}

	/** Loads the block using the format string.
	 */
	inline void LoadBlock(Field * fields, T * Allocated)
	{
		char * p = Storage<T, StorageType>::_formatString;
		char * structpointer = (char*)Allocated;
		Field * f = fields;
		for(; *p != 0; ++p, ++f)
		{
            switch(*p)
			{
			case 'u':	// Unsigned integer
				*(uint32*)structpointer = f->GetUInt32();
				structpointer += sizeof(uint32);
				break;

			case 'i':	// Signed integer
				*(int32*)structpointer = f->GetInt32();
				structpointer += sizeof(uint64);
				break;

			case 's':	// Null-terminated string
				*(uint32*)structpointer = (uint32)strdup(f->GetString());
				structpointer += sizeof(char*);
				break;

			case 'x':	// Skip
				break;

			case 'f':	// Float
				*(float*)structpointer = f->GetFloat();
				structpointer += sizeof(float);
				break;

			case 'c':	// Char
				*(uint8*)structpointer = f->GetUInt8();
				structpointer += sizeof(uint8);
				break;

			case 'h':	// Short
				*(uint16*)structpointer = f->GetUInt16();
				structpointer += sizeof(uint16);
				break;

			default:	// unknown
				printf("Unknown field type in string: `%c`\n", *p);
				break;
			}
		}
	}

	/** Loads from the table.
	 */
	void Load(const char * IndexName, const char * FormatString)
	{
		Storage<T, StorageType>::Load(IndexName, FormatString);
		QueryResult * result = sDatabase.Query("SELECT MAX(entry) FROM %s", IndexName);
		uint32 Max = 999999;
		if(result)
		{
			Max = result->Fetch()[0].GetUInt32() + 1;
			delete result;
		}

		Storage<T, StorageType>::_storage.Setup(Max);

		int cols = strlen(FormatString);
		result = sDatabase.Query("SELECT * FROM %s", IndexName);
		Field * fields = result->Fetch();

		if(result->GetFieldCount() != cols)
		{
			printf("Invalid format in %s (%u/%u).", IndexName, cols, result->GetFieldCount());
			delete result;
			return;
		}

		uint32 Entry;
		T * Allocated;
		do 
		{
			Entry = fields[0].GetUInt32();
			Allocated = Storage<T, StorageType>::_storage.AllocateEntry(Entry);
			if(!Allocated)
				continue;

			LoadBlock(fields, Allocated);
		} while(result->NextRow());
		delete result;
	}

	/** Reloads the storage container
	 */
	void Reload()
	{
		QueryResult * result = sDatabase.Query("SELECT MAX(entry) FROM %s", Storage<T, StorageType>::_indexName);
		if(result == 0)
			return;

		uint32 Max = result->Fetch()[0].GetUInt32();
		delete result;
		if(!Max)
			return;

		Storage<T, StorageType>::_storage.Resetup(Max);

		int cols = strlen(Storage<T, StorageType>::_formatString);
		result = sDatabase.Query("SELECT * FROM %s", Storage<T, StorageType>::_indexName);
		Field * fields = result->Fetch();

		if(result->GetFieldCount() != cols)
		{
			printf("Invalid format in %s (%u/%u).", Storage<T, StorageType>::_indexName, cols, result->GetFieldCount());
			delete result;
			return;
		}

		uint32 Entry;
		T * Allocated;
		do 
		{
			Entry = fields[0].GetUInt32();
			Allocated = Storage<T, StorageType>::_storage.LookupEntryAllocate(Entry);
			LoadBlock(fields, Allocated);
		} while(result->NextRow());
		delete result;
	}
};

#endif
