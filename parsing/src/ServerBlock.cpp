#include "ServerBlock.hpp"

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
int					ServerBlock::setLocationBlock(LocationBlock tmp_location, vector<string> buf, int idx)
{
	while (buf[idx] != "\t\t}")
	{
		vector<string> tmp;
		tmp = split(buf[idx], ' ');

		if (tmp[0] == "\t\t\troot")
		{
			tmp_location.setRoot(tmp[1]);
			cout << tmp[1] << endl;
		}
		else if (tmp[0] == "\t\t\tindex")
		{
			for (size_t i = 1; i < tmp.size(); i++)
			{
				tmp_location.setIndex(tmp[i]);
				cout << tmp[i] << endl;
			}
		}
		else if (tmp[0] == "\t\t\tredirect")
		{
			tmp_location.setRedirect(tmp[1]);
			cout << tmp[1] << endl;
		}
		else if (tmp[0] == "\t\t\terror_page")
		{
			for (size_t i = 1; i < tmp.size(); i++)
			{
				tmp_location.setErrorPage(tmp[i]);
				cout << tmp[i] << endl;
			}
		}
		idx++;
	}
	location_block.push_back(tmp_location);
	return idx;
}
