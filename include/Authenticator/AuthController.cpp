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

		if(platform == "vk") {
			long long user_id = body.at("vk_user_id").as_int64();
			std::string user_name = body.at("name").as_string().c_str();

			// TODO: add the SQL request to PostgreSQL

			response_obj["status"] = "success";
			response_obj["message"] = "Account has been successfully linked.";
		}
		else if(platform == "web") {
			std::string user_email = body.at("email").as_string().c_str();
			std::string user_password = body.at("password").as_string().c_str();

			// TODO: add the SQL request to PostgreSQL

			response_obj["status"] = "success";
			response_obj["message"] = "Account has been created successfully.";
			response_obj["token"] = "auth_token_web_98765abcd";
		}
		else if(platform == "mobile") {
			std::string user_email = body.at("email").as_string().c_str();
			std::string user_password = body.at("password").as_string().c_str();

			// TODO: add the SQL request to PostgreSQL

			response_obj["status"] = "success";
			response_obj["message"] = "Account has been created successfully.";
			response_obj["token"] = "auth_token_mob_12345xyza";
		}
		else {
			throw std::runtime_error("Unknown platform");
		}
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
	/*boost::beast::http::response<boost::beast::http::string_body> res{boost::beast::http::status::ok, req.version()};
	res.set(boost::beast::http::field::content_type, "application/json");
	try {
		boost::json::value parsed = boost::json::parse(req.body());
		boost::json::object &body = parsed.as_object();

		std::string platform = body.at("platform").as_string().c_str();
		boost::json::object response_obj;

		if(platform == "vk") {
			long long user_id = body.at("vk_user_id").as_int64();
			std::string user_name = body.at("name").as_string().c_str();

			// TODO: add the SQL request to PostgreSQL

			response_obj["status"] = "success";
			response_obj["message"] = "Account has been successfully linked.";
		}
		else if(platform == "web") {
			std::string user_email = body.at("email").as_string().c_str();
			std::string user_password = body.at("password").as_string().c_str();

			// TODO: add the SQL request to PostgreSQL

			response_obj["status"] = "success";
			response_obj["message"] = "Account has been created successfully.";
			response_obj["token"] = "auth_token_web_98765abcd";
		}
		else if(platform == "mobile") {
			std::string user_email = body.at("email").as_string().c_str();
			std::string user_password = body.at("password").as_string().c_str();

			// TODO: add the SQL request to PostgreSQL

			response_obj["status"] = "success";
			response_obj["message"] = "Account has been created successfully.";
			response_obj["token"] = "auth_token_mob_12345xyza";
		}
		else {
			throw std::runtime_error("Unknown platform");
		}
	}
	catch (const std::exception &e) {
		res.result(boost::beast::http::status::bad_request);
		response_obj["status"] = "error";
		response_obj["message"] = "The user with this email already exist";
	};
	res.body() = boost::json::serialize(response_obj);
	res.prepare_payload();
	return res;
	*/
}
