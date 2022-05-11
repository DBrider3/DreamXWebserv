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

//string base64_encode(const string &in) { 
//	typedef unsigned char uchar; 
//	string out;
//	int val = 0, valb = -6;
//	for (uchar c : in) {
//		val = (val << 8) + c;
//		valb += 8;
//		while (valb >= 0) {
//		
//		out.push_back("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"[(val >> valb) & 0x3F]); valb -= 6; } }
//}
