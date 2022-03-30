#ifndef MANAGER_HPP
# define MANAGER_HPP

#include "HttpBlock.hpp"
#include "Utils.hpp"

class Manager
{
	private:
		vector<string> buffer;
		HttpBlock http_block;
		
	public:
		/*
		** canonicalForm part
		*/
		Manager();
		~Manager();
		Manager(Manager& copy);
		Manager& operator = (const Manager& m);

		/*
		** setter part
		*/
		void	setBuffer(string input);
		int		setHttpBlock(vector<string> buf);

		/*
		** method part
		*/
		void	confParsing(string conf);
		//void serverCompose();
		//void serverRun();
};

#endif
