#ifndef MANAGER_HPP
# define MANAGER_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <sstream>
#include "HttpBlock.hpp"

using namespace std;

class Manager
{
	public:
		Manager();
		~Manager();
		//Manager(Manager& copy);
		//Manager& operator = (const Manager& m);

		// string get_buffer(void);
		void	setBuffer(string input);
		int		setHttpBlock(vector<string> buf);
		void confParsing(string conf);
		//void serverCompose();
		//void serverRun();

	public:
		vector<string> split(string input, char delimiter);

	private:
		HttpBlock http_block;
		vector<string> buffer;
		//void httpParsing();


};

#endif
