#ifndef LOCATIONBLOCK_HPP
# define LOCATIONBLOCK_HPP

#include <iostream>
#include <string>
#include <map>
#include <vector>

using namespace std;

class LocationBlock
{
	public:
		LocationBlock();
		~LocationBlock();
		LocationBlock(LocationBlock& copy);
		LocationBlock& operator = (const LocationBlock& hb);

	private:
		map<string, vector<string> > directive;
};

#endif
