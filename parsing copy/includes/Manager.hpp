#ifndef MANAGER_HPP
# define MANAGER_HPP

#include "HttpBlock.hpp"
#include "Utils.hpp"
#include "WebServer.hpp"

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
		void		setBuffer(string input);
		int			setHttpBlock(vector<string> buf);

		/*
		** getter part
		*/
		HttpBlock	getHttpBlock(void);

		/*
		** method part
		*/
		void	fileOpen(string conf);
		void	confParsing(void);
		void 	composeServer(HttpBlock conf_data, t_servinfo web_serv);
		void 	runServer(HttpBlock conf_data, t_servinfo web_serv);
};

#endif
