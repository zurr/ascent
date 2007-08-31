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

#include "StdAfx.h"
#include "EventableObject.h"

EventableObject::~EventableObject()
{
	/* decrement event count on all events */

	m_lock.Acquire();
	EventMap::iterator itr = m_events.begin();
	for(; itr != m_events.end();)
	{
		(*itr)->deleted = true;
		(*itr)->DecRef();
		itr = m_events.erase(itr);
	}

	m_lock.Release();
}

EventableObject::EventableObject()
{
	/* commented, these will be allocated when the first event is added. */
	//m_event_Instanceid = event_GetInstanceID();
	//m_holder = sEventMgr.GetEventHolder(m_event_Instanceid);

	m_holder = 0;
	m_event_Instanceid = -1;
}

void EventableObject::event_AddEvent(TimedEvent * ptr)
{
	m_lock.Acquire();

	if(!m_holder)
	{
		m_event_Instanceid = event_GetInstanceID();
		m_holder = sEventMgr.GetEventHolder(m_event_Instanceid);
	}

	ptr->IncRef();
	ptr->instanceId = m_event_Instanceid;

	/* let's do some garbage collection */
	for(EventMap::iterator itr = m_events.begin(); itr != m_events.end();)
	{
		if((*itr)->deleted)
		{
			(*itr)->DecRef();
			itr = m_events.erase(itr);
		}
		else
			++itr;
	}
	m_events.push_back(ptr);
	m_lock.Release();

	/* Add to event manager */
	if(!m_holder)
	{
		/* relocate to -1 eventholder :/ */
		m_event_Instanceid = -1;
		m_holder = sEventMgr.GetEventHolder(m_event_Instanceid);
		ASSERT(m_holder);
	}

	m_holder->AddEvent(ptr);
}

void EventableObject::event_RemoveByPointer(TimedEvent * ev)
{
	m_lock.Acquire();
	for(EventMap::iterator itr = m_events.begin(); itr != m_events.end(); ++itr)
	{
		if(*itr == ev)
		{
			ev->deleted = true;
			ev->DecRef();
			m_events.erase(itr);
			m_lock.Release();
			return;
		}
	}
	m_lock.Release();
}

void EventableObject::event_RemoveEvents(uint32 EventType)
{
	m_lock.Acquire();

	if(EventType == EVENT_REMOVAL_FLAG_ALL)
	{
		EventMap::iterator itr = m_events.begin();
		for(; itr != m_events.end();)
		{
			(*itr)->deleted = true;
			(*itr)->DecRef();
			itr = m_events.erase(itr);
		}
	}
	else
	{
		EventMap::iterator itr = m_events.begin();
		for(; itr != m_events.end();)
		{
			if((*itr)->eventType == EventType)
			{
				(*itr)->deleted = true;
				(*itr)->DecRef();
				itr = m_events.erase(itr);
			}
			else if((*itr)->deleted)
			{
				(*itr)->DecRef();
				itr = m_events.erase(itr);
			}
			else
				++itr;
		}
	}

	m_lock.Release();
}

void EventableObject::event_RemoveEvents()
{
	event_RemoveEvents(EVENT_REMOVAL_FLAG_ALL);
}

void EventableObject::event_ModifyTimeLeft(uint32 EventType, uint32 TimeLeft,bool unconditioned)
{
	m_lock.Acquire();

	for(EventMap::iterator itr = m_events.begin(); itr != m_events.end();)
	{
		if((*itr)->deleted)
		{
			(*itr)->DecRef();
			itr = m_events.erase(itr);
			continue;
		}
		
		if((*itr)->eventType == EventType)
		{
			if(unconditioned)
				(*itr)->currTime = TimeLeft;
			else
				((int32)TimeLeft > (*itr)->msTime) ? (*itr)->msTime : (int32)TimeLeft;
		}
		++itr;
	}

	m_lock.Release();
}

void EventableObject::event_ModifyTime(uint32 EventType, uint32 Time)
{
	m_lock.Acquire();

	for(EventMap::iterator itr = m_events.begin(); itr != m_events.end();)
	{
		if((*itr)->deleted)
		{
			(*itr)->DecRef();
			itr = m_events.erase(itr);
			continue;
		}
		
		if((*itr)->eventType == EventType)
			(*itr)->msTime = Time;

		++itr;
	}

	m_lock.Release();
}

void EventableObject::event_ModifyTimeAndTimeLeft(uint32 EventType, uint32 Time)
{
	m_lock.Acquire();

	for(EventMap::iterator itr = m_events.begin(); itr != m_events.end(); ++itr)
	{
		if((*itr)->deleted)
		{
			(*itr)->DecRef();
			itr = m_events.erase(itr);
			continue;
		}
		else if((*itr)->eventType == EventType) {
			(*itr)->msTime = (*itr)->currTime = Time;
		}
	}

	m_lock.Release();
}


bool EventableObject::event_HasEvent(uint32 EventType)
{
	bool result=false;
	m_lock.Acquire();

	for(EventMap::iterator itr = m_events.begin(); itr != m_events.end();)
	{
		if((*itr)->deleted)
		{
			(*itr)->DecRef();
			itr = m_events.erase(itr);
			continue;
		}
		else if((*itr)->eventType == EventType)
			result=true;

		++itr;
	}

	m_lock.Release();
	return result;
}

