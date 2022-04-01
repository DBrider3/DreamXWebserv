#include "../includes/WebServer.hpp"
#include "../includes/Utils.hpp"
#include "../includes/Manager.hpp"

#include <errno.h>
/*
 *
 */

// void Manager::check_msg(t_request rmsg)
// {
// 	cout << rmsg.method << endl;
// 	cout << rmsg.uri << endl;
// 	cout << rmsg.version << endl;
	
// 	for (map<string, vector<string> >::iterator it = rmsg.header.begin(); it != rmsg.header.end(); it++)
// 	{
// 		for (vector<string>::iterator itt = it->second.begin(); itt != it->second.end(); itt++)
// 			cout << *itt << " " ;
// 		cout << endl;
// 	}
// 	cout << rmsg.cgi << endl;
// 	cout << rmsg.fd << endl;
// 	for (vector<string>::iterator it = rmsg.body.begin(); it != rmsg.body.end(); it++)
// 		cout << *it << endl;
// }

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
	request_msg.method = "";
	request_msg.uri = "";
	request_msg.version = "";
	request_msg.fd = client_socket;
	return (request_msg);
}

/*
 * sever_socket을 토대로 client_socket을 구성하는 함수입니다.
 */

void setClientsocket(vector<t_request> &request_msgs, vector<struct kevent> &change_list, uintptr_t server_socket)
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
	for (size_t i = 0; i < request_msgs.size(); i++)
		if (request_msgs[i].fd == curr_fd)
			return (i);
	return (-1);
}

/*
 * 버퍼에 담아둔 request를 파싱하여 구조체에 담아주는 작업을 하는 함수입니다.
 */

void parseRequest(t_request &request_msg, string request)
{
	stringstream ss;
	vector<string> result; //요청메시지가 한 줄 한 줄 저장되는 변수
	vector<string>::iterator it;
	//char *tmp;
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

void readRequest(t_request &request_msg, int curr_fd)
{
	/* read data from client */
	char buf[10];
	stringstream ss;
	string msg;
	int n;
	
	n = 0;
	while ((n = read(curr_fd, buf, sizeof(buf) - 1)) > 0)
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
	//     if (n < 0)
	//         cerr << "client read error!" << endl;
	// 	cout << "1\n";
	//     disconnect_client(curr_fd);
	// }
	// else
	parseRequest(request_msg, msg);
}

int checkMethod(t_request request_msg, vector<string> method)
{
	for (vector<string>::iterator it = method.begin(); it != method.end(); it++)
		if (request_msg.method == *it)
			return (1);
	return (0);
}


void sendErrorPage(t_request msgs, string err_num, string err_str)
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

	local_uri = "./error_pages/" + err_num + ".html";
	stat(local_uri.c_str(), &st);
	ct_len = st.st_size;

	bodyfd = open(local_uri.c_str(), O_RDONLY);

	n = 0;
	//cout << "여기는 버퍼 입니다." << endl;
	while ((n = read(bodyfd, buf, sizeof(buf) - 1)) > 0)
	{
		buf[9] = '\0'; //여기에 널이 필요할까?
		ss << buf;
		//cout << "buf: "<< buf << endl;
		body += ss.str();
		ss.str("");
		memset(buf, 0, 10);
	}
	//if (n < 0)
		//perror("[ERR] Failed to read request.\n");
	//cout << "ss!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << ss << endl;
	//body += ss.str();
	//cout << "여기는 바디 입니다." << endl;
	//cout << body << endl;
	//"HTTP/1.1 %s %s\nContent-Length: %ld\nContent-Type: %s\n\n%s"
	
	sprintf(r_header, RESPONSE_FMT, err_num.c_str(), err_str.c_str(), ct_len, "html/text");
	write(msgs.fd, r_header, strlen(r_header));
	write(msgs.fd, body.c_str(), body.size());
	cout << "c_fd" << msgs.fd << endl;

	disconnect_client(msgs.fd);
	// write(1, r_header, strlen(r_header));
	// write(1, body.c_str(), body.size());
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
	int idx;

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
			;
		change_list.clear();

		for (int i = 0; i < new_events; ++i)
		{
			curr_event = &event_list[i];
			if (curr_event->flags & EV_ERROR)
			{
				if (check_socket(curr_event->ident, web_serv.server_socket))
					exit_with_perror("server socket error"); // 500 error
				else
				{
					cerr << "client socket error" << endl;
					cout << "2\n";
					disconnect_client(curr_event->ident); // 400 error
				}
			}
			else if (curr_event->filter == EVFILT_READ)
			{
				if (check_socket(curr_event->ident, web_serv.server_socket))
				{
					setClientsocket(request_msgs, change_list, curr_event->ident);
				}
				else if ((idx = findClient(request_msgs, curr_event->ident)) >= 0)
				{
					readRequest(request_msgs[idx], curr_event->ident);
					//check_msg(request_msgs[idx]);
				}
			}
			else if (curr_event->filter == EVFILT_WRITE)
			{
				if ((idx = findClient(request_msgs, curr_event->ident)) >= 0)
				{
					if (checkMethod(request_msgs[idx], http_block.getLimitExcept()))
					{
	
					}
					else
					{
						sendErrorPage(request_msgs[idx], "403", "Forbidden");
					}
				}
			}
		}
	}
}