#include "../includes/ClientControl.hpp"
ClientControl::ClientControl() //의문.1 생성자 호출할때 어떻게할겨? ????
{
	// request.query_str = "";
	// response.cgi = 0;
}

// ClientControl::ClientControl(const t_request req) // 생성자에서 request 객체 받기
// {
// 	// request.query_str = "";
// 	// response.cgi = 0;
// 	// request = req;
// }

ClientControl::~ClientControl()
{

}

ClientControl::ClientControl(const ClientControl& copy)
{
	*this = copy;
}

ClientControl& ClientControl::operator = (const ClientControl& m)
{
	if (this == &m)
		return (*this);
	return (*this);
}

ServerBlock 		ClientControl::getServerBlock(void)
{
	return (server_block);
}

t_request	ClientControl::getRequest(void)
{
	return (request);
}

t_response	ClientControl::getResponse(void)
{
	return (response);
}

int		ClientControl::getClientFd(void)
{
	return (client_fd);
}

int		ClientControl::getRead()
{
	return (read_flag);
}

void 		ClientControl::setServerBlock(ServerBlock server_block)
{
	this->server_block = server_block;
}

void		ClientControl::setPort(string port)
{
	this->port = port;
}

void		ClientControl::setClientFd(int client_socket)
{
	this->client_fd = client_socket;
}

void		ClientControl::setServerFd(int server_socket)
{
	this->server_fd = server_socket;
}

void 		ClientControl::setServerIndex(string root_index)
{
	server_index.push_back(root_index);
}

void		ClientControl::setMethod(string str)
{
	request.method = str;
}

void		ClientControl::setUri(string str)
{
	request.uri = str;
}

void		ClientControl::setQuery(string str)
{
	request.query_str = str;
}

void		ClientControl::setVersion(string str)
{
	request.version = str;
}

void		ClientControl::setHeader(map<string, vector<string> > tmp)
{
	request.header = tmp;
}

void		ClientControl::setBody(string str)
{
	request.body.push_back(str);
}

void		ClientControl::setLocalUri(string str)
{
	response.local_uri = str;
}

void		ClientControl::setStateFlag(string str)
{
	response.state_flag = str;
}

void		ClientControl::setStateStr(string str)
{
	response.state_str = str;
}

void		ClientControl::setRedirectUri(string str)
{
	response.redirect_uri = str;
}

void		ClientControl::setRead(int n)
{
	read_flag = n;
}

int			ClientControl::getServerFd()
{
	return(this->server_fd);
}

void		ClientControl::findMime(void)
{
	size_t idx;
	string ext = "";
	idx = response.local_uri.find_last_of(".");
	if (idx == string::npos)
		throw (PrintError());
	ext = response.local_uri.substr(idx);
	if (!(ext.empty())) // file
	{
		if (ext == ".html")
			response.ct_type = "text/html";
		else if (ext == ".jpg" || ext == ".jpeg")
			response.ct_type = "image/jpeg";
		else if (ext == ".png")
			response.ct_type = "image/png";
		else if (ext == ".css")
			response.ct_type = "text/css";
		else if (ext == ".js")
			response.ct_type = "text/javascript";
		else if (ext == ".php" || ext == ".py")
			response.cgi = 1;
		else
			response.ct_type = "text/plain";
	}
}

void		ClientControl::setEnv(void)
{
	env_set["QUERY_STRING"] = request.query_str;
	env_set["REQUEST_METHOD"] = request.method; // request.method
	env_set["REDIRECT_STATUS"] = "200"; // 상태코드인데 아직 미정?!
	env_set["SCRIPT_FILENAME"] = server_block.getRoot() + response.local_uri; // 절대 경로, 상대 경로 (우선 순위)
	env_set["SERVER_PROTOCOL"] = request.version; // request.version
	//env_set["PATH_INFO"] = setPathInfo(argv[3]); // 절대 경로, 상대 경로 (우선 순위)
	 env_set["CONTENT_TYPE"] = response.ct_type;
	env_set["GATEWAY_INTERFACE"] = "CGI/1.1";
	env_set["REMOTE_ADDR"] = "127.0.0.1"; // 그대로 넣어 주면 될듯(?)
	if (!request.query_str.empty())
		env_set["REQUEST_URI"] = request.uri + "?" + request.query_str; // uri (상대 경로)
	else
		env_set["REQUEST_URI"] = request.uri;
	env_set["SERVER_PORT"] = port; // request.port
	env_set["SERVER_SOFTWARE"] = "DreamX"; // 간지템
	env_set["SCRIPT_NAME"] = response.local_uri; // uri (상대 경로)
}


