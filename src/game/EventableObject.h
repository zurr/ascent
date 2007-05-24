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

#ifndef _EVENTABLE_OBJECT_H
#define _EVENTABLE_OBJECT_H

class EventableObjectHolder;

/**
  * @class EventableObject
  * EventableObject means that the class inheriting this is able to take
  * events. This 'base' class will store and update these events upon
  * receiving the call from the instance thread / WorldRunnable thread.
  */

typedef set<TimedEvent*> EventList;
#define EVENT_REMOVAL_FLAG_ALL 0xFFFFFFFF

class SERVER_DECL EventableObject
{
	friend class EventMgr;
	friend class EventableObjectHolder;

protected:
	void event_RemoveEvents();
	void event_RemoveEvents(uint32 EventType);
	void event_ModifyTimeLeft(uint32 EventType, uint32 TimeLeft);
	void event_ModifyTime(uint32 EventType, uint32 Time);
	void event_ModifyTimeAndTimeLeft(uint32 EventType, uint32 Time);
	bool event_HasEvent(uint32 EventType);

public:
	uint32 event_GetEventPeriod(uint32 EventType);
	// Public methods
	EventableObject();
	virtual ~EventableObject();

	inline bool event_HasEvents() { return m_events.size() > 0 ? true : false; }
	void event_AddEvent(TimedEvent * ptr);
	void event_UpdateActiveStatus();
	bool event_UpdateEvents(uint32 diff);
	void event_SetActive(bool value);
	void event_Relocate();
	
	// this func needs to be implemented by all eventable classes. use it to retreive the instance
	// id that it needs to attach itself to.
	
	virtual int32 event_GetInstanceID() { return -1; }

protected:

	Mutex eventListLock;
	EventList m_events;
	EventableObjectHolder * m_eventHolder;
	EventableObjectHolder * m_lastHolder;
	uint32 m_active;
	bool m_isOwnInstance;
	
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

	void AddObject(EventableObject * obj);
	void RemoveObject(EventableObject * obj);
	inline uint32 GetInstanceID() { return mInstanceId; }
	bool deletedObject;

protected:
	int32 mInstanceId;
	//set<EventableObject*> myObjects;
	//set<EventableObject*> removalList;

	Mutex setLock;
	EventableObjectSet myObjects;
	RMutex iteratorLock;
	EventableObjectSet::iterator current;
};

#endif
