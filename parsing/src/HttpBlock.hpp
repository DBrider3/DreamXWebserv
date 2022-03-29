#ifndef HTTPBLOCK_HPP
# define HTTPBLOCK_HPP

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <sstream>
//#include "Manager.hpp"
#include "ServerBlock.hpp"

using namespace std;

class HttpBlock
{
	public:
		HttpBlock();
		~HttpBlock();
		//HttpBlock(HttpBlock& copy);
		//HttpBlock& operator = (const HttpBlock& hb);

		// vector<string>			getIndex();
		// vector<string>			getLimitExcept();
		// vector<ServerBlock>		getServerBlock();
		void					setIndex(string str);
		void					setLimitExcept(string str);
		int						setServerBlock(vector<string> buf, int idx);

	public:
		vector<string> split(string input, char delimiter);

		//void get_directive(string key, vector<string> value);
		//void get_server(vector<ServerBlock> server);
		//map<string, vector<string> > set_directive(void);
		//vector<ServerBlock> set_server(void);

	private:
		vector<string>		index;
		vector<string>		limit_except;
		vector<ServerBlock>	server_block;
};

#endif