/*
** execve() 함수의 인자로 전달되는 map형식을 2차원 배열로 변환 해주는 함수
** (map 형식을 쓰는 이유는 문자열 처리를 쉽게 하기 위해서)
** flag: 0 (command), 1 (environment)
*/
char**		ClientControl::convToChar(map<string, string> m, int flag) //소송
{
	char **return_value;
	std::string first_temp;
	std::string second_temp;

	if (!flag)
	 	return_value = (char **)calloc((m.size() + 2), sizeof(char *));
	else
		return_value = (char **)calloc((m.size() + 1), sizeof(char *)); //new로 바꾸기
	int i = 0;
	std::map<std::string, std::string>::iterator it;
	for (it = m.begin(); it != m.end(); it++)
	{
		if (flag)
			first_temp = (*it).first + "=" + (*it).second;
		else
			first_temp = (*it).first;
		char *p = (char *)malloc(first_temp.size() + 1); //new로 바꾸기
		strcpy(p, first_temp.c_str());
		return_value[i] = p;
		i++;
		if (!(flag))
		{
			second_temp = (*it).second;
			char *p2 = (char *)malloc(second_temp.size() + 1); //new 로 바꾸기
			strcpy(p2, second_temp.c_str());
			return_value[i] = p2;
			i++;
		}
	}
	return (return_value);
}

// void		ClientControl::fillResponse(void)
// {
// 	char	res[response.ct_length + 1024]; // 크기를 알 수 없으니 임시로 10000 설정 추후 이 부분에 대해 방안모색
// 	if (response.ct_length < 42000) // chunk 기준 42mb
// 	{
// 		sprintf(res, RESPONSE_FMT, 200, "OK", response.ct_length, response.ct_type.c_str(), body.c_str());
// 		//write(header->fd, res, strlen(res))); //fd 수정
// 		cout << res << endl;
// 	}
// 	else //chunk
// 	{
// 		sprintf(res, CHUNK_FMT, 200, "OK", response.ct_type.c_str());
// 		//write(header->fd, res, strlen(res))); //fd 수정
// 		cout << res;
// 		int i = 0;

// 		string tmp;
// 		while (1)
// 		{
// 			stringstream ss;
// 			string hexSize;
// 			if (body.size() - 100 * i < 100)
// 			{
// 				tmp = body.substr(100 * i, body.size() - 100 * i);
// 				ss<< hex << tmp.size();
// 				hexSize = ss.str();
// 			}
// 			else
// 			{
// 				tmp = body.substr(100 * i, 100);
// 				ss<< hex << 100;
// 				hexSize = ss.str();
// 			}
// 			//write(header->fd, hexSize.c_str(), hexSize.size());
// 			//write(header->fd, tmp.c_str(), tmp.size());
// 			cout << hexSize << endl;
// 			cout << tmp << endl;
// 			if (body.size() - 100 * i < 100)
// 				break ;
// 			i++;
// 		}
// 		//write(header->fd, "0\r\n", 3);
// 		cout << hex << "0\r\n\r\n";
// 	}
// }

void		ClientControl::processMultipart(void)
{
	string boundary_key;
	string end_code;

	// 개행은 ""으로 들어온다고 가정
	int res = 0;
	int idx = 0;

	boundary_key = "--" + request.header["Content-Type"][1];
	end_code = boundary_key + "--";
	t_multipart tmp;
	cout << "나는야 엔트맨 ----> " << end_code << endl;
	cout << "Before While🐶" << endl;
	while (request.body[res] != end_code)
	{
		cout << "request.body[res] != end_code : " << (request.body[res] != end_code) << endl;
		if (request.body[res++] == boundary_key)
		{
			cout << "inner while find🐵🐵🐵🐵🐵🐵" << endl;
			if (request.body[res].find("filename=\"") != string::npos)
			{
				multipart.push_back(t_multipart());
				multipart[idx].file_name = request.body[res].substr(request.body[res].find("filename=\"") + 10);
				multipart[idx].file_name.pop_back();
				res++;
				if (request.body[res].find("Content-Type: ") != string::npos)
				{
					multipart[idx].type = request.body[res].substr(request.body[res].find("Content-Type: ") + 14);
					res++;
				}
				while (request.body[++res] != boundary_key && request.body[res] != end_code)
					multipart[idx].data += request.body[res] + "\r\n";
				multipart[idx].data.pop_back();
				multipart[idx].data.pop_back();
				idx++;
			}
		}
		cout << "req : " << request.body[res] << endl;
	}
	cout << "!!!!!!!!!!!!!!!multipart[0].file_name : " << multipart[0].file_name << endl;
	cout << "@@@@@@@@@@@@@@@multipart[0].type : " << multipart[0].type << endl;
	cout << "###############multipart[0].data : \n" << multipart[0].data << endl;
	cout << "!!!!!!!!!!!!!!!multipart[1].file_name : " << multipart[1].file_name << endl;
	cout << "@@@@@@@@@@@@@@@multipart[1].type : " << multipart[1].type << endl;
	cout << "###############multipart[1].data : \n" << multipart[1].data << endl;
}



void ClientControl::checkAutoIndex()
{
	vector<string> temp;

	temp = getServerBlock().getIndex();
	for (vector<string>::iterator it = temp.begin(); it != temp.end(); it++)
		setServerIndex(*it);
	setLocalUri("autoindex.html");
}

