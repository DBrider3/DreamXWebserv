#include "../includes/Manager.hpp"

Manager::Manager()
{

}

Manager::~Manager()
{

}

Manager::Manager(const Manager& copy)
{
	*this = copy;
}

Manager& Manager::operator = (const Manager& m)
{
	if (this == &m)
		return (*this);
	http_block = m.http_block;
	buffer = m.buffer;
	web_serv.ports = m.web_serv.ports;
	web_serv.server_socket = m.web_serv.server_socket;
	return (*this);
}

HttpBlock	Manager::getHttpBlock()
{
	return (http_block);
}

t_servinfo	Manager::getWebServ()
{
	return (web_serv);
}

void	Manager::setBuffer(string input)
{
	buffer.push_back(input);
}

int		Manager::setHttpBlock(vector<string> buf)
{
	/*
	** buf를 split으로 쪼개어 key값을 비교 후, value들을 넣어줌
	*/
	int res = 1;
	
	while (!(buf[res].empty()))
	{
		vector<string> tmp;
		tmp = deleteSemicolon(split(buf[res], ' ')); // conf 파일의 각 줄의 

		if (tmp[0] == "\tindex") 
		{
			for (size_t i = 1; i < tmp.size(); i++)
				http_block.setIndex(tmp[i]);
		}
		else if (tmp[0] == "\tlimit_except")
		{
			for (size_t i = 1; i < tmp.size(); i++)
				http_block.setLimitExcept(tmp[i]);
		}
		res++;
	}
	return ++res;
}

void	Manager::fileOpen(string conf)
{
	/*
	** 인자 혹은 default파일을 열고 buffer에 저장
	*/

	try
	{
		struct stat sb;//파일의 속성 확인

		if (stat(conf.c_str(), &sb) == -1)
		{
			perror("stat error");
			throw(PrintError());
		}
		if (!S_ISREG(sb.st_mode)) // 정규 파일인지 아닌지 체크
			throw(PrintError());

		ifstream fin(conf); //파일 유무확인
		if (fin.is_open())
		{
			string temp;

			while (getline(fin, temp))
				setBuffer(temp);
			fin.close();
		}
		else
		{
			fin.close();
			throw(PrintError());
		}
	}
	catch(const exception& e)
	{
		cerr << e.what() << "Cannot open the File!😵‍" << endl;
		exit(1);
	}
}

void	Manager::confParsing(void)
{
	/*
	** idx를 활용하여 buffer 순회
	*/
	try
	{
		int idx = 0;
		if (buffer[idx] == "http {")
			idx = setHttpBlock(buffer); //conf파일의 http블록에 해당하는 정보를 파싱
		else
			throw(PrintError());
		while (buffer[idx] == "\tserver {")
			idx = http_block.setServerBlock(buffer, idx);  //conf파일의 server블록에 해당하는 정보를 파싱
		if (http_block.getServerBlock().size() == 0)
			throw(PrintError());
	}
	catch(const exception& e)
	{
		cerr << e.what() << "The file configuration is incorrect!😵‍" << endl;
		exit(1);
	}
}

int 	sendErrorPage(int socket_fd, string state_flag, string state_str)
{
	struct stat		st;
	string			local_uri;
	string			body; //html읽은 내용 담을 변수
	char			buf[100];
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
		buf[n] = '\0';
		ss << buf;
		body += ss.str();
		ss.str("");
		memset(buf, 0, 100);
	}
	close(bodyfd);
	if (n == -1)
		return (DISCONNECTED);
	sprintf(r_header, RESPONSE_FMT, state_flag.c_str(), state_str.c_str(), ct_len, "text/html", body.c_str());
	if(write(socket_fd, r_header, strlen(r_header)) == -1)
		return (DISCONNECTED);
	return (0);
}

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

