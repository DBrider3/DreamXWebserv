#ifndef CLIENTCONTROL_HPP
# define CLIENTCONTROL_HPP

#include <iostream>
#include <string>
#include <cstring>
#include <fstream>
#include <map>
#include <vector>
#include <exception>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <sstream>

using namespace std;
# define PHPCGI "/Users/songju/Desktop/DreamXWebserv/ComposeResponse/php-cgi"
//# define PHPCGI "/Users/dcho/Born2Code/DreamXWebserv/ComposeResponse/php-cgi"
# define RESPONSE_FMT "HTTP/1.1 %d %s\nContent-Length: %d\nContent-Type: %s\n\n%s"
# define CHUNK_FMT "HTTP/1.1 %d %s\nTransfer-Encoding: chunked\nContent-Type: %s\n\n"
# define REDIRECT_FMT "HTTP/1.1 %s %s\nLocation: %s\n"

typedef struct	s_multipart {
	string	boundary_key;
	string	file_name;
	string	type;
	string	data;
}				t_multipart;

typedef struct s_request {
	string	method;
	string	uri; // requset uri
	//string	local_uri; // root를 뺀 uri
	string	version;
	string	query_str;

	// server block 내부 변수
	string	client_body_size;
	//string	port;
	string	root;
	map<string, vector<string> >	header;
	vector<string>					body;
}				t_request;

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

class PrintError : public std::exception
{
	public:
		virtual const char* what() const throw();
};

class ClientControl
{
	private:
		t_response	response;
		t_request	request;
		t_multipart	multipart;
		string		body;
		//string		response;
		string		port;
		map<string, string> env_set;
		ServerBlock		server_block;


	public:
		/*
		** canonicalForm part
		*/
		ClientControl();
		ClientControl(t_request req);
		~ClientControl();
		ClientControl(ClientControl& copy);
		ClientControl& operator = (const ClientControl& cr);

		/*
		** getter part
		*/

		/*
		** setter part
		*/

		void	parseQuery(void);
		void	findMime(void);
		void	setEnv(void);
		char**	convToChar(map<string, string> m, int flag);
		void	processMultipart(void);
		void	coreResponse(void);
		void	fillResponse(void);
};

#endif
