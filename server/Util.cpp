#include "Util.hpp"

vector<string> split(string input, char delimiter)
{
	vector<string> answer;
	stringstream ss(input);
	string temp;

	while (getline(ss, temp, delimiter))
		answer.push_back(temp);
	return answer;
}

const char* PrintError::what() const throw()
{
	return ("[Error] : ");
}

void exit_with_perror(const string &msg)
{
	cerr << msg << endl;
	exit(42);
}

int     convStoi(string str)
{
	stringstream ss(str);
	int	num;

	ss >> num;
	return (num);
}