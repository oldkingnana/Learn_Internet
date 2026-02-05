#pragma once

#include <string>

enum ERRCODE 
{
	OK = 0,
	ADDR_ERR,
	SOCK_ERR,
	CONNECT_ERR,
	READ_ERR,
	FORK_ERR
};

#define MAX_STR 1024

#define FILENAME_SOCK "Socket"
#define FILENAME_TCPCLIENT "TcpServer"
#define FILENAME_PROTOCOL "PROTOCOL"


