#include "../includes/Utils.hpp"
#include "../includes/Manager.hpp"
#include "../includes/ClientControl.hpp"
#include <errno.h>
/*
 *
 */

void disconnectSocket(int socket_fd) //고쳐야함 소멸자불러야함
{
	cout << YELLOW << "🍀🍀🍀🍀🍀disconnected : " << socket_fd << EOC << endl;
	close(socket_fd);
}

/*
 * 현재 fd가 서버소켓인지 검사하는 함수입니다.
 */

int checkSocket(int curr_fd, vector<int> server_socket)
{
	int i;
	for (i = 0; i < (int)server_socket.size(); i++)
		if (server_socket[i] == curr_fd)
			return (i);
	return (-1);
	// for (size_t i = 0; i < server_socket.size(); i++)
	// 	if (server_socket[i] == curr_fd)
	// 		return (1);
	// return (0);
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
	setMsg("");
	setChunk(0);
	setLength(0);
	return ;
}

void ClientControl::sendChunk(void)
{
	size_t i = 0;
	string tmp;
	string chunk_body;

	size_t body_size;
	char*	r_header = new char[response.ct_length + 1024];

	// time_t temp2;
	// struct tm* timeinfo;
	// time(&temp2);
	// timeinfo = localtime(&temp2);
		
	// cout << "😮‍💨😮‍💨😮‍💨😮‍💨 4. chunk 쓰기 전 시간 : 😮‍💨😮‍💨😮‍💨😮‍💨" << asctime(timeinfo);

	sprintf(r_header, CHUNK_FMT, response.state_flag.c_str(), response.state_str.c_str(), response.ct_type.c_str());
	write(client_fd, r_header, strlen(r_header));
	body_size = body.size();
	// while (1)
	// {
	// 	stringstream ss;
	// 	size_t chunk_size;
	// 	chunk_size = 100;	
	// 	if (body.size() - chunk_size * i != 0)
	// 	{
	// 		if (body.size() - chunk_size * i < chunk_size)
	// 		{
	// 			tmp = body.substr(chunk_size * i, body.size() - chunk_size * i); //포인터 넘겨서 어찌저찌 하면 속도 엄청 올라간데요
	// 		/*	tmp = "";
	// 			for (size_t j = 0; j < (body_size - chunk_size * i); j++)
	// 				tmp.insert(j, 1, body[chunk_size * i + j]);*/
	// 			ss << hex << tmp.size();
	// 		}
	// 		else //
	// 		{
	// 			tmp = body.substr(chunk_size * i, chunk_size);
	// 		/*	tmp = "";
	// 			for (size_t j = 0; j < chunk_size; j++)
	// 				tmp.insert(j, 1, body[chunk_size * i + j]);*/
	// 			ss << hex << chunk_size;
	// 		}
	// 		chunk_body = ss.str() + "\r\n" + tmp + "\r\n";
	// 		write (client_fd, chunk_body.c_str(), chunk_body.size());
	// 	}
	// 	if (body.size() - chunk_size * i < chunk_size)
	// 		break ;
	// 	i++;
	// }
	
	//우리는 한번씩 잘라서 write하는데 현스키님은 아예 string에 다 담아서 했던거 같음
	//while 밖에서 body size 갖는 변수 선언 size() 호출 줄이고 , 
	//while 내부에서 body_size - i 크기로 body를 얼마나 tmp에 넣을지 정하고 또한 body_size - i 한 값만큼 16진수 적어주자
	//size - i 한거 만큼 body 포인트 이동하며 string에 넣어주자
	//tmp.size() == body_size - 2(chunk_size * i)이다.
	
	while (1)
	{
		stringstream ss;
		size_t chunk_size;
		chunk_size = 100;		
		if (body_size - chunk_size * i != 0)
		{
			if (body_size - chunk_size * i < chunk_size)
			{
				ss << hex << chunk_size;				
				write(client_fd, ss.str().c_str(), ss.str().size());				
				write(client_fd, "\r\n", 2);
				write(client_fd, body.c_str(), body_size - 2 * (chunk_size * i));
				write(client_fd, "\r\n", 2);
				body += body[body_size - 2 * (chunk_size * i)];			
			}
			else //
			{					
				ss << hex << chunk_size;				
				write(client_fd, ss.str().c_str(), ss.str().size());				
				write(client_fd, "\r\n", 2);
				write(client_fd, body.c_str(), chunk_size);
				write(client_fd, "\r\n", 2);
				body += body[chunk_size];
			}
			//chunk_body = ss.str() + "\r\n" + tmp + "\r\n";
			
		
		}
		if (body_size - chunk_size * i < chunk_size)
			break ;
		i++;
	}
	
	//if (response.cgi != 2)
	write (client_fd, "0\r\n\r\n", strlen("0\r\n\r\n"));
	// time_t temp1;
	// struct tm* timeinfo2;
	// time(&temp1);
	// timeinfo2 = localtime(&temp1);
		
	// cout << "😮‍💨😮‍💨😮‍💨😮‍💨 5. chunk 쓰기 후 시간 : 😮‍💨😮‍💨😮‍💨😮‍💨" << asctime(timeinfo2);
	// disconnectSocket(client_fd);
	delete[] r_header;
	// msg = "";
	// body = "";
}

