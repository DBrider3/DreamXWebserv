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
	char	res[10000]; // 크기를 알 수 없으니 임시로 10000 설정 추후 이 부분에 대해 방안모색
	sprintf(res, RESPONSE_FMT, 200, "OK", header.ct_len, header.ct_type.c_str(), body.c_str());
	cout << res << endl;
	//write(header->fd, res, strlen(res))); //fd 수정
}

void		ComposeResponse::coreResponse(void)
{
	struct stat st;
	string path_info = request.root + request.local_uri;

	//char bbody[100000];
	stat((path_info).c_str(), &st);
	header.ct_len = st.st_size;
	cout << "header.ct_len : " << header.ct_len << endl;
	cout << "path_info : " << path_info << endl;
	cout << "header.cgi : " << header.cgi << endl;
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
			fillResponse();
		}
		else
		{
			int pipe_fd[2];
			pid_t pid;
			//char **command1 = setCommand("php", "/Users/sonkang/Desktop/DreamXWebserv/test.php");
			map<string, string> cmd;
			cout << "header.ct_len in parent: " << header.ct_len << endl;
			/*
			** php-cgi response header + body 계산해서 동적할당후 초기화
			*/
			// char *foo = (char*)malloc(sizeof(char) * header.ct_len + 54);
			// memset(foo, 1, header.ct_len + 53);
			char foo[100];
			
			
			printf("foo : %lu\n", strlen(foo));
			cmd["php-cgi"] = path_info;
			pipe(pipe_fd);
			pid = fork();
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
				/*
				** 정확한 size 계산을 통해 while을 안쓰고 한방에 read
				** 이유는 while만 돌면 read에서 탈출이 안됨
				** 예상하는바 execve, fork, pipe를 이용하면 무조건 fd를 이용하게 되는데
				** 방식을 바꾸거나 read에서 탈출하는 해결방안을 모색할 필요가 있음
				*/
				int res;
				res = read(pipe_fd[0], foo, strlen(foo));
					body += static_cast<string> (foo);
				cout << "????\n";
				close(pipe_fd[1]);
				close(pipe_fd[0]);
				cout << body << endl;
				// fill_response(r_header, 200, strlen(foo), "text/html", foo);
				// write(header->fd, r_header, strlen(r_header));
				waitpid(pid, NULL, 0);
			}
			//header.ct_type = body.substr(body.find("Content-type: ") + 14, body.find("\n\n") - body.find("Content-type: ") - 14);
			//cout << header.ct_type << "\n";
		}
	}
}
