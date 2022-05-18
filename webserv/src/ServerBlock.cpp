#include "../includes/ServerBlock.hpp"

ServerBlock::ServerBlock()
{

}

ServerBlock::~ServerBlock()
{

}

ServerBlock::ServerBlock(const ServerBlock& copy)
{
	*this = copy;
}

ServerBlock& ServerBlock::operator = (const ServerBlock& sb)
{
	if (this == &sb)
		return (*this);
	server_name = sb.server_name;
	listen = sb.listen;
	client_body_size = sb.client_body_size;
	root = sb.root;
	index = sb.index;
	autoindex = sb.autoindex;
	location_block = sb.location_block;
	return (*this);
}

vector<string>			ServerBlock::getListen()
{
	return (listen);
}

string					ServerBlock::getClientBodySize()
{
	return (client_body_size);
}

string					ServerBlock::getRoot()
{
	return (root);
}

vector<string>			ServerBlock::getIndex()
{
	return (index);
}

string					ServerBlock::getAutoindex()
{
	return (autoindex);
}

vector<LocationBlock>	ServerBlock::getLocationBlock()
{
	return (location_block);
}

vector<string>			ServerBlock::getServerName(void)
{
	return (server_name);
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

void	ServerBlock::setErrorPage(string str)
{
	error_page.push_back(str);
}

int					ServerBlock::setLocationBlock(LocationBlock &tmp_location, vector<string> buf, int idx)
{
	/*
	** buf를 split으로 쪼개어 key값을 비교 후, value들을 tmp_location에 넣어줌
	** push_back으로 location_block에 넣어줌
	*/
	while (buf[idx] != "\t\t}")
	{
		vector<string> tmp;
		tmp = deleteSemicolon(split(buf[idx], ' '));

		if (tmp[0] == "\t\t\troot")
			tmp_location.setRoot(tmp[1]);
		else if (tmp[0] == "\t\t\tindex")
		{
			for (size_t i = 1; i < tmp.size(); i++)
				tmp_location.setIndex(tmp[i]);
		}
		else if (tmp[0] == "\t\t\treturn")
		{
			for (size_t i = 1; i < tmp.size(); i++)
				tmp_location.setRedirect(tmp[i]);
		}
		else if (tmp[0] == "\t\t\tlimit_except")
		{
			for (size_t i = 1; i < tmp.size(); i++)
				tmp_location.setLimitExcept(tmp[i]);
		}
		else if (tmp[0] == "\t\t\tclient_body_size")
		{
			tmp_location.setClientBodySize(tmp[1]);
		}
		idx++;
	}
	location_block.push_back(tmp_location);
	return idx;
}
