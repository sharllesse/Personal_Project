#include "Clients.h"
#include "Server_Network.h"

int main()
{
	while (true)
	{
		Tools::restart_clock();

		Server->update();
	}

    return EXIT_SUCCESS;
}