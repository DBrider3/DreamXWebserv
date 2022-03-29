#include "Manager.hpp"

Manager::Manager()
{

}
Manager::~Manager()
{

}
//Manager::Manager(Manager& copy)
//{

//}
//Manager& Manager::operator = (const Manager& m)
//{

//}
// string Manager::get_buffer(void)
// {
// 	return (buffer);
// }

// void Manager::set_buffer(string input)
// {
// 	buffer += input;
// 	buffer += "\n";
// }

void Manager::confParsing(string conf)
{
	ifstream fin(conf);
	if (fin.is_open())
	{
		string buffer;
		while (getline(fin, buffer))
		{

		}
		fin.close();
	}
	else
		//printError("File open error!😵‍\n");
		cout << "error\n" << endl;
}
