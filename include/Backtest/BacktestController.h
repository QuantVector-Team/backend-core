#ifndef BACKTEST_H_
#define BACKTEST_H_
#include <boost/beast/http.hpp>

class BacktestController {
	public:
		static boost::beast::http::response<boost::beast::http::string_body> runTest(const boost::beast::http::request<boost::beast::http::string_body> &req);
};
#endif
