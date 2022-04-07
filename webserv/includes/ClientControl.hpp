#ifndef COMPOSERESPONSE_HPP
# define COMPOSERESPONSE_HPP

# include <iostream>
# include <string>
# include <cstring>
# include <vector>
# include <map>
# include <sys/stat.h>
# include "ServerBlock.hpp"

# define STATE_FMT "HTTP/1.1 %s %s\nContent-Length: %ld\nContent-Type: %s\n\n%s\n"
# define REDIRECT_FMT "HTTP/1.1 %s %s\nLocation: %s\n"

using namespace std;

//template<int> //vector<int> 형을 return 하려고 하니까 template class 아니라고 에러 떠서 추가함

typedef struct s_request {
	string							method;
	string							uri;
	string							version;
	map<string, vector<string> >	header;
	vector<string>					body;
}               t_request;

typedef struct	s_response
{
	string	local_uri;
	string	date;
	int		ct_length;
	string	ct_type;
	int		cgi;
	string	state_flag; //현재 작업이 에러 시, 이벤트에 있는 read/write를 소모시키기 위해 플래그를 사용함.  
	string	state_str; //빼야함
	string	redirect_uri;
}				t_response;

class ClientControl //clientpro
{
	private:
		t_response		response; //t_response
		t_request 		request;
		//서버블록 
		string			body;
		string			response;
		int				client_fd;
		vector<string>	server_index; //서버 블록 내 index 절대 경로 담아둠
		ServerBlock		server_block;
		string			port;


	public:
		/*
		** canonicalForm part
		*/
		// ClientControl();
		// ~ClientControl();
		// ClientControl(ClientControl& copy);
		// ClientControl& operator = (const ClientControl& cr);

		/*
		** getter part
		*/
		string				getServerIndex(void);

		ServerBlock 		getServerBlock(void)
		{
			return (server_block);
		}

		t_request	getRequest(void)
		{
			return (request);
		}

		t_response	getResponse(void)
		{
			return (response);
		}

		int		getClientFd(void)
		{
			return (client_fd);
		}


		/*
		** setter part
		*/
		void 		setServerBlock(ServerBlock server_block)
		{
			this->server_block = server_block;
		}

		void		setPort(string port)
		{
			this->port = port;
		}

		void		setClientFd(int client_socket)
		{
			this->client_fd = client_socket;
		}

		void		setClientsocket(vector<struct kevent> &change_list, uintptr_t server_socket, ServerBlock server_block);
		void 		setServerIndex(string root_index)
		{
			server_index.push_back(root_index);
		}
		void		findMime(void);
		void 		initRequestMsg(void);
		void		processMethod(void);
		int			checkUri(ServerBlock server_block, t_request &request_msg);
		void		checkAutoIndex(ServerBlock server_block);
		void		readRequest(void);
		void		parseRequest(string msg);
		void		sendRedirectPage(void);
		int			checkMethod(vector<string> method);
		int			checkUri(void);
		void		checkAutoIndex(void);
		int			findIndex(string uri);
		int			deleteFile(void);
		//int		getFile();
		//int		postFile();
};


#endif
