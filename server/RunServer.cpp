#include "Server.hpp"
#include "Util.hpp"

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
 * 서버 실행하는 함수입니다.
 */

int RunServer(int *server_socket, vector<int> ports)
{
    int 					kq;

    map<int, string>        clients; // map for client socket:data
	vector<struct kevent>   change_list; // kevent vector for changelist
	struct kevent           event_list[8]; // kevent array for eventlist

	int                     new_events;
	struct kevent*          curr_event;
	vector<t_header>        headers;
    char                    r_header[1024];


	if ((kq = kqueue()) == -1)
		exit_with_perror("kqueue() error\n");
	/*server_socket 연결을 위한 읽기 이벤트 등록*/
	for (int i = 0; i < ports.size(); i++)
		change_events(change_list, server_socket[i], EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
    while (1)
    {
        
    }
    
}
