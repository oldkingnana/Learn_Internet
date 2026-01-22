#pragma once

#include "mythread.hpp"
#include <queue>
#include <functional>
#include <tuple>
#include <list>
#include <pthread.h>
#include "mutex.hpp"
#include "myeasylog.hpp"

namespace oldking
{

	// 几个常犯的错误
	// 1. 别忘记detach/手动join
	// 2. 移动语义一定不能试图释放公共资源!
	class MyThreadPool 
	{
	public:
		MyThreadPool(const size_t& cnt)
		{
			oldking::MyEasyLog::GetInstance().WriteLog(LOG_INFO, "mythread_pool", "begin to constructor");
			pthread_mutex_init(&_queue_mutex, nullptr);
			pthread_cond_init(&_task_cond, nullptr);
			for(size_t i = 0; i < cnt; i++)
			{
				_td_list_mutex.Lock();
				_thread_list.emplace_back(&_task_cond, &_queue_mutex, &_task_queue);
				_td_list_mutex.Unlock();
			}
		}

		void KillAll()
		{
			oldking::MyEasyLog::GetInstance().WriteLog(LOG_INFO, "mythread_pool", "begin to KillAll");
			std::function<void*(void*)> ExitTask;
		
			pthread_mutex_lock(&_queue_mutex);	
			for(size_t n = 0; n < _thread_list.size(); n++)
			{
				_task_queue.push(std::tuple<std::function<void*(void*)>, void*>(ExitTask, nullptr));
			}
			pthread_cond_broadcast(&_task_cond);
			pthread_mutex_unlock(&_queue_mutex);	
		}

		size_t GetThreadCnt()
		{
			return _thread_list.size();
		}

		void SendTask(std::function<void*(void*)> task,
				      void* arg) 
		{
			oldking::MyEasyLog::GetInstance().WriteLog(LOG_INFO, "mythread_pool", "SendTask:: begin");
			if(GetThreadCnt() < 1)
				return ;

			pthread_mutex_lock(&_queue_mutex);
			// 这里直接用中文写,因为这个点非常牛逼
			// 其一,如果我们使用push,实际上会多一层tuple的拷贝
			// 其二,使用move还可以最大化使用task,能直接剽窃task 
			// 最后值得注意的是,如果外部填的是一个lambda之类的临时变量的话,直接用引用会报错,要么用const修饰+引用的组合,但这样又会造成另一个恶心的问题,即queue只允许传入不带const的function(我觉得应该修正这个问题),实际传参传不进去,像这样就非常合适,并且这样还可以减少tuple的拷贝
			_task_queue.emplace(std::move(task), arg);
			pthread_cond_signal(&_task_cond);	
			pthread_mutex_unlock(&_queue_mutex);
		}

		size_t ClearTrash()
		{
			oldking::MyEasyLog::GetInstance().WriteLog(LOG_INFO, "mythread_pool", "ClearTrash:: begin");
			_td_list_mutex.Lock();
			size_t cnt = 0;
			for(auto it = _thread_list.begin(); it != _thread_list.end(); )	
			{
				if(*(it->GetStatPointer()) == KILLED_TD)
					it = _thread_list.erase(it); // 注意迭代器失效问题	
				else 
					it++;
				_td_list_mutex.Unlock();
				cnt++;
			}

			return cnt;
		}

		~MyThreadPool()
		{
			oldking::MyEasyLog::GetInstance().WriteLog(LOG_INFO, "mythread_pool", "begin to destructor");
			KillAll();
			sleep(1);
			pthread_mutex_destroy(&_queue_mutex);
			pthread_cond_destroy(&_task_cond);
		}

	private:
		std::queue<std::tuple<std::function<void*(void*)>, void*>> _task_queue;	
		pthread_cond_t _task_cond;
		pthread_mutex_t _queue_mutex;

		oldking::mymutex _td_list_mutex;
		std::list<oldking::MyThreadforThreadPool> _thread_list;
		// _stat_manager
	};
}
