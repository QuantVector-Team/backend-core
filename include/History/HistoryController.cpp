#include "HistoryController.h"
#include <boost/beast/version.hpp>
#include <boost/json.hpp>
#include <pqxx/pqxx>
#include <iostream>
#include <thread>
#include <chrono>

boost::beast::http::response<boost::beast::http::string_body> HistoryController::getUserHistory(const boost::beast::http::request<boost::beast::http::string_body> &req) {

	boost::beast::http::response<boost::beast::http::string_body> res{boost::beast::http::status::ok, req.version()};
	res.set(boost::beast::http::field::content_type, "application/json");
	boost::json::object res_obj;
	try {
		boost::json::value parsed = boost::json::parse(req.body());
		boost::json::object parse_obj = parsed.as_object();
		std::string platform = parse_obj.at("platform").as_string().c_str();
		std::string vk_user_id = "";
		std::string token = "";
		if(platform == "vk") 
			 vk_user_id = parse_obj.at("vk_user_id").as_string().c_str();
		if(platform == "mobile" || platform == "web")
			token = parse_obj.at("token").as_string().c_str();
		long long limit = parse_obj.at("limit").as_int64();

		pqxx::connection db_conn("dbname=quant_db user=postgres password=12345 host=127.0.0.1 port=5432");
		pqxx::work txn(db_conn);
		std::string sql;
		if(platform == "vk")
			sql = "SELECT * FROM history WHERE vk_user_id = '" + vk_user_id + "';";
		else 
			sql = "SELECT * FROM history WHERE owner_token = '" + token + "';";
		pqxx::result result = txn.exec(sql);

		boost::json::array data;
		for(auto row : result) {
			boost::json::object test_result;
			test_result["date"] = row["created_at"].c_str();
			test_result["symbol"] = row["symbol"].c_str();
			test_result["strategy_name"] = row["strategy_name"].c_str();
			test_result["profit_percent"] = row["profit_percent"].as<double>();
			data.push_back(test_result);
		}

		res_obj["status"] = "success";
		res_obj["data"] = data;
		res.body() = boost::json::serialize(res_obj);
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
