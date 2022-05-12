#include "../includes/ClientControl.hpp"

ClientControl::ClientControl()
{
	env_set.clear();
	server_index.clear(); //서버 블록 내 index 절대 경로 담아둠

	port = "";
	root = "";
	directory = "";
	file = "";
	read_flag = 0;
	write_flag = 0;
	resource_fd = -1;
    client_body_size = -1;
	msg = "";
	chunk_flag = 0;
	body = "";
	client_fd = 0;
	server_fd = 0;
	fout = NULL;
	eof = 0;

	response.local_uri = "";
	response.date = "";
	response.ct_length = 0;
	response.ct_type = "";
	response.cgi = 0;
	response.state_flag = ""; //현재 작업이 에러 시, 이벤트에 있는 read/write를 소모시키기 위해 플래그를 사용함.
	response.state_str = ""; //빼야함
	response.redirect_uri = "";

	request.method = "";
	request.uri = "";
	request.query_str = "";
	request.version = "";
	request.header.clear();
	request.body.clear();
	request.ct_length = 0;

	multipart.clear();
}

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
	env_set = m.env_set;
	server_index = m.server_index; //서버 블록 내 index 절대 경로 담아둠
	server_block = m.server_block;
	http_block = m.http_block;
	port = m.port;
	root = m.root; //방금 추가 put & post
	directory = m.directory;
	file = m.file;
	read_flag = m.read_flag;
	write_flag = m.write_flag;
	resource_fd = m.resource_fd;
    client_body_size = m.client_body_size;
	msg = m.msg;
	chunk_flag = m.chunk_flag;
	body = m.body;
	client_fd = m.client_fd;
	server_fd = m.server_fd;
	fout = m.fout;
	eof = m.eof;

	/*
	** response
	*/
	response.local_uri = m.response.local_uri;
	response.date = m.response.date;
	response.ct_length = m.response.ct_length;
	response.ct_type = m.response.ct_type;
	response.cgi = m.response.cgi;
	response.state_flag = m.response.state_flag; //현재 작업이 에러 시, 이벤트에 있는 read/write를 소모시키기 위해 플래그를 사용함.
	response.state_str = m.response.state_str; //빼야함
	response.redirect_uri = m.response.redirect_uri;

	/*
	** request
	*/
	request.method = m.request.method;
	request.uri = m.request.uri;
	request.query_str = m.request.query_str;
	request.version = m.request.version;
	request.header = m.request.header;
	request.body = m.request.body;
	request.ct_length = m.request.ct_length;

	/*
	** multipart
	*/
	multipart = m.multipart;
	return (*this);
}

ServerBlock& 		ClientControl::getServerBlock(void)
{
	return (server_block);
}

HttpBlock& 		ClientControl::getHttpBlock(void)
{
	return (http_block);
}

t_request&	ClientControl::getRequest(void)
{
	return (request);
}

t_response&	ClientControl::getResponse(void)
{
	return (response);
}

int&		ClientControl::getClientFd(void)
{
	return (client_fd);
}

int&		ClientControl::getRead()
{
	return (read_flag);
}

string&	ClientControl::getRoot()
{
	return (root);
}

int&		ClientControl::getClientBodySize()
{
	return (client_body_size);
}

string&		ClientControl::getMsg()
{
	return (msg);
}

int&			ClientControl::getChunk()
{
	return (chunk_flag);
}

int&			ClientControl::getServerFd()
{
	return(this->server_fd);
}

int&			ClientControl::getResourceFd()
{
	return (resource_fd);
}

int&			ClientControl::getWrite()
{
	return (write_flag);
}

FILE*		ClientControl::getFout()
{
	return (fout);
}

int&			ClientControl::getEOF()
{
	return (eof);
}

void		ClientControl::setChunk(int chunk_flag)
{
	this->chunk_flag = chunk_flag;
}

