#ifndef UTILS_HPP
# define UTILS_HPP

#include <string>
#include <iostream>
#include <vector>
#include <sstream>
#include <fstream>
#include <map>
#include <exception>

#define TRUE 1
#define FALSE 0

using namespace std;

class PrintError : public std::exception
{
	public:
		virtual const char* what() const throw();
};

vector<string>		deleteSemicolon(vector<string> answer);
vector<string>		split(string input, char delimiter);

#endif