#include "HttpBlock.hpp"

HttpBlock::HttpBlock()
{

}
HttpBlock::~HttpBlock()
{

}
void	HttpBlock::setIndex(string str)
{
	index.push_back(str);
}

vector<string> HttpBlock::split(string input, char delimiter)
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

void	HttpBlock::setLimitExcept(string str)
{
	limit_except.push_back(str);
}
// vector<ServerBlock>	HttpBlock::getServerBlock() // Location을 넣는
// {


// 	server_block.
// }

 int			HttpBlock::setServerBlock(vector<string> buf, int idx)
 {
	ServerBlock tmp_server;

	while (buf[idx] != "\t}")
	{
		vector<string> tmp;
		tmp = split(buf[idx], ' ');

		if (tmp[0] == "\t\tserver_name")
		{
			for (size_t i = 1; i < tmp.size(); i++)
			{
				tmp_server.setServerName(tmp[i]);
				cout << tmp[i] << endl;
			}
		}
		else if (tmp[0] == "\t\tlisten")
		{
			for (size_t i = 1; i < tmp.size(); i++)
			{
				tmp_server.setListen(tmp[i]);
				cout << tmp[i] << endl;
			}
		}
		else if (tmp[0] == "\t\tclient_body_size")
		{
			tmp_server.setClientBodySize(tmp[1]);
			cout << tmp[1] << endl;
		}
		else if (tmp[0] == "\t\troot")
		{
			tmp_server.setRoot(tmp[1]);
			cout << tmp[1] << endl;
		}
		else if (tmp[0] == "\t\tindex")
		{
			for (size_t i = 1; i < tmp.size(); i++)
			{
				tmp_server.setIndex(tmp[i]);
				cout << tmp[i] << endl;
			}
		}
		else if (tmp[0] == "\t\tautoindex")
		{
			tmp_server.setAutoindex(tmp[1]);
			cout << tmp[1] << endl;
		}
		else if (tmp[0] == "\t\tlocation")
		{
			LocationBlock tmp_location;
			//location_match
			for (size_t i = 1; i < tmp.size() - 1; i++)
			{
				tmp_location.setMatch(tmp[i]);
				cout << tmp[i] << endl;
			}
			// 여기에 얘들을 재워 두죠
		}
		idx++;
	}
	server_block.push_back(tmp_server);
	return ++idx;
 }
