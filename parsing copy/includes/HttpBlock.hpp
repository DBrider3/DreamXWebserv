#ifndef HTTPBLOCK_HPP
# define HTTPBLOCK_HPP

#include "Utils.hpp"
#include "ServerBlock.hpp"

class HttpBlock
{
	private:
		vector<string>		index;
		vector<string>		limit_except;
		vector<ServerBlock>	server_block;

	public:
		/*
		** canonicalForm part
		*/
		HttpBlock();
		~HttpBlock();
		HttpBlock(HttpBlock& copy);
		HttpBlock& operator = (const HttpBlock& hb);

		/*
		** getter part
		*/
		// vector<string>			getIndex();
		// vector<string>			getLimitExcept();
		vector<ServerBlock>		getServerBlock();

		/*
		** setter part
		*/
		void					setIndex(string str);
		void					setLimitExcept(string str);
		int						setServerBlock(vector<string> buf, int idx);
};

#endif
