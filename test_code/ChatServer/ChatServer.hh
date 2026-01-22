#include "myeasylog.hpp"
#include "mutex.hpp"
#include "UdpServer.hh"
#include "mythread_pool.hpp"

#include <string>
#include <memory>
#include <queue>
#include <list>
#include <utility>

namespace oldking 
{
	class ChatServer
	{
	public:
		ChatServer(int16_t port)
		: Udp_(std::make_unique<UdpServer>(port))
		, TP_(3)
		{
			Udp_->init();		
		}

		ChatServer(const ChatServer& other) = delete;
		ChatServer(ChatServer&& other) = delete;

		~ChatServer()
		{
			TP_.KillAll();
			TP_.ClearTrash();
		}

		void start();

	private:
		void get_msg_();

		void send_msg_(const std::string& msg, const std::string& ip, const int16_t port);

		static void* send_task_(void* arg);
	
		// 任务参数 
		struct task_arg
		{
			ChatServer* self;	
			std::string msg;
			std::string ip;
			int16_t port;
		};

	private:
		std::unique_ptr<UdpServer> Udp_;	
		std::queue<std::tuple<std::string, std::string, int16_t>> waiting_queue_;
		oldking::MyThreadPool TP_;
		std::list<std::pair<std::string, int16_t>> client_list_;
	};
}



