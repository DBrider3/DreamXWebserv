#ifndef UTILS_HPP
# define UTILS_HPP

# include <string>
# include <iostream>
# include <vector>
# include <sstream>
# include <fstream>
# include <map>
# include <exception>

# include <sys/socket.h>
# include <arpa/inet.h>

# include <fcntl.h> //close()
# include <unistd.h>

# include <sys/types.h>
# include <sys/event.h> //kevent()
# include <sys/time.h>

# define TRUE 1
# define FALSE 0
# define LISTEN_NUM 5

using namespace std;

class PrintError : public std::exception
{
	public:
		virtual const char* what() const throw();
};

vector<string>		deleteSemicolon(vector<string> answer);
vector<string>		split(string input, char delimiter);

int    convStoi(string str);
vector<string> split(string input, char delimiter);

void exit_with_perror(const string &msg);

#endif
