#pragma once 

#include <string>
#include <unordered_map>

namespace oldking
{
	// <method><space><request-target><space><version>\r\n
	// <key>:<space><value>\r\n 
	// <key>:<space><value>\r\n 
	// ... 
	// \r\n
	// <data>
	
	// Request Object
	struct HTTPReqObj 
	{
	public:
		HTTPReqObj()
		{}

		~HTTPReqObj()
		{}

		std::string method_;
		std::string req_target_;
		std::string version_;
		std::unordered_map<std::string, std::string> header_;
		std::string data_;
	};

	// <version><space><res_code><space><res_state>\r\n 
	// <key>:<space><value>
	// <key>:<space><value>
	// ...
	// \r\n 
	// <data>

	// Response Object
	struct HTTPResObj
	{
	public:
		HTTPResObj()
		{}

		~HTTPResObj()
		{}

		std::string version_;
		std::string res_code_;
		std::string res_state_;
		std::unordered_map<std::string, std::string> header_;
		std::string data_;
	};
}

