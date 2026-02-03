#include "Protocol.hh"
#include <memory>


//=================== server =====================

// <number>
// <number>\r
// <number>\r\n 
// <number>\r\n<functional part>
// <number>\r\n<functional part>\r\n
// <number>\r\n<functional part>\r\n
// <number>\r\n<functional part>\r\n<number>
// <number>\r\n<functional part>\r\n<number>\r\n
// ...
bool oldking::ProtocolServer::obtain(oldking::Info& info)
{
	if(!sock_->recv(buff_))
		return false;
	
	//check buffer
	if(buff_.size() == 0 || buff_.find_first_of("\r\n") == std::string::npos)
	{
		oldking::MyEasyLog::GetInstance().WriteLog(LOG_WARNING, FILENAME_PROTOCOL, "ProtocolServer->buffer failed");
		return false;
	}

	uint16_t number_area_len = buff_.find_first_of("\r\n");
	uint16_t number = std::stoi(buff_.substr(0, buff_.find_first_of("\r\n")));	

	if(buff_.size() - number_area_len - 2 < number)
	{
		oldking::MyEasyLog::GetInstance().WriteLog(LOG_WARNING, FILENAME_PROTOCOL, "ProtocolServer->buffer failed");
		return false;
	}

	info = deserialize(buff_);
	buff_.erase(0, number_area_len);
	buff_.erase(0, buff_.find_first_of("\r\n"));

	return true;
}

bool oldking::ProtocolServer::deliver(oldking::Result result)
{
	if(!sock_->send(serialize(result)))
		return false;
	return true;
}

std::string oldking::ProtocolServer::serialize(oldking::Result result)
{
	Json::Value v;
	Json::FastWriter writer;

	v["result"] = result.v_;
	std::string s2 = writer.write(v);
	std::string s1 = std::to_string(s2.size());

	return s1 + "\r\n" + s2 + "\r\n";
}

oldking::Info oldking::ProtocolServer::deserialize(std::string msg)
{
	Info info;
	uint16_t number_area_len = msg.find_first_of("\r\n");
	auto buff1 = msg.substr(0, msg.find_first_of("\r\n"));
	msg.erase(0, number_area_len);
	auto buff2 = msg.substr(0, msg.find_first_of("\r\n"));
	msg.erase(0, msg.find_first_of("\r\n"));
	Json::Value v;
	Json::Reader reader;
	bool ok = reader.parse(buff2, v);
	if(ok)
	{
		info.x_ = v["x"].asInt();
		info.y_ = v["y"].asInt();
		info.oper_ = v["oper"].asInt();
	}
	return info;
}

void oldking::ProtocolServer::run()
{
	func_(std::make_shared<ProtocolServer>(*this));
}

//=================== client =====================


bool oldking::ProtocolClient::obtain(struct Result& result)
{
	if(!sock_->recv(buff_))
		return false;
	
	//check buffer
	if(buff_.size() == 0 || buff_.find_first_of("\r\n") == std::string::npos)
	{
		oldking::MyEasyLog::GetInstance().WriteLog(LOG_WARNING, FILENAME_PROTOCOL, "ProtocolServer->buffer failed");
		return false;
	}

	uint16_t number_area_len = buff_.find_first_of("\r\n");
	uint16_t number = std::stoi(buff_.substr(0, buff_.find_first_of("\r\n")));	

	if(buff_.size() - number_area_len - 2 < number)
	{
		oldking::MyEasyLog::GetInstance().WriteLog(LOG_WARNING, FILENAME_PROTOCOL, "ProtocolServer->buffer failed");
		return false;
	}

	result = deserialize(buff_);
	buff_.erase(0, number_area_len);
	buff_.erase(0, buff_.find_first_of("\r\n"));

	return true;
}

bool oldking::ProtocolClient::deliver(struct Info info)
{
	if(!sock_->send(serialize(info)))
		return false;
	return true;
}

std::string oldking::ProtocolClient::serialize(struct Info info)
{
	Json::Value v;
	Json::FastWriter writer;

	v["x"] = info.x_;
	v["y"] = info.y_;
	v["oper"] = info.oper_;

	std::string s2 = writer.write(v);
	std::string s1 = std::to_string(s2.size());

	return s1 + "\r\n" + s2 + "\r\n";
}

oldking::Result oldking::ProtocolClient::deserialize(std::string msg)
{
	Result result;
	uint16_t number_area_len = msg.find_first_of("\r\n");
	auto buff1 = msg.substr(0, msg.find_first_of("\r\n"));
	msg.erase(0, number_area_len);
	auto buff2 = msg.substr(0, msg.find_first_of("\r\n"));
	msg.erase(0, msg.find_first_of("\r\n"));
	Json::Value v;
	Json::Reader reader;
	bool ok = reader.parse(buff2, v);
	if(ok)
	{
		result.v_ = v["result"].asInt();
	}
	return result;
}

void oldking::ProtocolClient::run()
{
	func_(std::make_shared<ProtocolClient>(*this));
}





