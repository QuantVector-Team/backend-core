#include "BacktestController.h"
#include <boost/json.hpp>
#include <boost/beast/version.hpp>
#include <iostream>

boost::beast::http::response<boost::beast::http::string_body> BacktestController::runTest(const boost::beast::http::request<boost::beast::http::string_body> &req) {
	boost::beast::http::response<boost::beast::http::string_body> res{boost::beast::http::status::ok, req.version()};
	res.set(boost::beast::http::field::content_type, "application/json");
	try {
		boost::json::value parsed = boost::json::parse(req.body());
		boost::json::object &body = parsed.as_object();
		std::string coin = body.at("coin").as_string().c_str();
		std::string timeframe = body.at("timeframe").as_string().c_str();
		bool need_chart = false;
		if(body.contains("need_chart_data")) {
			need_chart = body.at("need_char_data").as_bool();
		}
			//TODO: math part...
		
		boost::json::object &strategy_obj = body.at("strategy").as_object();
		std::string strat_name = strategy_obj.at("name").as_string().c_str();
		long long fast_period = strategy_obj.at("fast_period").as_int64();
		long long slow_period = strategy_obj.at("slow_period").as_int64();
		std::cout << "[BACKTEST] Coin: " << coin << "\nTimeframe: " << timeframe << "\nNeed the chart: " << (need_chart ? "Yes" : "No") << "\nStrategy name: " << strat_name << std::endl;
		
		boost::json::object response_obj;
		response_obj["status"] = "success";
		boost::json::object summary;
		summary["profit_percent"] = 22.5;
		summary["total_trades"] = 84;
		response_obj["summary"] = summary;
		if(need_chart) {
			boost::json::array equity_curve;
			equity_curve.push_back({{"time", 1672531200}, {"balance", 1000.0}});
			equity_curve.push_back({{"time", 1575443864}, {"balance", 1225.0}});
			response_obj["equity_curve"] = equity_curve;
		}
		res.body() = boost::json::serialize(response_obj);
	}
	catch(std::exception const &e) {
		std::cerr << "[ERROR] Backtest parsing error: " << e.what() << std::endl;
		boost::json::object err_obj;
		err_obj["status"] = "error";
		err_obj["message"] = "invalid format JSON or absent field (coin, timeframe, strategy)";
		res.body() = boost::json::serialize(err_obj);
	}
	res.prepare_payload();
	return res;
}

