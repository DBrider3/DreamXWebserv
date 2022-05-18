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
	setMulti(0);
	setLength(0);
	return ;
}

void ClientControl::sendChunk(void)
{
	size_t i = 0;
	string tmp;
	string chunk_body;

	size_t body_size; //?????
	char*	r_header = new char[response.ct_length + 1024];

	sprintf(r_header, CHUNK_FMT, response.state_flag.c_str(), response.state_str.c_str(), response.ct_type.c_str());
	if (write(client_fd, r_header, strlen(r_header)) == -1)
	{
		setEOF(DISCONNECTED);
		return ;
	}
	body_size = body.size(); // ????? 

	while (1)
	{
		stringstream ss;
		size_t chunk_size;

		chunk_size = 4096;
		if (body.size() - chunk_size * i != 0) // i가 증가함에따라 chunk_size를 곱하여 size에 맞게 잘라서 응답을 보내줌
		{
			if (body.size() - chunk_size * i < chunk_size) // 정해진 chunk size로 응답을 작성하는 부분
			{
				tmp = body.substr(chunk_size * i, body.size() - chunk_size * i);
				ss << hex << tmp.size();
			}
			else // 받은 body가 chunk_size 보다 작아 마지막 남은 body라고 판단될 때
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
		if (body.size() - chunk_size * i < chunk_size) //마지막 남은 body를 받았다면 나가자
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
   	if ((client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &addr_size)) == -1) //서버소켓 기반으로 클라이언트 소켓 생성
	{
		sendErrorPage(server_socket, "500", "Internal server error"); //클라이언트 생성실패
		return (-1);
	}
    cout << GREEN << "accept new client: " << client_socket << EOC << endl;
	
	setRead(0); // read flag 설정
	fcntl(client_socket, F_SETFL, O_NONBLOCK); //NON_BLOCK 설정

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
	size_t							len;

	len = 0;
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
			if (key == "Content-Type" && i == 1) //multipart 확인
			{
				ss_tmp << val_tmp;
				getline(ss_tmp, val_tmp, '=');
				getline(ss_tmp, val_tmp, '\0');
				setMulti(1);
			}
			val.push_back(val_tmp);
		}
		header_tmp[key] = val;
		if (key == "Transfer-Encoding" && val.front() == "chunked") //chunked 확인
			setChunk(1);
		if (key == "Host") //요청 도메인이 conf파일에 저장된 Server_name으로 들어왔는지 확인
		{
			if (checkHost(val.front()))
				return (1);
		}
	}
	if (len != 0 && result.back().size() == len && header_tmp["Content-Length"].front() != "0")  // curl -X POST 형태로 들어오는 상태 (요청메시지 마지막에 \r\n\r\n이 없는 경우)
		if (result.back()[len - 1] != '\n' && result.back()[len - 2] != '\r') // content_lenth로 비교하는 이유는 chunked는 content_lenth값이 들어오지 않기 때문에 chunked와 구별지어 적용시킬 수 있음 
			result.back() += "\r\n\r\n";
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

int		ClientControl::parseUri(void) //
{
	if (checkUri())
		return (1);
	if (getRequest().uri.size() > 8190)
	{
		setStateFlag("414");
		setStateStr("Request-URI too long");
		return (1);
	}
	if (getRequest().uri.find('?') != string::npos) //query문 파싱
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
		result = parseStartline(request); // 요청받은 메시지의 method | URI |버전 파싱
		if (parseHeader(result, it)) // 요청받은 메시지의 header에 담겨있는 정보 파싱(chunk, multipart 등등 )
		{
			setStateFlag("404");
			setStateStr("Not found");
			return ;
		}
	}
	if (getChunk() == 1 && request.compare((request.size() - 5), 5, "0\r\n\r\n")) // chunk의 body가 전부 받아진 경우
	{
		setRead(0);
 		return ;
	}
	else if (getMulti() == 1 && request.compare((request.size() - 4), 4, "--\r\n")) // multipart의 body가 전부 받아진경우
	{
		setRead(0);
		return ;
	}
	else if (getChunk() == 1 || getMulti() == 1) // chunk나 Multipart인 경우에 body까지 다 읽은 경우 parseChunk 진행
		parseChunk(request, result, it); // body 이전의 메시지 처리가 끝난 상태이므로 body의 시작 이전까지 offset의 위치 이동;
	if (parseUri())
		return ;
	/*
	*  Body 파싱
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
 * 요청받은 메시지를 읽어 데이터백에 저장하는 함수입니다.
 */
 
# define SIZE 5000000
void ClientControl::readRequest() 
{
	/*
	** read data from client
	*/
	char buf[SIZE];
	int n;

	n = 0;
	if ((n = read(getClientFd(), buf, SIZE - 1)) > 0)
	{
		buf[n] = 0;
		msg += static_cast<string> (buf);
	}
	if (n == 0) // 요청이 끝났을때
	{
		setEOF(DISCONNECTED);
		return ;
	}
	if (n < 0) // 요청을 받았지만 에러가 발생 했을때
	{
		cout << "read error : " << this->client_fd << endl;
		setEOF(DISCONNECTED);
		return ;
	}
	if (!getChunk() && !getMulti()) // 일반적인 body 요청인경우 밑의 if문으로 body구분, 이경우엔 parse에 구분되어 들어가야함
	{
		/* 
		*	1. request header가 완성되지 않은 경우 빠져나감
		*	2. chunked, multipart가 아닌 일반적인 body 요청일 경우
		*/
		if (msg.find("\r\n\r\n") != string::npos) // request header가 다 읽은 경우
			parseRequest(msg); 
	}
	else 
		parseRequest(msg);

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
