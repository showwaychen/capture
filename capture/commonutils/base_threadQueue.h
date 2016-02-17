#pragma once

#include <string.h>
#include <map>
#include <queue>

#include "base_threadSemaphore.h"
#include "base_Lock.h"


using namespace std;

template<typename Data>
class CBase_threadQueue
{
private:
    std::queue<Data> the_queue;
    mutable base_Mutex mMutex;
    CBase_threadSemaphore mSemaphore;

public:
    void  push(Data const& data)
    {
        base_Lock lock(mMutex);
        the_queue.push(data);
        mSemaphore.post();
    }

    bool empty() const
    {
        base_Lock lock(mMutex);
        return the_queue.empty();
    }

    bool try_pop(Data& popped_value)
    {
        base_Lock lock(mMutex);
        if(the_queue.empty())
        {
            return false;
        }
        popped_value = the_queue.front();
        the_queue.pop();
        
        return true;
    }

    void wait_and_pop(Data& popped_value)
    {
		mMutex.lock();
		{
			while(the_queue.empty())
			{
				mMutex.unlock();
				{
				   mSemaphore.wait();
				}
				mMutex.lock();
			}
	        
			popped_value=the_queue.front();
			the_queue.pop();
		}
		mMutex.unlock();
    }

	void wait_and_pop_last(Data& popped_value)
	{
		do
		{
		    this->wait_and_pop(popped_value);
		}
        while(this->empty() == false);
	}
	int get_semaphore_num()
	{
		base_Lock lock(mMutex);
		return the_queue.size();
	}
};
