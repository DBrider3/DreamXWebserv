#ifndef SERVERBLOCK_HPP
# define SERVERBLOCK_HPP

# include "LocationBlock.hpp"

class ServerBlock
{
	public:     //public static 상수

	private:    //static private 변수

	private:    //인스턴스 변수
		std::vector<std::string>	server_name;
		std::vector<std::string>	listen; //벡터로 만들어주세여
		std::string					client_body_size;
		std::string					root;
		std::vector<std::string>	index;
		std::string					autoindex;
		std::vector<LocationBlock>	location_block;

	public: 	//method
		ServerBlock();
		ServerBlock(const ServerBlock& src);
		ServerBlock & operator=(const ServerBlock& src);
		~ServerBlock();

		std::vector<std::string>	getServerName();
		std::string					getListen();	
		std::string					getClientBodySize();
		std::string					getRoot();
		std::vector<std::string>	getIndex();
		std::string					getAutoindex();
		std::vector<LocationBlock>	getLocationBlock();
};

#endif