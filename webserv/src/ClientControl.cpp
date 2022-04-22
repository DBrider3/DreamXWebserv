#include "../includes/ClientControl.hpp"

ClientControl::ClientControl() //의문.1 생성자 호출할때 어떻게할겨? ????
{
	// request.query_str = "";
	//response.cgi = 0;


	
	response.local_uri = "";
	response.date = "";
	response.ct_length = 0;
	response.ct_type = "";
	response.cgi = 0;
	response.state_flag = ""; //현재 작업이 에러 시, 이벤트에 있는 read/write를 소모시키기 위해 플래그를 사용함.
	response.state_str = ""; //빼야함
	response.redirect_uri = "";
	
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

HttpBlock 		ClientControl::getHttpBlock(void)
{
	return (http_block);
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

string	ClientControl::getRoot()
{
	return (root);
}

void 		ClientControl::setHttpBlock(HttpBlock http_block)
{
	this->http_block = http_block;
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

void		ClientControl::setRoot(string root)
{
	this->root = root;
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
	{
		response.cgi = 0;
		return ;
	}
		//throw (PrintError());
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
		else if (ext == ".php")
			response.cgi = 1;
		else if (ext == ".bla")
			response.cgi = 2;
		else
			response.ct_type = "text/plain"; //bla 해야함
	}
}

void		ClientControl::setEnv(void)
{
	env_set["PATH_INFO"] = response.local_uri;
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

void		ClientControl::saveFile(void)
{
	std::fstream file;
	string extension;

	for (int idx = 0; idx < (int)multipart.size(); idx++)
	{
		if (multipart[idx].file_name == "")
			continue;
		extension = multipart[idx].file_name.substr(multipart[idx].file_name.find_last_of(".") + 1);
		if(extension != "jpg" && extension != "png" && extension != "jpeg"
			&& extension != "gif" && extension != "txt")
			continue ;
		//php수정해야함. 만약 올리는 5개중 1개가 해당하는 확장자가 아니면 php에서 어떤파일이 이상한지 띄워줄것
		// 확장자가 txt인지 png인지 검사해야함
		// 파일 체크 우선


		file.open("/Users/daekim/subject/cadet/DreamXWebserv/webserv/save/" + multipart[idx].file_name, std::ios::out);//바꿔
		file << multipart[idx].data;
		file.close();
	//	if (file.fail())
			//실패 시, 에러페이지 서버에러인가?? 서버에러라면 cgi
	}
}


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
	while (request.body[res] != end_code)
	{
		if (request.body[res++] == boundary_key)
		{
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
			cout << "size : " << multipart.size() << endl;
		}
	}
	saveFile();
}

int	ClientControl::checkAutoIndex()
{
	string request_uri;
	
	request_uri = getRequest().uri;
	if (request_uri == "/" && getServerBlock().getAutoindex() == "on") // autoindex
	{
		vector<string> temp;

		temp = getServerBlock().getIndex();
		for (vector<string>::iterator it = temp.begin(); it != temp.end(); it++)
			setServerIndex(*it);
		setLocalUri("/autoindex.html");
		return (1);
	}
	return (0);
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

// string checkIndex(Iterator it, ServerBlock sb)
// {

// }

int ClientControl::checkUri(string result)
{
	string directory;
	string file;
	//string location_uri;
	string tmp;
	string request_uri;
	vector<LocationBlock>::iterator it;
	vector<LocationBlock> temp = getServerBlock().getLocationBlock();
	int	i;
	result = "";
	request_uri = getRequest().uri;

	// cout << "uri" << request_uri << endl;
//1. 점이 있는지 확인
//2. 있으면 첫 / 부터 다음 / 까지
//3. 없으면 첫 /부터 다음 / 까지 혹은 NULL까지
//doyun - directory vector로 해서 /기준으로 끊어 담자 그렇게 해서 0번쨰 인덱스로 location block 찾고 혹시 하위에 추가적 경로가 붙더라도 인덱스로 찾아갈 수 있도록!
	//그래서 빨리 알려주세요 코드
	//어떻게 해야 directory에 첫번째 디렉토리를 담죠??????
	//네????????????

	// a.html
	// aaa
	// aaa/bbb
	// aaa/bbb/a.html

	tmp = request_uri;
	tmp.erase(0, 1);
	if (tmp.find('/') == string::npos)
	{
		if (tmp.find('.') != string::npos)
		{
			file = tmp;
			directory = "/";
		}
		else
		{
			directory = "/" + tmp;
			file = "";
		}
	}
	else
	{
		directory = "/" + tmp.substr(0, tmp.find('/')); 
		if (tmp.find('.') != string::npos)
			file = tmp.substr(tmp.find_last_of('/') + 1);
		else
			file = "";
	}
//너무 좋아요 찬성!!
	// 아래 코드의 대한 질문 : file이 없을때는? 그냥 디렉토리만 있을때는? 아무것도 담기지 않겠지?
	//네 그래서 "" 담고 밑에 if문에서 판별합니다. 감사합니다람쥐구멍
	// file = request_uri.substr(request_uri.find_last_of('/') + 1); 

	// if (file.size() != 0 && file.find('.') == string::npos)
	// {
	// 	directory = "/" + file;
	// 	file = "";
	// }
	// else
	// {
	// 	if (file.size() == 0)
	// 	{
	// 		if (request_uri.size() == 1)
	// 			directory = request_uri;
	// 		else
	// 			directory = request_uri.substr(0, request_uri.size() - 1);
	// 	}
	// 	else
	// 		directory = request_uri.substr(0, request_uri.find_last_of('/'));
	// }

	cout << "direct :  " << directory << endl;
	cout << "file : " << file << endl;
	if (file == "") //디렉토리로 들어온 경우
	{
		for (it = temp.begin(); it != temp.end(); it++)
		{
			//directory == it->getMatch()
			if (directory.compare(it->getMatch()) == 0)
			{
				if (it->getLimitExcept().size() > 0)
				{
					for (i = 0; i != static_cast<int>(it->getLimitExcept().size()) ; i++)
						if (getRequest().method == it->getLimitExcept()[i])
							break;
					if (i == static_cast<int>(it->getLimitExcept().size()))
					{
						cout << "here1 -------------------\n";
						setStateFlag("405");
						setStateStr("Method Not Allowed");
						return (-1);
					}
				}
				else
				{
					for (i = 0; i != static_cast<int>(getHttpBlock().getLimitExcept().size()) ; i++)
						if (getRequest().method == getHttpBlock().getLimitExcept()[i])
							break;
					if (i == static_cast<int>(getHttpBlock().getLimitExcept().size()))
					{
						cout << "here2 -------------------\n";
						setStateFlag("405");
						setStateStr("Method Not Allowed");
						return (-1);
					}
				}
				if (directory == "/") //인덱스 검사 부분 추가
				{					
					if (it->getIndex().size() != 0)
						setLocalUri("/" + it->getIndex()[0]);// it = location block;
					else
						setLocalUri("/" + getServerBlock().getIndex()[0]);
				}
				else
				{
					if (it->getIndex().size() > 0)
						setLocalUri(request_uri + "/" + it->getIndex()[0]); //디렉 붙여줌? 
					else
						setLocalUri(request_uri);
				}
				// if (result.find("\r\n\r\n") == string::npos)
				// {
				// 	setStateFlag("400");
				// 	setStateStr("bad request");
				// 	return (-1);
				// }

				// POST -> content-type -> multipart

				// POST -> content-type -> text/html //일 때 데이터 생성 uri가 File로 왓을때와 directo
				// 	 -> content-type -> no_value
				// 	 	cgi -> ok -> cgi process	
				// 리소스를 읽고 get처럼 쓰인다.

				// data -> submit 할때 그런 데이터들? get에 queryString 처럼 행동



				// PUT -> URI -> /aaa/Makefile Makefile 만들어야 돼

				// Transfer-Encoding: chunked
				
				// location -> /aaa PUT -> Makefile 
				// data -> data -> 0 //이게 일부를 이어붙이는건지 아니면 내용을 싹 다 받은 바디로 바꿔야하는지 궁금함.

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
			if (directory == "/") //location으로 찾지못하더랃server의 root내에 있는 경로도 찾아야될거같아요
			{
				setLocalUri("/" + getServerBlock().getIndex()[0]);
				return (0);
			}
			setStateFlag("404");
			setStateStr("Not Found");
			return (-1);
		}
	}
	else //파일로 들어온 경우
	{
		for (it = temp.begin(); it != temp.end(); it++)
		{
			if ((directory.compare(it->getMatch()) == 0) \
				|| (it->getMatch().find_last_of(".") != string::npos \
				&& file.substr(file.find_last_of(".")) \
				== it->getMatch().substr(it->getMatch().find_last_of("."))))
			{
				if (it->getLimitExcept().size() > 0)
				{
					for (i = 0; i != static_cast<int>(it->getLimitExcept().size()) ; i++)
						if (getRequest().method == it->getLimitExcept()[i])
							break;
					if (i == static_cast<int>(it->getLimitExcept().size()))
					{
						cout << "here3 -------------------\n";
						setStateFlag("405");
						setStateStr("Method Not Allowed");
						return (-1);
					}
				}
				else
				{
					for (i = 0; i != static_cast<int>(getHttpBlock().getLimitExcept().size()) ; i++)
						if (getRequest().method == getHttpBlock().getLimitExcept()[i])
							break;
					if (i == static_cast<int>(getHttpBlock().getLimitExcept().size()))
					{
						cout << "here4 -------------------\n";
						setStateFlag("405");
						setStateStr("Method Not Allowed");
						return (-1);
					}
				}
				if (it->getRedirect().size() != 0)
				{
					// if (result.find("\r\n\r\n") == string::npos)
					// {
					// 	setStateFlag("400");
					// 	setStateStr("bad request");
					// 	return (-1);
					// }
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
		setLocalUri(request_uri);
	}
	if (it->getRoot().size() > 0)
	{
		setRoot(it->getRoot());
		cout << "size Root : " << it->getRoot() << endl;
	}
	else
	{
		setRoot(getServerBlock().getRoot());
		cout << "Serv Root : " << getServerBlock().getRoot() << endl;
	}
	return (0);
}

void ClientControl::deleteFile()
{
	string root;
	struct stat st;
	string path_info = "/Users/daekim/subject/cadet/DreamXWebserv/webserv/state_pages/delete.html"; //바꿔

	root = "/Users/daekim/subject/cadet/DreamXWebserv/webserv/save" + getRequest().uri;//바꿔
	if (!access(root.c_str(), F_OK)) //directory도 삭제가 되는지 확인해야함
	{
		if (!unlink(root.c_str()))
		{
			//findMime();
			stat((path_info).c_str(), &st);
			response.ct_length = st.st_size;
			response.ct_type = "text/html";
			processStatic(path_info);
		}
		else
		{
			setStateFlag("403");
			setStateStr("Forbidden");
		}
	}
	else
	{
				cout << "cc 446" << endl;
		setStateFlag("404");
		setStateStr("Not found");
	}
}

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
		cout << "cc 465" << endl;
		setStateFlag("404");
		setStateStr("Not found");
		return ;
	}
}

void		ClientControl::processCGI(string path_info)
{
	cout << "in processCGI function 🥵" << endl;
	pid_t pid;
	map<string, string> cmd;
	FILE *fOut = tmpfile();
	long fdOut = fileno(fOut);

	cout << "cgi number : "<< response.cgi << endl;
	if (response.cgi == 1)
		cmd["php-cgi"] = path_info;
	else if (response.cgi == 2)
		cmd["cgi_tester"] = path_info;
	cout << "cmd path info : " << cmd["cgi_tester"] << endl;
	//cmd["php-cgi"] = path_info;
	pid = fork();

	if (!pid)
	{
		dup2(fdOut, STDOUT_FILENO);
		if (response.cgi == 1)
			execve(PHPCGI, convToChar(cmd, 0), convToChar(env_set, 1));
		else if (response.cgi == 2)
			execve(CGITESTER, convToChar(cmd, 0), convToChar(env_set, 1));
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

//새로운 시작 made me interesting. like win everything
//오늘따라 하루가 길다

void	ClientControl::processChunk()
{
	int idx;
	int sum;
	int len;
	vector<string> tmp;

	idx = -1;
	sum = 0;
	len = 0;
	cout << "😥😥😥😥😥😥😥😥😥😥😥😥😥in processChunk function😓😓😓😓😓😓😓😓😓😓😓😓😓😓😓" << endl;
	while (request.body[++idx] != "0")
	{
		if (idx % 2 == 0)
		{
			len = static_cast<int>(strtol(request.body[idx].c_str(), NULL, 16));
			sum += len;
		}
		else
		{
//			if (request.body[idx].size() != len)
//				return ;	//받은 hex랑 받은 body랑 길이가 다르다면 에러 표시를 해야됨
			tmp.push_back(request.body[idx]);
		}
//		if (sum > 10000)
//			return ; //누락되서 0이 없거나, 너무 크기가 크면 에러? 해야할까요
	}
	response.ct_length = sum;
	request.body = tmp;
	cout << "나는 야 컨텐츠 len : " << sum << endl;
	// cout << "나는 야 body : " << request.body[0] << endl;
}

//새로운 끝
/*
**int i = 0;
**string buf;

**while (body[i] != "0")
**{
**	int h;
	h << hex << body[i];-----------------------------------------------
**	if (body[++i].size() != h)
		error;
	buf << body[i];
	i++
**}
*/

string ClientControl::check_is_file()
{
	ifstream	fin;
	string		tmp;

	cout << "root : " << getRoot() << " / local : " << response.local_uri << endl;
	tmp = getRoot() + response.local_uri;
	fin >> tmp; //이거 tmp를 fin에 넣어야 하는거 아니야?
	cout << "this is local : " << tmp << endl;
	if (fin.is_open()) // 수정 어케함?? 내용을 싹 밀어버려?
	{
		setStateFlag("204");
		setStateStr("No content"); //200OK도 고려
	}
	else if (getRequest().method == "POST")
	{
		setStateFlag("201");
		setStateStr("Created");
	}
	else
	{
		setStateFlag("201");
		setStateStr("Created");
	}
	return (tmp);
}

void ClientControl::processPP(string file_name)
{
	fstream file;
	vector<string>::iterator it;

	cout << "file name : " << file_name << endl; 
	file.open(file_name, std::ios::out);
	for (it = request.body.begin(); it != request.body.end(); it++)
		file << *it;
	file.close();
}

void	ClientControl::processMethod()
{
	if (getRequest().method == "DELETE")
	{
		deleteFile();
		return ;
	}

	if (checkAutoIndex())
		return ;
		
	response.cgi = 0; // ??
	findMime();
	setEnv();

	string path_info = server_block.getRoot() + response.local_uri; // root

	cout << "This is Root : " << server_block.getRoot() << endl;
	cout << "This is Local URI : " << response.local_uri << endl;
	cout << "This is Path Info : " << path_info << endl;

	if (getRequest().method == "GET")
	{
		struct stat st;
		stat((path_info).c_str(), &st);
		response.ct_length = st.st_size;

		if (response.cgi != 1) //static이거나 , 확장자가 ".bla"여서 cgi의 값이 2일때
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
		if (request.header["Content-Type"].size() == 2)
			processMultipart();
		if (request.header["Transfer-Encoding"][0] == "chunked")
		 	processChunk();
		if (!response.cgi) //파일이나 디렉토리체크 해야하나 ?? 근데 일단 청크처리 먼저 curl기준으로 구현해주고 테스터 돌리면 답이 나오지 않을까여 22
			processPP(check_is_file());
		else
			processCGI(path_info);
		//헷갈리는 점 
		//1. fd로 읽을 수 있는 mesage가 순차적으로 들어와서 한번 읽는다고 모든 chunked를 읽을 수 없다. -> chunked가 안끝났으면 디스커넥트 안되게???
		//2. chunked된 mesage가 들어와서 한번 읽는거로 파싱이 가능하다.
	
		
	}
	else if (getRequest().method == "PUT")
	{
			cout << "----I'm in PUT----" << endl;
			//if (request.header["Content-Type"].size() == 2)
			//	processMultipart();
		if (request.header["Transfer-Encoding"][0] == "chunked")
		 	processChunk();
		if (!response.cgi)
			processPP(check_is_file());
		else
			processCGI(path_info);
	}
}
