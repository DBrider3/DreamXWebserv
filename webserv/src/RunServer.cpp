#include "../includes/Utils.hpp"
#include "../includes/Manager.hpp"
#include "../includes/ComposeResponse.hpp"
#include <errno.h>
/*
 *
 */

void disconnect_client(int client_fd) //고쳐야함 소멸자불러야함
{
	cout << "client disconnected: " << client_fd << endl;
	close(client_fd);
}

/*
 * 현재 fd가 서버소켓인지 검사하는 함수입니다.
 */

int check_socket(int curr_fd, vector<int> server_socket)
{
	for (size_t i = 0; i < server_socket.size(); i++)
		if (server_socket[i] == curr_fd)
			return (1);
	return (0);
}

/*
 * 소켓을 기반으로 list에 특정 이벤트 생성
 */

void change_events(vector<struct kevent>& change_list, uintptr_t ident, int16_t filter,
		uint16_t flags, uint32_t fflags, intptr_t data, void *udata)
{
	struct kevent temp_event;

	EV_SET(&temp_event, ident, filter, flags, fflags, data, udata);
	change_list.push_back(temp_event);
}


/*
 * header를 초기화하고, client_socket을 저장하는 함수입니다.
 */

void ComposeResponse::initRequestMsg()
{
	getRequest().method = "";
	getRequest().uri = "";
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
	disconnect_client(socket_fd);
}

void ComposeResponse::sendRedirectPage()
{
	char			r_header[1024];

	sprintf(r_header, REDIRECT_FMT, getResponse().state_flag.c_str(), getResponse().state_str.c_str(), getResponse().redirect_uri.c_str());
	write(getClientFd(), r_header, strlen(r_header));
	disconnect_client(getClientFd());
}

/*
 * sever_socket을 토대로 client_socket을 구성하는 함수입니다.
 */

void ComposeResponse::setClientsocket(vector<struct kevent> &change_list, uintptr_t server_socket, ServerBlock server_block)
{
	/* accept new client */
	int client_socket;

   if ((client_socket = accept(server_socket, NULL, NULL)) == -1)
		sendStatePage(server_socket, "500", "Internal server error"); //클라이언트 생성실패
	cout << "accept new client: " << client_socket << endl;
	fcntl(client_socket, F_SETFL, O_NONBLOCK);

	/* add event for client socket - add read && write event */
	change_events(change_list, client_socket, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
	change_events(change_list, client_socket, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);

	setServerBlock(server_block);
	setPort(server_block.getListen()[0]);
	setClientFd(client_socket);
	initRequestMsg();
}

/*
 * curr_fd가 client_socket목록에 있는지 체크하는 함수입니다.
 */

int findClient(vector<ComposeResponse> compose_response, int curr_fd)
{
	for (size_t i = 0; i < compose_response.size(); i++)
		if (compose_response[i].getClientFd() == curr_fd)
			return (i);
	return (-1);
}

/*
 * 버퍼에 담아둔 request를 파싱하여 구조체에 담아주는 작업을 하는 함수입니다.
 */

void ComposeResponse::parseRequest(string request)
{
	stringstream ss;
	vector<string> result; //요청메시지가 한 줄 한 줄 저장되는 변수
	vector<string>::iterator it;
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

void ComposeResponse::readRequest()
{
	/* read data from client */
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
	//     disconnect_client(curr_fd);
	// }
	// else
	parseRequest(msg);
}

int ComposeResponse::checkMethod(vector<string> method_limit)
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
	map<int, string>        clients; // map for client socket:data
	vector<struct kevent>   change_list; // kevent vector for changelist
	struct kevent           event_list[8]; // kevent array for eventlistcompRespo

	int                     new_events;
	struct kevent*          curr_event;
	// vector<t_request>		request_msgs;
	vector<ComposeResponse> compose_response;

	int						idx;

	idx = 0;
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
		change_events(change_list, web_serv.server_socket[i], EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);

	while (1)
	{
	//cout << "change_list: " << change_list[0].ident << endl;

		new_events = kevent(kq, &change_list[0], change_list.size(), event_list, 8, NULL);
	//cout << "new_event : " << new_events << endl;

		if (new_events == -1)
			sendStatePage(curr_event->ident, "500", "Internal server error"); //kq관리 실패
		change_list.clear();

		for (int i = 0; i < new_events; ++i)
		{
			curr_event = &event_list[i];
			if (curr_event->flags & EV_ERROR)
			{
				if (check_socket(curr_event->ident, web_serv.server_socket))
				{
					sendStatePage(curr_event->ident, "500", "Internal server error"); //의문.1 서버 에러시, 어디로 명확하게 전달되는 것이 확인되지 않음. ????
				}	//의문 .2 서버 에러시, 서버를 종료시켜야하나 ????
				else
					sendStatePage(curr_event->ident, "400", "Bad Request");
			}
			else if (curr_event->filter == EVFILT_READ)
			{
				if (check_socket(curr_event->ident, web_serv.server_socket))
				{
					compose_response.push_back(ComposeResponse());
					compose_response[i].setClientsocket(change_list, curr_event->ident, http_block.getServerBlock()[i]); //event의 index와 server index의 상관관계 재확인 필요,
				}
 				else if ((idx = findClient(compose_response, curr_event->ident)) >= 0)
				{
					compose_response[idx].readRequest();
					//check_msg(request_msgs[idx]);
				}
			}
			else if (curr_event->filter == EVFILT_WRITE)
			{
				if ((idx = findClient(compose_response, curr_event->ident)) >= 0)
				{
					if (compose_response[idx].getResponse().state_flag != "")  //compose_response[idx].readRequest();했을 때 에러가 있다면 먼저 띄워줌
						sendStatePage(compose_response[idx].getClientFd(), compose_response[idx].getResponse().state_flag, compose_response[idx].getResponse().state_str);
					if (compose_response[idx].checkMethod(http_block.getLimitExcept()))
					{//정해지면 헤더에 붙여넣자~ yeah~
						compose_response[idx].processMethod();
						if (compose_response[idx].getResponse().state_flag != "")
						{
							if (compose_response[idx].getResponse().state_flag == "301")
								compose_response[idx].sendRedirectPage();
							else
								sendStatePage(compose_response[idx].getClientFd(), compose_response[idx].getResponse().state_flag, compose_response[idx].getResponse().state_str);
						}
					}
					else
					{
						sendStatePage(compose_response[idx].getClientFd(), "403", "Forbidden");
					}
				}
			}
		}
	}
}