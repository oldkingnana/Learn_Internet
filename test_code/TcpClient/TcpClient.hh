#pragma once 

#include <string>
#include <functional>

#include "TcpBase.hh"
#include "Addr.hh"

namespace oldking
{
	#define FILENAME "TcpClient"

	#define MAX_BUFF 1024

	class TcpClient : public TcpBase
	{
	public:
		TcpClient(const std::string& ip, const int16_t& port)
		: socket_fd_(0)
		, addr_(ip, port)
		, is_init_(false)
		{}

		~TcpClient()
		{
			close();
		}

		bool init();
		
		bool send(const std::string& msg);

		bool recv(std::string& buff);

		bool close();

	private:
		int16_t socket_fd_;
		Addr_in addr_;
		bool is_init_;
	};
}


