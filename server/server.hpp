#ifndef SERVER_HPP
# define SERVER_HPP

# include <iostream>
# include <vector>
# include <string>
# include <map>
# include "ServerBlock.hpp"

class Server
{
	public:     //public static 상수

	private:    //static private 변수

	private:    //인스턴스 변수
		std::vector<std::string>		index;
		std::vector<std::string>		limit_except;
		std::vector<ServerBlock>		server_block;
		
	public: 	//method
		Server();
		Server(const Server& src);
		//Server(map<string, vector<map<string, vector<string>>>> conf);
		Server & operator=(const Server& src);
		~Server();

		void						composeServer(std::map<std::string, std::vector<std::map<std::string, std::vector<std::string> > > > conf);
		void						runServer();
		std::vector<std::string>	getIndex();
		std::vector<std::string>	getLimitExcept();
		std::vector<ServerBlock>	getServerBlock();	
		void						setIndex();
		void						setLimitExcept();
		void						setServerBlock();	
};

#endif