void ClientControl::sendNobodyPage(void)
{
	struct stat		st;
	string			local_uri;
	char			r_header[1024];
	int				ct_len;

	
	local_uri = "./state_pages/" + response.state_flag + ".html";
	stat(local_uri.c_str(), &st);
	ct_len = st.st_size;

	sprintf(r_header, RESPONSE_FMT, response.state_flag.c_str(), response.state_str.c_str(), response.ct_length, "text/html", "");
	write(client_fd, r_header, strlen(r_header));
	// cout << "[🍈🍈🍈🍈🍈🍈nobody response🍈🍈🍈🍈🍈🍈]\n" << r_header << endl;
	// disconnectSocket(client_fd);
}

void ClientControl::sendSuccessPage(void)
{
	char*	r_header = new char[response.ct_length + 1024];
	//memset(r_header, 0x00 , sizeof(r_header));
	//chunk
	
	if (response.state_flag == "204")
	{
		sprintf(r_header, RESPONSE_FMT, response.state_flag.c_str(), response.state_str.c_str(), 0, "text/plain", "");
	}
	else if (response.state_flag == "201")
	{
		struct stat		st;
		string			local_uri;
		char			buf[10];
		int				bodyfd;
		int				n;
		stringstream	ss;

		local_uri = "./state_pages/" + response.state_flag + ".html";
		stat(local_uri.c_str(), &st);
		response.ct_length = st.st_size;
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
		close(bodyfd);
		sprintf(r_header, RESPONSE_FMT, response.state_flag.c_str(), response.state_str.c_str(), response.ct_length, "text/html", body.c_str());
	}
	//else if (response.cgi == 2)
	//	sprintf(r_header, CHUNK_FMT, response.state_flag.c_str(), response.state_str.c_str(), response.ct_type.c_str());
	else
	{
		sprintf(r_header, RESPONSE_FMT, response.state_flag.c_str(), response.state_str.c_str(), response.ct_length, response.ct_type.c_str(), body.c_str());
	}
	write(client_fd, r_header, strlen(r_header));
	// cout << "[🍈🍈🍈🍈🍈🍈success response🍈🍈🍈🍈🍈🍈]\n" << r_header << endl;
	// msg = ""; // request body parsing variable: msg 초기화
	// body = "";
	// cout << "잘 보냈다옹~ 🐱" << endl;
	// disconnectSocket(client_fd);
	delete[] r_header;

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
	while ((n = read(bodyfd, buf, sizeof(buf) - 1)) > 0) //괘씸 ????
	{
		buf[9] = '\0';
		ss << buf;
		body += ss.str();
		ss.str("");
		memset(buf, 0, 10);
	}
	close(bodyfd);
	// sprintf(r_header, ERROR_FMT, state_flag.c_str(), state_str.c_str());
	sprintf(r_header, RESPONSE_FMT, state_flag.c_str(), state_str.c_str(), ct_len, "text/html", body.c_str());
	write(socket_fd, r_header, strlen(r_header));
	// cout << "[🍈🍈🍈🍈🍈🍈error response🍈🍈🍈🍈🍈🍈]\n" << r_header << endl;
	// disconnectSocket(socket_fd);
}

