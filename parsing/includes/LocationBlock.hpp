#ifndef LOCATIONBLOCK_HPP
# define LOCATIONBLOCK_HPP

#include "Utils.hpp"

class LocationBlock
{
	private:
		vector<string>		location_match;
		string				root;
		vector<string>		index;
		string				redirect;
		vector<string>		error_page;

	public:
		/*
		** canonicalForm part
		*/
		LocationBlock();
		~LocationBlock();
		LocationBlock(const LocationBlock& copy);
		LocationBlock& operator = (const LocationBlock& lb);

		/*
		** getter part
		*/
		//string				getMatch();
		//string				getRoot();
		//vector<string>		getIndex();
		//string				getRedirect();
		//vector<string>		getErrorPage();
		
		/*
		** setter part
		*/
		void					setMatch(string str);
		void					setRoot(string str);
		void					setIndex(string str);
		void					setRedirect(string str);
		void					setErrorPage(string str);
};

#endif
