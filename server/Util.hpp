#ifndef UTIL_HPP
# define UTIL_HPP

# include <iostream>
# include <cstdlib>
# include <string>
# include <cstring>

# include <sys/socket.h>
# include <arpa/inet.h>

# include <sys/types.h>
# include <sys/event.h>
# include <sys/time.h>

# define LISTEN_NUM 10 

using namespace std;

int    exit_with_perror(const string& msg);
int     convStoi(string str);

#endif