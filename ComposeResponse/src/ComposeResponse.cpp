#include "ComposeResponse.hpp"

ComposeResponse::ComposeResponse()
{
	header.query = "";
}

ComposeResponse::ComposeResponse(t_request req) // 생성자에서 request 객체 받기
{
	header.query = "";
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
	env_set["REQUEST_URI"] = request.uri + "?" + header.query; // uri (상대 경로)
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

void		ComposeResponse::coreResponse(void)
{
	struct stat st;
	string path_info = request.root + request.local_uri;

	stat((path_info).c_str(), &st);
	header.ct_len = st.st_size;
	if (header.ct_len < 0)
			throw (PrintError());
	else if (header.ct_len < 10000) //42MB = 44040192 이지만 chunk 처리 쉽게 하려고 100으로 둠
	{
		if (!header.cgi)
		{
			try
			{
				ifstream fin(path_info);
				if (fin.is_open())
				{
					//string ;
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
		}
		else
		{
			int pipe_fd[2];
			pid_t pid;
			//char **command1 = setCommand("php", "/Users/sonkang/Desktop/DreamXWebserv/test.php");
			map<string, string> cmd;
			char foo[100];

			cmd["php-cgi"] = path_info;
			pipe(pipe_fd);
			pid = fork();
			//memset((char *)foo, '\0', 1024);
			if (!pid)
			{
				dup2(pipe_fd[1], STDOUT_FILENO);
				close(pipe_fd[0]);
				close(pipe_fd[1]);

				execve(PHPCGI, convToChar(cmd, 0), convToChar(env_set, 1));
				// else if (!strcmp(command2[0], "cgi_tester"))
				// 	execve("./tester/cgi_tester", command2, NULL);
			}
			else
			{
				// while (read(pipe_fd[0], foo, sizeof(foo)) > 0)
        		// 	body += static_cast<string> (foo);
				//ssize_t i;
				int res;
				while (1)
				{
					res = read(pipe_fd[0], foo, sizeof(foo));
					cout << "!!!!!!!!!!res : " << res << endl;
					body += static_cast<string> (foo);
					//cout << "str "<< str << endl;
					// if (res < 100)
					// 	break;
				}
				close(pipe_fd[1]);
				close(pipe_fd[0]);
				// fill_response(r_header, 200, strlen(foo), "text/html", foo);
				// write(header->fd, r_header, strlen(r_header));
				wait(NULL);
			}
		}
		cout << "body : " << body << endl;
	}
}
