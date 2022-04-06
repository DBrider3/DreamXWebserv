#include "../includes/LocationBlock.hpp"

LocationBlock::LocationBlock()
{

}
LocationBlock::~LocationBlock()
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
	index = lb.index;
	redirect = lb.redirect;
	// error_page = lb.error_page;
	return (*this);
}

vector<string>		LocationBlock::getMatch()
{
	return (location_match);
}


//string				LocationBlock::getName()
//{

//}
string				LocationBlock::getRoot()
{
	return (root);
}

vector<string>		LocationBlock::getIndex()
{
	return (index);
}

vector<string>		LocationBlock::getRedirect()
{
	return (redirect);
}
//vector<string>		LocationBlock::getErrorPage()
//{

//}

void				LocationBlock::setMatch(string str)
{
	location_match.push_back(str);
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
// void				LocationBlock::setErrorPage(string str)
// {
// 	error_page.push_back(str);
// }
