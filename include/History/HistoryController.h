#ifndef HISTORY_CONTROLLER_H_
#define HISTORY_CONTROLLER_H_

#include <boost/beast/http.hpp>
#include <iostream>

class HistoryController {
	public:
		static boost::beast::http::response<boost::beast::http::string_body> getUserHistory(const boost::beast::http::request<boost::beast::http::string_body> &req);
};

#endif
