#ifndef SERVERBLOCK_HPP
# define SERVERBLOCK_HPP

#include <iostream>
#include <string>
#include <map>
#include <vector>

using namespace std;

class ServerBlock
{
	public:
		ServerBlock();
		~ServerBlock();
		ServerBlock(ServerBlock& copy);
		ServerBlock& operator = (const ServerBlock& hb);

	private:
		map<string, vector<string> > directive;
};

#endif
