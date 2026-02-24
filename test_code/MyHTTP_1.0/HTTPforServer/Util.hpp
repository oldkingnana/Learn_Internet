#pragma once 

#include <ios>
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
			std::ifstream in(path, std::ios::binary);
			// std::cout << "path: " << path << std::endl;
			if(!in.is_open())
				return false;
			
			in.seekg(0, std::ios::end);
			std::streamsize size = in.tellg();
			in.seekg(0, std::ios::beg);

			if(size < 0)
				return false;

			data.resize(size);
			in.read(&data[0], size);

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

