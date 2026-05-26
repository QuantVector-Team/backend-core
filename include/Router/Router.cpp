#include "Router.h"
#include "../Backtest/BacktestController.h"
#include "../Authenticator/AuthController.h"
#include "../History/HistoryController.h"


boost::beast::http::response<boost::beast::http::string_body> Router::routeRequest(boost::beast::http::request<boost::beast::http::string_body> &req) {
	if(req.method() == boost::beast::http::verb::options) {
		boost::beast::http::response<boost::beast::http::string_body> res{boost::beast::http::status::ok, req.version()};
		res.set(boost::beast::http::field::access_control_allow_origin, "*");
		res.set(boost::beast::http::field::access_control_allow_methods, "GET, POST, OPTIONS");
		res.set(boost::beast::http::field::access_control_allow_headers, "Content-Type, Authorization");
		res.prepare_payload();
		return res;
	}
	std::string target = req.target();

	if(target == "/api/register" && req.method() == boost::beast::http::verb::post) {
		return AuthController::registerUser(req);
	}
	else if(target == "/api/login" && req.method() == boost::beast::http::verb::post) {
		return AuthController::loginUser(req);
	}
	else if(target == "/api/backtest/run" && req.method() == boost::beast::http::verb::post) {
		return BacktestController::runTest(req);
	}
	else if(target == "/api/history" && req.method() == boost::beast::http::verb::post) {
		return HistoryController::getUserHistory(req);
	}
	else {
		boost::beast::http::response<boost::beast::http::string_body> res{boost::beast::http::status::not_found, req.version()};
	       	res.set(boost::beast::http::field::content_type, "application/json");
		res.body() = "{\"error\":\"Not found\"}";
		res.prepare_payload();
		return res;	
	}
};
