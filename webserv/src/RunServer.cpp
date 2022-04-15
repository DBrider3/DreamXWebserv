#include "../includes/Utils.hpp"
#include "../includes/Manager.hpp"
#include "../includes/ClientControl.hpp"
#include <errno.h>
/*
 *
 */

void disconnectSocket(int socket_fd) //고쳐야함 소멸자불러야함
{
	cout << "Disconnect Socket!!  socket_fd : " << socket_fd << endl;
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

int checkBeforeServer(int curr_fd, vector<int> before_server)
{
	for (size_t i = 0; i < before_server.size(); i++)
		if (before_server[i] == curr_fd)
			return (0);
	return (1);
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
	setMethod("");
	setUri("");
	setQuery("");
	setVersion("");
	return ;
}

void ClientControl::sendChunk(char** r_header)
{
	int i = 0;
	string tmp;
	string chunk_body;

	sprintf(*r_header, CHUNK_FMT, response.state_flag.c_str(), response.state_str.c_str(), response.ct_type.c_str());
	write(client_fd, *r_header, strlen(*r_header));
	while (1)
	{
		stringstream ss;
		if (body.size() - 100 * i < 100)
		{
			tmp = body.substr(100 * i, body.size() - 100 * i);
			ss<< hex << tmp.size();
		}
		else
		{
			tmp = body.substr(100 * i, 100);
			ss<< hex << 100;
		}
		chunk_body = ss.str() + "\r\n" + tmp + "\r\n";
		write (client_fd, chunk_body.c_str(), chunk_body.size());
		if (body.size() - 100 * i < 100)
			break ;
		i++;
	}
	write (client_fd, "0\r\n\r\n", strlen("0\r\n\r\n"));

}

void ClientControl::sendSuccessPage(void)
{

	char*	r_header = new char[response.ct_length + 1024];

	//chunk
	if (response.ct_length == 0) //임시 있긴 있다고함 의문 ????
		;//;
	else if (response.ct_length > 10)
	{
		sendChunk(&r_header);
		disconnectSocket(client_fd);
		return ;
	}
	else
		sprintf(r_header, RESPONSE_FMT, response.state_flag.c_str(), response.state_str.c_str(), response.ct_length, response.ct_type.c_str(), body.c_str());
	write(client_fd, r_header, strlen(r_header));
	disconnectSocket(client_fd);
}

void 	sendErrorPage(int socket_fd, string state_flag, string state_str)
{
	struct stat		st;
	string			local_uri;
	string			body; //html읽은 내용 담을 변수
	char			buf[10];
	char			r_header[1024];
	int				ct_len;
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
	sprintf(r_header, RESPONSE_FMT, state_flag.c_str(), state_str.c_str(), ct_len, "text/html", body.c_str());
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
		// cout << "is client_socket made? : " << client_socket << endl;
		sendErrorPage(server_socket, "500", "Internal server error"); //클라이언트 생성실패
		return (-1);
	}
    cout << "accept new client: " << client_socket << endl;
	setRead(0);
	fcntl(client_socket, F_SETFL, O_NONBLOCK);

	/* add event for client socket - add read && write event */
	changeEvents(change_list, client_socket, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
	changeEvents(change_list, client_socket, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);

	cout << "rs 176" << endl;
	setServerBlock(server_block);
	cout << "rs 178" << endl;
	setPort(server_block.getListen()[0]);
	cout << "rs 180" << endl;
	setClientFd(client_socket);
	cout << "rs 182" << endl;
	setServerFd(static_cast<int>(server_socket));
	cout << "rs 184" << endl;
	initRequestMsg();
	cout << "rs 186" << endl;
	return (0);
}

/*
 * curr_fd가 client_socket목록에 있는지 체크하는 함수입니다.
 */

vector<ClientControl>::iterator findClient(vector<ClientControl> &client_control, int curr_fd)
{
	vector<ClientControl>::iterator it;

	// cout << "rs 192" << endl;
	for (it = client_control.begin(); it != client_control.end(); it++)
	{
		// cout << "list : " << it->getClientFd() << "find : " << curr_fd << endl;
		if (it->getClientFd() == curr_fd)
		{
			//cout << "Before end process" << endl;
			if (it == client_control.end())
				cout << "here" << endl;
			return (it);
		}
	}
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
	map<string, vector<string> > header_tmp;
	string	temp;

/*
 * Startline 파싱
 */
	size_t	previous;
	size_t	current;

	previous = 0;
	current = request.find("\r\n"); // \r\n == crlf
	//find 함수는 해당 위치부터 문자열을 찾지 못할 경우 npos를 반환한다.
	while (current != string::npos)
	{
		// 첫 인자의 위치부터 두번째 인자 길이만큼 substring을 반환
		string substring = request.substr(previous, current - previous);
		result.push_back(substring);
		previous = current + 2;
		//previous 부터 "\r\n"이 나오는 위치를 찾는다.
		current = request.find("\r\n", previous);
		// cout << "request :: " << substring << endl;
	}

	setMethod(strtok(const_cast<char*>(result[0].c_str()), " "));
	setUri(strtok(NULL, " "));
	setVersion(strtok(NULL, "\n"));

	if (getRequest().uri.size() > 8190)
	{
		setStateFlag("414");
		setStateStr("Request-URI too long");
		return ;
	}

	if (getRequest().uri.find('?') != string::npos)
	{
		ss << getRequest().uri;
		getline(ss, temp, '?');
		setUri(temp);
		getline(ss, temp, '\0');
		setQuery(temp);
	}

	/*
	* Header 파싱
	*/

	for (it = result.begin() + 1; it->size() > 0; it++)
	{
		stringstream ss(*it);
		stringstream ss_tmp;
		string key;
		vector<string> val;
		string val_tmp;

		getline(ss, key, ':');
		ss.get(); //인덱스 +1 -> 콜론 뒤 공백에서 다음 인덱스로 이동

		for (int i = 0; getline(ss, val_tmp, ' '); i++)
		{
			if (key == "Content-Type" && i == 1)
			{
				ss_tmp << val_tmp;
				getline(ss_tmp, val_tmp, '=');
				getline(ss_tmp, val_tmp, '\0');
			}
			val.push_back(val_tmp);
		}
		header_tmp[key] = val;
	}
	setHeader(header_tmp);



	/*
	* Body 파싱
	*/
	if (getRequest().header["Content-Length"].size() > 0)
	{
		if(convStoi(*(getRequest().header["Content-Length"].begin())) > convStoi(getServerBlock().getClientBodySize()))
		{
			setStateFlag("413");
			setStateStr("Payload Too Large");
			return ;
		}
	}

	while (++it != result.end())
	{
		setBody(*it);
		// cout << "body start ------------------------------\n" << *it << endl;
		// cout << "끝 =======================================\n" << endl;
	}

	if (getRequest().header["Content-Type"].size() == 2 && getRequest().body.size() == 0)
	{
		// cout << "is here -----------------------------------\n" << endl;
		setStateFlag("403");
		setStateStr("Forbidden");
		return ;
	}
}

void	resetBeforeServer(int server_fd, vector<int>& before_server)
{
	for (int i = 0; i < static_cast<int>(before_server.size()); i++)
		if (server_fd == before_server[i])
		{
			before_server.erase(before_server.begin() + i);
			break;
		}
}

/*
 * curr_fd가 전달하는 내용을 버퍼에 담아주는 함수입니다.
 */

void ClientControl::readRequest()
{
	/*
	** read data from client
	*/
	char buf[142];
	stringstream ss;
	string msg;
	int n;

	setRead(1);
	n = 0;
	while ((n = read(getClientFd(), buf, sizeof(buf) - 1)) > 0)
	{
		// if (!buf[0])
		// {
		// 	n = 0;
		// 	break;
		// }
		buf[n] = '\0';
		ss << buf;
		// cout << "buf : " << buf << endl;
	}
	msg += ss.str();
	// if (n <= 0)
	// {
	//	sendErrorPage(curr_event->ident, "400", "Bad request"); //의문.3 에러 처리 방법이 명확하게 떠오르지 않음.. ????
	//     if (n < 0)
	//         cerr << "client read error!" << endl;
	// 	cout << "1\n";
	//     disconnectSocket(curr_fd);
	// }
	// else
	if (msg.size() > 0)
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
	vector<int>				before_server;

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
	//for (int j = 0; j < 30; j++)
	{
		new_events = kevent(kq, &change_list[0], change_list.size(), event_list, 8, NULL); // timeout 설정 확인
		// for(int i = 0; i < 8 ; i++)
		// 	cout << "i :" << i << " " <<"evfd : " << event_list[i].ident << endl;
		// for(int i = 0; i < 8 ; i++)
		//     cout << "i : " << i << " evfd : " << event_list[i].ident << endl;

		if (new_events == -1)
			sendErrorPage(curr_event->ident, "500", "Internal server error"); //kq관리 실패
		change_list.clear();
		idx = 0;
		for (int i = 0; i < new_events; ++i)
		{			
			curr_event = &event_list[i];
			// cout << "[" << i << "]번째" << "new_events" << endl;			
			//cout << "gross : " << new_events << " curr fd : " << curr_event << endl;
			if (curr_event->flags & EV_ERROR)
			{
				if (checkSocket(curr_event->ident, web_serv.server_socket))
				{
					sendErrorPage(curr_event->ident, "500", "Internal server error"); //의문.1 서버 에러시, 어디로 명확하게 전달되는 것이 확인되지 않음. ????
				}	//의문 .2 서버 에러시, 서버를 종료시켜야하나 ????
				else
				{
					cout << "err client" << endl;
					sendErrorPage(curr_event->ident, "400", "Bad Request");
					it = findClient(client_control, curr_event->ident);
					resetBeforeServer(it->getServerFd(), before_server);
					client_control.erase(it);
				}
			}
			else if (curr_event->filter == EVFILT_READ)
			{
				if (checkSocket(curr_event->ident, web_serv.server_socket) && checkBeforeServer(curr_event->ident, before_server))
				{
					before_server.push_back(curr_event->ident);
					cout << "ser read" << endl;
					client_control.push_back(ClientControl());
					// cout << curr_event->ident << " / "<< http_block.getServerBlock()[client_control.size() - 1].getRoot() << endl;
					if (client_control.back().setClientsocket(change_list, curr_event->ident, http_block.getServerBlock()[client_control.size() - 1]))
						client_control.pop_back();
					cout << "rs 448 , size : " << client_control.size() << endl;
					//cout << "server accept client2" << endl;
				}
 				else 
				{
					for (it = client_control.begin(); it != client_control.end(); it++)
					{
						if (it->getClientFd() == (int)curr_event->ident)
						{
							if (it == client_control.end())
								cout << "here" << endl;
							break ;
						}
					}
					if (!(it == client_control.end()))
					{
						cout << "cli read" << endl;
						it->readRequest();
					}		
				}
			}
			else if (curr_event->filter == EVFILT_WRITE)
			{
				if ((it = findClient(client_control, curr_event->ident)) != client_control.end() && it->getRead() == 1)
				{
					cout << "cli write" << endl;
					if (!(it->getResponse().state_flag.empty()))  //it->readRequest();했을 때 에러가 있다면 먼저 띄워줌
						sendErrorPage(it->getClientFd(), it->getResponse().state_flag, it->getResponse().state_str);
					else if (it->checkMethod(http_block.getLimitExcept()))
					{
						it->processMethod();
						if (!(it->getResponse().state_flag.empty()))
						{
							if (it->getResponse().state_flag == "301")
								it->sendRedirectPage();
							else if (it->getResponse().state_flag[0] == '2')
							{
								it->sendSuccessPage();
							}
							else
								sendErrorPage(it->getClientFd(), it->getResponse().state_flag, it->getResponse().state_str);
						}
					}
					else
						sendErrorPage(it->getClientFd(), "403", "Forbidden");
					resetBeforeServer(it->getServerFd(), before_server);
					client_control.erase(it);//iterator로 삭제 가능
				}				
				//cout << "write test : " << it->getClientFd() << endl;
			}
			
		}
	}
}
