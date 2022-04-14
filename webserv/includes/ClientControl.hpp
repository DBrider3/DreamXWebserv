#ifndef CLIENTCONTROL_HPP
# define CLIENTCONTROL_HPP

# include "Utils.hpp"
# include "ServerBlock.hpp"

//# define PHPCGI "/Users/daekim/subject/cadet/DreamXWebserv/ComposeResponse/php-cgi"
// # define PHPCGI "/Users/songju/Desktop/DreamXWebserv/ComposeResponse/php-cgi"
# define PHPCGI "/Users/dcho/Born2Code/DreamXWebserv/ComposeResponse/php-cgi"

# define NOBODY_FMT "HTTP/1.1 %s %s\n"  //또 뭐넣어야함?
# define RESPONSE_FMT "HTTP/1.1 %s %s\nContent-Length: %d\nContent-Type: %s\n\n%s\n"
# define REDIRECT_FMT "HTTP/1.1 %s %s\nLocation: %s\n"
# define CHUNK_FMT "HTTP/1.1 %s %s\nTransfer-Encoding: chunked\nContent-Type: %s\n\n"

// # define RESPONSE_FMT "HTTP/1.1 %d %s\nContent-Length: %d\nContent-Type: %s\n\n%s"

using namespace std;

//template<int> //vector<int> 형을 return 하려고 하니까 template class 아니라고 에러 떠서 추가함

typedef struct s_request {
	string							method;
	string							uri;
	string							query_str;
	string							version;
	map<string, vector<string> >	header;
	vector<string>					body;
}               t_request;

typedef struct	s_response
{
	string	local_uri;
	string	date;
	int		ct_length;
	string	ct_type;
	int		cgi;
	string	state_flag; //현재 작업이 에러 시, 이벤트에 있는 read/write를 소모시키기 위해 플래그를 사용함.
	string	state_str; //빼야함
	string	redirect_uri;
}				t_response;

typedef struct	s_multipart {
	string	boundary_key;
	string	file_name;
	string	type;
	string	data;
}				t_multipart;

class ClientControl
{
	private:
		t_response		response;
		t_request 		request;
		t_multipart		multipart;

		map<string, string> env_set;
		vector<string>	server_index; //서버 블록 내 index 절대 경로 담아둠
		ServerBlock		server_block;
		string			body;
		string			port;
		int				client_fd;
		int				server_fd;
		int				read_flag;



	public:
		/*
		** canonicalForm part
		*/

		ClientControl();
		//ClientControl(const t_request req);
		~ClientControl();
		ClientControl(const ClientControl& copy);
		ClientControl& operator = (const ClientControl& cr);

		/*
		** getter part
		*/
		string				getServerIndex(void);
		ServerBlock 		getServerBlock(void);
		t_request	getRequest(void);
		t_response	getResponse(void);
		int		getClientFd(void);
		int		getRead();


		/*
		** setter part
		*/
		void 		setServerBlock(ServerBlock server_block);
		void		setPort(string port);
		void		setClientFd(int client_socket);
		void		setServerFd(int server_socket);
		int			setClientsocket(vector<struct kevent> &change_list, uintptr_t server_socket, ServerBlock server_block);
		void 		setServerIndex(string root_index);
		void		setMethod(string str);
		void		setUri(string str);
		void		setQuery(string str);
		void		setVersion(string str);
		void		setHeader(map<string, vector<string> > tmp);
		void		setBody(string str);
		void		setLocalUri(string str);
		void		setStateFlag(string str);
		void		setStateStr(string str);
		void		setRedirectUri(string str);
		void		setRead(int n);

		int			getServerFd();

		void 		initRequestMsg(void);
		void		processMethod(void);
		void		checkAutoIndex(ServerBlock server_block);
		void		readRequest(void);
		void		parseRequest(string msg);
		void		sendRedirectPage(void);
		int			checkMethod(vector<string> method);
		int			checkUri(void);
		void		checkAutoIndex(void);
		int			findIndex(string uri);
		void		deleteFile(void);


		// 추가
		void		findMime(void);
		void		setEnv(void);
		char**		convToChar(map<string, string> m, int flag);
		void		processMultipart(void);
		void		coreResponse(void);
		void		fillResponse(void);
		void		sendSuccessPage(void);
		void 		sendChunk(char** r_header);

		//int		getFile();
		//int		postFile();
};


#endif
