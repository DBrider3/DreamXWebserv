#include "../includes/Manager.hpp"

/*
 * header를 초기화하고, client_socket을 저장하는 함수입니다.
 */

void ClientControl::initRequestMsg()
{
	setMethod("");
	setUri("");
	setQuery("");
	setVersion("");
	setMsg("");
	setChunk(0);
	setLength(0);
	return ;
}

void ClientControl::sendChunk(void)
{
	size_t i = 0;
	string tmp;
	string chunk_body;

	size_t body_size;
	char*	r_header = new char[response.ct_length + 1024];

	sprintf(r_header, CHUNK_FMT, response.state_flag.c_str(), response.state_str.c_str(), response.ct_type.c_str());
	if (write(client_fd, r_header, strlen(r_header)) == -1)
	{
		setEOF(DISCONNECTED);
		return ;
	}
	body_size = body.size();

	while (1)
	{
		stringstream ss;
		size_t chunk_size;
		chunk_size = 4096;
		if (body.size() - chunk_size * i != 0)
		{
			if (body.size() - chunk_size * i < chunk_size)
			{
				tmp = body.substr(chunk_size * i, body.size() - chunk_size * i); //포인터 넘겨서 어찌저찌 하면 속도 엄청 올라간데요
				ss << hex << tmp.size();
			}
			else
			{
				tmp = body.substr(chunk_size * i, chunk_size);
				ss << hex << chunk_size;
			}
			chunk_body = ss.str() + "\r\n" + tmp + "\r\n";
			if (write (client_fd, chunk_body.c_str(), chunk_body.size()) == -1)
			{
				setEOF(DISCONNECTED);
				return ;
			}
			usleep(10);
		}
		if (body.size() - chunk_size * i < chunk_size)
			break ;
		i++;
	}

	if (write (client_fd, "0\r\n\r\n", strlen("0\r\n\r\n")) == -1)
	{
		setEOF(DISCONNECTED);
		return ;
	}
	delete[] r_header;
}

void ClientControl::sendNobodyPage(void)
{
	char			r_header[1024];

	sprintf(r_header, RESPONSE_FMT, response.state_flag.c_str(), response.state_str.c_str(), response.ct_length, "text/html", "");
	if(write(client_fd, r_header, strlen(r_header)) == -1)
		setEOF(DISCONNECTED);
}

void ClientControl::sendSuccessPage(void)
{
	char*	r_header = new char[response.ct_length + 1024];
	//chunk

	if (response.state_flag == "204")
	{
		sprintf(r_header, RESPONSE_FMT, response.state_flag.c_str(), response.state_str.c_str(), 0, "text/plain", "");
	}
	else if (response.state_flag == "201")
	{
		struct stat		st;
		string			local_uri;
		char			buf[100];
		int				bodyfd;
		int				n;
		stringstream	ss;

		local_uri = "./state_pages/" + response.state_flag + ".html";
		stat(local_uri.c_str(), &st);
		response.ct_length = st.st_size;
		bodyfd = open(local_uri.c_str(), O_RDONLY);

		n = 0;
		while ((n = read(bodyfd, buf, sizeof(buf) - 1)) > 0)
		{
			buf[n] = '\0';
			ss << buf;
			body += ss.str();
			ss.str("");
			memset(buf, 0, 100);
		}
		close(bodyfd);
		if (n == -1)
		{
			setEOF(DISCONNECTED);
			return ;
		}
		sprintf(r_header, RESPONSE_FMT, response.state_flag.c_str(), response.state_str.c_str(), response.ct_length, "text/html", body.c_str());
	}
	else
		sprintf(r_header, RESPONSE_FMT, response.state_flag.c_str(), response.state_str.c_str(), response.ct_length, response.ct_type.c_str(), body.c_str());
	if (write(client_fd, r_header, strlen(r_header)) == -1)
		setEOF(DISCONNECTED);
	delete[] r_header;
}

