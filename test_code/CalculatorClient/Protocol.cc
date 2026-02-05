#include "Protocol.hh"
#include "myeasylog.hpp"
#include <memory>
#include <string>


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

//=================== client =====================


bool oldking::ProtocolClient::obtain(struct Result& result)
{
	oldking::MyEasyLog::GetInstance().WriteLog(LOG_INFO, FILENAME_PROTOCOL, "obtain start!");

	if(!sock_.recv(buff_))
		return false;
	
	//check buffer
	if(buff_.size() == 0 || buff_.find_first_of("\r\n") == std::string::npos)
	{
		oldking::MyEasyLog::GetInstance().WriteLog(LOG_WARNING, FILENAME_PROTOCOL, "ProtocolServer->buffer failed");
		return false;
	}

	uint16_t number_area_len = buff_.find_first_of("\r\n");
	uint16_t number = std::stoi(buff_.substr(0, buff_.find_first_of("\r\n")));	

	if(buff_.size() - number_area_len - 4 < number)
	{
		oldking::MyEasyLog::GetInstance().WriteLog(LOG_WARNING, FILENAME_PROTOCOL, "ProtocolServer->buffer failed");
		return false;
	}
	
	uint16_t msg_len = number_area_len + 2 + number + 2;
	result = deserialize(buff_.substr(0, msg_len));
	buff_.erase(0, msg_len);

	oldking::MyEasyLog::GetInstance().WriteLog(LOG_INFO, FILENAME_PROTOCOL, "obtain finish!");
	return true;
}

bool oldking::ProtocolClient::deliver(struct Info info)
{
	if(!sock_.send(serialize(info)))
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
	oldking::MyEasyLog::GetInstance().WriteLog(LOG_INFO, FILENAME_PROTOCOL, "deserialize start!");
	Result result;
	uint16_t number_area_len = msg.find_first_of("\r\n");
	uint16_t number = std::stoi(msg.substr(0, number_area_len));
	auto buff = msg.substr(number_area_len + 2, number);
	Json::Value v;
	Json::Reader reader;
	bool ok = reader.parse(buff, v);
	if(ok)
	{
		result.v_ = v["result"].asInt();
	}
	oldking::MyEasyLog::GetInstance().WriteLog(LOG_INFO, FILENAME_PROTOCOL, "deserialize finish!");
	return result;
}

void oldking::ProtocolClient::run()
{
	oldking::MyEasyLog::GetInstance().WriteLog(LOG_INFO, FILENAME_PROTOCOL, "run start!");
	func_(this);
	oldking::MyEasyLog::GetInstance().WriteLog(LOG_INFO, FILENAME_PROTOCOL, "run finish!");
}





