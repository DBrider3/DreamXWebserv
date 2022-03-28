#include <iostream>
#include <vector>

class	LocationBlock;
class	ServerBlock;

class Server
{
	public:     //public static 상수

	private:    //static private 변수

	private:    //인스턴스 변수
		vector<string>			index;
		vector<string>			limit_except;
		vector<ServerBlock>		server_block;
	public: 	//method
		Server();
		Server(const Server& src);
		Server & operator=(const Server& src);
		~Server();

		composeServer();
		runServer();
};

class ServerBlock
{
	public:     //public static 상수

	private:    //static private 변수

	private:    //인스턴스 변수
		vector<string>		server_name;
		string				listen;
		string				client_body_size;
		string				root;
		vector<string>		index;
		string				autoindex;
		vector<LocationBlock>	location_block;

	public: 	//method
		ServerBlock();
		ServerBlock(const ServerBlock& src);
		ServerBlock & operator=(const ServerBlock& src);
		~ServerBlock();

		getServerName();
		getListen();	
		getClientBodySize();
		getRoot();
		getIndex();
		getAutoindex();
		getLocationBlock();
}

class LocationBlock
{
	public:     //public static 상수

	private:    //static private 변수
		
	private:    //인스턴스 변수
		string				location_name;
		string				root;
		vector<string>		index;
		string				redirect;
		vector<string>		error_page;

	public: 	//method
		LocationBlock();
		LocationBlock(const LocationBlock& src);
		LocationBlock & operator=(const LocationBlock& src);
		~LocationBlock();

		getName();
		getRoot();
		getIndex();
		getRedirect();
		getErrorPage();
}