#include "HttpRouter.hh"
#include "HTTPObj.hpp"
#include "Util.hpp"

void oldking::HttpRouter::run()
{
	HTTPReqObj req;
	if(!connection_->obtain(req))
		return ;

	oldking::HTTPResObj res;
	// check path 
	if(req.req_target_ == "/")
	{
		if(oldking::Util::ReadFile(WEBROOT + std::string("/index.html"), res.data_))
		{
			res.header_["Content-Type"] = "text/html; charset=UTF-8";
			res.res_code_ = "200";
			res.res_state_ = "OK";
		}
		else 
		{
			// err
			oldking::Util::ReadFile(WEBROOT + std::string("/404.html"), res.data_);
			res.res_code_ = "404";
			res.res_state_ = "Not Found";
		}
	}
	else 
	{
		std::string exten;
		if(oldking::Util::GetFileExten(req.req_target_, exten))
		{
			if(oldking::Util::ReadFile(WEBROOT + req.req_target_, res.data_))
			{
				res.header_["Content-Type"] = mime_map[exten];
				res.res_code_ = "200";
				res.res_state_ = "OK";
			}
			else 
			{
				// err
				oldking::Util::ReadFile(WEBROOT + std::string("/404.html"), res.data_);
				res.res_code_ = "404";
				res.res_state_ = "Not Found";
			}
		}
		else 
		{
			// err 
			oldking::Util::ReadFile(WEBROOT + std::string("/404.html"), res.data_);
			res.res_code_ = "404";
			res.res_state_ = "Not Found";
		}
	}

	res.version_ = "HTTP/1.0";
	res.header_["Connection"] = "close";
 
	res.header_["Content-Length"] = std::to_string(res.data_.length());
	
	connection_->deliver(res);
	return ;
}

