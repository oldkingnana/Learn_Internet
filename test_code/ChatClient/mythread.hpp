#pragma once

#include <pthread.h>
#include <atomic>
#include <functional>
#include <queue>
#include <tuple>
#include "myeasylog.hpp"

namespace oldking
{
	typedef int THREAD_STAT;
	
	#define KILLED_TD 0
	#define WAIT_TD 1
	#define RUNNING_TD 2

	class MyThreadforThreadPool
	{
	private: 
		void _swap(MyThreadforThreadPool& other) noexcept
		{
			std::swap(_ppqueue, other._ppqueue);
			std::swap(_ppcond, other._ppcond);
			std::swap(_ppqmutex, other._ppqmutex);
			std::swap(_stat, other._stat);
			std::swap(_tid, other._tid);
		}

	public:
		MyThreadforThreadPool(const MyThreadforThreadPool& other) = delete;
		MyThreadforThreadPool& operator=(const MyThreadforThreadPool& other) = delete;

		MyThreadforThreadPool(MyThreadforThreadPool&& other) noexcept
		: _tid(0)
		, _stat(nullptr)
		{
			oldking::MyEasyLog::GetInstance().WriteLog(LOG_INFO, "mythread.hpp", "begin to move copy");
			_swap(other);	
			if(_stat == nullptr)
			{
				oldking::MyEasyLog::GetInstance().WriteLog(LOG_WARNING, "mythread.hpp", "_stat is a nullptr");
			}
		}

		
		MyThreadforThreadPool& operator=(MyThreadforThreadPool&& other) noexcept
		{
			oldking::MyEasyLog::GetInstance().WriteLog(LOG_INFO, "mythread.hpp", "begin to move operator=");
			_swap(other);
			if(_stat == nullptr)
			{
				oldking::MyEasyLog::GetInstance().WriteLog(LOG_WARNING, "mythread.hpp", "_stat is a nullptr");
			}
			return *this;
		}

		MyThreadforThreadPool(pthread_cond_t* ppcond, pthread_mutex_t* ppqmutex, std::queue<std::tuple<std::function<void*(void*)>, void*>>* ppqueue)	
		: _ppqueue(ppqueue)	
		, _ppcond(ppcond)
		, _ppqmutex(ppqmutex)
		, _stat(new std::atomic<THREAD_STAT>)
		{
			oldking::MyEasyLog::GetInstance().WriteLog(LOG_INFO, "mythread.hpp", "begin to constructor");
			if(_stat == nullptr)
			{
				oldking::MyEasyLog::GetInstance().WriteLog(LOG_WARNING, "mythread.hpp", "_stat is a nullptr");
			}
			pthread_create(&_tid, nullptr, entry, static_cast<void*>(this));
		}

		~MyThreadforThreadPool()
		{
			oldking::MyEasyLog::GetInstance().WriteLog(LOG_INFO, "mythread.hpp", "begin to destructor");
			if(_stat != nullptr)
			{
				oldking::MyEasyLog::GetInstance().WriteLog(LOG_WARNING, "mythread.hpp", "delete the _stat!");
				if(_tid == 0)
				{
					oldking::MyEasyLog::GetInstance().WriteLog(LOG_ERROR, "mythread.hpp", "delete the other thread's _stat!");
				}
				delete _stat;
			}
		}
		
		std::atomic<THREAD_STAT>* GetStatPointer()
		{
			return _stat;
		}

		pthread_cond_t* GetCondPointer()
		{
			return _ppcond; 
		}

		pthread_mutex_t* GetMutexPointer()
		{
			return _ppqmutex; 
		}

		pthread_t GetthreadID()
		{
			return _tid;
		}

		std::queue<std::tuple<std::function<void*(void*)>, void*>>* GetQueuePointer()
		{
			return _ppqueue;
		}

		void ChangeStat(THREAD_STAT stat)
		{
			// thread safe
			oldking::MyEasyLog::GetInstance().WriteLog(LOG_INFO, "mythread.hpp", "change stat");
			if(_stat == nullptr)
			{
				oldking::MyEasyLog::GetInstance().WriteLog(LOG_FATAL, "mythread.hpp", "the pstat is a nullptr!");
			}
			*_stat = stat;			
		}

	private:
		static void* entry(void* entry_arg)
		{

			// get info
			oldking::MyEasyLog::GetInstance().WriteLog(LOG_INFO, "mythread.hpp", "entry:: get info");
			MyThreadforThreadPool* pthis = static_cast<MyThreadforThreadPool*>(entry_arg);
			pthread_mutex_t* ppqmutex = pthis->GetMutexPointer();
			pthread_cond_t* ppcond = pthis->GetCondPointer();
			std::queue<std::tuple<std::function<void*(void*)>, void*>>* ppqueue = pthis->GetQueuePointer();

			const pthread_t pid = pthis->GetthreadID(); 
			
			// detach
			oldking::MyEasyLog::GetInstance().WriteLog(LOG_INFO, "mythread.hpp", "entry:: detach");
			pthread_detach(pid);
	
			// change stat to waitting 
			oldking::MyEasyLog::GetInstance().WriteLog(LOG_INFO, "mythread.hpp", "entry:: change stat to waitting");
			pthis->ChangeStat(WAIT_TD);

			while(true)
			{
				// wait & lock for task_queue
				oldking::MyEasyLog::GetInstance().WriteLog(LOG_INFO, "mythread.hpp", "entry:: wait & lock for task_queue");
				pthread_mutex_lock(ppqmutex);
				while(ppqueue->empty())
				{
					pthread_cond_wait(ppcond, ppqmutex);
				}
				// change stat to running  
				oldking::MyEasyLog::GetInstance().WriteLog(LOG_INFO, "mythread.hpp", "entry:: change stat to running");
				pthis->ChangeStat(RUNNING_TD);

				// get task & arg
				oldking::MyEasyLog::GetInstance().WriteLog(LOG_INFO, "mythread.hpp", "entry:: get task & arg");
				std::function<void*(void*)> task = std::get<0>(ppqueue->front());
				void* parg = std::get<1>(ppqueue->front());
	
				// pop task from task queue
				oldking::MyEasyLog::GetInstance().WriteLog(LOG_INFO, "mythread.hpp", "entry:: pop task from task queue");
				ppqueue->pop();

				// unlock
				oldking::MyEasyLog::GetInstance().WriteLog(LOG_INFO, "mythread.hpp", "entry unlock");
				pthread_mutex_unlock(ppqmutex);
	
				if(!task)
					break;

				void* ret_val = task(parg);
				(void)ret_val;

				// save return value
				// ...
			}

			oldking::MyEasyLog::GetInstance().WriteLog(LOG_INFO, "mythread.hpp", "entry:: thread return");
			pthis->ChangeStat(KILLED_TD);
			return nullptr;
		}

		std::queue<std::tuple<std::function<void*(void*)>, void*>>* _ppqueue;	
		pthread_cond_t* _ppcond;
		pthread_mutex_t* _ppqmutex;
		pthread_t _tid;	
		std::atomic<THREAD_STAT>* _stat;
	};
}

