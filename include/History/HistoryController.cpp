#include "HistoryController.h"
#include <boost/beast/version.hpp>
#include <boost/json.hpp>
#include <iostream>

boost::beast::http::response<boost::beast::http::string_body> HistoryController::getUserHistory(const boost::beast::http::request<boost::beast::http::string_body> &req) {
	boost::beast::http::response<boost::beast::http::string_body> res{boost::beast::http::status::ok, req.version()};
	res.set(boost::beast::http::field::content_type, "application/json");
	try {
		boost::json::value parsed = boost::json::parse(req.body());
		boost::json::object parse_obj = parsed.as_object();
		std::string platform = parse_obj.at("platform").as_string().c_str();
		if(platform == "vk") {
			long long user_id = parse_obj.at("vk_user_id").as_int64();
			long long limit = parse_obj.at("limit").as_int64();

			boost::json::object res_obj;
			res_obj["status"] = "success";
			boost::json::array history_strat;
			history_strat.push_back({{"date", "2026-05-20"}, {"symbol", "BTCUSDT"}, {"strategy", "RSI_Strategy"}, {"profit_percent", 15.4}});
			history_strat.push_back({{"date", "2026-05-19"}, {"symbol", "ETHUSDT"}, {"strategy", "SMA_Cross"}, {"profit_percent", -2.1}});
			res_obj["history"] = history_strat;
			res.body() = boost::json::serialize(res_obj);
		}
	}
	catch(std::exception &e) {
		std::cerr << "[ERROR] Parsing history error: " << e.what() << std::endl;
		boost::json::object err_obj;
		err_obj["status"] = "error";
		err_obj["message"] = "Parsing history error.";
		res.body() = boost::json::serialize(err_obj);
	}
	res.prepare_payload();
	return res;
}
