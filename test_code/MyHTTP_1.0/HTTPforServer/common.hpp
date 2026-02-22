#pragma once

#include <string>
#include <unordered_map>
#include <unordered_set>

enum ERRCODE 
{
	OK_ERR = 0,
	ADDR_ERR,
	BIND_ERR,
	SOCK_ERR,
	LISTEN_ERR,
	READ_ERR,
	FORK_ERR,
	PROTOCOL_ERR
};

#define MAX_STR 1024

#define FILENAME_SOCK "Socket"
#define FILENAME_TCPSERVER "TcpServer"
#define FILENAME_HTTPFORSERVER "HTTPforServer"
#define FILENAME_CAL "CALCULATORSERVER"

#define WEBROOT "./webroot"

static std::unordered_map<std::string, std::string> mime_map = 
{
	{".html", "text/html; charset=UTF-8"},
	{".css", "text/css"},
    {".js", "application/javascript"},
    {".png", "image/png"},
    {".jpg", "image/jpeg"},
    {".ico", "image/x-icon"}
};
		
static std::unordered_set<std::string> method_list = 
{
	"GET",
	"POST"
};
