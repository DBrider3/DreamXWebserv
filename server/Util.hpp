#ifndef UTIL_HPP
# define UTIL_HPP

# include <iostream>
# include <cstdlib> //exit()
# include <string>
# include <cstring>
# include <unistd.h>
# include <fcntl.h> //close()

# include <sys/socket.h>
# include <arpa/inet.h>

# include <sys/types.h>
# include <sys/event.h> //kevent()
# include <sys/time.h>

# define LISTEN_NUM 10 

using namespace std;

typedef struct s_request {
	char*							method;
	char*							uri;
	char*							version;
	map<string, vector<string> >	header;
	char							ct_type[40];
	char*							local_uri;
	int								cgi;
	int								fd;
}               t_request;

int    exit_with_perror(const string& msg);
int     convStoi(string str);

#endif