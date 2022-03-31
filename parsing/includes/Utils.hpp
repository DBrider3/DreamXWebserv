#ifndef UTILS_HPP
# define UTILS_HPP

# include <string>
# include <iostream>
# include <vector>
# include <sstream>
# include <fstream>
# include <map>
# include <exception>

# define TRUE 1
# define FALSE 0
# define LISTEN_NUM 10

using namespace std;

class PrintError : public std::exception
{
	public:
		virtual const char* what() const throw();
};

vector<string>		deleteSemicolon(vector<string> answer);
vector<string>		split(string input, char delimiter);

int    exit_with_perror(const string& msg);
int    convStoi(string str);
vector<string> split(string input, char delimiter);

#endif
