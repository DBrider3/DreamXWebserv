#include "LocationBlock.hpp"

LocationBlock::LocationBlock()
{

}
LocationBlock::~LocationBlock()
{

}

//LocationBlock::LocationBlock(LocationBlock& copy)
//{

//}
//LocationBlock& LocationBlock::operator = (const LocationBlock& hb)
//{

//}

//string				LocationBlock::getName()
//{

//}
//string				LocationBlock::getRoot()
//{

//}
//vector<string>		LocationBlock::getIndex()
//{

//}
//string				LocationBlock::getRedirect()
//{

//}
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
	redirect = str;
}
void				LocationBlock::setErrorPage(string str)
{
	error_page.push_back(str);
}
