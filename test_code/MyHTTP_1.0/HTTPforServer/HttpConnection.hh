#pragma once 

#include "HttpMessageCodec.hh"
#include "Socket.hh"
#include "HTTPObj.hpp"

#include <functional>
#include <memory>
#include <unordered_map>

namespace oldking 
{
	class HttpConnection 
	{
	public:
		enum req_err 
		{
			REQ_METHOD_UNKNOWN = 0, 
			REQ_OK,
			REQ_LENGTH_ERR,
			REQ_LENGTH_NOT_FOUND,
			REQ_PATH_ERR,
			REQ_VERSION_ERR
		};

		enum res_err 
		{
			RES_CODE_UNKNOWN = 0,
			RES_OK,
			RES_STATE_ERR,
			RES_VERSION_ERR,
			RES_LENGTH_ERR,
			RES_LENGTH_NOT_FOUND,
		};
	
	public:
		HttpConnection(std::shared_ptr<Socket> sock)
		: MsgCodec_(std::make_unique<oldking::HttpMessageCodec>(sock))
		{}
		
		~HttpConnection()
		{}
		
		req_err obtain(HTTPReqObj& obj);

		res_err deliver(HTTPResObj obj);

	private:
		static req_err req_check(HTTPReqObj& obj, HttpConnection* pthis);
		
		static req_err check_get(HTTPReqObj& obj, HttpConnection* pthis);

		static req_err check_post(HTTPReqObj& obj, HttpConnection* pthis);

		static HTTPResObj res_for_bad_req();

	private:
		std::unique_ptr<oldking::HttpMessageCodec> MsgCodec_;
	};
}


