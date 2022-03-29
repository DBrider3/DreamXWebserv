#ifndef HTTPBLOCK_HPP
# define HTTPBLOCK_HPP

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include "ServerBlock.hpp"

using namespace std;

class HttpBlock
{
	public:
		HttpBlock();
		~HttpBlock();
		//HttpBlock(HttpBlock& copy);
		//HttpBlock& operator = (const HttpBlock& hb);

		//void get_directive(string key, vector<string> value);
		//void get_server(vector<ServerBlock> server);
		//map<string, vector<string> > set_directive(void);
		//vector<ServerBlock> set_server(void);

	private:
		map<string, vector<string> > directive;
		vector<ServerBlock> server;
};

#endif
