#ifndef AUTHCONTROLLER_H_
#define AUTHCONTROLLER_H_
#include <boost/beast/http.hpp>

class AuthController {
	public:
		static boost::beast::http::response<boost::beast::http::string_body> registerUser(const boost::beast::http::request<boost::beast::http::string_body> &req);
		static boost::beast::http::response<boost::beast::http::string_body> loginUser(const boost::beast::http::request<boost::beast::http::string_body> &req);
};

#endif
