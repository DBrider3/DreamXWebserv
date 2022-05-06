#include "../includes/Manager.hpp"
#include "../includes/Utils.hpp"

void handleSigpipe(int signo)
{
	cout << "앙 기모찌 뭐야 이거 ㅋㅋㅋㅋㅋ" << endl;
	(void)signo;
}

int main(int argc, char **argv)
{
	Manager manager;	

	try
	{
		string conf = "default";
		if (argc > 2)
			throw(PrintError());

		else if (argc == 2)
			conf = argv[1];
		signal(SIGPIPE, handleSigpipe);
		manager.fileOpen(conf);
		manager.confParsing();
	}
	catch(const exception& e)
	{
		cerr << e.what() << "Too many arguments!😵‍💫" << endl;
	}
	manager.composeServer();
	manager.runServer();
//서버소켓 닫아주기
 	return 0;
}
