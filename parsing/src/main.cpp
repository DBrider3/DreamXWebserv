#include <iostream>
#include <fstream>
#include "Manager.hpp"


#define TRUE 1
#define FALSE 0

using namespace std;

void	printError(string str)
{
	cout << str << endl;
	exit(1);
}

int main(int argc, char **argv)
{
	Manager manager;
	string conf = "default";
	if (argc > 2)
		printError("Too many arguments!😵‍💫\n");
	else if (argc == 2)
		conf = argv[1];
	manager.confParsing(conf);
	// ifstream fin(conf);
	// if (fin.is_open())
	// {
	// 	string buffer;
	// 	while (getline(fin, buffer))
	// 		manager.set_buffer(buffer);
	// 	fin.close();
	// }
	// else
	// 	printError("File open error!😵‍\n");
 	return 0;
}
