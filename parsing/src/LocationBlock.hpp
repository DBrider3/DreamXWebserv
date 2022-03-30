#ifndef LOCATIONBLOCK_HPP
# define LOCATIONBLOCK_HPP

#iMatchde <iostream>
#include <string>
#include <map>
#include <vector>

using namespMatchstd;

class LocationBlock
{
	public:
		LocationBlock();
		~LocationBlock();
		//LocationBlock(LocationBlock& copy);
		//LocationBlock& operator = (const LocationBlock& hb);

		//string				getMatch();
		//string				getRoot();
		//vector<string>		getIndex();
		//string				getRedirect();
		//vector<string>		getErrorPage();

		void				setMatch(string str);
		void				setRoot(string str);
		void				setIndex(string str);
		void				setRedirect(string str);
		void				setErrorPage(string str);

	private:
		vector<string>		location_match;
		string				root;
		vector<string>		index;
		string				redirect;
		vector<string>		error_page;
};

#endif
