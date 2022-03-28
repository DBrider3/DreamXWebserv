#ifndef LOCATIONBLOCK_HPP
# define LOCATIONBLOCK_HPP

# include <iostream>
# include <vector>
# include <string>

class LocationBlock
{
	public:     //public static 상수

	private:    //static private 변수
		
	private:    //인스턴스 변수
		std::string					location_name;
		std::string					root;
		std::vector<std::string>	index;
		std::string					redirect;
		std::vector<std::string>	error_page;

	public: 	//method
		LocationBlock();
		LocationBlock(const LocationBlock& src);
		LocationBlock & operator=(const LocationBlock& src);
		~LocationBlock();

		std::string					getName();
		std::string					getRoot();
		std::vector<std::string>	getIndex();
		std::string					getRedirect();
		std::vector<std::string>	getErrorPage();
};

#endif