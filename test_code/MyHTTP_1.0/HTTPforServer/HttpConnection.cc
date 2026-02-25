#include "HttpConnection.hh"
#include "HTTPObj.hpp"
#include "common.hpp"
#include <string>
#include <sstream>
#include <iomanip>

oldking::HttpConnection::req_err oldking::HttpConnection::obtain(HTTPReqObj& obj)
{
	HTTPReqObj req;
	
	// get req
	MsgCodec_->obtain(req);

	// check 
	req_err check_result = req_check(req, this);

	if(check_result == req_err::REQ_OK)
		obj = req;

	return check_result;
}

oldking::HttpConnection::res_err oldking::HttpConnection::deliver(HTTPResObj obj)
{
	if(obj.res_code_ == "")
		return res_err::RES_CODE_UNKNOWN;
	if(obj.res_state_ == "")
		return res_err::RES_STATE_ERR;
	if(obj.version_ == "")
		return res_err::RES_VERSION_ERR;

	// length check 
	if(obj.header_.find("Content-Length") == obj.header_.end())
		return res_err::RES_LENGTH_NOT_FOUND;
	if(std::stoul(obj.header_["Content-Length"]) != obj.data_.length())
		return res_err::RES_LENGTH_ERR;

	// add Date
	std::time_t now = std::time(nullptr);
    std::tm gmt{};
    gmtime_r(&now, &gmt);   // Linux 线程安全版本

    std::ostringstream oss;
    oss << std::put_time(&gmt, "%a, %d %b %Y %H:%M:%S GMT");
	obj.header_["Date"] = oss.str();

	// add Server - ?

	MsgCodec_->deliver(obj);	
	return res_err::RES_OK;
}

oldking::HttpConnection::req_err oldking::HttpConnection::req_check(HTTPReqObj& obj, HttpConnection* pthis)
{
	if(obj.req_target_ == "")
	{
		pthis->deliver(res_for_bad_req());
		return REQ_PATH_ERR;
	}
	if(obj.version_ == "")
	{
		pthis->deliver(res_for_bad_req());
		return REQ_VERSION_ERR;
	}

	// ! Not perfect enough !
	if(obj.method_ == "GET")
	{
		return check_get(obj, pthis);
	}
	else if(obj.method_ == "POST")
	{
		return check_post(obj, pthis);
	}
	else 
	{
		return req_err::REQ_METHOD_UNKNOWN;
	}
}

oldking::HttpConnection::req_err oldking::HttpConnection::check_get(HTTPReqObj& obj, HttpConnection* pthis)
{
	// check length & method
	if(obj.header_.find("Content-Length") != obj.header_.end() && std::stoul(obj.header_["Content-Length"]) != obj.data_.length())
	{
		pthis->deliver(res_for_bad_req());
		return req_err::REQ_LENGTH_ERR;
	}

	// version check - ?
	
	return req_err::REQ_OK;	
}

oldking::HttpConnection::req_err oldking::HttpConnection::check_post(HTTPReqObj& obj, HttpConnection* pthis)
{
	// check length & method
	if(obj.header_.find("Content-Length") == obj.header_.end())
	{
		pthis->deliver(res_for_bad_req());
		return req_err::REQ_LENGTH_NOT_FOUND;
	}
	else if(std::stoul(obj.header_["Content-Length"]) != obj.data_.length())
	{
		pthis->deliver(res_for_bad_req());
		return req_err::REQ_LENGTH_ERR;
	}

	return req_err::REQ_OK;
}

oldking::HTTPResObj oldking::HttpConnection::res_for_bad_req()
{
	HTTPResObj err_res;
	err_res.res_code_ = "400";
	err_res.res_state_ = "Bad Request";
	err_res.version_ = "HTTP/1.0";
	err_res.header_["Content-Length"] = "0";
	return err_res;
}



