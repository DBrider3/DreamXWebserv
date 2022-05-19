#include "../includes/LocationBlock.hpp"

LocationBlock::LocationBlock(void)
{

}

LocationBlock::~LocationBlock(void)
{

}

LocationBlock::LocationBlock(const LocationBlock& copy)
{
	*this = copy;
}

LocationBlock& LocationBlock::operator = (const LocationBlock& lb)
{
	if (this == &lb)
		return (*this);
	location_match = lb.location_match;
	root = lb.root;
	client_body_size = lb.client_body_size;
	index = lb.index;
	redirect = lb.redirect;
	limit_except = lb.limit_except;
	return (*this);
}

string				LocationBlock::getMatch(void)
{
	return (location_match);
}

string				LocationBlock::getRoot(void)
{
	return (root);
}

vector<string>		LocationBlock::getIndex(void)
{
	return (index);
}

vector<string>		LocationBlock::getRedirect(void)
{
	return (redirect);
}

vector<string>		LocationBlock::getLimitExcept(void)
{
	return (limit_except);
}

string				LocationBlock::getClientBodySize(void)
{
	return (client_body_size);
}

void				LocationBlock::setMatch(string str)
{
	location_match = str;
}

void				LocationBlock::setRoot(string str)
{
	root = str;
}

void				LocationBlock::setIndex(string str)
{
	index.push_back(str);
}

void				LocationBlock::setRedirect(string str)
{
	redirect.push_back(str);
}

void				LocationBlock::setLimitExcept(string str)
{
	limit_except.push_back(str);
}

void				LocationBlock::setClientBodySize(string str)
{
	client_body_size = str;
}