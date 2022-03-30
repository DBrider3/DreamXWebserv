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

void	Manager::setBuffer(string input)
{
	buffer.push_back(input);
}

vector<string> Manager::split(string input, char delimiter)
{
	vector<string> answer;
	stringstream ss(input);
	string temp;

	while (getline(ss, temp, delimiter))
		answer.push_back(temp);
	// 문장 마지막 부분 ';' 삭제 part (refactor 필요)
	string tmp = answer.back();
	answer.pop_back();
	answer.push_back(tmp.substr(0, tmp.length() - 1));
	return answer;
}

int		Manager::setHttpBlock(vector<string> buf)
{
	int res = 1;
	while (!(buf[res].empty()))
	{
		vector<string> tmp;
		tmp = split(buf[res], ' ');

		if (tmp[0] == "\tindex")
		{
			for (size_t i = 1; i < tmp.size(); i++)
			{
				http_block.setIndex(tmp[i]);
				cout << tmp[i] << endl;
			}
		}
		else if (tmp[0] == "\tlimit_except")
		{
			for (size_t i = 1; i < tmp.size(); i++)
			{
				http_block.setLimitExcept(tmp[i]);
				cout << tmp[i] << endl;
			}
		}
		res++;
	}
	return ++res;
}

void Manager::confParsing(string conf)
{
	ifstream fin(conf);
	if (fin.is_open())
	{
		string temp;

		while (getline(fin, temp))
			setBuffer(temp);
		fin.close();
	}
	else
		//printError("File open error!😵‍\n");
		cout << "error\n" << endl;
	int idx = 0;
	if (buffer[idx] == "http {")
		idx = setHttpBlock(buffer);
	else
		cout << " http block parsing error" << endl; // error 처리 (try, except)
	if (buffer[idx] == "\tserver {")
		idx = http_block.setServerBlock(buffer, idx);
	else
		cout << " server block parsing error" << endl; // error 처리 (try, except)
	//if (buffer[idx] == "\t\tlocation ")
	//	idx = http_block.setServerBlock(buffer, idx);
	//else
	//	cout << " server block parsing error" << endl; // error 처리 (try, except)

}
