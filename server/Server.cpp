#include "Server.hpp"

Server::Server()
{
  
}

Server::Server(const Server& src)
{
    *this = src;    
}

Server& Server::operator=(const Server& src)
{
    if (this != &src)
    {
        this->index = src.index;
        this->limit_except = src.limit_except;
        this->server_block = src.server_block;
    }
    return (*this);
}

Server::~Server()
{

}
                        
void Server::composeServer(std::map<std::string, std::vector<std::map<std::string, std::vector<std::string> > > > conf)
{
    conf["index"];
    while (conf.begin != conf.end)
    {
        (conf["index"])
    }
}

void Server::runServer()
{

}

std::vector<std::string>	Server::getIndex()
{

}

std::vector<std::string>	Server::getLimitExcept()
{
    
}

std::vector<ServerBlock>	Server::getServerBlock()
{

}

void	Server::setIndex()
{

}

void	Server::setLimitExcept()
{

}

void    Server::setServerBlock()	
{

}