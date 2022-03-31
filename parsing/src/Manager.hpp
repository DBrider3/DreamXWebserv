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
		int		composeHttpBlock(vector<string> buf);

		/*
		** method part
		*/
		void	fileOpen(string conf);
		void	confParsing(void);
		//void serverCompose();
		//void serverRun();
};

#endif
