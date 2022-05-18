#ifndef LOCATIONBLOCK_HPP
# define LOCATIONBLOCK_HPP

#include "Utils.hpp"

class LocationBlock
{
	private:
		string				location_match;
		string				root;
		string				client_body_size;
		vector<string>		index;
		vector<string>		redirect;
		vector<string>		limit_except;

	public:
		/*
		** canonicalForm part
		*/
		LocationBlock(void);
		~LocationBlock(void);
		LocationBlock(const LocationBlock& copy);
		LocationBlock& operator = (const LocationBlock& lb);

		/*
		** getter part
		*/
		string					getMatch(void);
		string					getRoot(void);
		vector<string>			getIndex(void);
		vector<string>			getRedirect(void);
		vector<string>			getLimitExcept(void);
		string					getClientBodySize(void);

		/*
		** setter part
		*/
		void					setMatch(string str);
		void					setRoot(string str);
		void					setIndex(string str);
		void					setRedirect(string str);
		void					setLimitExcept(string str);
		void					setClientBodySize(string str);
};

#endif
