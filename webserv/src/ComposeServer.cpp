#include "../includes/Manager.hpp"
#include "../includes/Utils.hpp"

/*
 * 서버내 포트번호를 컨테이너에 담아주는 작업을 하는 함수입니다.
 */
void Manager::composePort()
{
	int port;

	for (size_t i = 0; i < http_block.getServerBlock().size(); i++)
	{
		try
		{
			port = convStoi((http_block.getServerBlock()[i]).getListen()[0]);
			if (port < 0 || port > 65536)
				throw (PrintError());
			web_serv.ports.push_back(port);
		}
		catch(const std::exception& e)
		{
			cerr << e.what() << "port error" << endl;
		}
	}
}

/*
 * 다중 서버에 따른 소켓을 생성해주는 함수입니다.
 */
void Manager::composeSocket()
{
	struct sockaddr_in *server_addr = new sockaddr_in[web_serv.ports.size()];
	int temp;

	for (size_t i = 0; i < web_serv.ports.size(); i++)
	{
		try
		{
			temp = socket(PF_INET, SOCK_STREAM, 0);
			if (temp == -1)
				throw (PrintError());
			int option = 1;
            setsockopt(temp, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(int));
			web_serv.server_socket.push_back(temp);
			memset(&server_addr[i], 0, sizeof(server_addr[i]));
			server_addr[i].sin_family = AF_INET;
			server_addr[i].sin_addr.s_addr = htonl(INADDR_ANY);
			server_addr[i].sin_port = htons(web_serv.ports[i]);
			try
			{
				if (bind(web_serv.server_socket[i], (struct sockaddr*)&server_addr[i], sizeof(server_addr[i])) == -1)
					throw (PrintError());
				try
				{
					if (listen(web_serv.server_socket[i], LISTEN_NUM) == -1)
						throw (PrintError());
					fcntl(web_serv.server_socket[i], F_SETFL, O_NONBLOCK);
				}
				catch(const exception& e)
				{
					cerr << e.what() << "listen() error" << endl;
				}
			}
			catch(const exception& e)
			{
				cerr << e.what() << "bind() error" << endl;
			}
		}
		catch(const exception& e)
		{
			cerr << e.what() << "socket() error" << endl;
		}
	}
	delete[] server_addr;
}

/*
 * Serversocket 구성 및 전처리 작업을 하는 함수입니다.
 */
void Manager::composeServer()
{
	composePort();
	composeSocket();
}
