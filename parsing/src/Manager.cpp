#include "Manager.hpp"

Manager::Manager()
{

}
Manager::~Manager()
{

}
Manager::Manager(Manager& copy)
{
	*this = copy;
}
Manager& Manager::operator = (const Manager& m)
{
	if (this == &m)
		return (*this);
	http_block = m.http_block;
	buffer = m.buffer;
	return (*this);
}

// string Manager::get_buffer(void)
// {
// 	return (buffer);
// }

void	Manager::setBuffer(string input)
{
	buffer.push_back(input);
}

int		Manager::setHttpBlock(vector<string> buf)
{
	/*
	** buf를 split으로 쪼개어 key값을 비교 후, value들을 넣어줌
	*/
	int res = 1;
	while (!(buf[res].empty()))
	{
		vector<string> tmp;
		tmp = deleteSemicolon(split(buf[res], ' '));

		if (tmp[0] == "\tindex")
		{
			for (size_t i = 1; i < tmp.size(); i++)
			{
				http_block.setIndex(tmp[i]);
				cout << tmp[i] << endl;
			}
		}
		else if (tmp[0] == "\tlimit_except")
		{
			for (size_t i = 1; i < tmp.size(); i++)
			{
				http_block.setLimitExcept(tmp[i]);
				cout << tmp[i] << endl;
			}
		}
		res++;
	}
	return ++res;
}

void	Manager::fileOpen(string conf)
{
	/*
	** 인자 혹은 default파일을 열고 buffer에 저장
	*/
	try
	{
		ifstream fin(conf);
		if (fin.is_open())
		{
			string temp;

			while (getline(fin, temp))
				setBuffer(temp);
			fin.close();
		}
		else
			throw(PrintError());
	}
	catch(const exception& e)
	{
		cerr << e.what() << "Cannot open the File!😵‍" << endl;
		exit(1);
	}
}

void	Manager::confParsing(void)
{
	/*
	** idx를 활용하여 buffer 순회
	*/
	try
	{
		int idx = 0;
		if (buffer[idx] == "http {")
			idx = setHttpBlock(buffer);
		else
			throw(PrintError());
		while (buffer[idx] == "\tserver {")
			idx = http_block.setServerBlock(buffer, idx);
		if (http_block.getServerBlock().size() == 0)
			throw(PrintError());
	}
	catch(const exception& e)
	{
		cerr << e.what() << "The file configuration is incorrect!😵‍" << endl;
		exit(1);
	}
}
