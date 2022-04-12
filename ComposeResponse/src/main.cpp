#include "ClientControl.hpp"

int	main(void)
{
	try
	{
		t_request r;
		t_response res;
		//char** env;

		//r.root = "/Users/dcho/Born2Code/DreamXWebserv";
		r.root = "/Users/songju/Desktop/DreamXWebserv";
		res.local_uri = "/testcode/info.php";
		//r.local_uri = "/testcode/test.html";
		r.method = "POST";
		r.uri = "/testcode/info.php";
		//r.uri = "/testcode/test.html";
		r.version = "HTTP/1.1";
		ClientControl c(r);
		c.port = "80";
		//
		c.parseQuery();
		c.findMime(); 
		c.setEnv();
		//env = c.convToChar();

		//cout << "1" << endl;
		// int i = 0;
		// while (env[i] != NULL)
		// 	cout << env[i++] << endl;
		c.coreResponse();

	}
	catch(const std::exception& e)
	{
		cerr << e.what() << "request message error" << endl;
	}

	return 0;
}
