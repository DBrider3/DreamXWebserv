#ifndef COMPOSERESPONSE_HPP
# define COMPOSERESPONSE_HPP

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

typedef struct s_request {
	string	method;
	string	uri; // requset uri
	string	local_uri; // root를 뺀 uri
	string	version;

	// server block 내부 변수
	string	client_body_size;
	string	port;
	string	root;
}				t_request;

typedef struct	s_header
{
	string	date;
	int		ct_len;
	string	ct_type;
	int		cgi;
	string 	query;
}				t_header;

class PrintError : public std::exception
{
	public:
		virtual const char* what() const throw();
};

class ComposeResponse
{
	private:
		t_header	header;
		string		body;
		string		response;
		t_request	request;
		map<string, string> env_set;


	public:
		/*
		** canonicalForm part
		*/
		ComposeResponse();
		ComposeResponse(t_request req);
		~ComposeResponse();
		ComposeResponse(ComposeResponse& copy);
		ComposeResponse& operator = (const ComposeResponse& cr);

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
		//char**	setCommand(string command, string path);
		void	coreResponse(void);
		void	fillResponse(void);
};

#endif
