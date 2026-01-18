#pragma once 

#include <string>
#include <functional>

#include "TcpBase.hh"

namespace oldking
{
	#define FILENAME "TcpClient"

	class TcpClient : public TcpBase
	{
	public:
		TcpClient(const std::string& ip, const int16_t& port, std::function<std::string(std::string)> func)
		: ip_(ip)
		, port_(port)
		, socket_fd_(0)
		, func_(func)
		{}

		bool init();

		void start();

		~TcpClient()
		{}

	private:
		std::string ip_;
		int16_t port_;
		int16_t socket_fd_;
		std::function<std::string(std::string)> func_;
	};
}


