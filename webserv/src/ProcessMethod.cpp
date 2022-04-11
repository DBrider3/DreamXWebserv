#include "../includes/ClientControl.hpp"

void ClientControl::checkAutoIndex()
{
	// vector<string>::iterator it;
	vector<string> temp;
	
	temp = getServerBlock().getIndex();
	for (vector<string>::iterator it = temp.begin(); it != temp.end(); it++)
		setServerIndex(*it);
	setLocalUri("autoindex.html");
}

int ClientControl::findIndex(string uri)
{
	int i;
	vector<string>::iterator it;

	i = 0;
	for (it = getServerBlock().getIndex().begin(); it != getServerBlock().getIndex().end(); it++)
	{
		if (*it == uri.erase(0, 1))
			return (i);
		i++;
	}
	return (-1);
}

int ClientControl::checkUri()
{
	string location_uri;
	string request_uri;
	vector<LocationBlock>::iterator it;
	int		idx;

	request_uri = getRequest().uri;
	if (request_uri == "/" && getServerBlock().getAutoindex() != "on")
	{
		setLocalUri(*(getServerBlock().getIndex().begin())); // 1번
		// getResponse().local_uri = *(getServerBlock().getIndex().begin()); // 1번
	}
	else if (request_uri == "/" && getServerBlock().getAutoindex() == "on") // autoindex
	{
		checkAutoIndex();
	}
	else
	{
		if ((idx = findIndex(request_uri)) > -1) //완전 일치
			setLocalUri(getServerBlock().getIndex()[idx]);
			// getResponse().local_uri = getServerBlock().getIndex()[idx];
		else
		{
			location_uri = request_uri.erase(0, 1); //두번째 인자를 넘기지 않으면 자동으로 str 맨 끝까지 복사한다.
			
			for (it = getServerBlock().getLocationBlock().begin(); it != getServerBlock().getLocationBlock().end(); it++)
			{
				if (location_uri.compare(it->getMatch().back()) == 0)
				{
					setLocalUri(it->getIndex()[0]);
					// getResponse().local_uri = it->getIndex()[0];
					if (*(it->getRedirect().begin()) != "")
					{
						setStateFlag("301");
						setStateStr("Moved permanently");
						setRedirectUri(it->getRedirect().back());
						// getResponse().state_flag = "301";
						// getResponse().state_str = "Moved permanently";
						// getResponse().redirect_uri = it->getRedirect().back();
						return (-1);
					}
					break ;
				}
			}
			if (it == getServerBlock().getLocationBlock().end())
			{
				setStateFlag("404");
				setStateStr("Not found");
				// getResponse().state_flag = "404";
				// getResponse().state_str = "Not found";
				return (-1);
			}
		}
	}
	return (0);
}

void ClientControl::deleteFile()
{
	if (!access(getRequest().uri.c_str(), F_OK)) //directory도 삭제가 되는지 확인해야함
	{
		if (!unlink(getRequest().uri.c_str()))
		{
			setStateFlag("200");
			setStateStr("OK");
			// getResponse().state_flag = "200";
			// getResponse().state_str = "OK";
		}
		else	
		{
			setStateFlag("403");
			setStateStr("Forbidden");
			// getResponse().state_flag = "403";
			// getResponse().state_str = "Forbidden";
		}
	}
	else
	{
		setStateFlag("404");
		setStateStr("Not found");
		// getResponse().state_flag = "404";
		// getResponse().state_str = "Not found";
	}
}

void ClientControl::processMethod()
{
	if (getRequest().method == "GET")
	{
		if (checkUri())
			return ;
		setStateFlag("200");
		setStateStr("OK");
		//getFile();
	}
	else if (getRequest().method == "POST")
	{
		if (checkUri())
			return ;
		setStateFlag("200");
		setStateStr("OK");
		//postFile();
	}
	else if (getRequest().method == "DELETE")
	{
		deleteFile();
	}
}