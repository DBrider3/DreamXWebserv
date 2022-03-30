#ifndef UTIL_HPP
# define UTIL_HPP

# include <iostream>
# include <sstream>
# include <cstdlib> //exit()
# include <string> //getline()
# include <cstring>
# include <unistd.h>
# include <fcntl.h> //close()
# include <map> // map<>
# include <vector>

# include <sys/socket.h>
# include <arpa/inet.h>

# include <sys/types.h>
# include <sys/event.h> //kevent()
# include <sys/time.h>

# define LISTEN_NUM 10 

using namespace std;

class PrintError : public exception
{
	public:
		virtual const char* what() const throw();
};

int    exit_with_perror(const string& msg);
int    convStoi(string str);
vector<string> split(string input, char delimiter);

#endif