#include "AuthController.h"
#include <boost/json.hpp>
#include <iostream>

boost::beast::http::response<boost::beast::http::string_body> AuthController::registerUser(const boost::beast::http::request<boost::beast::http::string_body> &req) {
	boost::beast::http::response<boost::beast::http::string_body> res{boost::beast::http::status::ok, req.version()};
	res.set(boost::beast::http::field::content_type, "application/json");
	boost::json::object response_obj;
	try {
		boost::json::value parsed = boost::json::parse(req.body());
		boost::json::object &body = parsed.as_object();

		std::string platform = body.at("platform").as_string().c_str();
		if(platform != "vk" && platform != "web" && platform != "mobile") 
			throw std::runtime_error("Unknown platform");
	
		boost::json::value auth_data = body.at("auth_data");
		boost::json::object auth_data_obj = auth_data.as_object();
		long long vk_user_id = 0;
		if(platform == "vk") 
			vk_user_id  = auth_data_obj.at("vk_user_id").as_int64();

		std::string email = auth_data_obj.at("email").as_string().c_str();
		std::string password = auth_data_obj.at("password").as_string().c_str();
		std::string user_name = auth_data_obj.at("user_name").as_string().c_str();
		std::string user_surname = auth_data_obj.at("user_surname").as_string().c_str();

		// TODO: add the SQL request to PostgreSQL

		response_obj["status"] = "success";
		response_obj["token"] = "secret_token_123";

	}
	catch (const std::exception &e) {
		res.result(boost::beast::http::status::bad_request);
		response_obj["status"] = "error";
		response_obj["message"] = "The user with this email already exist";
	}
	res.body() = boost::json::serialize(response_obj);
	res.prepare_payload();
	return res;
}

//TODO: add the login user mechanic
boost::beast::http::response<boost::beast::http::string_body> AuthController::loginUser(const boost::beast::http::request<boost::beast::http::string_body> &req) {
	boost::beast::http::response<boost::beast::http::string_body> res{boost::beast::http::status::ok, req.version()};
	res.set(boost::beast::http::field::content_type, "application/json");
	boost::json::object response_obj;
	try {
		boost::json::value parsed = boost::json::parse(req.body());
		boost::json::object &body = parsed.as_object();

		std::string platform = body.at("platform").as_string().c_str();
		if(platform != "vk" && platform != "web" && platform != "mobile") 
			throw std::runtime_error("Unknown platform");
	
		std::string email = body.at("email").as_string().c_str();
		std::string password = body.at("password").as_string().c_str();
		std::string user_name = body.at("user_name").as_string().c_str();
		std::string user_surname = body.at("user_surname").as_string().c_str();

		// TODO: add the SQL request to PostgreSQL

		response_obj["status"] = "success";
		response_obj["token"] = "secret_token_123";
		response_obj["user_name"] = "sava";
		response_obj["user_surname"] = "nazarov";

	}
	catch (const std::exception &e) {
		res.result(boost::beast::http::status::bad_request);
		response_obj["status"] = "error";
		response_obj["message"] = "The email or password is invalid";
	};
	res.body() = boost::json::serialize(response_obj);
	res.prepare_payload();
	return res;
}
