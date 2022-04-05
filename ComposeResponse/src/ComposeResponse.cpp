#include "ComposeResponse.hpp"

ComposeResponse::ComposeResponse()
{

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



void		ComposeResponse::findMime(void) {
	//string root = "/var/www/html";
	//header.local_uri = root + "/index.test.html";
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
