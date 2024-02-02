#include "Clients.h"
#include "Server_Network.h"

unsigned short get_random_ID(std::list<std::unique_ptr<Clients>>& _clients, std::unique_ptr<Clients>& _main_client)
{
	unsigned short tmp_int = Tools::rand(10u, 10000u);

	for (auto client = _clients.begin(); client != _clients.end();)
	{
		if (tmp_int == (*client)->m_client_information.m_ID)
			tmp_int = Tools::rand(10u, 10000u);
		else
			client++;
	}

	return tmp_int;
}

int main()
{
	while (true)
	{
		Tools::restart_clock();

		Server->update();
	}

    return EXIT_SUCCESS;

}