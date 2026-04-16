#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/json.hpp>
#include <boost/json/src.hpp>
#include "../include/Router/Router.h"
#include <iostream>
#include <string>


int main() {
	try{
		boost::asio::io_context ioc;
		boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::make_address("0.0.0.0"), 8080);
		boost::asio::ip::tcp::acceptor acceptor(ioc, endpoint);
		std::cout << "[SERVER] Start on the port 8080.  Waiting a requests..." << std::endl;

		while(true) {
			boost::asio::ip::tcp::socket socket(ioc);
			acceptor.accept(socket);
			std::cout << "[SERVER] Someone is connected!" << std::endl;

			boost::beast::flat_buffer buffer;
			boost::beast::http::request<boost::beast::http::string_body> req;
			try{
				boost::beast::http::read(socket, buffer, req);
				boost::beast::http::response<boost::beast::http::string_body> res = Router::routeRequest(req);
				boost::beast::http::write(socket, res);
			}
			catch (boost::system::system_error const &se) {
				std::cerr << "[NETWORK ERROR] Error of client: " << se.what() << std::endl; 
			}
			boost::system::error_code ec;
			socket.shutdown(boost::asio::ip::tcp::socket::shutdown_send, ec);
		}
	}
	catch(std::exception const &e) {
		std::cerr << "[FATAL ERROR] Server is broken " << e.what() << std::endl;
	}
	return 0;
}
