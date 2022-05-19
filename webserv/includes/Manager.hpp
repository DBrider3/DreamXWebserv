#ifndef MANAGER_HPP
# define MANAGER_HPP

# include "Utils.hpp"
# include "ClientControl.hpp"

typedef struct s_servinfo
{
	vector<int>			ports;
	vector<int>			server_socket;
}			t_servinfo;

class Manager
{
	private:
		vector<string>	buffer;
		HttpBlock		http_block;
		t_servinfo		web_serv;

	public:
		/*
		** canonicalForm part
		*/
		Manager();
		~Manager();
		Manager(const Manager& copy);
		Manager& operator = (const Manager& m);

		/*
		** setter part
		*/
		void		setBuffer(string input);
		int			setHttpBlock(vector<string> buf);

		/*
		** getter part
		*/
		HttpBlock	getHttpBlock(void);
		t_servinfo	getWebServ(void);

		/*
		** method part
		*/
		void	fileOpen(string conf);
		void	confParsing(void);
		void 	composeServer(void);
		void	composePort(void);
		void	composeSocket(void);
		void 	runServer(void);

		/*
		** process part
		*/
		void	processError(vector<ClientControl>& client_control, uintptr_t curr_id, vector<int> server_sockets);
		int		processRead(vector<ClientControl>& client_control, uintptr_t curr_id, vector<int> server_sockets, vector<struct kevent>& change_list);
		void	processWrite(vector<ClientControl>& client_control, uintptr_t curr_id, int& count);

		void 	check_msg(t_request rmsg);
};

	void	disconnectSocket(int socket_fd);
	int 	checkSocket(int curr_fd, vector<int> server_socket);
	void 	changeEvents(vector<struct kevent>& change_list, uintptr_t ident, int16_t filter,
						uint16_t flags, uint32_t fflags, intptr_t data, void *udata);
	int 	sendErrorPage(int socket_fd, string state_flag, string state_str);
	vector<ClientControl>::iterator findClient(vector<ClientControl> &client_control, int curr_fd);

#endif