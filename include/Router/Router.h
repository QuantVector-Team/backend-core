#ifndef ROUTER_H
#define ROUTER_H


#include <boost/beast/http.hpp>

class Router {
	public:
		static boost::beast::http::response<boost::beast::http::string_body> routeRequest(boost::beast::http::request<boost::beast::http::string_body> &req);
};

#endif
