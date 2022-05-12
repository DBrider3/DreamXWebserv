#ifndef CLIENTCONTROL_HPP
# define CLIENTCONTROL_HPP

# include "Utils.hpp"
# include "HttpBlock.hpp"
# include "ServerBlock.hpp"

//# define PHPCGI "/Users/daekim/subject/cadet/DreamXWebserv/webserv/tester/php-cgi"//바꿔
//# define CGITESTER "/Users/daekim/subject/cadet/DreamXWebserv/webserv/tester/cgi_tester"
//   # define PHPCGI "/Users/junghan/Desktop/DreamXWebserv/webserv/tester/php-cgi"
//   # define CGITESTER "/Users/junghan/Desktop/DreamXWebserv/webserv/tester/cgi_tester"
# define PHPCGI "/Users/songju/Desktop/DreamXWebserv/webserv/tester/php-cgi"
# define CGITESTER "/Users/songju/Desktop/DreamXWebserv/webserv/tester/cgi_tester"
//   # define PHPCGI "/Users/dcho/DreamXWebserv/webserv/tester/php-cgi"
//   # define CGITESTER "/Users/dcho/DreamXWebserv/webserv/tester/cgi_tester"

# define NOBODY_FMT "HTTP/1.1 %s %s\r\nContent-Length: %d\r\nContent-Type: %s"
# define RESPONSE_FMT "HTTP/1.1 %s %s\r\nContent-Length: %d\r\nContent-Type: %s\r\n\r\n%s"
# define REDIRECT_FMT "HTTP/1.1 %s %s\r\nLocation: %s"
# define CHUNK_FMT "HTTP/1.1 %s %s\r\nTransfer-Encoding: chunked\r\nContent-Type: %s\r\n\r\n"
# define ERROR_FMT "HTTP/1.1 %s %s"

using namespace std;

typedef enum s_IOFlag
{
	REQUEST_RECEIVING,
	REQUEST_COMPLETE,
	RESPONSE_COMPLETE,
	DISCONNECTED
}			t_IOFlag;

typedef struct s_request {
	string							method;
	string							uri;
	string							query_str;
	string							version;
	map<string, vector<string> >	header;
	vector<string>					body;
	int								ct_length;
}               t_request;

typedef struct	s_response
{
	string							local_uri;
	string							date;
	int								ct_length;
	string							ct_type;
	int								cgi;
	string							state_flag; //현재 작업이 에러 시, 이벤트에 있는 read/write를 소모시키기 위해 플래그를 사용함.
	string							state_str; //빼야함
	string							redirect_uri;
}				t_response;

typedef struct	s_multipart {
	string							file_name;
	string							type;
	string							data;
}				t_multipart;

class ClientControl
{
	private:
		t_response					response;
		t_request 					request;
		vector<t_multipart>			multipart;

		map<string, string>			env_set;
		vector<string>				server_index; //서버 블록 내 index 절대 경로 담아둠
		ServerBlock					server_block;
		HttpBlock					http_block;
		string						body;
		string						port;
		string						root;
		string						directory;
		string						file;
		int							client_fd;
		int							server_fd;
		int							read_flag;
		int							client_body_size;
		int							resource_fd;
		int							write_flag;
		FILE*						fout; //cgi 처리 시, file의 읽기가 종료되는 시점에서 fd를 닫아주기 위해 저장

		string						msg;
		int							chunk_flag;
		int							eof;



	public:
		/*
		** canonicalForm part
		*/

		ClientControl();
		~ClientControl();
		ClientControl(const ClientControl& copy);
		ClientControl& operator = (const ClientControl& cr);

		/*
		** getter part
		*/
		string&			getServerIndex(void);
		ServerBlock& 	getServerBlock(void);
		HttpBlock& 		getHttpBlock(void);
		t_request&		getRequest(void);
		t_response&		getResponse(void);
		int&			getClientFd(void);
		int&			getServerFd();
		int&			getRead();
		string&			getRoot();
		int&			getClientBodySize();

		string&			getMsg();
		int&			getChunk();
		int&			getResourceFd();
		int&			getWrite();
		FILE*			getFout();
		int&			getEOF();
		/*
		** setter part
		*/
		void 			setMsg(string msg);
		void			setChunk(int chunk_flag);

		void			setHttpBlock(HttpBlock http_block);
		void 			setServerBlock(ServerBlock server_block);
		void			setPort(string port);
		void			setClientFd(int client_socket);
		void			setServerFd(int server_socket);
		int				setClientsocket(vector<struct kevent> &change_list, uintptr_t server_socket, ServerBlock server_block);
		void 			setServerIndex(string root_index);
		void			setMethod(string str);
		void			setUri(string str);
		void			setQuery(string str);
		void			setVersion(string str);
		void			setHeader(map<string, vector<string> > tmp);
		void			setBody(string str);
		void			setLocalUri(string str);
		void			setStateFlag(string str);
		void			setStateStr(string str);
		void			setRedirectUri(string str);
		void			setRead(int n);
		void			setRoot(string root);
		void			setClientBodySize(string body_size);
		void			setLength(int n);
		void			setResourceFd(int fd);
		void			setWrite(int n);
		void			setFout(FILE* fd);
		void			setEOF(int eof);

		void 			initRequestMsg(void);
		void			processMethod(void);
		int				checkAutoIndex(void);
		void			readRequest(void);
		vector<string>	parseStartline(string request);
		int				parseHeader(vector<string>& result, vector<string>::iterator& it);
		void			parseChunk(string request, vector<string>& result, vector<string>::iterator& it);
		int				parseUri(void);
		void			parseRequest(string msg);
		void			sendRedirectPage(void);
		int				checkMethod(vector<string> method);
		int				checkUri(void);
		int				findIndex(string uri);
		void			deleteFile(void);


		// 추가
		void			findMime(void);
		void			setEnv(void);
		char**			convToChar(map<string, string> m, int flag);
		void			processMultipart(void);
		void			coreResponse(void);
		void			fillResponse(void);
		void			sendSuccessPage(void);
		void 			sendChunk();

		void			processStatic(string path_info);
		void			processCGI(string path_info);
		void			saveFile(void);
		void			processPP(string file_name);
		string			check_is_file(void);
		void			processChunk(void);
		void			sendNobodyPage(void);
		int				checkHost(string host);
		//int		getFile();
		//int		postFile();

		void			readResource(void);
		void			resetClient(int client_socket, int server_socket, ServerBlock server_block);

		// 추가된 부분
		int				classifyDirUri(string& directory, string& request_uri, vector<LocationBlock>::iterator& it, vector<LocationBlock>& location_block);
		int				classifyFileUri(string& file, string& request_uri, vector<LocationBlock>::iterator& it, vector<LocationBlock>& location_block);
		int				processLimitExcept(vector<LocationBlock>::iterator& it);
};


#endif
