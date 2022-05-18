#ifndef SERVERBLOCK_HPP
# define SERVERBLOCK_HPP

#include "Utils.hpp"
#include "LocationBlock.hpp"

class ServerBlock
{
	private:
		vector<string>			server_name;
		vector<string>			listen;
		string					client_body_size;
		string					root;
		vector<string>			index;
		string					autoindex;
		vector<string>			error_page;
		vector<LocationBlock>	location_block;

	public:
		/*
		** canonicalForm part
		*/
		ServerBlock(void);
		~ServerBlock(void);
		ServerBlock(const ServerBlock& copy);
		ServerBlock& operator = (const ServerBlock& sb);

		/*
		** getter part
		*/
		vector<string>				getListen(void);
		string						getClientBodySize(void);
		string						getRoot(void);
		vector<string>				getIndex(void);
		string						getAutoindex(void);
		vector<LocationBlock>		getLocationBlock(void);
		vector<string>				getServerName(void);

		/*
		** setter part
		*/
		void						setServerName(string str);
		void						setListen(string str);
		void						setClientBodySize(string str);
		void						setRoot(string str);
		void						setIndex(string str);
		void						setAutoindex(string str);
		void						setErrorPage(string str);
		int							setLocationBlock(LocationBlock &tmp_location, vector<string> buf, int idx);
};

#endif
