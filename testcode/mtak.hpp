#ifndef CGI_HPP
#define CGI_HPP
#include <unistd.h>
#include <cstdlib>
#include <sys/wait.h>
#include <cstring>
#include <string>
#include <fstream>
#include <map>
//#include "Request.hpp"
#define SUCCESS 0
#define FAIL -1
class Cgi
{
  /* enumerator */
  enum ENVIRON_LIST
  {
    AUTH_TYPE,
    CONTENT_LENGTH,
    CONTENT_TYPE,
    GATEWAY_INTERFACE,
    PATH_INFO,
    PATH_TRANSLATED,
    QUERY_STRING,
    REMOTE_ADDR,
    REMOTE_IDENT,
    REMOTE_USER,
    REQUEST_METHOD,
    REQUEST_URI,
    SCRIPT_FILENAME,
    SERVER_NAME,
    SERVER_PORT,
    SERVER_PROTOCOL,
    SERVER_SOFTWARE,
    REDIRECT_STATUS,
    NON_OF_ALL
  };
  public:
    /* constructor & destructor */
    Cgi(void);
    ~Cgi();
    /* public function */
    int execute(Request req, std::string cgiFilePath, std::string file);
    /* getter & setter */
    std::string getCgiResponseHeader(void);
    std::string getCgiResponseBody(void);
    int getStatusCode(void);
    void setStatusCode(int code);
  private:
    /* private static variable */
    static const std::string _environList[NON_OF_ALL];
    /* private variable */
    char** _environ;
    size_t _allocSize;
    std::string _cgiResponseHeader;
    std::string _cgiResponseBody;
    std::string _cgiInput;
    int _statusCode;
    /* private function */
    std::map<std::string, std::string> _makeEnvMap(const Request& req, std::string file) const;
    std::string _getInput(std::string file);
    void _setEnviron(const Request& req, std::string file);
    void _setCgiResponseHeader(const std::vector<std::string>& str);
    void _setCgiResponseBody(const std::vector<std::string>& str);
};
#endif