void ClientControl::sendRedirectPage()
{
	char			r_header[1024];

	sprintf(r_header, REDIRECT_FMT, getResponse().state_flag.c_str(), getResponse().state_str.c_str(), getResponse().redirect_uri.c_str());
	write(getClientFd(), r_header, strlen(r_header));
	// cout << "[🍈🍈🍈🍈🍈🍈redirect response🍈🍈🍈🍈🍈🍈]\n" << r_header << endl;
	// msg = "";
	// disconnectSocket(getClientFd());
}

/*
 * sever_socket을 토대로 client_socket을 구성하는 함수입니다.
 */

int ClientControl::setClientsocket(vector<struct kevent> &change_list, uintptr_t server_socket, ServerBlock server_block)
{
	struct sockaddr_in  client_addr;
	socklen_t			addr_size = sizeof(client_addr);

	/* accept new client */
	int client_socket;
   	if ((client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &addr_size)) == -1)
	{
		sendErrorPage(server_socket, "500", "Internal server error"); //클라이언트 생성실패
		return (-1);
	}
	//std::cout << "\033[32m server connection called \033[0m" << std::endl;
    cout << GREEN << "accept new client: " << client_socket << EOC << endl;
	setRead(0);
	fcntl(client_socket, F_SETFL, O_NONBLOCK);

	/* add event for client socket - add read && write event */
	changeEvents(change_list, client_socket, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
	changeEvents(change_list, client_socket, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);

	setServerBlock(server_block);
	setPort(server_block.getListen()[0]);
	setClientFd(client_socket);
	setServerFd(static_cast<int>(server_socket));
	initRequestMsg();
	return (0);
}

/*
 * curr_fd가 client_socket목록에 있는지 체크하는 함수입니다.
 */

