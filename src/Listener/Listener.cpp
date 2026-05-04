#include "Listener.h"
#include "../Session/Session.h"
#include <boost/asio.hpp>
#include <iostream>
#include <memory>

Listener::Listener(boost::asio::io_context &ioc, boost::asio::ip::tcp::endpoint endpoint) : ioc_(ioc), acceptor_(ioc)  {
	boost::system::error_code ec;
	acceptor_.open(endpoint.protocol(), ec);
	acceptor_.set_option(boost::asio::socket_base::reuse_address(true), ec);
	acceptor_.bind(endpoint, ec);
	acceptor_.listen(boost::asio::socket_base::max_listen_connections, ec);
	if(ec) std::cerr << "Error initalize Listener: " << ec.message() << std::endl;
}

void Listener::run() {
	do_accept();
}

void Listener::do_accept() {
	std::shared_ptr<Listener> self = shared_from_this();
	acceptor_.async_accept(ioc_, [this, self](const boost::system::error_code &ec, boost::asio::ip::tcp::socket socket) {
		if(ec)	std::cerr << "Error accept: " << ec.message() << std::endl;
		else {
			std::lock_guard<std::mutex> lock(mutex_);
			std::cout << "[SERVER] Someone is connected!" << std::endl;
			std::make_shared<Session>(std::move(socket))->run();
		}
		do_accept();
	});
}

