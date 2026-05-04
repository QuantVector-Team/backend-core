#include "Session.h"
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/asio.hpp>
#include "../../include/Router/Router.h"
#include <iostream>

Session::Session(boost::asio::ip::tcp::socket socket) : socket_(std::move(socket)) {};

void Session::run() {
	do_read();
}

void Session::do_read() {
	req_ = {};
	std::shared_ptr<Session> self = shared_from_this();
	boost::beast::http::async_read(socket_, buffer_, req_, [this, self](const boost::system::error_code &ec, std::size_t length) {
		self->on_read(ec, length);
	});

}
void Session::on_read(const boost::system::error_code &ec, std::size_t bytes_transferred) {
	if(ec == boost::beast::http::error::end_of_stream) return do_close();
	if(ec) {
		std::cerr << "[NETWORK ERROR] Error of client" << std::endl; 
		std::cerr << "[ERROR] Error reading: " << ec.message() <<  std::endl;
		return;
	}	
	res_ = Router::routeRequest(req_);
	res_.set(boost::beast::http::field::access_control_allow_origin, "*");
	res_.set(boost::beast::http::field::access_control_allow_methods, "GET, POST, OPTIONS");
	res_.set(boost::beast::http::field::access_control_allow_headers, "Content-Type, Authorization");
	do_write();
}

void Session::do_write() {
	std::shared_ptr<Session> self = shared_from_this();

	boost::beast::http::async_write(socket_, res_, [this, self](const boost::system::error_code &ec, std::size_t length) {
		self->on_write(ec, length);
	});
}

void Session::on_write(const boost::system::error_code &ec, std::size_t bytes_transferred) {
	if(ec){
		std::cerr << "[ERROR] Error writing: " << ec.message() << std::endl;
		return;
	}
	do_close();
}

void Session::do_close() {
	boost::system::error_code ec;
	socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_send, ec);
}
