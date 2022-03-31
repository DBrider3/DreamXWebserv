#include "../includes/Manager.hpp"
#include "../includes/Utils.hpp"
#include "../includes/WebServer.hpp"

int main(int argc, char **argv)
{
	Manager manager;
	t_servinfo web_serv;

	try
	{
		string conf = "default";
		if (argc > 2)
			throw(PrintError());
		else if (argc == 2)
			conf = argv[1];
		manager.fileOpen(conf);
		manager.confParsing();
	}
	catch(const exception& e)
	{
		cerr << e.what() << "Too many arguments!😵‍💫" << endl;
	}
	manager.composeServer(manager.getHttpBlock(), web_serv);
	manager.runServer(manager.getHttpBlock(), web_serv);

 	return 0;
}
