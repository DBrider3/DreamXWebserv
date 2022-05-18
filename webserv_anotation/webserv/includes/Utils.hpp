#ifndef UTILS_HPP
# define UTILS_HPP

# include <string>
# include <iostream>
# include <sstream>
# include <cstring>
# include <fstream>
# include <cstdlib>
# include <map>
# include <vector>
# include <exception>

# include <sys/socket.h>
# include <sys/stat.h>
# include <arpa/inet.h>

# include <fcntl.h> //close()
# include <unistd.h>

# include <sys/types.h>
# include <sys/event.h> //kevent()
# include <sys/time.h>
# include <ctime>

# include <dirent.h> // OpenDIR

# define _CRT_SECURE_NO_WARNINGS 

# define TRUE 1
# define FALSE 0
# define LISTEN_NUM 4096

# define RED    "\033[1;31m"
# define GREEN  "\033[1;32m"
# define DARKGREEN "\033[2;32m"
# define YELLOW "\033[1;33m"
# define BLUE "\033[1;34m"
# define BROWN "\033[0;40;33m"
# define PRPL "\033[0;35m"
# define EOC    "\033[0;0m"
# define ITALIC "\033[3m"

using namespace std;

class PrintError : public std::exception
{
	public:
		virtual const char* what() const throw();
};

vector<string>		deleteSemicolon(vector<string> answer);
vector<string>		split(string input, char delimiter);

int   				convStoi(string str);
vector<string> 		split(string input, char delimiter);
void 				exit_with_perror(const string &msg);

#endif
