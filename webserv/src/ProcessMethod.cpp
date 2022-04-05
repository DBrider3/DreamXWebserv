#include "../includes/ComposeResponse.hpp"

void ComposeResponse::processMethod(ServerBlock server_block, t_request request_msg)
{
	string root_index;
	string	location_uri;
	string request_uri;
	size_t location_uri_pos;
	vector<LocationBlock>::iterator it;

	request_uri = request_msg.uri;
	for (vector<string>::iterator it = server_block.getIndex().begin(); it != server_block.getIndex().end(); it++)
	{
		root_index = server_block.getRoot() + *it;
		//if (access(root_index.c_str(), F_OK))
		setServerIndex(root_index);	
	}
	header.local_uri = getServerIndex()[0];

	/*
	** checking location of request uri
	*/	
	location_uri_pos = request_uri.find_first_of("/");
	location_uri = request_uri.substr(location_uri_pos); //두번째 인자를 넘기지 않으면 자동으로 str 맨 끝까지 복사한다.
	
	for (it = server_block.getLocationBlock().begin(); it != server_block.getLocationBlock().end(); it++)
	{
		//for (vector<string>::iterator it = it.getMatch(); it != server_block.getLocationBlock().end(); it++)
		if (location_uri.compare(it->getMatch().back()) == 0)
		{
			processLocation();
			break ;
		}
	}
	if (it == server_block.getLocationBlock().end())
	{
		request_msg.err_flag = "404";
		request_msg.err_str = "Not found";
	}
	
	// if (location_uri.compare(0, /*키값의 size*/,/* 맵의 키값을 찾아야함*/))
	// {
	// 	server_block.getLocationBlock().getRoot();
	// }
	// else if()//php인지 확인
	// {

	// }
	// else //에러처리
	// {

	// }

	if(request_msgs[idx].method == "GET")
	{
		request_msgs[idx].server_block.getRoot()
		
	}
	else if (request_msgs[idx].method == "POST")
	{
		

	}
	else if (request_msgs[idx].method == "DELETE")
	{

	}
	return (0);
}