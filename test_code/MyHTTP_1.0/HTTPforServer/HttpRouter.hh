#pragma once 

#include "common.hpp"
#include "HttpConnection.hh"
#include "Socket.hh"

#include <memory>

namespace oldking 
{
	class HttpRouter
	{
	public:
		HttpRouter(std::shared_ptr<Socket> sock)
		: connection_(std::make_unique<HttpConnection>(sock))
		{}
		
		~HttpRouter()
		{}

		void run();

	private:
		std::unique_ptr<oldking::HttpConnection> connection_;
	};
}
