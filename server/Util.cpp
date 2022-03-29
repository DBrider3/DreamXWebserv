#include "Util.hpp"

void exit_with_perror(const string &msg)
{
	cerr << msg << endl;
	exit(1);
}

int     convStoi(string str)
{
	char	*tmp;
	int		ret;

	tmp = static_cast<char *>(str);
	ret = 0;
	for(int i = 0; i < strlen(tmp); i++)
		ret = ret * 10 + tmp[i];
	return (ret);
}