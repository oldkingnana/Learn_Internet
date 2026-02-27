#include "HttpServer.hh"
#include "HTTPObj.hpp"
#include "HttpRouter.hh"

void oldking::HttpServer::run()
{
	HTTPReqObj req;
	auto req_result = connector_->obtain(req);
	if(req_result != oldking::HttpConnection::req_err::REQ_OK)
	{
		oldking::MyEasyLog::GetInstance().WriteLog(LOG_WARNING, "HttpRouter", "bad request!");
		return ;
	}
	HTTPResObj res = router_.routing(req);
	connector_->deliver(res);
	return ;
}
