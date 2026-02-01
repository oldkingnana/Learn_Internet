#include "common.hh"
#include "mutex.hpp"
#include "myeasylog.hpp"
#include "TcpClient.hh"

#include <cstdio>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string>
#include <arpa/inet.h>
#include <unistd.h>

bool oldking::TcpClient::init()
{
	if(is_init_)
		return is_init_;

	socket_fd_ = socket(AF_INET, SOCK_STREAM, 0);	
	if(socket_fd_ < 0)
	{
		oldking::MyEasyLog::GetInstance().WriteLog(LOG_FATAL, FILENAME, "socket err");
		return false;
	}

	struct sockaddr_in addr_in = addr_.getAddr_in();
	socklen_t len = sizeof(addr_in);

	if(connect(socket_fd_, (struct sockaddr*)(&addr_in), len) < 0)
	{
		oldking::MyEasyLog::GetInstance().WriteLog(LOG_FATAL, FILENAME, "connect err");
		return is_init_;
	}

	is_init_ = true;
	return is_init_;
}

bool oldking::TcpClient::send(const std::string& msg)
{
	if(!is_init_)
		return is_init_;

	if(::send(socket_fd_, msg.c_str(), msg.length(), MSG_NOSIGNAL) < 0)
	{
		oldking::MyEasyLog::GetInstance().WriteLog(LOG_WARNING, FILENAME, "send err");
		return false;
	}

	return true;
}

bool oldking::TcpClient::recv(std::string& buff)
{
	if(!is_init_)
		return is_init_;

	char cbuff[MAX_BUFF];	
	int16_t read_result = ::read(socket_fd_, cbuff, sizeof(cbuff));

	if(read_result < 0)
	{
		oldking::MyEasyLog::GetInstance().WriteLog(LOG_WARNING, FILENAME, "read err");
		return false;
	}
	else if(read_result == 0)
	{
		oldking::MyEasyLog::GetInstance().WriteLog(LOG_WARNING, FILENAME, "read err, fd is closed");
		return false;
	}
	else 
	{
		cbuff[read_result] = 0;
		buff = cbuff;
	}

	return true;
}

bool oldking::TcpClient::close()
{
	if(!is_init_)
		return is_init_;
	::close(socket_fd_);
	is_init_ = false;

	return true;
}
