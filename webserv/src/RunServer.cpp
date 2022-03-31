#include "../includes/WebServer.hpp"
#include "../includes/Utils.hpp"
#include "../includes/Manager.hpp"

/*
 *
 */

void Manager::check_msg(t_request rmsg)
{
	cout << "???" << endl;
	cout << rmsg.method << endl;
	cout << rmsg.uri << endl;
	cout << rmsg.version << endl;
	for (map<string, vector<string> >::iterator it = rmsg.header.begin(); it != rmsg.header.end(); it++)
	{
		cout << "where1" << endl;
		cout << "key : " << it->first << " / value : " ; 
		for (vector<string>::iterator itt = it->second.begin(); itt != it->second.end(); itt++)
		{
			cout << "where2" << endl;
			cout << *itt << " " ;
		}
		cout << "where3" << endl;
		cout << endl;
	}
	cout << rmsg.cgi << endl;
	cout << rmsg.fd << endl;
	for (vector<string>::iterator it = rmsg.body.begin(); it != rmsg.body.end(); it++)
		cout << *it << endl;
	cout << "where4" << endl;
	
}

void disconnect_client(int client_fd)
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

t_request initRequestMsg(int client_socket)
{
	t_request request_msg;

	request_msg.cgi = 0;
	request_msg.method = 0;
	request_msg.uri = 0;
	request_msg.version = 0;
	request_msg.fd = client_socket;
	return (request_msg);
}

/*
 * sever_socket을 토대로 client_socket을 구성하는 함수입니다.
 */

void setClientsocket(vector<t_request> request_msgs, vector<struct kevent> change_list, uintptr_t server_socket)
{
	/* accept new client */
	int client_socket;

   if ((client_socket = accept(server_socket, NULL, NULL)) == -1)
        exit_with_perror("accept() error\n");
    cout << "accept new client: " << client_socket << endl;
    fcntl(client_socket, F_SETFL, O_NONBLOCK);

	/* add event for client socket - add read && write event */
	change_events(change_list, client_socket, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
	change_events(change_list, client_socket, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);

	request_msgs.push_back(initRequestMsg(client_socket));
}

/*
 * curr_fd가 client_socket목록에 있는지 체크하는 함수입니다.
 */

int findClient(vector<t_request> request_msgs, int curr_fd)
{
	cout << "in findCline function" << endl;
	for (size_t i = 0; i < request_msgs.size(); i++)
		if (request_msgs[i].fd == curr_fd)
		{
			cout << "???????" << endl;
			return (i);
		}
	cout << "error" << endl;
	return (-1);
}

/*
 * 버퍼에 담아둔 request를 파싱하여 구조체에 담아주는 작업을 하는 함수입니다.
 */

void parseRequest(t_request request_msg, string request)
{
	stringstream ss;
	vector<string> result; //요청메시지가 한 줄 한 줄 저장되는 변수
	vector<string>::iterator it;

/*
 * Startline 파싱
 */
	result = split(request, '\n');
	request_msg.method = strtok(const_cast<char*>(result[0].c_str()), " ");
	request_msg.uri = strtok(NULL, " ");
	request_msg.version = strtok(NULL, "\n");

/*
 * Header 파싱
 */
	for (it = result.begin() + 1; it->size() > 0; it++)
	{
		stringstream ss(*it);
		string key;
		vector<string> val;
		string val_tmp;

		getline(ss, key, ':');
		ss.get(); //인덱스 +1 -> 콜론 뒤 공백에서 다음 인덱스로 이동

		for (int i = 0; getline(ss, val_tmp, ' '); i++)
			val.push_back(val_tmp);
		request_msg.header[key] = val;
	}

/*
 * Body 파싱
 */
	while (++it != result.end())
		request_msg.body.push_back(*it);
}

/*
 * curr_fd가 전달하는 내용을 버퍼에 담아주는 함수입니다.
 */

void readRequest(t_request request_msg, int curr_fd)
{
    /* read data from client */
    char buf[1024];
    string msg;
	int n;
	
    while ((n = read(curr_fd, buf, sizeof(buf))) > 0)
        msg += static_cast<string> (buf);
    if (n < 0)
    {
        if (n < 0)
            cerr << "client read error!" << endl;
        disconnect_client(curr_fd);
    }
    else
        parseRequest(request_msg, msg);
}

/*
 * 서버 실행하는 함수입니다.
 */

void Manager::runServer()
{
	int 					kq;

	map<int, string>        clients; // map for client socket:data
	vector<struct kevent>   change_list; // kevent vector for changelist
	struct kevent           event_list[8]; // kevent array for eventlist

	int                     new_events;
	struct kevent*          curr_event;
	vector<t_request>		request_msgs;
	//char                    r_header[1024];
	http_block.getServerBlock();
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
			;
		change_list.clear();

		for (int i = 0; i < new_events; ++i)
		{
			cout << new_events << endl;
			curr_event = &event_list[i];
			if (curr_event->flags & EV_ERROR)
			{
				if (check_socket(curr_event->ident, web_serv.server_socket))
					exit_with_perror("server socket error"); // 500 error
				else
				{
					cerr << "client socket error" << endl;
					disconnect_client(curr_event->ident); // 400 error
				}
			}
			else if (curr_event->filter == EVFILT_READ)
			{
				int idx = 0;

				if (check_socket(curr_event->ident, web_serv.server_socket))
				{
					setClientsocket(request_msgs, change_list, curr_event->ident);
				}
				else if ((idx = findClient(request_msgs, curr_event->ident)) >= 0)
				{
					cout << "after in findCline function" << endl;
					readRequest(request_msgs[idx], curr_event->ident);
					check_msg(request_msgs[idx]);
				}
			}
		}
	}
}
