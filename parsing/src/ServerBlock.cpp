#include "ServerBlock.hpp"

ServerBlock::ServerBlock()
{

}
ServerBlock::~ServerBlock()
{

}

//vector<string>			getServerName()
//{

//}
//vector<string>			getListen()
//{

//}
//string					getClientBodySize()
//{

//}
//string					getRoot()
//{

//}
//vector<string>			getIndex()
//{

//}
//string					getAutoindex()
//{

//}
// vector<LocationBlock>	getLocationBlock()
// {
// }


vector<string> ServerBlock::split(string input, char delimiter)
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

void					ServerBlock::setServerName(string str)
{
	server_name.push_back(str);
}
void					ServerBlock::setListen(string str)
{
	listen.push_back(str);
}
void					ServerBlock::setClientBodySize(string str)
{
	client_body_size = str;
}
void					ServerBlock::setRoot(string str)
{
	root = str;
}
void					ServerBlock::setIndex(string str)
{
	index.push_back(str);
}
void					ServerBlock::setAutoindex(string str)
{
	autoindex = str;
}
int					ServerBlock::setLocationBlock(vector<string> buf, int idx)
{
	LocationBlock tmp_location;

	while (!(buf[idx].empty()))
	{
		vector<string> tmp;
		tmp = split(buf[idx], ' ');

		if (tmp[0] == "\t\tserver_name")
		{
			for (size_t i = 1; i < tmp.size(); i++)
			{
				tmp_location.setServerName(tmp[i]);
				cout << tmp[i] << endl;
			}
		}
		else if (tmp[0] == "\t\tclient_body_size")
		{
			tmp_location.setClientBodySize(tmp[1]);
			cout << tmp[1] << endl;
		}

		idx++;
	}
	location_block.push_back(tmp_location);
	return ++idx;
}
