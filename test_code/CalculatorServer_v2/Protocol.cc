#include "Protocol.hh"
#include "common.hh"
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
bool oldking::ProtocolServer::obtain(oldking::Info& info)
{
	while(sock_.recv(buff_))
	{
		//check buffer
		if(buff_.size() == 0 || buff_.find_first_of("\r\n") == std::string::npos)
		{
			oldking::MyEasyLog::GetInstance().WriteLog(LOG_WARNING, FILENAME_PROTOCOL, "ProtocolServer->buffer failed");
			continue;
		}
	
		uint16_t number_area_len = buff_.find_first_of("\r\n");
		uint16_t number = std::stoi(buff_.substr(0, buff_.find_first_of("\r\n")));	
	
		if(buff_.size() - number_area_len - 4 < number)
		{
			oldking::MyEasyLog::GetInstance().WriteLog(LOG_WARNING, FILENAME_PROTOCOL, "ProtocolServer->buffer failed");
			continue;
		}
		
		// ok 
		uint16_t msg_len = number_area_len + 2 + number + 2;
		info = deserialize(buff_.substr(0, msg_len));
		buff_.erase(0, msg_len);

		return true;
	}
	return false;
}

bool oldking::ProtocolServer::deliver(oldking::Result result)
{
	if(!sock_.send(serialize(result)))
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
	//std::cout << "number_area_len: " << number_area_len << std::endl;
	uint16_t number = std::stoi(msg.substr(0, number_area_len));
	auto buff = msg.substr(number_area_len + 2, number);
	Json::Value v;
	Json::Reader reader;
	bool ok = reader.parse(buff, v);
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
	func_(this);
}