void ClientControl::sendRedirectPage()
{
	char			r_header[1024];

	sprintf(r_header, REDIRECT_FMT, getResponse().state_flag.c_str(), getResponse().state_str.c_str(), getResponse().redirect_uri.c_str());
	if(write(getClientFd(), r_header, strlen(r_header)) == -1 )
		setEOF(DISCONNECTED);
}

/*
 * sever_socket을 토대로 client_socket을 구성하는 함수입니다.
 */

int ClientControl::setClientsocket(vector<struct kevent> &change_list, uintptr_t server_socket, ServerBlock server_block)
{
	struct sockaddr_in  client_addr;
	socklen_t			addr_size = sizeof(client_addr);

	/* accept new client */
	int client_socket;
   	if ((client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &addr_size)) == -1)
	{
		sendErrorPage(server_socket, "500", "Internal server error"); //클라이언트 생성실패
		return (-1);
	}
    cout << GREEN << "accept new client: " << client_socket << EOC << endl;
	setRead(0);
	fcntl(client_socket, F_SETFL, O_NONBLOCK);

	/* add event for client socket - add read && write event */
	changeEvents(change_list, client_socket, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
	changeEvents(change_list, client_socket, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);

	setServerBlock(server_block);
	setPort(server_block.getListen()[0]);
	setClientFd(client_socket);
	setServerFd(static_cast<int>(server_socket));
	initRequestMsg();
	return (0);
}

/*
 * 버퍼에 담아둔 request를 파싱하여 구조체에 담아주는 작업을 하는 함수입니다.
 */

vector<string>	ClientControl::parseStartline(string request)
{
	vector<string>	result;
	size_t			previous;
	size_t			current;

	previous = 0;
	current = request.find("\r\n"); // \r\n == crlf

	//find 함수는 해당 위치부터 문자열을 찾지 못할 경우 npos를 반환한다.
	while (current != string::npos)
	{
		// 첫 인자의 위치부터 두번째 인자 길이만큼 substring을 반환
		string substring = request.substr(previous, current - previous);
		result.push_back(substring);
		previous = current + 2; //previous 부터 "\r\n"이 나오는 위치를 찾는다.
		current = request.find("\r\n", previous);
	}
	setMethod(strtok(const_cast<char*>(result[0].c_str()), " "));
	setUri(strtok(NULL, " "));
	setVersion(strtok(NULL, "\n"));
	return (result);
}

int		ClientControl::checkHost(string host)
{
	vector<string> server_name = getServerBlock().getServerName();
	vector<string>::iterator it;
	for (it = server_name.begin(); it != server_name.end(); it++)
	{
		if (*it == host)
			return (0);
	}
	return (1);
}

int		ClientControl::parseHeader(vector<string>& result, vector<string>::iterator& it)
{
	map<string, vector<string> >	header_tmp;

	for (it = result.begin() + 1; it != result.end() && it->size() > 0; it++) //수정함
	{
		stringstream ss(*it);
		stringstream ss_tmp;
		string key;
		vector<string> val;
		string val_tmp;
		getline(ss, key, ':');
		ss.get(); //인덱스 +1 -> 콜론 뒤 공백에서 다음 인덱스로 이동
		for (int i = 0; getline(ss, val_tmp, ' '); i++)
		{
			if (key == "Content-Type" && i == 1)
			{
				ss_tmp << val_tmp;
				getline(ss_tmp, val_tmp, '=');
				getline(ss_tmp, val_tmp, '\0');
			}
			val.push_back(val_tmp);
		}
		header_tmp[key] = val;
		if (key == "Transfer-Encoding" && val.front() == "chunked")
			setChunk(1);
		if (key == "Host")
		{
			if (checkHost(val.front()))
				return (1);
		}
	}
	setHeader(header_tmp);
	return (0);
}

void	ClientControl::parseChunk(string request, vector<string>& result, vector<string>::iterator& it)
{
	size_t						previous;
	size_t						current;

	previous = 0;
	current = request.find("\r\n");
	while (current != string::npos)
	{
		string substring = request.substr(previous, current - previous);
		result.push_back(substring);
		previous = current + 2;
		current = request.find("\r\n", previous);
	}

	for (it = result.begin(); it != result.end() && it->size() > 0; it++)
		;
}

