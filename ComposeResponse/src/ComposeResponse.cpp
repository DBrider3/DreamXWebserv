#include "ComposeResponse.hpp"

ComposeResponse::ComposeResponse()
{
	header.query = "";
	header.cgi = 0;
}

ComposeResponse::ComposeResponse(t_request req) // 생성자에서 request 객체 받기
{
	header.query = "";
	header.cgi = 0;
	request = req;
}

ComposeResponse::~ComposeResponse()
{

}

ComposeResponse::ComposeResponse(ComposeResponse& copy)
{
	*this = copy;
}

ComposeResponse& ComposeResponse::operator = (const ComposeResponse& m)
{
	if (this == &m)
		return (*this);
	return (*this);
}

void		ComposeResponse::parseQuery(void)
{
	size_t idx;
	string tmp;

	idx = request.local_uri.find("?");
	if (idx != string::npos)
	{
		header.query = request.local_uri.substr(idx + 1);
		tmp = request.local_uri.substr(0, idx);
		request.local_uri = tmp;
	}
}

void		ComposeResponse::findMime(void)
{
	size_t idx;
	string ext = "";
	idx = request.local_uri.find_last_of(".");
	if (idx == string::npos)
		throw (PrintError());
	ext = request.local_uri.substr(idx);
	if (!(ext.empty())) // file
	{
		if (ext == ".html")
			header.ct_type = "text/html";
		else if (ext == ".jpg" || ext == ".jpeg")
			header.ct_type = "image/jpeg";
		else if (ext == ".png")
			header.ct_type = "image/png";
		else if (ext == ".css")
			header.ct_type = "text/css";
		else if (ext == ".js")
			header.ct_type = "text/javascript";
		else if (ext == ".php" || ext == ".py")
			header.cgi = 1;
		else
			header.ct_type = "text/plain";
	}
}

void		ComposeResponse::setEnv(void)
{
	env_set["QUERY_STRING"] = header.query;
	env_set["REQUEST_METHOD"] = request.method; // request.method
	env_set["REDIRECT_STATUS"] = "200"; // 상태코드인데 아직 미정?!
	env_set["SCRIPT_FILENAME"] = request.root + request.local_uri; // 절대 경로, 상대 경로 (우선 순위)
	env_set["SERVER_PROTOCOL"] = request.version; // request.version
	//env_set["PATH_INFO"] = setPathInfo(argv[3]); // 절대 경로, 상대 경로 (우선 순위)
	 env_set["CONTENT_TYPE"] = header.ct_type;
	env_set["GATEWAY_INTERFACE"] = "CGI/1.1";
	env_set["REMOTE_ADDR"] = "127.0.0.1"; // 그대로 넣어 주면 될듯(?)
	if (!header.query.empty())
		env_set["REQUEST_URI"] = request.uri + "?" + header.query; // uri (상대 경로)
	else
		env_set["REQUEST_URI"] = request.uri;
	env_set["SERVER_PORT"] = request.port; // request.port
	env_set["SERVER_SOFTWARE"] = "DPT"; // 간지템
	env_set["SCRIPT_NAME"] = request.local_uri; // uri (상대 경로)
}


/*
** execve() 함수의 인자로 전달되는 map형식을 2차원 배열로 변환 해주는 함수
** (map 형식을 쓰는 이유는 문자열 처리를 쉽게 하기 위해서)
** flag: 0 (command), 1 (environment)
*/
char**		ComposeResponse::convToChar(map<string, string> m, int flag)
{
	char **return_value;
	std::string first_temp;
	std::string second_temp;

	return_value = (char **)malloc(sizeof(char *) * (m.size() + 1));
	int i = 0;
	std::map<std::string, std::string>::iterator it;
	for (it = m.begin(); it != m.end(); it++)
	{
		if (flag)
			first_temp = (*it).first + "=" + (*it).second;
		else
			first_temp = (*it).first;
		char *p = (char *)malloc(first_temp.size() + 1);
		strcpy(p, first_temp.c_str());
		return_value[i] = p;
		i++;
		if (!(flag))
		{
			second_temp = (*it).second;
			char *p2 = (char *)malloc(second_temp.size() + 1);
			strcpy(p2, second_temp.c_str());
			return_value[i] = p2;
			i++;
		}
	}
	return_value[i] = NULL;
	return (return_value);
}

const char* PrintError::what() const throw()
{
	return ("[Error] : ");
}

// char**	ComposeResponse::setCommand(std::string command, std::string path)
// {
//   char **return_value;
//   return_value = (char **)malloc(sizeof(char *) * (3));

//   char *temp;

//   temp = (char *)malloc(sizeof(char) * (command.size() + 1));
//   strcpy(temp, command.c_str());
//   return_value[0] = temp;

