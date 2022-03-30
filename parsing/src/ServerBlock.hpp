#ifndef SERVERBLOCK_HPP
# define SERVERBLOCK_HPP

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <sstream>
#include "LocationBlock.hpp"


using namespace std;

class ServerBlock
{
	public:
		ServerBlock();
		~ServerBlock();
		//ServerBlock(ServerBlock& copy);
		//ServerBlock& operator = (const ServerBlock& hb);

		// getter
		//vector<string>			getServerName();
		//vector<string>			getListen();
		//string					getClientBodySize();
		//string					getRoot();
		//vector<string>			getIndex();
		//string					getAutoindex();
		//vector<LocationBlock>	getLocationBlock();

		// setter
		void					setServerName(string str);
		void					setListen(string str);
		void					setClientBodySize(string str);
		void					setRoot(string str);
		void					setIndex(string str);
		void					setAutoindex(string str);
		int						setLocationBlock(vector<string> buf, int idx);

	public:
		vector<string> ServerBlock::split(string input, char delimiter);

	private:
		vector<string>			server_name;
		vector<string>			listen;
		string					client_body_size;
		string					root;
		vector<string>			index;
		string					autoindex;
		vector<LocationBlock>	location_block;
};

#endif