void 		ClientControl::setMsg(string msg)
{
	this->msg = msg;
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
	cout << "ClientControl::setLocalUri" << str << endl;
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

void		ClientControl::setClientBodySize(string body_size)
{
	this->client_body_size = convStoi(body_size);
}

void		ClientControl::setLength(int n)
{
	request.ct_length = n;
}

void		ClientControl::setResourceFd(int fd)
{
	resource_fd = fd;
}

void		ClientControl::setWrite(int n)
{
	write_flag = n;
}

void		ClientControl::setFout(FILE* fd)
{
	fout = fd;
}

void		ClientControl::setEOF(int eof)
{
	this->eof = eof;
}

void		ClientControl::findMime(void)
{
	size_t idx;
	string ext = "";
	cout << "ClientControl::findMime : " << response.local_uri << endl;
	idx = response.local_uri.find_last_of(".");
	if (idx == string::npos)
	{
		response.cgi = 0;
		return ;
	}
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
	env_set["PATH_INFO"] = server_block.getRoot() + response.local_uri;//response.local_uri;//
	env_set["QUERY_STRING"] = request.query_str;
	env_set["REQUEST_METHOD"] = request.method; // request.method
	env_set["REDIRECT_STATUS"] = "200"; // 상태코드인데 아직 미정?!
	env_set["SCRIPT_FILENAME"] = server_block.getRoot() + response.local_uri; // 절대 경로, 상대 경로 (우선 순위)
	env_set["SERVER_PROTOCOL"] = request.version; // request.version
	env_set["CONTENT_TYPE"] = response.ct_type;
	env_set["GATEWAY_INTERFACE"] = "CGI/1.1";
	env_set["REMOTE_ADDR"] = "127.0.0.1"; // 그대로 넣어 주면 될듯(?)
	if (!request.query_str.empty())
		env_set["REQUEST_URI"] = getRoot() + request.uri + "?" + request.query_str; // uri (상대 경로)
	else
		env_set["REQUEST_URI"] = getRoot() + request.uri;
	env_set["SERVER_PORT"] = port; // request.port
	env_set["SERVER_SOFTWARE"] = "DreamX"; // 간지템
	env_set["SCRIPT_NAME"] = response.local_uri; // uri (상대 경로)
	if (getRequest().header["X-Secret-Header-For-Test"].size())
		env_set["HTTP_X_SECRET_HEADER_FOR_TEST"] = getRequest().header["X-Secret-Header-For-Test"][0];
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
	std::map<std::string, std::string>::iterator it;

	if (!flag)
	 	return_value = (char **)calloc((m.size() + 2), sizeof(char *));
	else
		return_value = (char **)calloc((m.size() + 1), sizeof(char *)); //new로 바꾸기
	int i = 0;
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
		// php수정해야함. 만약 올리는 5개중 1개가 해당하는 확장자가 아니면 php에서 어떤파일이 이상한지 띄워줄것
		// 확장자가 txt인지 png인지 검사해야함
		// 파일 체크 우선


		file.open("Users/songju/Desktop/DreamXWebserv/webserv/save/" + multipart[idx].file_name, std::ios::out);//바꿔
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
		}
	}
	saveFile();
}

int	ClientControl::checkAutoIndex() //status 넣어주기
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

void	initUriTarget(string& directory, string& file, string request_uri)
{
	request_uri.erase(0, 1); // 맨 앞 '/' 잘라 주는 놈
	if (request_uri.find('/') == string::npos) // 뒤에 디렉토리 형식이 안들어오는 경우    /directory
	{
		if (request_uri.find('.') != string::npos) // 확장자 타입이 명시된 경우   /file.txt
		{
			file = request_uri;
			directory = "/";
		}
		else
		{
			directory = "/" + request_uri; // 확장자 타입이 명시 되지 않아 디렉토리 뒤에 합쳐짐
			file = "";
		}
	}
	else
	{
		directory = "/" + request_uri.substr(0, request_uri.find('/')); // /directory/
		if (request_uri.find('.') != string::npos)
			file = request_uri.substr(request_uri.find_last_of('/') + 1); // 확장자 타입 명시된 경우
		else
			file = ""; // 확장자 타입이 명시되지 않은 경우
	}
}

int		ClientControl::processLimitExcept(vector<LocationBlock>::iterator& it)
{
	size_t	i;

	if (it->getLimitExcept().size() > 0)
	{
		for (i = 0; i != it->getLimitExcept().size() ; i++)
			if (getRequest().method == it->getLimitExcept()[i])
				break;
		if (i == it->getLimitExcept().size())
		{
			setStateFlag("405");
			setStateStr("Method Not Allowed");
			return (-1);
		}
	}
	else
	{
		for (i = 0; i != getHttpBlock().getLimitExcept().size() ; i++)
			if (getRequest().method == getHttpBlock().getLimitExcept()[i])
				break;
		if (i == getHttpBlock().getLimitExcept().size())
		{
			setStateFlag("405");
			setStateStr("Method Not Allowed");
			return (-1);
		}
	}
	return (1);
}

