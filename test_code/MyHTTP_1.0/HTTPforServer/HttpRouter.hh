#pragma once 

#include "HTTPObj.hpp"
#include "common.hpp"
#include "HttpConnection.hh"
#include "Socket.hh"

#include <functional>
#include <memory>
#include <map>

namespace oldking 
{
	struct RouterKey
	{
		RouterKey() = default;
		~RouterKey() = default;

		bool operator==(const RouterKey& other) const 
		{
			return method == other.method && path == other.path;
		}
		
		bool operator<(const RouterKey& other) const 
		{
			return method < other.method ? true : path < other.path;
		}

		std::string method;
		std::string path;
	};

	class less 
	{
	public:
		bool operator()(const RouterKey& k1, const RouterKey& k2) const 
		{
			return k1 < k2;
		}
	};

	// 业务分发
	class HttpRouter
	{
	public:
		HttpRouter()
		: func_map_({})
		{}
		
		HttpRouter(const HttpRouter& other)
		: func_map_(other.func_map_)
		{}

		~HttpRouter()
		{}

		void addRouter(
				const RouterKey& key,  
				std::function<HTTPResObj(const HTTPReqObj&)> func
				);

		HTTPResObj routing(const HTTPReqObj& req);

	private:
		std::map<RouterKey, std::function<HTTPResObj(const HTTPReqObj&)>, less>func_map_;
	};
}
