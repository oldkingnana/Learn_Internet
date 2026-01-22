#include "myeasylog.hpp"
#include "mutex.hpp"
#include "UdpClient.hh"
#include "mythread_pool.hpp"

#include <string>
#include <memory>
#include <unistd.h>

namespace oldking 
{
	class ChatClient
	{
	public:
		ChatClient(std::string ip, int16_t port)
		: Udp_(std::make_unique<UdpClient>())
		, TP_(1)
		, ip_(ip)
		, port_(port)
		{
			Udp_->init(ip, port);		
		}

		ChatClient(const ChatClient& other) = delete;
		ChatClient(ChatClient&& other) = delete;

		~ChatClient()
		{
			Udp_->stop();
			TP_.KillAll();
			TP_.ClearTrash();
		}

		void start();

	private:
		void send_msg_(const std::string& msg);

		void recv_msg_();

		static void* recv_task_(void* arg);
	

	private:
		std::unique_ptr<UdpClient> Udp_;	
		oldking::MyThreadPool TP_;
		std::string ip_;
		int16_t port_;
	};
}

