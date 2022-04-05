#include "ComposeResponse.hpp"

ComposeResponse::ComposeResponse()
{
	header.query = "";
	for (int i = 0; basic_env[i] != NULL; i++)
	{
	  std::pair<std::string, std::string> env_temp;
	  env_temp.first = basic_env[i];
	  env_temp.second = "";
	  env_set.insert(env_temp);
	}
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
	int idx;
	string tmp;
	
	string root = "/var/www/html";
	header.local_uri = root + "/index.test.php?color=123&sport=basketball";
	idx = header.local_uri.find("?");
	header.query = header.local_uri.substr(idx + 1);
	tmp = header.local_uri.substr(0, idx);
	header.local_uri = tmp;
}

void		ComposeResponse::findMime(void)
{

	string root = "/var/www/html";
	header.local_uri = root + "/index.test.html";
	string ext = "";
	ext = header.local_uri.substr(header.local_uri.find_last_of("."));
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
	//cout << "find Mime:" << header.ct_type << endl;
}

void		ComposeResponse::setEnv(void)
{
	env_set["QUERY_STRING"] = header.query;
	env_set["REQUEST_METHOD"] = "GET"; // request.method
	env_set["REDIRECT_STATUS"] = "200"; // 상태코드인데 아직 미정?!
	env_set["SCRIPT_FILENAME"] = std::string(argv[3]); // 절대 경로, 상대 경로
	env_set["SERVER_PROTOCOL"] = "HTTP/1.1"; // request.version
	env_set["PATH_INFO"] = setPathInfo(argv[3]); // 절대 경로, 상대 경로 
	//env_set["CONTENT_TYPE"] = "application/x-www-form-urlencoded"; // 
	 env_set["CONTENT_TYPE"] = header.ct_type;
	env_set["GATEWAY_INTERFACE"] = "CGI/1.1";
	env_set["REMOTE_ADDR"] = "127.0.0.1";
	env_set["REQUEST_URI"] = setPathInfo(argv[3]); // uri
	env_set["SERVER_PORT"] = "8090"; // port
	env_set["SERVER_PROTOCOL"] = "HTTP/1.1"; // version
	env_set["SERVER_SOFTWARE"] = "DPT"; // 간지템

	if (!strcmp(command[0], "php")) {
	  env_set["SCRIPT_NAME"] = "/usr/bin/php";

	}
	else if (!strcmp(command[0], "cgi_tester")) {
	  env_set["SCRIPT_NAME"] = "/Users/doyun/Desktop/DreamXWebserv/tester/cgi_tester";
	}
}


/*
** execve() 함수의 인자로 전달되는 CGI 환경변수 map형식을 2차원 배열로 변환 해주는 함수
** (map 형식을 쓰는 이유는 문자열 처리를 쉽게 하기 위해서) 
*/
//char**		ComposeResponse::ConvToChar(void)
//{
//  char **return_value;
//  std::string temp;

//  return_value = (char **)malloc(sizeof(char *) * (env_set.size() + 1));
//  int i = 0;
//  std::map<std::string, std::string>::iterator it;
//  for (it = env_set.begin(); it != env_set.end(); it++) {
//	temp = (*it).first + "=" + (*it).second;
//	char *p = (char *)malloc(temp.size() + 1);
//	strcpy(p, temp.c_str());
//	return_value[i] = p;
//	i++;
//  }
//  return_value[i] = NULL;
//  return (return_value);
//}