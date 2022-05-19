#include "../includes/Utils.hpp"

class ClientControl;

const char* PrintError::what() const throw()
{
	return ("[Error] : ");
}

vector<string>	deleteSemicolon(vector<string> answer)
{
	string tmp = answer.back();
	answer.pop_back();
	answer.push_back(tmp.substr(0, tmp.length() - 1));
	return (answer);
}

vector<string> split(string input, char delimiter)
{
	vector<string> answer;
	stringstream ss(input);
	string temp;

	while (getline(ss, temp, delimiter))
		answer.push_back(temp);
	return (answer);
}

/*
**Server
*/

int     convStoi(string str)
{
	stringstream ss(str);
	int	num;

	ss >> num;
	return (num);
}
void exit_with_perror(const string &msg)
{
	cerr << msg << endl;
	exit(42);
}
