#pragma once 

#include <iostream>
#include <string>
#include <fstream>

namespace oldking 
{
	class Util 
	{
	public:
		Util() = delete;
		~Util() = delete;
		Util(const Util& other) = delete;
		Util& operator=(const Util& other) = delete;
		Util(Util&& other) = delete;
		Util& operator=(Util&& other) = delete;

		static bool ReadFile(const std::string& path, std::string& data)
		{
			std::ifstream in(path);
			if(!in.is_open())
				return false;
			std::string line;
			while(std::getline(in, line))
			{
				data += line;
			}
			in.close();
			return true;
		}

		static bool GetFileExten(const std::string& path, std::string& exten)
		{
			auto index = path.find(".");
			if(index == std::string::npos)
				return false;
			exten = path.substr(index);
			return true;
		}
	};
}

