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

#ifndef FQUEUE_H
#define FQUEUE_H

#include "Condition.h"
#include "Mutex.h"
 
template<class T> 
class FQueue 
{
public:
	inline FQueue() : cond(&lock) {first=last=NULL;size=0;}
	volatile unsigned int size;

	uint32 get_size()
	{
		uint32 ret;
		cond.BeginSynchronized();
		ret = size;
		cond.EndSynchronized();
		return ret;
	}

	void push(T &item)
	{
		h*p=new h;
		p->value=item;
		p->pNext=NULL;
		
		//lock.Acquire();
		cond.BeginSynchronized();
		if(last)//have some items
		{
			last->pNext=p;
			last=p;
			size++;
		}
		else//first item
		{
			last=first=p;
			size=1;
			cond.Signal();
		}
		//lock.Release();
		cond.EndSynchronized();
	}

	T pop()
	{
		//lock.Acquire();
		cond.BeginSynchronized();
		if(size==0)
		cond.Wait();

		h*tmp=first;

		if(--size)//more than 1 item
		{
			first=(h*)first->pNext;
		}
		else//last item
		{
			first=last=NULL;
		}
		//lock.Release();
		cond.EndSynchronized();

		T returnVal = tmp->value;
		delete tmp;
		
		return returnVal;
	}	
	
private:
	struct h
	{
		T value;
		void *pNext;
	};

	h*first;
	h*last;
	
	Mutex lock;
	Condition cond;

};

#endif 


