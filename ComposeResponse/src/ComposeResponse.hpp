#ifndef COMPOSERESPONSE_HPP
# define COMPOSERESPONSE_HPP

#include <iostream>
#include <string>
#include <cstring>
#include <map>

using namespace std;

static const char *basic_env[] = {
	"AUTH_TYPE",
	"CONTENT_LENGTH",
	"CONTENT_TYPE",
	"GATEWAY_INTERFACE",
	"PATH_INFO",
	"PATH_TRANSLATED",
	"QUERY_STRING",
	"REMOTE_ADDR",
	"REMOTE_IDENT",
	"REMOTE_USER",
	"REQUEST_METHOD",
	"REQUEST_URI",
	"SCRIPT_NAME",
	"SERVER_NAME",
	"SERVER_PORT",
	"SERVER_PROTOCOL",
	"SERVER_SOFTWARE",
	"REDIRECT_STATUS",
	NULL
	};

typedef struct	s_header
{
	string	uri;
	string	local_uri;
	string	date;
	int		ct_length;
	string	ct_type;
	int		cgi;
	string 	query;
}				t_header;

class ComposeResponse
{
	private:
		t_header	header;
		string		body;
		string		response;
		std::map<std::string, std::string> env_set;


	public:
		/*
		** canonicalForm part
		*/
		ComposeResponse();
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
		//char**	ConvToChar(void);
};

#endif
