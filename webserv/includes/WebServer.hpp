#ifndef WEBSERVER_HPP
# define WEBSERVER_HPP

# include <iostream>
# include <vector>
# include <string>
# include <map>
# include <sys/stat.h>
# include "ServerBlock.hpp"

# define STATE_FMT "HTTP/1.1 %s %s\nContent-Length: %ld\nContent-Type: %s\n\n%s\n"
# define REDIRECT_FMT "HTTP/1.1 %s %s\nLocation: %s\n"

//template<int> //vector<int> 형을 return 하려고 하니까 template class 아니라고 에러 떠서 추가함

typedef struct s_request {
	string							method;
	string							uri;
	string							version;
	map<string, vector<string> >	header;
	vector<string>					body;
	int								fd;
	string							state_flag; //현재 작업이 에러 시, 이벤트에 있는 read/write를 소모시키기 위해 플래그를 사용함.  
	string							state_str;
	ServerBlock						server_block;
	string							port;
	string							redirect;
}               t_request;

typedef struct s_servinfo
{
	vector<int>			ports;
	vector<int>			server_socket;
	//환경변수
}			t_servinfo;

#endif
