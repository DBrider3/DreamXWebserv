#ifndef COMPOSERESPONSE_HPP
# define COMPOSERESPONSE_HPP

#include <iostream>
#include <string>
#include <cstring>


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
		t_header	header;
		string		body;
		string		response;


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

		void	findMime(void);
};

#endif