int		ClientControl::parseUri(void)
{
	if (checkUri())
		return (1);
	if (getRequest().uri.size() > 8190)
	{
		setStateFlag("414");
		setStateStr("Request-URI too long");
		return (1);
	}
	if (getRequest().uri.find('?') != string::npos)
	{
		string	temp;
		stringstream ss;
		ss << getRequest().uri;
		getline(ss, temp, '?');
		setUri(temp);
		getline(ss, temp, '\0');
		setQuery(temp);
	}
	return (0);
}

void ClientControl::parseRequest(string request)
{
	vector<string> result; //요청메시지가 한 줄 한 줄 저장되는 변수
	vector<string>::iterator it;

	setRead(1);
	if (!getChunk())
	{
		result = parseStartline(request);
		if (parseHeader(result, it))
		{
			setStateFlag("404");
			setStateStr("Not found");
			return ;
		}
	}
	if (getChunk() == 1 && request.rfind("0\r\n\r\n") == string::npos)
	{
		setRead(0);
 		return ;
	}
	else if (getChunk() == 1)
		parseChunk(request, result, it);
	if (parseUri())
		return ;
	/*
	* Body 파싱
	*/
	if (it == result.end())
		return ;
	while (++it != result.end())
		setBody(*it);
	if (getRequest().header["Content-Type"].size() == 2 && getRequest().body.size() == 0)
	{
		setStateFlag("403");
		setStateStr("Forbidden");
		return ;
	}
}

void	resetBeforeServer(int server_fd, vector<int>& before_server)
{
	for (int i = 0; i < static_cast<int>(before_server.size()); i++)
		if (server_fd == before_server[i])
		{
			before_server.erase(before_server.begin() + i);
			break;
		}
}

/*
 * curr_fd가 전달하는 내용을 버퍼에 담아주는 함수입니다.
 */

# define SIZE 5000000
void ClientControl::readRequest()
{
	/*
	** read data from client
	*/
	char buf[SIZE];
	size_t pos;
	size_t msg_size;
	int n;

	n = 0;
	if ((n = read(getClientFd(), buf, SIZE - 1)) > 0)
	{
		buf[n] = 0;
		msg += static_cast<string> (buf);
	}
	if (n == 0) // 구분할지 말지
	{
		setEOF(DISCONNECTED);
		return ;
	}
	if (n < 0)
	{
		cout << "read error : " << this->client_fd << endl;
		setEOF(DISCONNECTED);
		return ;
	}
	msg_size = msg.size();
	if (msg_size > 4 && msg[msg_size - 4] == '\r')
	{
		pos = msg.rfind("\r\n\r\n");
 		if (pos != string::npos && pos + 4 == msg_size)
			parseRequest(msg);
	}
	cout << msg << endl;
}

/*
 * 서버 실행하는 함수입니다.
 */

void	ClientControl::readResource()
{
	char foo[SIZE];
	int res = 0;
	int fd = getResourceFd();

	if ((res = read(fd, foo, SIZE - 1)) > 0)
	{
		foo[res] = 0;
		body += static_cast<string> (foo);
	}
	if (res == -1)
	{
		cout << "read error 🚀🚀🚀🚀🚀🚀🚀🚀" << endl;
		setStateFlag("403");
		setStateStr("Forbidden");
		close(fd);
		setResourceFd(-1);
	}
	else if (res != SIZE - 1)
	{
		setStateFlag("200");
		setStateStr("OK");
		if (response.cgi && !(response.cgi == 2 && request.method == "GET"))
		{
			string search = "";
			if (response.cgi == 1)
				search = "Content-type: ";
			else if (response.cgi == 2)
				search = "Content-Type: ";
			response.ct_type = body.substr(body.find(search) + 14, body.find("\r\n\r\n") - body.find(search) - 14);
			body = body.substr(body.find("\r\n\r\n") + 4, body.size() - body.find("\r\n\r\n") - 4);
			response.ct_length = body.size();
			fclose(getFout());
		}
		close(fd);
		setResourceFd(-1);
	}
}
