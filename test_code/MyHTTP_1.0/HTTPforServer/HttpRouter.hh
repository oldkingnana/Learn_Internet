#pragma once 

#include "common.hpp"
#include "HttpConnection.hh"
#include "Socket.hh"

#include <memory>

namespace oldking 
{
	// 业务分发
	class HttpRouter
	{
	public:
		HttpRouter(std::shared_ptr<Socket> sock)
		: sock_(std::make_shared<Socket>(sock))
		{}
		
		~HttpRouter()
		{}

		void run();

	private:
		std::shared_ptr<oldking::Socket> sock_;
	};
}
