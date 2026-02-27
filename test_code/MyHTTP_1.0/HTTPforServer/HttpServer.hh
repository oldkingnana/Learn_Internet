#pragma once 

#include "HttpConnection.hh"
#include "HttpRouter.hh"
#include "Socket.hh"
#include <memory>

namespace oldking 
{
	class HttpServer 
	{
	public:
		HttpServer(std::shared_ptr<Socket> sock, HttpRouter router)
		: connector_(std::make_unique<HttpConnection>(sock))
		, router_(router)
		{}

		~HttpServer()
		{}

		void run();	

	private:
		std::unique_ptr<HttpConnection> connector_;
		HttpRouter router_;
	};
}

