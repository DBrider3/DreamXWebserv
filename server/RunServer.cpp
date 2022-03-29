#include "Server.hpp"
#include "Util.hpp"

/*
 * 
 */

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
    for (int i = 0; i < server_socket.size(); i++)
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

    request_msg.fd = 0;
    request_msg.cgi = 0;

    request_msg.host = 0;
    request_msg.local_uri = 0;
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
    for (int i = 0; i < request_msgs.size(); i++)
        if (request_msgs[i].fd == curr_fd)
            return (i);
    return (0);
}

/*
 * 버퍼에 담아둔 request를 파싱하여 구조체에 담아주는 작업을 하는 함수입니다.
 */

void parseRequest(t_request request_msg, char *request)
{
    char** result = ft_split(request, '\n');
    request_msg->method = strtok(result[0], " ");
    request_msg->uri = strtok(NULL, " ");
    request_msg->version = strtok(NULL, "\n");
    char tmp[1024];
    for (int i = 1; result[i] && (result[i][0] != NULL); i++)
    {
        request_msg->header[strtok(tmp, ": ")] = //strtok(NULL, "\n");
    }
    request_msg->fd = curr_fd;


    // 예시
    void header_validation()
    {
        if (header->method == "GET")
        {

        }
        else if (header->method == "POST")
        {

        }
    }
}

/*
 * curr_fd가 전달하는 내용을 버퍼에 담아주는 함수입니다.
 */

void readRequest(t_request request_msg, int curr_fd)
{
    /* read data from client */
    char buf[1024];
    string msg;

    while (read(curr_fd, buf, sizeof(buf)) > 0)
        msg += static_cast<string> buf;
    if (n < 0)
    {
        if (n < 0)
            cerr << "client read error!" << endl;
        disconnect_client(curr_fd, clients);
    }
    else
        parseRequest(request_msg, msg.c_str());
}

/*
 * 서버 실행하는 함수입니다.
 */

int RunServer(vector<int> server_socket, vector<int> ports)
{
    int 					kq;

    map<int, string>        clients; // map for client socket:data
	vector<struct kevent>   change_list; // kevent vector for changelist
	struct kevent           event_list[8]; // kevent array for eventlist

	int                     new_events;
	struct kevent*          curr_event;
	vector<t_request>        request_msgs;
    char                    r_header[1024];


	if ((kq = kqueue()) == -1)
		exit_with_perror("kqueue() error\n");
	/*server_socket 연결을 위한 읽기 이벤트 등록*/
	for (int i = 0; i < ports.size(); i++)
		change_events(change_list, server_socket[i], EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
    while (1)
    {
        new_events = kevent(kq, &change_list[0], change_list.size(), event_list, 8, NULL);

		if (new_events == -1)
			exit_with_perror("kevent() error\n");
        
        change_list.clear();

        for (int i =0; i < new_events; ++i)
        {
            curr_event = &event_list[i];
            if (curr_event->flags & EV_ERROR)
			{
				if (check_socket(curr_event->ident, server_socket))
					exit_with_perror("server socket error");
				else
				{
					cerr << "client socket error" << endl;
					disconnect_client(curr_event->ident);
				}
			}
            else if (curr_event->filter == EVFILT_READ)
            {
                int idx;

                if (check_socket(curr_event->ident, server_socket))
                    setClientsocket(request_msgs, change_list, curr_event->ident);
                else if (idx = findClient(request_msgs, curr_event->ident))
                    readRequest(request_msg[idx], curr_event->ident);
            }
        }
    }
    
}
