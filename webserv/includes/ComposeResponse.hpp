#ifndef COMPOSERESPONSE_HPP
# define COMPOSERESPONSE_HPP

#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include "ServerBlock.hpp"
#include "WebServer.hpp"

using namespace std;

typedef struct	s_header
{
	string	local_uri;
	string	date;
	int		ct_length;
	string	ct_type;
	int		cgi;
}				t_header;

class ComposeResponse
{
	private:
		t_header		header;
		string			body;
		string			response;
		vector<string>	server_index; //서버 블록 내 index 절대 경로 담아둠


	public:
		/*
		** canonicalForm part
		*/
		// ComposeResponse();
		// ~ComposeResponse();
		// ComposeResponse(ComposeResponse& copy);
		// ComposeResponse& operator = (const ComposeResponse& cr);

		/*
		** getter part
		*/
		string	getServerIndex();


		/*
		** setter part
		*/
		void 	setServerIndex(string root_index)
		{
			server_index.push_back(root_index);
		}
		void	findMime(void);
		void	processMethod(ServerBlock server_block, t_request &request_msg);
		int		checkUri(ServerBlock server_block, t_request &request_msg);
		void	checkAutoIndex(ServerBlock server_block);
};


		// void	ComposeResponse::setServerIndex(string root_index)
		// {
		// 	server_index.push_back(root_index);
		// }

#endif
