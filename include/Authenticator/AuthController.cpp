#include "AuthController.h"
#include <pqxx/pqxx>
#include <boost/json.hpp>
#include <iostream>

std::string generateFakeToken(const std::string &email) {
	return "token_" + email + "_12345";
};



boost::beast::http::response<boost::beast::http::string_body> AuthController::registerUser(const boost::beast::http::request<boost::beast::http::string_body> &req) {
	boost::beast::http::response<boost::beast::http::string_body> res{boost::beast::http::status::ok, req.version()};
	res.set(boost::beast::http::field::content_type, "application/json");
	boost::json::object response_obj;
	try {
		boost::json::value parsed = boost::json::parse(req.body());
		boost::json::object body = parsed.as_object();

		std::string platform = body.at("platform").as_string().c_str();
	
		boost::json::value auth_data = body.at("auth_data");
		boost::json::object auth_data_obj = auth_data.as_object();
		std::string login = auth_data_obj.at("login").as_string().c_str();


		pqxx::connection db_conn("dbname=quant_db user=postgres password=12345 host=127.0.0.1 port=5432");
		pqxx::work txn(db_conn);

		if(platform == "vk")  {
			std::string vk_user_id  = auth_data_obj.at("vk_user_id").as_string().c_str();
			std::string sql = "INSERT INTO users (platform, vk_user_id, ) VALUES (" + txn.quote(platform) + ", " + txn.quote(vk_user_id) + ", " + txn.quote(login) +
				") ON CONFLICT (vk_user_id) DO NOTHING;";
			txn.exec(sql);
		}
		if(platform == "mobile" || platform == "web") {
			std::string email = auth_data_obj.at("email").as_string().c_str();
			std::string password = auth_data_obj.at("password").as_string().c_str();
			std::string token = generateFakeToken(email);
		       	std::string sql =  "INSERT INTO users (platform, email, password_hash, token, login) VALUES (" + txn.quote(platform) + ", " + txn.quote(email) + ", " + txn.quote(password) +
				", "  + txn.quote(token) + ", " +  txn.quote(login) + ");";
			txn.exec(sql);	
		}
		txn.commit();


		

		response_obj["status"] = "success";
		response_obj["token"] = "secret_token_123";

	}
	catch (const std::runtime_error &e) {
		std::cerr << "Runtime error: " << e.what() << std::endl;
		res.result(boost::beast::http::status::bad_request);
		response_obj["status"] = "error";
		response_obj["message"] = "Unknow platform";
		response_obj["error"] = "Runtime Error: " +std::string(e.what());
		
	}
	catch (const std::exception &e) {
		std::cerr << "Exception: "  << e.what() << std::endl;
		res.result(boost::beast::http::status::bad_request);
		response_obj["status"] = "error";
		response_obj["message"] = "The user with this email already exist";
		response_obj["error"] = "Exception: " + std::string(e.what());
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
		if(platform == "vk")
			throw std::runtime_error("VK bot does not need login endpoint");
	
		std::string email = body.at("email").as_string().c_str();
		std::string password = body.at("password").as_string().c_str();
		std::string login = body.at("login").as_string().c_str();

		std::cout << "[LOGIN] The login attempt: " << login << std::endl;
		pqxx::connection db_conn("dbname=quant_db uesr=postgres password=12345 host=127.0.0.1 port=5432");
		pqxx::work txn(db_conn);

		std::string sql = "SELECT password_hash, login, token FROM users WHERE email = " + txn.quote(email) + ";";
		pqxx::result result = txn.exec(sql);
		if(result.empty())
			throw std::runtime_error("User not found");
		std::string db_password = result[0]["password_hash"].as<std::string>();
		std::string db_login = result[1]["login"].as<std::string>();
		std::string db_token = result[2]["token"].as<std::string>();
		if(db_password != password)
			throw std::runtime_error("Wrong password");
		
		std::cout << "[DB] Successful login!"<< std::endl;
		response_obj["status"] = "success";
		response_obj["token"] = db_token;
		response_obj["login"] = db_login;

	}
	catch (const std::exception &e) {
		res.result(boost::beast::http::status::bad_request);
		response_obj["status"] = "error";
		response_obj["message"] = "Error login: invalid email or password";
	};
	res.body() = boost::json::serialize(response_obj);
	res.prepare_payload();
	return res;
}