vector<ClientControl>::iterator findClient(vector<ClientControl> &client_control, int curr_fd)
{
	vector<ClientControl>::iterator it;

	for (it = client_control.begin(); it != client_control.end(); it++)
	{
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

void	Manager::processError(vector<ClientControl>& client_control, uintptr_t curr_id, vector<int> server_sockets)
{
	vector<ClientControl>::iterator it;

	if (checkSocket(curr_id, server_sockets) > -1)
	{
		cout << "err server" << endl;
		sendErrorPage(curr_id, "500", "Internal server error"); //의문.1 서버 에러시, 어디로 명확하게 전달되는 것이 확인되지 않음. ????
	}	//의문 .2 서버 에러시, 서버를 종료시켜야하나 ????
	else
	{
		cout << "err client" << endl;
		sendErrorPage(curr_id, "400", "Bad Request");
		it = findClient(client_control, curr_id);
		client_control.erase(it);
	}
}

int Manager::processRead(vector<ClientControl>& client_control, uintptr_t curr_id, vector<int> server_sockets, vector<struct kevent>& change_list)
{
	vector<ClientControl>::iterator it;
	int								idx;

	if ((idx = checkSocket(curr_id, server_sockets)) > -1)
	{
		client_control.push_back(ClientControl());
		if (client_control.back().setClientsocket(change_list, curr_id, http_block.getServerBlock()[idx]))
			client_control.pop_back();
	}
	else if ((it = findClient(client_control, curr_id)) != client_control.end())
	{
		if (it->getRead() == REQUEST_RECEIVING) //request 읽을 때
		{
			it->setHttpBlock(this->http_block);
			it->readRequest();
			if (it->getEOF() == DISCONNECTED)
			{
				disconnectSocket(it->getClientFd());
				client_control.erase(it);
				return (1);
			}
			if (!(it->getResponse().state_flag.empty()))
			{
				it->setWrite(1);
				return (1);
			}
		}
		if (it->getRead() == REQUEST_COMPLETE) //file 읽을 때
		{
			if (!(it->getResponse().state_flag.empty()))
				it->setWrite(1);
			else if (it->getResourceFd() == -1)//resource 파악
			{
				it->processMethod();
				if (it->getResourceFd() != -1) //서버 내에서 요청하고자 하는 resource가 있는 경우 해당 파일의 fd로 read event생성 
					changeEvents(change_list, it->getResourceFd(), EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
			}
			else //read
				it->readResource(); //로컬 리소스 읽기
		}
		if (!(it->getResponse().state_flag.empty()))
			it->setWrite(1);
	}
	return (0);
}

void Manager::processWrite(vector<ClientControl>& client_control, uintptr_t curr_id, int& count)
{
	vector<ClientControl>::iterator it;
	int error;

	error = 0;
	it = findClient(client_control, curr_id);
	if (it != client_control.end() && it->getWrite() == 1)
	{
		if (!(it->getResponse().state_flag.empty()))
		{
			if (it->getRequest().method == "HEAD")
				it->sendNobodyPage();
			else if (it->getResponse().state_flag == "301")
			{
				it->sendRedirectPage();
				disconnectSocket(it->getClientFd());
				client_control.erase(it);
				return ;

			}
			else if (it->getResponse().state_flag[0] == '2')
			{
				if (it->getResponse().ct_length > 4096)
					it->sendChunk();
				else
					it->sendSuccessPage();
			}
			else
				error = sendErrorPage(it->getClientFd(), it->getResponse().state_flag, it->getResponse().state_str);
			if (it->getEOF() == DISCONNECTED || error == DISCONNECTED)
			{
				disconnectSocket(it->getClientFd());
				client_control.erase(it);
				return ;
			}
		}
		it->resetClient(it->getClientFd(), it->getServerFd(), it->getServerBlock()); //broken_pipe 방지를 목적으로 소켓을 재사용하기 위해 데이터백 리셋🤗
		cout << BLUE << "count : " << ++count << EOC <<endl;
	}
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

	for (size_t i = 0; i < web_serv.ports.size(); i++) // 포트번호는 중복되지 않으니 서버소켓의 갯수만큼 읽기 event등록
		changeEvents(change_list, web_serv.server_socket[i], EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL); 
	int count = 0;
	//struct timespec timeout; //설정 이유 ????? kevent 
	//timeout.tv_sec = 5;
	//timeout.tv_nsec = 0;

	while (1)
	{
		new_events = kevent(kq, &change_list[0], change_list.size(), event_list, 1024, NULL); // timeout 설정 확인
		if (new_events == -1)
			sendErrorPage(curr_event->ident, "500", "Internal server error"); //kq관리 실패
		change_list.clear(); //새로운 이벤트를 받기위해 이벤트 버퍼초기화
		idx = 0;
		for (int i = 0; i < new_events; ++i)
		{
			curr_event = &event_list[i];
			if (curr_event->flags & EV_ERROR)
				processError(client_control, curr_event->ident, web_serv.server_socket);
			else if (curr_event->filter == EVFILT_READ)
			{
				if (processRead(client_control, curr_event->ident, web_serv.server_socket, change_list))
					continue ;
			}
			else if (curr_event->filter == EVFILT_WRITE)
				processWrite(client_control, curr_event->ident, count);
		}
	}
	//close(socket_fd); ????? 서버소켓 갯수만큼 닫아주기
}
