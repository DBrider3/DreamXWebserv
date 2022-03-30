#include "Utils.hpp"

const char* PrintError::what() const throw()
{
	return ("[Error] : ");
}

vector<string> split(string input, char delimiter)
{
	vector<string> answer;
	stringstream ss(input);
	string temp;

	while (getline(ss, temp, delimiter))
		answer.push_back(temp);
	string tmp = answer.back();
	answer.pop_back();
	answer.push_back(tmp.substr(0, tmp.length() - 1));
	return answer;
}