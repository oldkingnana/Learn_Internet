#include "HttpConnection.hh"
#include "HTTPObj.hpp"
#include "common.hpp"
#include <string>
#include <sstream>
#include <iomanip>

bool oldking::HttpConnection::obtain(HTTPReqObj& obj)
{
	HTTPReqObj req;
	
	// get req
	MsgCodec_->obtain(req);

	// check
	bool is_OK = true;

	// check length & method
	if(req.header_.find("Content-Length") != req.header_.end() && std::stoi(req.header_["Content-Length"]) >= 0)
		is_OK = false;

	if(method_list.find(req.method_) == method_list.end())
		is_OK = false;

	if(is_OK == false)
	{
		HTTPResObj err_res;
		err_res.res_code_ = "400";
		err_res.res_state_ = "Bad Request";
		err_res.version_ = "HTTP/1.0";
		err_res.header_["Content-Length"] = "0";
		deliver(err_res);
	}

	// version check - ? 

	if(is_OK)
	{
		obj = req;
	}

	return is_OK;
}

bool oldking::HttpConnection::deliver(HTTPResObj obj)
{
	if(obj.res_code_ == "")
		return false;
	if(obj.res_state_ == "")
		return false;
	if(obj.version_ == "")
		return false;

	// length check 
	if(obj.header_.find("Content-Length") == obj.header_.end())
		return false;
	if(std::stoi(obj.header_["Content-Length"]) != static_cast<int>(obj.data_.length()))
		return false;

	// add Date
	std::time_t now = std::time(nullptr);
    std::tm gmt{};
    gmtime_r(&now, &gmt);   // Linux 线程安全版本

    std::ostringstream oss;
    oss << std::put_time(&gmt, "%a, %d %b %Y %H:%M:%S GMT");
	obj.header_["Date"] = oss.str();

	// add Server - ?

	MsgCodec_->deliver(obj);	
	return true;
}
