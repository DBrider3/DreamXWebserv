#include "Manager.hpp"
#include "Utils.hpp"

int main(int argc, char **argv)
{
	try
	{
		Manager manager;
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
 	return 0;
}
