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
		vector<LocationBlock>	location_block;

	public:
		/*
		** canonicalForm part
		*/
		ServerBlock();
		~ServerBlock();
		ServerBlock(const ServerBlock& copy);
		ServerBlock& operator = (const ServerBlock& sb);

		/*
		** getter part
		*/
		//vector<string>			getServerName();
		//vector<string>			getListen();
		//string					getClientBodySize();
		//string					getRoot();
		//vector<string>			getIndex();
		//string					getAutoindex();
		//vector<LocationBlock>		getLocationBlock();

		/*
		** setter part
		*/
		void						setServerName(string str);
		void						setListen(string str);
		void						setClientBodySize(string str);
		void						setRoot(string str);
		void						setIndex(string str);
		void						setAutoindex(string str);
		void						setLocationBlock(LocationBlock lb);
		int							composeLocationBlock(LocationBlock tmp_location, vector<string> buf, int idx);

};

#endif
