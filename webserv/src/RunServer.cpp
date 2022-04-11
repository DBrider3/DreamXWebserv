#include "../includes/Utils.hpp"
#include "../includes/Manager.hpp"
#include "../includes/ClientControl.hpp"
#include <errno.h>
/*
 *
 */

void disconnectSocket(int socket_fd) //고쳐야함 소멸자불러야함
{
	cout << "Disconnet Socket!!  socket_fd : " << socket_fd << endl;
	close(socket_fd);
}

/*
 * 현재 fd가 서버소켓인지 검사하는 함수입니다.
 */

int checkSocket(int curr_fd, vector<int> server_socket)
{
	for (size_t i = 0; i < server_socket.size(); i++)
		if (server_socket[i] == curr_fd)
			return (1);
	return (0);
}

/*
 * 소켓을 기반으로 list에 특정 이벤트 생성
 */

void changeEvents(vector<struct kevent>& change_list, uintptr_t ident, int16_t filter,
		uint16_t flags, uint32_t fflags, intptr_t data, void *udata)
{
	struct kevent temp_event;

	EV_SET(&temp_event, ident, filter, flags, fflags, data, udata);
	change_list.push_back(temp_event);
}


/*
 * header를 초기화하고, client_socket을 저장하는 함수입니다.
 */

void ClientControl::initRequestMsg()
{
	getRequest().method = "";
	getRequest().uri = "";
	getRequest().query_str = "";
	getRequest().version = "";
	return ;
}


void sendStatePage(int socket_fd, string state_flag, string state_str)
{
	struct stat		st;
	string			local_uri;
	string			body; //html읽은 내용 담을 변수
	char			buf[10];
	char			r_header[1024];
	size_t			ct_len;
	int				bodyfd;
	int				n;
	stringstream	ss;

	local_uri = "./state_pages/" + state_flag + ".html";
	stat(local_uri.c_str(), &st);
	ct_len = st.st_size;

	bodyfd = open(local_uri.c_str(), O_RDONLY);

	n = 0;
	while ((n = read(bodyfd, buf, sizeof(buf) - 1)) > 0)
	{
		buf[9] = '\0';
		ss << buf;
		body += ss.str();
		ss.str("");
		memset(buf, 0, 10);
	}
	sprintf(r_header, STATE_FMT, state_flag.c_str(), state_str.c_str(), ct_len, "text/html", body.c_str());
	write(socket_fd, r_header, strlen(r_header));
	disconnectSocket(socket_fd);
}

void ClientControl::sendRedirectPage()
{
	char			r_header[1024];

	sprintf(r_header, REDIRECT_FMT, getResponse().state_flag.c_str(), getResponse().state_str.c_str(), getResponse().redirect_uri.c_str());
	write(getClientFd(), r_header, strlen(r_header));
	disconnectSocket(getClientFd());
}

/*
 * sever_socket을 토대로 client_socket을 구성하는 함수입니다.
 */

