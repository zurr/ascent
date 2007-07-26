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

#ifndef _EVENTABLE_OBJECT_H
#define _EVENTABLE_OBJECT_H

class EventableObjectHolder;

/**
  * @class EventableObject
  * EventableObject means that the class inheriting this is able to take
  * events. This 'base' class will store and update these events upon
  * receiving the call from the instance thread / WorldRunnable thread.
  */

typedef list<TimedEvent*> EventList;
typedef multimap<uint32, TimedEvent*> EventMap;

#define EVENT_REMOVAL_FLAG_ALL 0xFFFFFFFF

class SERVER_DECL EventableObject
{
	friend class EventMgr;
	friend class EventableObjectHolder;

protected:
	void event_RemoveEvents();
	void event_RemoveEvents(uint32 EventType);
	void event_ModifyTimeLeft(uint32 EventType, uint32 TimeLeft,bool unconditioned=false);
	void event_ModifyTime(uint32 EventType, uint32 Time);
	void event_ModifyTimeAndTimeLeft(uint32 EventType, uint32 Time);
	bool event_HasEvent(uint32 EventType);
	void event_RemoveByPointer(TimedEvent * ev);
	inline int32 event_GetCurrentInstanceId() { return m_event_Instanceid; }

public:
	uint32 event_GetEventPeriod(uint32 EventType);
	// Public methods
	EventableObject();
	virtual ~EventableObject();

	inline bool event_HasEvents() { return m_events.size() > 0 ? true : false; }
	void event_AddEvent(TimedEvent * ptr);
	void event_Relocate();
	
	// this func needs to be implemented by all eventable classes. use it to retreive the instance
	// id that it needs to attach itself to.
	
	virtual int32 event_GetInstanceID() { return -1; }

protected:

	int32 m_event_Instanceid;
	Mutex m_lock;
	EventMap m_events;
	EventableObjectHolder * m_holder;
	
};

/**
  * @class EventableObjectHolder
  * EventableObjectHolder will store eventable objects, and remove/add them when they change
  * from one holder to another (changing maps / instances).
  *
  * EventableObjectHolder also updates all the timed events in all of its objects when its
  * update function is called.
  *
  */

typedef set<EventableObject*> EventableObjectSet;

class EventableObjectHolder
{
public:
	EventableObjectHolder(int32 instance_id);
	~EventableObjectHolder();

	void Update(uint32 time_difference);

	void AddEvent(TimedEvent * ev);
	void AddObject(EventableObject * obj);

	inline uint32 GetInstanceID() { return mInstanceId; }

protected:
	int32 mInstanceId;
	Mutex m_lock;
	EventList m_events;

	Mutex m_insertPoolLock;
	typedef list<TimedEvent*> InsertableQueue;
	InsertableQueue m_insertPool;
};

#endif
