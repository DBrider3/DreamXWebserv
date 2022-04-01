#ifndef WEBSERVER_HPP
# define WEBSERVER_HPP

# include <iostream>
# include <vector>
# include <string>
# include <map>
# include <sys/stat.h>
# include "ServerBlock.hpp"

# define RESPONSE_FMT "HTTP/1.1 %s %s\nContent-Length: %ld\nContent-Type: %s\n\n%s\n"

//template<int> //vector<int> 형을 return 하려고 하니까 template class 아니라고 에러 떠서 추가함

typedef struct s_request {
	string							method;
	string							uri;
	string							version;
	map<string, vector<string> >	header;
	vector<string>					body;
	int								cgi;
	int								fd;
}               t_request;

typedef struct s_servinfo
{
	vector<int>			ports;
	vector<int>			server_socket;
	//환경변수
}			t_servinfo;

#endif