int ClientControl::setClientsocket(vector<struct kevent> &change_list, uintptr_t server_socket, ServerBlock server_block)
{
	/* accept new client */
	int client_socket;

   	if ((client_socket = accept(server_socket, NULL, NULL)) == -1)
	{
		sendStatePage(server_socket, "500", "Internal server error"); //클라이언트 생성실패
		return (-1);
	}
	cout << "accept new client: " << client_socket << endl;
	fcntl(client_socket, F_SETFL, O_NONBLOCK);

	/* add event for client socket - add read && write event */
	changeEvents(change_list, client_socket, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
	changeEvents(change_list, client_socket, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);

	setServerBlock(server_block);
	setPort(server_block.getListen()[0]);
	setClientFd(client_socket);
	initRequestMsg();
	return (0);
}

/*
 * curr_fd가 client_socket목록에 있는지 체크하는 함수입니다.
 */

vector<ClientControl>::iterator findClient(vector<ClientControl> client_control, int curr_fd)
{
	vector<ClientControl>::iterator it;

	for (it = client_control.begin(); it != client_control.end(); it++)
		if (it->getClientFd() == curr_fd)
			return (it);
	return (it);
}

/*
 * 버퍼에 담아둔 request를 파싱하여 구조체에 담아주는 작업을 하는 함수입니다.
 */

void ClientControl::parseRequest(string request)
{
	stringstream ss;
	vector<string> result; //요청메시지가 한 줄 한 줄 저장되는 변수
	vector<string>::iterator it;
	char *temp;
/*
 * Startline 파싱
 */
	result = split(request, '\n');
	getRequest().method = strtok(const_cast<char*>(result[0].c_str()), " ");
	getRequest().uri = strtok(NULL, " ");
	getRequest().version = strtok(NULL, "\n");

	if (getRequest().uri.size() > 8190)
	{
		getResponse().state_flag = "414";
		getResponse().state_str = "Request-URI too long";
		return ;
	}
	if ((temp = strtok(const_cast<char*>(getRequest().uri.c_str()), "?")) != NULL)
	{
		getRequest().query_str = strtok(NULL, "\0");
		getRequest().uri = temp;
	}

/*
 * Header 파싱
 */
	for (it = result.begin() + 1; it->size() > 1; it++)
	{
		stringstream ss(*it);
		string key;
		vector<string> val;
		string val_tmp;

		getline(ss, key, ':');
		ss.get(); //인덱스 +1 -> 콜론 뒤 공백에서 다음 인덱스로 이동

		for (int i = 0; getline(ss, val_tmp, ' '); i++)
			val.push_back(val_tmp);
		getRequest().header[key] = val;
	}

/*
 * Body 파싱
 */

	if (convStoi(*(getRequest().header["Content-Length"].begin())) > convStoi(getServerBlock().getClientBodySize()))
	{
		getResponse().state_flag = "413";
		getResponse().state_str = "Payload Too Large";
		return ;
	}
	while (++it != result.end())
		getRequest().body.push_back(*it);
}

/*
 * curr_fd가 전달하는 내용을 버퍼에 담아주는 함수입니다.
 */

void ClientControl::readRequest()
{
	/*
	** read data from client
	*/
	char buf[10];
	stringstream ss;
	string msg;
	int n;
	
	n = 0;
	while ((n = read(getClientFd(), buf, sizeof(buf) - 1)) > 0)
	{
		// if (!buf[0])
		// {
		// 	n = 0;
		// 	break;
		// }
		buf[9] = '\0';
		ss << buf;
	}
	msg += ss.str();

	// if (n <= 0)
	// {
	//	sendStatePage(curr_event->ident, "400", "Bad request"); //의문.3 에러 처리 방법이 명확하게 떠오르지 않음.. ????
	//     if (n < 0)
	//         cerr << "client read error!" << endl;
	// 	cout << "1\n";
	//     disconnectSocket(curr_fd);
	// }
	// else
	parseRequest(msg);
}

int ClientControl::checkMethod(vector<string> method_limit)
{
	for (vector<string>::iterator it = method_limit.begin(); it != method_limit.end(); it++)
		if (getRequest().method == *it)
			return (1);
	return (0);
}

/*
 * 서버 실행하는 함수입니다.
 */

void Manager::runServer()
{
	int 					kq;
	int						idx;
	map<int, string>        clients; // map for client socket:data
	vector<struct kevent>   change_list; // kevent vector for changelist
	struct kevent           event_list[8]; // kevent array for eventlistcompRespo

	int                     new_events;
	struct kevent*          curr_event;
	vector<ClientControl>	client_control;
	vector<ClientControl>::iterator it;

	try
	{
		kq = kqueue();
		if (kq == -1)
			throw(PrintError());
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << "kqueue() error" << endl;
		exit(42);
	}

	/*server_socket 연결을 위한 읽기 이벤트 등록*/
	for (size_t i = 0; i < web_serv.ports.size(); i++)
		changeEvents(change_list, web_serv.server_socket[i], EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);

	while (1)
	{
		new_events = kevent(kq, &change_list[0], change_list.size(), event_list, 8, NULL);

		if (new_events == -1)
			sendStatePage(curr_event->ident, "500", "Internal server error"); //kq관리 실패
		change_list.clear();
		idx = 0;
		for (int i = 0; i < new_events; ++i)
		{
			curr_event = &event_list[i];
			if (curr_event->flags & EV_ERROR)
			{
				if (checkSocket(curr_event->ident, web_serv.server_socket))
				{
					sendStatePage(curr_event->ident, "500", "Internal server error"); //의문.1 서버 에러시, 어디로 명확하게 전달되는 것이 확인되지 않음. ????
				}	//의문 .2 서버 에러시, 서버를 종료시켜야하나 ????
				else
				{
					sendStatePage(curr_event->ident, "400", "Bad Request");
					it = findClient(client_control, curr_event->ident);
					client_control.erase(it);
				}
			}
			else if (curr_event->filter == EVFILT_READ)
			{
				if (checkSocket(curr_event->ident, web_serv.server_socket))
				{
					client_control.push_back(ClientControl());
					if (client_control[i].setClientsocket(change_list, curr_event->ident, http_block.getServerBlock()[i]))
						client_control.pop_back();
				}
 				else if ((it = findClient(client_control, curr_event->ident)) != client_control.end())
				{
					it->readRequest();
					//check_msg(request_msgs[idx]);
				}
			}
			else if (curr_event->filter == EVFILT_WRITE)
			{
				if ((it = findClient(client_control, curr_event->ident)) != client_control.end())
				{
					if (!(it->getResponse().state_flag.empty()))  //it->readRequest();했을 때 에러가 있다면 먼저 띄워줌
						sendStatePage(it->getClientFd(), it->getResponse().state_flag, it->getResponse().state_str);
					if (it->checkMethod(http_block.getLimitExcept()))
					{
						it->processMethod();
						if (!(it->getResponse().state_flag.empty()))
						{
							if (it->getResponse().state_flag == "301")
								it->sendRedirectPage();
							else
								sendStatePage(it->getClientFd(), it->getResponse().state_flag, it->getResponse().state_str);
						}
					}
					else
						sendStatePage(it->getClientFd(), "403", "Forbidden");
					client_control.erase(it);//iterator로 삭제 가능 
				}
			}
		}
	}
}