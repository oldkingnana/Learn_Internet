#include "HttpMessageCodec.hh"
#include "common.hh"
#include <memory>
#include <string>
#include <unordered_map>
#include "HTTPObj.hpp"
#include "myeasylog.hpp"

// ---------------public API---------------

bool oldking::HttpMessageCodec::obtain(oldking::HTTPReqObj& obj)
{
	oldking::MyEasyLog::GetInstance().WriteLog(LOG_INFO, FILENAME_HTTPFORSERVER, "obtain!");
	while(true)
	{
		if(!sock_->recv(buff_))
		{
			oldking::MyEasyLog::GetInstance().WriteLog(LOG_WARNING, FILENAME_HTTPFORSERVER, "socket recv err");
			return false;
		}
		
		if(buff_.find("\r\n\r\n") != std::string::npos)
			break;
	}

	std::string req_and_header = buff_.substr(0, buff_.find("\r\n\r\n") + 2);
	std::cout << "req_and_header->" << req_and_header;
	buff_.erase(0, buff_.find("\r\n\r\n") + 2);

	// get request line
	uint64_t request_line_len = req_and_header.find("\r\n");
	if(request_line_len == std::string::npos)
	{
		oldking::MyEasyLog::GetInstance().WriteLog(LOG_FATAL, FILENAME_HTTPFORSERVER, "req or header err!");
		exit(PROTOCOL_ERR);
	}
	std::string request_line = req_and_header.substr(0, request_line_len);
	std::cout << "request->" << request_line;
	req_and_header.erase(0, request_line_len + 2);
	deserialize_req_line(request_line, obj);
	oldking::MyEasyLog::GetInstance().WriteLog(LOG_INFO, FILENAME_HTTPFORSERVER, std::string("get request line: ") + "\n" + 
																				 "method: " + obj.method_ + "\n" +
																				 "req_target: " + obj.req_target_ + "\n" + 
																				 "version: " + obj.version_);

	// get header 
	uint64_t header_len = req_and_header.length();
	if(header_len == 0)
	{
		oldking::MyEasyLog::GetInstance().WriteLog(LOG_INFO, FILENAME_HTTPFORSERVER, "obtain finish");
		return true; 
	}
	else 
	{
		std::string header = req_and_header;
		std::cout << "header->" << header;
		req_and_header.clear();
		deserialize_header(header, obj);
		oldking::MyEasyLog::GetInstance().WriteLog(LOG_INFO, FILENAME_HTTPFORSERVER, std::string("get header: "));
		for(const auto& it : obj.header_)
		{
			oldking::MyEasyLog::GetInstance().WriteLog(LOG_INFO, FILENAME_HTTPFORSERVER, it.first + ": " + it.second);
		}
	}
	
	// check data length
	if(obj.header_.find("Content-Length") != obj.header_.end() && 
	   obj.header_["Content-Length"] != "0")
	{
		unsigned long data_len = static_cast<unsigned long>(std::stoi(obj.header_["Content-Length"]));
		while(true)
		{
			if(buff_.length() < data_len)
			{
				if(!sock_->recv(buff_))
				{
					oldking::MyEasyLog::GetInstance().WriteLog(LOG_WARNING, FILENAME_HTTPFORSERVER, "socket recv err");
					return false;
				}
			}
			else 
				break;
		}
		// get data 
		deserialize_data(buff_.substr(0, data_len), data_len, obj);
		buff_.erase(0, data_len);
	}
	
	oldking::MyEasyLog::GetInstance().WriteLog(LOG_INFO, FILENAME_HTTPFORSERVER, "obtain finish");
	return true;
}

bool oldking::HttpMessageCodec::deliver(oldking::HTTPResObj obj)
{
	std::string msg = serialize(obj);
	if(!sock_->send(msg))
		return false;
	return true;
}

void oldking::HttpMessageCodec::run()
{
	func_(this);
}

// ------serialize & deserialize API-------

std::string oldking::HttpMessageCodec::serialize(oldking::HTTPResObj obj)
{
	std::string state_line = obj.version_ + " " + obj.res_code_ + " " + obj.res_state_ + "\r\n"; 

	std::string header;
	for(const auto& it : obj.header_)
	{
		header += std::string(it.first + ": " + it.second + "\r\n");
	}

	return std::string(
			state_line + 
			header + 
			"\r\n" + 
			obj.data_
			);
}
		
void oldking::HttpMessageCodec::deserialize_req_line(std::string req_line, oldking::HTTPReqObj& obj)
{
	obj.method_ = get_method_(req_line);
	obj.req_target_ = get_req_target_(req_line);
	obj.version_ = get_version_(req_line);
}
		
void oldking::HttpMessageCodec::deserialize_header(std::string header, oldking::HTTPReqObj& obj)
{
	obj.header_ = get_header_(header);
}
		
void oldking::HttpMessageCodec::deserialize_data(std::string buff, uint64_t length, HTTPReqObj& obj)
{
	oldking::MyEasyLog::GetInstance().WriteLog(LOG_INFO, FILENAME_HTTPFORSERVER, "get data: " + buff.substr(0, length));
	buff.erase(0, 2);
	obj.data_ = buff.substr(0, length);
}

// --------------about detail--------------

// ~~~~~~~~~~~about request line~~~~~~~~~~~
std::string oldking::HttpMessageCodec::get_method_(std::string& request_line)
{
	uint16_t method_len = request_line.find(" ");
	std::string ret = request_line.substr(0, method_len);
	request_line.erase(0, method_len + 1);
	return ret;
}

std::string oldking::HttpMessageCodec::get_req_target_(std::string& request_line)
{
	uint16_t req_target_len = request_line.find(" ");
	std::string ret = request_line.substr(0, req_target_len);
	request_line.erase(0, req_target_len + 1);
	return ret;
}

std::string oldking::HttpMessageCodec::get_version_(std::string& request_line)
{
	std::string ret = std::move(request_line);
	return ret;
}
		
std::unordered_map<std::string, std::string> oldking::HttpMessageCodec::get_header_(std::string& msg)
{
	std::unordered_map<std::string, std::string> ret;
	while(true)
	{
		if(msg.empty())
			return ret;

		// get key-value
		uint64_t kv_len = msg.find("\r\n");
		std::string kv;
		if(kv_len != std::string::npos)
		{
			kv = msg.substr(0, kv_len);
			msg.erase(0, kv_len + 2);
		}
		else 
		{
			exit(PROTOCOL_ERR);
		}

		// get key
		uint64_t key_len = kv.find(": ");
		std::string key;
		if(key_len != std::string::npos)
		{
			key = kv.substr(0, key_len);
			kv.erase(0, key_len + 2);
		}
		else 
		{
			exit(PROTOCOL_ERR);
		}

		// get value
		std::string value = std::move(kv);
		oldking::MyEasyLog::GetInstance().WriteLog(LOG_INFO, FILENAME_HTTPFORSERVER, std::string("get kv->  ") + key + ": " + value);
		// insert to map 
		ret[key] = value;
	}
}


