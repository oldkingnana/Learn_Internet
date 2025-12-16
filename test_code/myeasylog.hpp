#pragma once

#include <iostream>
#include <cstring>
#include <string>
#include <stdio.h>
#include <time.h>

namespace oldking
{
	typedef std::string log_level_t;
	#define SIMP_LEVEL "SIMPLE"
	#define WARN_LEVEL "WARNNING"
	#define ERR_LEVEL "ERROR"

	class my_easy_log
	{
	public:
		// my_easy_log(const int& max_line, const std::string& file)
		my_easy_log(const std::string& file)
		// _max_line(max_line)
		: _linecnt(0)
		, _plogfile(nullptr)
		, _file(file)
		{}

		bool LoadFile()
		{
			FILE* tmp = fopen(_file.c_str(), "a+");
			if(tmp != nullptr)
			{
				std::cout << "LoadFile!" <<std::endl;
				_plogfile = tmp;
				return true;
			}
			else
			{
				std::cerr << "fopen err: " << strerror(errno) << std::endl;
				return false;
			}
		}

		void Write(const std::string& message, const log_level_t& level)
		{
			// std::cout << "Write!" << std::endl;

			if(_plogfile == nullptr)
			{
				std::cerr << "_plogfile is nullptr" << std::endl;
				return ;
			}

			time_t now = time(nullptr);
			struct tm* tm_info = localtime(&now);

			char buffer[64];
			strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", tm_info);

			fprintf(_plogfile, "time: %s ,    level: %s ,   message: %s\n", buffer, level.c_str(), message.c_str());
			_linecnt++;
		}

		// void SetMax(const unsigned long long& max_number)
		// {
		// 	_max_line = max_number;
		// }

		unsigned long long GetLineCount()
		{
			return _linecnt;
		}

		~my_easy_log()
		{
			if(_plogfile == nullptr) return ;
			fclose(_plogfile);
		}

	private:
		// const unsigned long long _max_line;
		unsigned long long _linecnt;
		FILE* _plogfile;
		std::string _file;
	};
}

