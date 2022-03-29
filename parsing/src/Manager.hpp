#ifndef MANAGER_HPP
# define MANAGER_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
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
		// void set_buffer(string input);

		void confParsing(string conf);
		void serverCompose();
		void serverRun();

	private:
		HttpBlock http_block;
		//string buffer;
		void httpParsing();

};

#endif
