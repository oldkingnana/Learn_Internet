#include "ChatServer.hh"

int main()
{
	oldking::ChatServer CS(7777);

	CS.start();

	return 0;
}

