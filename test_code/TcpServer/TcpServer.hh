#pragma once 

#include "TcpBase.hh"

#include <string>
#include <cstdint>
#include <functional>

namespace oldking 
{
	#define FILENAME "Tcpserver"

	class TcpServer : public TcpBase 
	{
	public:
		// state type
		#define STARTING 1
		#define RUNNING 2

		// constructer & destructer
		TcpServer(const std::string& server_ip, const uint16_t& server_port, std::function<std::string(std::string)> func)
		: server_ip_(server_ip)
		, server_port_(server_port)
		, func_(func)
		, state_(STARTING)
		, socket_fd_(0)
		{}

		~TcpServer()
		{}

		// API
		bool init();

		void start();

	private:
		std::string server_ip_;
		uint16_t server_port_;
		std::function<std::string(std::string)> func_;
		int16_t state_;
		int16_t socket_fd_;
	};
}




