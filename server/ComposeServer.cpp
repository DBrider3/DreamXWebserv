#include "Server.hpp"
#include "Util.hpp"

/*
 * 서버내 포트번호를 컨테이너에 담아주는 작업을 하는 함수입니다.
 */

vector<int> composePort(Server conf_data)
{
	vector<int> ret;
	int port;

	for (int i = 0; i < conf_data.getServerBlock().size(); i++)
	{
		port = convStoi(conf_data.getServerBlock()[i].getListen()[0]);
		if (port < 0 || port > 65536)
			exit_with_perror("port error\n");
		if (port)
			ret.push_back(port);
	}
	return (ret);
}

/*
 * 다중 서버에 따른 소켓을 생성해주는 함수입니다.
 */

void composeSocket(int *server_socket, vector<int> ports)
{
	struct sockaddr_in *server_addr = new sockaddr_in[ports.size()];

	for (int i = 0; i < ports.size(); i++)
	{
		if ((server_socket[i] = socket(PF_INET, SOCK_STREAM, 0)) == -1)
			exit_with_perror("socket() error\n");
		memset(&server_addr[i], 0, sizeof(server_addr[i]));
		server_addr[i].sin_family = AF_INET;
		server_addr[i].sin_addr.s_addr = htonl(INADDR_ANY);
		server_addr[i].sin_port = htons(ports[i]);
		if (bind(server_socket[i], (struct sockaddr*)&server_addr[i], sizeof(server_addr[i])) == -1)
			exit_with_perror("bind() error\n");
		if (listen(server_socket[i], LISTEN_NUM) == -1)
			exit_with_perror("listen() error\n");
		fcntl(server_socket[i], F_SETFL, O_NONBLOCK);
	}
}

/*
 * Serversocket 구성 및 전처리 작업을 하는 함수입니다.
 */

int composeServer(Server conf_data)
{
    /*port*/
	vector<int>				ports;
	int*					server_socket;
    /*kqueue*/
	int						kq;
	map<int, string> 		clients;
	vector<struct kevent>	change_list;
	struct kevent			event_list[8];
	/*events*/
	int						new_events;
	struct kevent*			curr_event;
	vector<t_header>		headers;
	char					r_header[1024];


	ports = composePort(conf_data);
	server_socket = new int(ports.size());
	composeSocket(server_socket, ports);	 
	return (0);
}  
	 