EventableObjectHolder::EventableObjectHolder(int32 instance_id) : mInstanceId(instance_id)
{
	sEventMgr.AddEventHolder(this, instance_id);
}

EventableObjectHolder::~EventableObjectHolder()
{
	sEventMgr.RemoveEventHolder(this);

	/* decrement events reference count */
	m_lock.Acquire();
	EventList::iterator itr = m_events.begin();
	for(; itr != m_events.end(); ++itr)
		(*itr)->DecRef();
	m_lock.Release();
}

void EventableObjectHolder::Update(uint32 time_difference)
{
	m_lock.Acquire();			// <<<<

	/* Insert any pending objects in the insert pool. */
	m_insertPoolLock.Acquire();
	InsertableQueue::iterator iqi;
	InsertableQueue::iterator iq2 = m_insertPool.begin();
	while(iq2 != m_insertPool.end())
	{
		iqi = iq2++;
		if((*iqi)->deleted || (*iqi)->instanceId != mInstanceId)
			(*iqi)->DecRef();
		else
			m_events.push_back( (*iqi) );

		m_insertPool.erase(iqi);
	}
	m_insertPoolLock.Release();

	/* Now we can proceed normally. */
	EventList::iterator itr = m_events.begin();
	EventList::iterator it2;
	TimedEvent * ev;

	while(itr != m_events.end())
	{
		it2 = itr++;

		if((*it2)->instanceId != mInstanceId || (*it2)->deleted || 
			( mInstanceId == WORLD_INSTANCE && (*it2)->eventFlag & EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT))
		{
			// remove from this list.
			(*it2)->DecRef();
			m_events.erase(it2);
			continue;
		}

		// Event Update Procedure
		ev = *it2;

		if((uint32)ev->currTime <= time_difference)
		{
			// execute the callback
			ev->cb->execute();

			// check if the event is expired now.
            if(ev->repeats && --ev->repeats == 0)
			{
				// Event expired :>
				
				/* remove the event from the object */
				/*obj = (EventableObject*)ev->obj;
				obj->event_RemoveByPointer(ev);*/

				/* remove the event from here */
				ev->deleted = true;
				ev->DecRef();
				m_events.erase(it2);

				continue;
			}
			else if(ev->deleted)
			{
				// event is now deleted
				ev->DecRef();
				m_events.erase(it2);
				continue;
			}

			// event has to repeat again, reset the timer
			ev->currTime = ev->msTime;
		}
		else
		{
			// event is still "waiting", subtract time difference
			ev->currTime -= time_difference;
		}
	}

	m_lock.Release();
}

void EventableObject::event_Relocate()
{
	/* prevent any new stuff from getting added */
	m_lock.Acquire();

	EventableObjectHolder * nh = sEventMgr.GetEventHolder(event_GetInstanceID());
	if(nh != m_holder)
	{
		// whee, we changed event holder :>
		// doing this will change the instanceid on all the events, as well as add to the new holder.
		
		// no need to do this if we don't have any events, though.
		if(m_events.size())
		{
			/* shitty sanity check. xUdd sucks. */
			if(!nh)
				nh = sEventMgr.GetEventHolder(-1);

			nh->AddObject(this);
		}

		// reset our m_holder pointer and instance id
		m_event_Instanceid = nh->GetInstanceID();
		m_holder = nh;
	}

	/* safe again to add */
	m_lock.Release();
}

uint32 EventableObject::event_GetEventPeriod(uint32 EventType)
{
	uint32 ret;
	m_lock.Acquire();
	for(EventMap::iterator itr = m_events.begin(); itr != m_events.end(); ++itr)
	{
		if((*itr)->eventType == EventType)
		{
			ret = (*itr)->msTime;
			m_lock.Release();
			return ret;
		}
	}

	m_lock.Release();
	return 0;
}

void EventableObjectHolder::AddEvent(TimedEvent * ev)
{
	// m_lock NEEDS TO BE A RECURSIVE MUTEX
	m_lock.Acquire();
	ev->IncRef();
	m_events.push_back( ev );
	m_lock.Release();
}

void EventableObjectHolder::AddObject(EventableObject * obj)
{
	// transfer all of this objects events into our holder
	if(!m_lock.AttemptAcquire())
	{
		// The other thread is obviously occupied. We have to use an insert pool here, otherwise
		// if 2 threads relocate at once we'll hit a deadlock situation.
		m_insertPoolLock.Acquire();
		EventMap::iterator it2;

		for(EventMap::iterator itr = obj->m_events.begin(); itr != obj->m_events.end();++itr)
		{
			// ignore deleted events (shouldn't be any in here, actually)
			if((*itr)->deleted)
				continue;

			if(mInstanceId == WORLD_INSTANCE && (*itr)->eventFlag & EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT)
				continue;

			(*itr)->IncRef();
			(*itr)->instanceId = mInstanceId;
			m_insertPool.push_back((*itr));
		}

		// Release the insert pool.
		m_insertPoolLock.Release();

		// Ignore the rest of this stuff
		return;
	}

	for(EventMap::iterator itr = obj->m_events.begin(); itr != obj->m_events.end(); ++itr)
	{
		// ignore deleted events
		if((*itr)->deleted)
			continue;

		if(mInstanceId == WORLD_INSTANCE && (*itr)->eventFlag & EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT)
			continue;

		(*itr)->IncRef();
		(*itr)->instanceId = mInstanceId;
		m_events.push_back( (*itr) );
	}

	m_lock.Release();
}
