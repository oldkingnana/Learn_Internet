#pragma once 

#include "HttpMessageCodec.hh"
#include "Socket.hh"
#include "HTTPObj.hpp"

#include <memory>
#include <unordered_set>

namespace oldking 
{
	class HttpConnection 
	{
	public:
		HttpConnection(std::shared_ptr<Socket> sock)
		: MsgCodec_(std::make_unique<oldking::HttpMessageCodec>(sock))
		{}
		
		~HttpConnection()
		{}
		
		bool obtain(HTTPReqObj& obj);

		bool deliver(HTTPResObj obj);

	private:
		std::unique_ptr<oldking::HttpMessageCodec> MsgCodec_;
	};
}