//   temp = (char *)malloc(sizeof(char) * (path.size() + 1));
//   strcpy(temp, path.c_str());
//   return_value[1] = temp;
//   return_value[2] = NULL;
//   return (return_value);
// }

void		ComposeResponse::fillResponse(void)
{
	char	res[header.ct_len + 1024]; // 크기를 알 수 없으니 임시로 10000 설정 추후 이 부분에 대해 방안모색
	if (header.ct_len < 42000) // chunk 기준 42mb
	{
		sprintf(res, RESPONSE_FMT, 200, "OK", header.ct_len, header.ct_type.c_str(), body.c_str());
		//write(header->fd, res, strlen(res))); //fd 수정
		cout << res << endl;
	}
	else //chunk
	{
		sprintf(res, CHUNK_FMT, 200, "OK", header.ct_type.c_str());
		//write(header->fd, res, strlen(res))); //fd 수정
		cout << res;
		int i = 0;

		string tmp;
		while (1)
		{
			stringstream ss;
			string hexSize;
			if (body.size() - 100 * i < 100)
			{
				tmp = body.substr(100 * i, body.size() - 100 * i);
				ss<< hex << tmp.size();
				hexSize = ss.str();
			}
			else
			{
				tmp = body.substr(100 * i, 100);
				ss<< hex << 100;
				hexSize = ss.str();
			}
			//write(header->fd, hexSize.c_str(), hexSize.size());
			//write(header->fd, tmp.c_str(), tmp.size());
			cout << hexSize << endl;
			cout << tmp << endl;
			if (body.size() - 100 * i < 100)
				break ;
			i++;
		}
		//write(header->fd, "0\r\n", 3);
		cout << hex << "0\r\n\r\n";
	}
}

void		ComposeResponse::coreResponse(void)
{
	struct stat st;
	string path_info = request.root + request.local_uri;

	//char bbody[100000];
	stat((path_info).c_str(), &st);
	header.ct_len = st.st_size;
	//cout << "header.ct_len : " << header.ct_len << endl;
	cout << "path_info : " << path_info << endl;
	cout << "method : " << request.method << endl;
	//cout << "header.cgi : " << header.cgi << endl;
	if (header.ct_len < 0)
			throw (PrintError());
	else// if (header.ct_len < 10000) //42MB = 44040192 이지만 chunk 처리 쉽게 하려고 100으로 둠
	{
		if (!header.cgi)
		{
			try
			{
				ifstream fin(path_info);
				if (fin.is_open())
				{
					char c;
					while (fin.get(c))
						body += c;
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
			//fillResponse();
		}
		else
		{
			pid_t pid;
			map<string, string> cmd;
			if (request.method == "GET")
			{
				FILE *fOut = tmpfile();
				long fdOut = fileno(fOut);

				cmd["php-cgi"] = path_info;
				pid = fork();
				if (!pid)
				{
    				dup2(fdOut, STDOUT_FILENO);
					execve(PHPCGI, convToChar(cmd, 0), convToChar(env_set, 1));
				}
				else
				{
					waitpid(pid, NULL, 0);
					lseek(fdOut, 0, SEEK_SET);
					char foo[1024] = {0,};
					int res = 0;
					
					while ((res = read(fdOut, foo, 1024)) > 0)
					{
						foo[res] = 0;
						body += static_cast<string> (foo);
						memset(foo, 0, sizeof(foo));
						if (res == -1)
							throw (PrintError());
					}
				}
				string search = "Content-type: ";
				header.ct_type = body.substr(body.find(search) + 14, body.find("\r\n\r\n") - body.find(search) - 14);
				body = body.substr(body.find("\r\n\r\n") + 4, body.size() - body.find("\r\n\r\n") - 4);
				header.ct_len = body.size();
			}
			else if (request.method == "POST")
			{
				FILE *fOut = tmpfile();
				long fdOut = fileno(fOut);

				cmd["php-cgi"] = path_info;
				pid = fork();
				if (!pid)
				{
    				dup2(fdOut, STDOUT_FILENO);
					execve(PHPCGI, convToChar(cmd, 0), convToChar(env_set, 1));
				}
				else
				{
					waitpid(pid, NULL, 0);
					lseek(fdOut, 0, SEEK_SET);
					char foo[1024] = {0,};
					int res = 0;
					
					while ((res = read(fdOut, foo, 1024)) > 0)
					{
						foo[res] = 0;
						body += static_cast<string> (foo);
						memset(foo, 0, sizeof(foo));
						if (res == -1)
							throw (PrintError());
					}
				}
				//cout << body << endl;
				string search = "Content-type: ";
				header.ct_type = body.substr(body.find(search) + 14, body.find("\r\n\r\n") - body.find(search) - 14);
				body = body.substr(body.find("\r\n\r\n") + 4, body.size() - body.find("\r\n\r\n") - 4);
				header.ct_len = body.size();
			}
			fillResponse();
		}
	}
}
