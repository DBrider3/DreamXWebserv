#include "../includes/ComposeResponse.hpp"



void ComposeResponse::checkAutoIndex(ServerBlock server_block)
{
	string root_index;
	vector<string>::iterator it;

	if (server_block.getAutoindex() == "on")
	{
		for (it = server_block.getIndex().begin(); it != server_block.getIndex().end(); it++)
		{
			root_index = server_block.getRoot() + *it;
			setServerIndex(root_index);
		}
	}
}

int findIndex(ServerBlock server_block, string uri)
{
	int i;
	vector<string>::iterator it;

	i = 0;
	for (it = server_block.getIndex().begin(); it != server_block.getIndex().end(); it++)
	{
		if (*it == uri.erase(0, 1))
			return (i);
		i++;
	}
	return (-1);
}

int ComposeResponse::checkUri(ServerBlock server_block, t_request &request_msg)
{
	string location_uri;
	string request_uri;
	vector<LocationBlock>::iterator it;
	int		idx;

	request_uri = request_msg.uri;
	if (request_uri == "/")
		header.local_uri = *(server_block.getIndex().begin()); // 1번
	else
	{
		if ((idx = findIndex(server_block, request_uri)) > -1) //완전 일치
			header.local_uri = server_block.getIndex()[idx];
		else
		{
			location_uri = request_uri.erase(0, 1); //두번째 인자를 넘기지 않으면 자동으로 str 맨 끝까지 복사한다.
			
			for (it = server_block.getLocationBlock().begin(); it != server_block.getLocationBlock().end(); it++)
			{
				//for (vector<string>::iterator it = it.getMatch(); it != server_block.getLocationBlock().end(); it++)
				if (location_uri.compare(it->getMatch().back()) == 0)
				{
					header.local_uri = it->getIndex()[0];
					if (*(it->getRedirect().begin()) != "")
					{
						request_msg.err_flag = "301";
						request_msg.err_str = "Moved permanently";
						request_msg.redirect = it->getRedirect().back();
						return (-1);
					}
					break ;
				}
			}
			if (it == server_block.getLocationBlock().end())
			{
				request_msg.err_flag = "404";
				request_msg.err_str = "Not found";
				return (-1);
			}
		}
	}
	return (0);
}

void ComposeResponse::processMethod(ServerBlock server_block, t_request &request_msg)
{
	if (request_msg.method == "GET")
	{
		if (checkUri(server_block, request_msg))
			return ;
		checkAutoIndex(server_block);
	}
	else if (request_msg.method == "POST")
	{
		if (checkUri(server_block, request_msg))
			return ;
		checkAutoIndex(server_block);
	}
	else if (request_msg.method == "DELETE")
	{

	}
}