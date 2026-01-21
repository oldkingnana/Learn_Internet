#pragma once

#include <string>
#include <sys/socket.h>
#include <strings.h>

#include "myeasylog.hpp"
#include "UdpBase.hh"

namespace oldking
{
	#define MSG_MAX 1024

	class UdpServer : UdpBase 
	{
	public:
		UdpServer(int32_t port)
		: port_(port)
		, is_running_(false)
		{
			oldking::MyEasyLog::GetInstance().WriteLog(LOG_FATAL, "UdpServer", "UdpServer constructer");
		}

		~UdpServer()
		{
			oldking::MyEasyLog::GetInstance().WriteLog(LOG_FATAL, "UdpServer", "UdpServer distructer");
		}

		bool init();

		void send(const std::string& msg, const std::string& dst_ip, const int16_t& dst_port);

		void recv(std::string& buff, std::string& sender_ip, int16_t& sender_port);

	private:
		int32_t port_;
		int16_t socket_fd_;

		bool is_running_;
	};
}


