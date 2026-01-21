#include <string>
#include <iostream>

#include "UdpClient.hh"

int main(int argc, char* argv[])
{
	if(argc < 3)
	{
		std::cerr << "argv failed! program exit!" << std::endl;	
		return -1;
	}

	oldking::UdpClient client;
	client.init(argv[1], std::stoi(argv[2]));

	std::string input_buf;
	std::string output_buf;
	while(1)
	{
		std::getline(std::cin, input_buf);
		if(input_buf != "quit")
		{
			client << input_buf;
			usleep(1000);
			client >> output_buf;
			std::cout << "server say: " << output_buf << std::endl;
		}
		else
		{
			std::cout << "quit!" << std::endl;
			break;
		}
	}
	return 0;
}


