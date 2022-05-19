#include "../includes/HttpBlock.hpp"

HttpBlock::HttpBlock()
{

}

HttpBlock::~HttpBlock()
{

}

HttpBlock::HttpBlock(const HttpBlock& copy)
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

vector<string>	HttpBlock::getLimitExcept(void)
{
	return (limit_except);
}

vector<ServerBlock>	HttpBlock::getServerBlock(void)
{
	return (server_block);
}

int			HttpBlock::setServerBlock(vector<string> buf, int idx) 
{
	/*
	** buf를 split으로 쪼개어 key값을 비교 후, value들을 tmp_server에 넣어줌
	** push_back으로 server_block에 넣어줌
	*/
	ServerBlock tmp_server;

	while (buf[idx] != "\t}")
	{
		if (!buf[idx].empty())
		{
			vector<string> tmp;
			tmp = deleteSemicolon(split(buf[idx], ' '));

			if (tmp[0] == "\t\tserver_name")
			{
				for (size_t i = 1; i < tmp.size(); i++)
					tmp_server.setServerName(tmp[i]);
			}
			else if (tmp[0] == "\t\tlisten")
			{
				for (size_t i = 1; i < tmp.size(); i++)
					tmp_server.setListen(tmp[i]);
			}
			else if (tmp[0] == "\t\tclient_body_size")
				tmp_server.setClientBodySize(tmp[1]);
			else if (tmp[0] == "\t\troot")
				tmp_server.setRoot(tmp[1]);
			else if (tmp[0] == "\t\tindex")
			{
				for (size_t i = 1; i < tmp.size(); i++)
					tmp_server.setIndex(tmp[i]);
			}
			else if (tmp[0] == "\t\tautoindex")
				tmp_server.setAutoindex(tmp[1]);
			else if (tmp[0] == "\t\terror_page")
			{
				for (size_t i = 1; i < tmp.size(); i++)
					tmp_server.setErrorPage(tmp[i]);
			}
			else if (tmp[0] == "\t\tlocation")
			{
				LocationBlock tmp_location;

				tmp_location.setMatch(tmp[1]);
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
