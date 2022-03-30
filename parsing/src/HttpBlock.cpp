#include "HttpBlock.hpp"

HttpBlock::HttpBlock()
{

}
HttpBlock::~HttpBlock()
{

}
HttpBlock::HttpBlock(HttpBlock& copy)
{
	*this = copy;
}
HttpBlock& HttpBlock::operator = (const HttpBlock& hb)
{
	if (this == &hb)
		return (*this);
	index = hb.index;
	limit_except = hb.limit_except;
	server_block = hb.server_block;
	return (*this);
}


void	HttpBlock::setIndex(string str)
{
	index.push_back(str);
}

void	HttpBlock::setLimitExcept(string str)
{
	limit_except.push_back(str);
}


vector<ServerBlock>	HttpBlock::getServerBlock()
{
	return (server_block);
}

int			HttpBlock::setServerBlock(vector<string> buf, int idx)
{
	ServerBlock tmp_server;

	while (buf[idx] != "\t}")
	{
		if (!buf[idx].empty())
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
				idx = tmp_server.setLocationBlock(tmp_location, buf, ++idx);
			}
		}
		idx++;
	}
	server_block.push_back(tmp_server);
	if (buf[++idx].empty())
		return ++idx;
	return idx;
}
