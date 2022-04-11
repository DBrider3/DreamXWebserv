#include "ComposeResponse.hpp"

int	main(void)
{
	try
	{
		t_request r;
		//char** env;

		r.root = "/Users/dcho/Born2Code/DreamXWebserv";
		//r.root = "/Users/songju/Desktop/DreamXWebserv";
		r.local_uri = "/testcode/test.php";
		//r.local_uri = "/testcode/test.html";
		r.method = "GET";
		r.uri = "/testcode/test.php";
		//r.uri = "/testcode/test.html";
		r.port = "80";
		r.version = "HTTP/1.1";
		ComposeResponse c(r);
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