int		ClientControl::classifyDirUri(string& directory, string& request_uri, vector<LocationBlock>::iterator& it, vector<LocationBlock>& location_block)
{
	for (it = location_block.begin(); it != location_block.end(); it++)
	{
		if (directory.compare(it->getMatch()) == 0)
		{
			if (!(processLimitExcept(it)))
				return (-1);
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
		} // for end
	}
	if (it == location_block.end())
	{
		if (directory == "/")
		{
			setLocalUri("/" + getServerBlock().getIndex()[0]);
			return (0);
		}
		setStateFlag("404");
		setStateStr("Not Found");
		return (-1);
	}
	return (1);
}

int		ClientControl::classifyFileUri(string& directory, string& file, string& request_uri, vector<LocationBlock>::iterator& it, vector<LocationBlock>& location_block)
{
	for (it = location_block.begin(); it != location_block.end(); it++)
	{
		if ((directory.compare(it->getMatch()) == 0) \
			|| (it->getMatch().find_last_of(".") != string::npos \
			&& file.substr(file.find_last_of(".")) \
			== it->getMatch().substr(it->getMatch().find_last_of("."))))
		{
			if (!(processLimitExcept(it)))
				return (-1);
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
	setLocalUri(request_uri);
	return (1);
}

int ClientControl::checkUri(void)
{
	string	directory;
	string	file;
	string	request_uri;
	int		result;
	vector<LocationBlock> location_block = getServerBlock().getLocationBlock();
	vector<LocationBlock>::iterator it;


	request_uri = getRequest().uri;
	initUriTarget(directory, file, request_uri);

	if (file == "") //디렉토리로 들어온 경우
	{
		result = classifyDirUri(directory, request_uri, it, location_block);
		if (result <= 0)
			return (result);
	}
	else //파일로 들어온 경우
	{
		result = classifyFileUri(directory, file, request_uri, it, location_block);
		if (result < 0)
			return (result);
	}
	if (it != getServerBlock().getLocationBlock().end() && !(it->getClientBodySize().empty()))
		setClientBodySize(it->getClientBodySize());
	if (it->getRoot().size() > 0)
		setRoot(it->getRoot());
	else
		setRoot(getServerBlock().getRoot());
	return (0);
}

void ClientControl::deleteFile()
{
	string root;
	struct stat st;
	string path_info = "Users/songju/Desktop/DreamXWebserv/webserv/state_pages/delete.html"; //바꿔

	root = "Users/songju/Desktop/DreamXWebserv/webserv/save" + getRequest().uri;//바꿔
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
		setStateFlag("404");
		setStateStr("Not found");
	}
}

void		ClientControl::processStatic(string path_info)
{
	ifstream	fin(path_info);

	if (fin.is_open())
	{
		setResourceFd(open(path_info.c_str(), O_RDONLY));
		fin.close();
	}
	else
	{
		fin.close();
		setStateFlag("404");
		setStateStr("Not found");
		return ;
	}
}

void		ClientControl::processCGI(string path_info)
{
	pid_t pid;
	map<string, string> cmd;

	FILE *fIn = tmpfile();
	long fdIn = fileno(fIn);
	FILE *fout = tmpfile();
	long fdOut = fileno(fout);
	vector<string>::iterator it;

	int request_size = 0;
	if (response.cgi == 1)
		cmd["php-cgi"] = path_info;
	else if (response.cgi == 2)
	{
		for (it = request.body.begin(); it != request.body.end(); it++)
		{
			if(write(fdIn, it->c_str(), it->size()) == -1)
			{
				cout << "write error 🚀🚀🚀🚀🚀🚀🚀🚀" << endl;
				setStateFlag("500"); //???
				setStateStr("Internal server error");
				fclose(fIn);
				close(fdIn);
				fclose(fout);
				close(fdOut);
				return ;
			}
			request_size += (int)it->size();
		}
		lseek(fdIn, 0, SEEK_SET);
	}
	pid = fork();
	if (!pid)
	{
		dup2(fdIn, STDIN_FILENO);
		dup2(fdOut, STDOUT_FILENO);
		if (response.cgi == 1)
			execve(PHPCGI, convToChar(cmd, 0), convToChar(env_set, 1));
		else if (response.cgi == 2)
			execve(CGITESTER, NULL, convToChar(env_set, 1));
	}
	else
	{
		waitpid(pid, NULL, 0);
		lseek(fdOut, 0, SEEK_SET); //lseek는 파일 디스크립터의 읽기/쓰기 포인터 위치를 변경하는 데 사용되는 시스템 호출입니다
		setResourceFd(fdOut);
		setFout(fout);
	}
	fclose(fIn);
	close(fdIn);

}

void	ClientControl::processChunk()
{
	int idx;
	int sum;
	int len;
	vector<string> tmp;

	idx = -1;
	sum = 0;
	len = 0;
	while (request.body[++idx] != "0")
	{
		if (idx % 2 == 0)
		{
			len = static_cast<int>(strtol(request.body[idx].c_str(), NULL, 16));
			sum += len;
		}
		else
			tmp.push_back(request.body[idx]);
	}
	request.ct_length = sum;
	request.body = tmp;
}

string ClientControl::check_is_file()
{
	ifstream	fin;
	string		tmp;

	tmp = getRoot() + response.local_uri;
	fin >> tmp;
	if (fin.is_open() && getRequest().method != "POST")
	{
		setStateFlag("204");
		setStateStr("No content");
	}
	else
	{
		setStateFlag("201");
		setStateStr("Created");
	}
	fin.close();
	return (tmp);
}

void ClientControl::processPP(string file_name)
{
	fstream file;
	vector<string>::iterator it;

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

	response.cgi = 0;
	findMime();
	setEnv();


	string path_info = server_block.getRoot() + response.local_uri;
	cout << "root : " << root << " local_uri : " << response.local_uri << endl;
	if (getRequest().method == "GET")
	{
		struct stat st;
		stat((path_info).c_str(), &st);
		response.ct_length = st.st_size;
		cout << "path_info : " << path_info << endl;
		if (response.cgi != 1) //static이거나 , 확장자가 ".bla"여서 cgi의 값이 2일때
			processStatic(path_info);
		else
			processCGI(path_info);
	}
	else if (getRequest().method == "POST")
	{
		if (request.header["Content-Type"].size() == 2)
			processMultipart();
		if (request.header["Transfer-Encoding"].size() != 0 && request.header["Transfer-Encoding"][0] == "chunked")
		 	processChunk();

		if (getClientBodySize() != -1 && request.ct_length > getClientBodySize())
		{
			setStateFlag("413");
			setStateStr("Payload Too Large");
			return ;
		}

		if (!response.cgi)
		{
			string aa;

			// processPP(check_is_file());

			aa = check_is_file();
			//processPP(aa);
		}
		else
			processCGI(path_info);
	}
	else if (getRequest().method == "PUT")
	{
		if (request.header["Transfer-Encoding"][0] == "chunked")
		 	processChunk();
		if (!response.cgi)
			processPP(check_is_file());
		else
			processCGI(path_info);
	}
}

void	ClientControl::resetClient(int client_socket, int server_socket, ServerBlock server_block)
{
	env_set.clear();
	server_index.clear();

	port = "";
	root = "";
	directory = "";
	file = "";
	read_flag = 0;
	write_flag = 0;
	resource_fd = -1;
    client_body_size = -1;
	msg = "";
	chunk_flag = 0;
	body = "";
	client_fd = 0;
	server_fd = 0;
	fout = NULL;
	eof = 0;

	response.local_uri = "";
	response.date = "";
	response.ct_length = 0;
	response.ct_type = "";
	response.cgi = 0;
	response.state_flag = ""; //현재 작업이 에러 시, 이벤트에 있는 read/write를 소모시키기 위해 플래그를 사용함.
	response.state_str = ""; //빼야함
	response.redirect_uri = "";

	request.method = "";
	request.uri = "";
	request.query_str = "";
	request.version = "";
	request.header.clear();
	request.body.clear();
	request.ct_length = 0;

	multipart.clear();

	setServerBlock(server_block);
	setPort(server_block.getListen()[0]);
	setClientFd(client_socket);
	setServerFd(static_cast<int>(server_socket));
	initRequestMsg();
}
