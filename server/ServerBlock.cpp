#include "ServerBlock.hpp"

ServerBlock::ServerBlock()
{
	
}

ServerBlock::ServerBlock(const ServerBlock& src)
{
	*this = src;    
}

ServerBlock & ServerBlock::operator=(const ServerBlock& src)
{
	if (this != &src)
	{
		this->server_name = src.server_name;
		this->listen = src.listen;
		this->client_body_size = src.client_body_size;
		this->root = src.root;
		this->index = src.index;
		this->autoindex = src.autoindex;
		this->location_block = src.location_block;
	}
	return (*this);
}

ServerBlock::~ServerBlock()
{

}

std::vector<std::string>	ServerBlock::getServerName()
{

}

std::string		ServerBlock::getListen()
{

}

std::string		ServerBlock::getClientBodySize()
{

}

std::string		ServerBlock::getRoot()
{

}

std::vector<std::string>	ServerBlock::getIndex()
{

}

std::string		ServerBlock::getAutoindex()
{

}

std::vector<LocationBlock>	ServerBlock::getLocationBlock()
{

}