int ClientControl::findIndex(string uri)
{
	int i;
	vector<string>::iterator it;
	vector<string> temp = getServerBlock().getIndex();

	i = 0;
	uri.erase(0, 1);
	for (it = temp.begin(); it != temp.end(); it++)
	{
		if (*it == uri)
			return (i);
		i++;
	}
	return (-1);
}

int ClientControl::checkUri()
{
	string location_uri;
	string request_uri;
	vector<LocationBlock>::iterator it;
	vector<LocationBlock> temp = getServerBlock().getLocationBlock();
	int		idx;

	request_uri = getRequest().uri;
	if (request_uri == "/" && getServerBlock().getAutoindex() != "on")
	{
		setLocalUri(*(getServerBlock().getIndex().begin())); // 1번
	}
	else if (request_uri == "/" && getServerBlock().getAutoindex() == "on") // autoindex
	{
		checkAutoIndex();
	}
	else
	{
		if ((idx = findIndex(request_uri)) > -1) //완전 일치
			setLocalUri(getServerBlock().getIndex()[idx]);
		else
		{
			location_uri = request_uri;//.erase(0, 1); //두번째 인자를 넘기지 않으면 자동으로 str 맨 끝까지 복사한다.
			for (it = temp.begin(); it != temp.end(); it++)
			{
				if (location_uri.compare(it->getMatch().back()) == 0)
				{
					setLocalUri(it->getIndex()[0]);
					if (it->getRedirect().size() != 0)
					{
						if (*(it->getRedirect().begin()) != "")
						{
							setStateFlag("301");
							setStateStr("Moved permanently");
							setRedirectUri(it->getRedirect().back());
							return (-1);
						}
					}
					break ;
				}
			}
			if (it == temp.end())
			{
				setStateFlag("404");
				setStateStr("Not found");
				return (-1);
			}
		}
	}
	return (0);
}

void ClientControl::deleteFile()
{
	if (!access(getRequest().uri.c_str(), F_OK)) //directory도 삭제가 되는지 확인해야함
	{
		if (!unlink(getRequest().uri.c_str()))
		{
			setStateFlag("200");
			setStateStr("OK");
		}
		else
		{
			setStateFlag("403");
			setStateStr("Forbidden");
		}
	}
	else
	{
		setStateFlag("404");
		setStateStr("Not found");
	}
}

// void ClientControl::processMethod()
// {
// 	if (getRequest().method == "GET")
// 	{
// 		if (checkUri())
// 			return ;
// 		// setStateFlag("200");
// 		// setStateStr("OK");
// 		getFile();
// 	}
// 	else if (getRequest().method == "POST")
// 	{
// 		if (checkUri())
// 			return ;
// 		// setStateFlag("200");
// 		// setStateStr("OK");
// 		postFile();
// 	}
// 	else if (getRequest().method == "DELETE")
// 	{
// 		deleteFile();
// 	}
// }
void		ClientControl::processStatic(string path_info)
{
	ifstream fin(path_info);

	if (fin.is_open())
	{
		char c;
		while (fin.get(c))
			body += c;
		fin.close();
		setStateFlag("200");
		setStateStr("OK");
	}
	else
	{
		setStateFlag("404");
		setStateStr("Not found");
		return ;
	}
}

void		ClientControl::processCGI(string path_info)
{
	pid_t pid;
	map<string, string> cmd;
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
		lseek(fdOut, 0, SEEK_SET); //lseek는 파일 디스크립터의 읽기/쓰기 포인터 위치를 변경하는 데 사용되는 시스템 호출입니다
		char foo[1024];
		int res = 0;

		memset(foo, 0, sizeof(foo));
		while ((res = read(fdOut, foo, 1023)) > 0)
		{
			foo[res] = 0;
			body += static_cast<string> (foo);
			memset(foo, 0, sizeof(foo));
		}
		if (res == -1)
		{
			setStateFlag("403");
			setStateStr("Forbidden");
			return ;
		}
	}
	setStateFlag("200");
	setStateStr("OK");
	string search = "Content-type: ";
	response.ct_type = body.substr(body.find(search) + 14, body.find("\r\n\r\n") - body.find(search) - 14);
	body = body.substr(body.find("\r\n\r\n") + 4, body.size() - body.find("\r\n\r\n") - 4);
	response.ct_length = body.size();
}

void ClientControl::processMethod()
{

	if (checkUri())
		return ;

	findMime();
	setEnv();

	string path_info = server_block.getRoot() + response.local_uri;

	if (getRequest().method == "GET")
	{
		struct stat st;
		//char bbody[100000];
		stat((path_info).c_str(), &st);
		response.ct_length = st.st_size;

		if (!response.cgi)
		{
			processStatic(path_info);
		}
		else
		{
			processCGI(path_info);
		}
	} // get, post cgi function
	else if (getRequest().method == "POST")
	{
		cout << "----I'm in POST----" << endl;
		processMultipart();
		processCGI(path_info);
	}
	else if (getRequest().method == "DELETE")
	{
		deleteFile();
	}
}
