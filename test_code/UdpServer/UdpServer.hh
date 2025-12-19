#pragma once

#include <functional>
#include <string>
#include <sys/socket.h>
#include <strings.h>

#include "myeasylog.hpp"
#include "UdpBase.hh"

namespace oldking
{
	class UdpServer : UdpBase 
	{
	public:
		UdpServer(int32_t port, std::function<std::string(std::string)> func)
		: port_(port)
		, func_(func)
		, is_running_(false)
		{
			oldking::MyEasyLog::GetInstance().WriteLog(LOG_FATAL, "UdpServer", "UdpServer constructer");
		}

		~UdpServer()
		{
			oldking::MyEasyLog::GetInstance().WriteLog(LOG_FATAL, "UdpServer", "UdpServer distructer");
		}

		bool init();

		void start();

	private:
		int32_t port_;
		std::function<std::string(std::string)> func_;
		int16_t socket_fd_;

		bool is_running_;
	};
}


