#include "HttpRouter.hh"
#include "HTTPObj.hpp"
#include "HttpConnection.hh"
#include "Util.hpp"
#include "myeasylog.hpp"

void oldking::HttpRouter::addRouter(
		const oldking::RouterKey& key,  
		std::function<HTTPResObj(const oldking::HTTPReqObj&)> func)
{
	func_map_.insert({key, func});
	return;
}

oldking::HTTPResObj oldking::HttpRouter::routing(const oldking::HTTPReqObj& req)
{
	RouterKey k;
	k.method = req.method_;
	k.path = req.req_target_;

	// 动态路由
	if(func_map_.find(k) != func_map_.end())
	{
		return func_map_[k](req);
	}

	// 静态路由
	oldking::HTTPResObj res;
	std::string exten;
	if(req.req_target_ == "/" && oldking::Util::ReadFile(WEBROOT + std::string("/index.html"), res.data_))
	{
		res.header_["Content-Type"] = "text/html; charset=UTF-8";
		res.res_code_ = "200";
		res.res_state_ = "OK";
	}
	else if(oldking::Util::GetFileExten(req.req_target_, exten) && oldking::Util::ReadFile(WEBROOT + req.req_target_, res.data_))
	{
		res.header_["Content-Type"] = mime_map[exten];
		res.res_code_ = "200";
		res.res_state_ = "OK";
	}
	else 
	{
		// 404 
		oldking::Util::ReadFile(WEBROOT + std::string("/404.html"), res.data_);
		res.res_code_ = "404";
		res.res_state_ = "Not Found";
	}
	res.version_ = "HTTP/1.0";
	res.header_["Connection"] = "close";
 
	res.header_["Content-Length"] = std::to_string(res.data_.length());
		
	return res;
}