vector<ClientControl>::iterator findClient(vector<ClientControl> &client_control, int curr_fd)
{
	vector<ClientControl>::iterator it;

	for (it = client_control.begin(); it != client_control.end(); it++)
	{
		//cout << "cf: "<< it->getClientFd() <<  "rf: " << it->getResourceFd() << "curr : "<< curr_fd << endl;
		if (it->getResourceFd() == -1)
		{
			if (it->getClientFd() == curr_fd)
				return (it);
		}
		else
		{
			if (it->getResourceFd() == curr_fd)
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
	setRead(1);

/*
 * Startline 파싱
 */
	size_t	previous;
	size_t	current;

	if (!getChunk())
	{
		previous = 0;
		current = request.find("\r\n"); // \r\n == crlf 

		//요청 메시치 에러처러 : /r/n이 없이 들어오는 경우 에러처리 필요!! 요청 메시지가 중간에 누락되는 경우!
		if (current == string::npos)
		{
			setStateFlag("400");
			setStateStr("bad request");
			return ;
		}
		//current = request.find("\r\n"); // \r\n == crlf 
		//find 함수는 해당 위치부터 문자열을 찾지 못할 경우 npos를 반환한다.
		while (current != string::npos)
		{
			// 첫 인자의 위치부터 두번째 인자 길이만큼 substring을 반환
			string substring = request.substr(previous, current - previous);
			result.push_back(substring);
			previous = current + 2; //previous 부터 "\r\n"이 나오는 위치를 찾는다.
			current = request.find("\r\n", previous);
		}
		setMethod(strtok(const_cast<char*>(result[0].c_str()), " "));
		setUri(strtok(NULL, " "));
		setVersion(strtok(NULL, "\n"));
		// if (checkUri(request))
		// 	return ;
		// cout << "🥗🥗🥗🥗parseRequest5\n";
		// if (getRequest().uri.size() > 8190)
		// {
		// 	setStateFlag("414");
		// 	setStateStr("Request-URI too long");
		// 	return ;
		// }
		// if (getRequest().uri.find('?') != string::npos)
		// {
		// 	ss << getRequest().uri;
		// 	getline(ss, temp, '?');
		// 	setUri(temp);
		// 	getline(ss, temp, '\0');
		// 	setQuery(temp);
		// }
		/*
		* Header 파싱
		*/
		for (it = result.begin() + 1; it != result.end() && it->size() > 0; it++) //수정함
		{
			stringstream ss(*it);
			stringstream ss_tmp;
			string key;
			vector<string> val;
			string val_tmp;
			//*it.find(':') == npos ;
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
			header_tmp[key] = val; //multipart 확인예정
			if (key == "Transfer-Encoding" && val.front() == "chunked")
			{
				setChunk(1);
			}
		}
		setHeader(header_tmp);
	}
	if (getChunk() == 1 && request.rfind("0\r\n\r\n") == string::npos)
	{
		setRead(0);
 		return ;
	}
	else if (getChunk() == 1)
	{
		previous = 0;
		current = request.find("\r\n"); // \r\n == crlf 
		while (current != string::npos)
		{
			// 첫 인자의 위치부터 두번째 인자 길이만큼 substring을 반환
			string substring = request.substr(previous, current - previous);
			result.push_back(substring);
			previous = current + 2; //previous 부터 "\r\n"이 나오는 위치를 찾는다.
			current = request.find("\r\n", previous);
		}

		for (it = result.begin(); it != result.end() && it->size() > 0; it++)
			;
	}
	/*
	* Body 파싱
	*/
	// if (getRequest().header["Content-Length"].size() > 0)
	// {
	// 	if(convStoi(*(getRequest().header["Content-Length"].begin())) > convStoi(getServerBlock().getClientBodySize()))
	// 	{
	// 		setStateFlag("413");
	// 		setStateStr("Payload Too Large");
	// 		return ;
	// 	}
	// }
		if (checkUri(request))
		return ;
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
	// time_t temp1;
	// struct tm* timeinfo;
	// time(&temp1);
	// timeinfo = localtime(&temp1);
		
	//cout << "😮‍💨😮‍💨😮‍💨😮‍💨 2. request 입력 후 시간 : 😮‍💨😮‍💨😮‍💨😮‍💨" << asctime(timeinfo);
	
	if (it == result.end())
		return ; 
	while (++it != result.end())
	{		
		setBody(*it);
	}
	if (getRequest().header["Content-Type"].size() == 2 && getRequest().body.size() == 0)
	{
		setStateFlag("403");
		setStateStr("Forbidden");
		return ;
	}
	// time_t temp2;
	// struct tm* timeinfo2;
	// time(&temp2);
	// timeinfo2 = localtime(&temp2);
		
	//cout << "😮‍💨😮‍💨😮‍💨😮‍💨 3. 파싱 후 시간 😮‍💨😮‍💨😮‍💨😮‍💨 : " << asctime(timeinfo2);
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
# define SIZE 5000000
void ClientControl::readRequest()
{
	/*
	** read data from client
	*/
	char buf[SIZE];
	// string msg;
	int n;

	
	// if (msg == "")
	// {
	// 	time_t temp3;
	// 	struct tm* timeinfo3;
	// 	time(&temp3);
	// 	timeinfo3 = localtime(&temp3);
			
	// 	//cout << "😮‍💨😮‍💨😮‍💨😮‍💨 1. requset 입력 전 시간 😮‍💨😮‍💨😮‍💨😮‍💨 : " << asctime(timeinfo3);
	// }
	n = 0;
	// while ((n = read(getClientFd(), buf, SIZE - 1)) > 0)
	if ((n = read(getClientFd(), buf, SIZE - 1)) > 0)
	{
		buf[n] = 0;
		msg += static_cast<string> (buf);
	}
	if (n == 0)
	{
	//	cout << RED << "🏀   Disconnected read   🏀  " << getClientFd() << EOC << endl;
		setEOF(DISCONNECTED);
		disconnectSocket(getClientFd());
		
	}
	if (n < 0)
	{
		 cout << "read error : " << getClientFd() << endl;
		setEOF(DISCONNECTED);
		disconnectSocket(getClientFd());
	}
 	if (msg.rfind("\r\n\r\n") + 4 == msg.size()) //잘 읽음 // 완성된 뿐만 아니라  POST 바디까지 해줌.
		parseRequest(msg);
}

/*
 * 서버 실행하는 함수입니다.
 */

void	ClientControl::readResource()
{
	char foo[1000000];
	int res = 0;
	int fd = getResourceFd();

	// memset(foo, 0, sizeof(foo));
	if ((res = read(fd, foo, 999999)) > 0)
	{
		foo[res] = 0;
		body += static_cast<string> (foo);
		// memset(foo, 0, sizeof(foo));
	}
	//cout << "response bodyyy ::::::::::::::::::::::::::::::: \n"<<foo << endl;
	//cout << "res : " << "(" << res << ")" << endl;
	if (res == -1)
	{
		cout <<"read error 🚀🚀🚀🚀🚀🚀🚀🚀" << endl;
		setStateFlag("403");
		setStateStr("Forbidden");
		close(fd);
		setResourceFd(-1);
	}
	else if (res != 999999)
	{
		//cout << "EOF" << endl;
		setStateFlag("200");
		setStateStr("OK");
		if (response.cgi && !(response.cgi == 2 && request.method == "GET"))
		{
			string search = "";
			if (response.cgi == 1)
				search = "Content-type: ";
			else if (response.cgi == 2)
				search = "Content-Type: ";
			response.ct_type = body.substr(body.find(search) + 14, body.find("\r\n\r\n") - body.find(search) - 14);
			body = body.substr(body.find("\r\n\r\n") + 4, body.size() - body.find("\r\n\r\n") - 4);
			response.ct_length = body.size(); // 수정 필요??
			fclose(getFout());
		}
		close(fd);
		setResourceFd(-1);
	}
	// close(fd);
	// setResourceFd(-1);
}

void Manager::runServer()
{
	int 					kq;
	int						idx;
	map<int, string>        clients; // map for client socket:data
	vector<struct kevent>   change_list; // kevent vector for changelist
	struct kevent           event_list[1024]; // kevent array for eventlistcompRespo
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
	int count = 0;
	int if_count = 0;
	struct timespec timeout;
	timeout.tv_sec = 5;
	timeout.tv_nsec = 0;

	while (1)
	{
		new_events = kevent(kq, &change_list[0], change_list.size(), event_list, 1024, &timeout); // timeout 설정 확인
		if (new_events == -1)
			sendErrorPage(curr_event->ident, "500", "Internal server error"); //kq관리 실패
		change_list.clear();
		idx = 0;
		for (int i = 0; i < new_events; ++i)
		{
			curr_event = &event_list[i];

			if (curr_event->flags & EV_ERROR)
			{
				if (checkSocket(curr_event->ident, web_serv.server_socket) > -1)
				{
					cout << "err server" << endl;
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
				if ((idx = checkSocket(curr_event->ident, web_serv.server_socket)) > -1)// && checkBeforeServer(curr_event->ident, before_server))
				{
					//  cout << "read filter 🥰!!" << endl;
					client_control.push_back(ClientControl());
					if_count++;
					if (client_control.back().setClientsocket(change_list, curr_event->ident, http_block.getServerBlock()[idx]))
						client_control.pop_back();
				}
				else if ((it = findClient(client_control, curr_event->ident)) != client_control.end())
				{
					//  cout << "find 🔥!!" << endl;
					if (it->getRead() == REQUEST_RECEIVING) //request 읽을 때
					{
						// cout << "find 🔥!!" << endl;
						it->setHttpBlock(this->http_block);
						it->readRequest();
						if (it->getEOF() == DISCONNECTED)
						{
							client_control.erase(it);
							continue ;
						}
						// cout << "receiving 🔥!! " << it->getRead() << endl;
						//cout << "r0_id : " << it->getClientFd() << endl;
						if (!(it->getResponse().state_flag.empty()))
						{
							it->setWrite(1);
							continue ;
						}
					}
					if (it->getRead() == REQUEST_COMPLETE) //file 읽을 때
					{
						// cout << "read 🥰!!" << endl;
						if (it->getResourceFd() == -1)//method 파악
						{
							//cout << "process 🥰!!" << endl;
							it->processMethod();
							if (it->getResourceFd() != -1)
							{
								//cout << "resource_fd 🥰!!" << endl;
								changeEvents(change_list, it->getResourceFd(), EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
							}
						}
						else //read
							it->readResource();
						//cout << "r1_id : " << it->getClientFd() << endl;
					}
					if (!(it->getResponse().state_flag.empty()))
					{
					//	cout << "write💩🚜 !!" << endl;
						//cout << "rw_id : " << it->getClientFd() << endl;
						it->setWrite(1);
					}
					// cout << "📒📒📒📒📒📒getEOF : " << it->getEOF() << "socket : " << it->getClientFd()<< endl;
				}
			}
			else if (curr_event->filter == EVFILT_WRITE)
			{
				it = findClient(client_control, curr_event->ident);
				//cout << "w_id : " << curr_event->ident << " / comp : " << (it != client_control.end()) << endl; // << " / getWrite : " << it->getWrite() << endl; 
				if (it != client_control.end() && it->getWrite() == 1)
				{
					//  cout << "write😃 !! event2" << endl;
					if (!(it->getResponse().state_flag.empty()))
					{
						if (it->getRequest().method == "HEAD")
							it->sendNobodyPage();
						else if (it->getResponse().state_flag == "301")
							it->sendRedirectPage();
						else if (it->getResponse().state_flag[0] == '2')
						{
							if (it->getResponse().ct_length > 10000) // && it->getResponse().cgi != 2)
								it->sendChunk();
							else
								it->sendSuccessPage();
						}
						else
							sendErrorPage(it->getClientFd(), it->getResponse().state_flag, it->getResponse().state_str);
					}
					// else
					// 	sendErrorPage(it->getClientFd(), "403", "Forbidden");
					//resetBeforeServer(it->getServerFd(), before_server);
					// client_control.erase(it);//iterator로 삭제 가능
					
					// reset function 
					// cout << "🥏🥏🥏🥏🥏🥏getEOF : " << it->getEOF() << "fd : "<<it->getClientFd() << endl;
					// if (it->getEOF() == DISCONNECTED)
					// {
					// 	disconnectSocket(it->getClientFd());
					// 	client_control.erase(it);
					// }
					// else
					// {
						it->resetClient(it->getClientFd(), it->getServerFd(), it->getServerBlock());
						cout << BLUE << "count : " << ++count << EOC <<endl;
					// }
					//
					// for (int i = 0; i < new_events; ++i)
					// {
					// 	cout << "🧸🧸 ident :" << event_list[i].ident << " / filter : "<< event_list[i].filter << " 🧸🧸" << endl;
					// }
					// sleep(1);
				}
				
			}
			// sleep(1);
			// 	cout << "💡💡 curr_ident :" << curr_event->ident << " / curr_filter : "<< curr_event->filter << " 💡💡" << endl;
			// 	for (int i = 0; i < new_events; ++i)
			// 	{
			// 		cout << "🧸🧸 ident :" << event_list[i].ident << " / filter : "<< event_list[i].filter << " 🧸🧸" << endl;
			// 	}
		}

		// cout << "🔥🔥🔥🔥🔥🔥cli_cont" <<client_control.size() << endl;
	}
	//close(socket_fd);
	//usleep(30);
}

