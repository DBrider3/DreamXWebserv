#ifndef MANAGER_HPP
# define MANAGER_HPP

#include "HttpBlock.hpp"
#include "Utils.hpp"
#include "ComposeResponse.hpp"

typedef struct s_servinfo
{
	vector<int>			ports;
	vector<int>			server_socket;
	//환경변수
}			t_servinfo;

class Manager
{
	private:
		vector<string>	buffer;
		HttpBlock		http_block;
		t_servinfo		web_serv; //server_socket_fd 배열, port 번호 배열
		//ComposeResponse compRespons;
		// vector<ComposeResponse> compose_response; //

	public:
		/*
		** canonicalForm part
		*/
		Manager();
		~Manager();
		Manager(const Manager& copy);
		Manager& operator = (const Manager& m);

		/*
		** setter part
		*/
		void		setBuffer(string input);
		int			setHttpBlock(vector<string> buf);

		/*
		** getter part
		*/
		HttpBlock	getHttpBlock(void);
		t_servinfo	getWebServ(void);


		/*
		** method part
		*/
		void	fileOpen(string conf);
		void	confParsing(void);
		void 	composeServer(void);
		void 	runServer(void);

//		t_request rmsg;
		void 	check_msg(t_request rmsg);
};

#endif
