#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/json.hpp>
#include <boost/json/src.hpp>
#include "../include/Router/Router.h"
#include "./Listener/Listener.h"
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <thread>


int main() {
	try{
		boost::asio::io_context ioc;
		boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::make_address("0.0.0.0"), 8080);
		std::cout << "[SERVER] Start on the port 8080.  Waiting a requests..." << std::endl;

		std::make_shared<Listener>(ioc, endpoint)->run();
		std::vector<std::thread> threads;
		for(int i = 0; i < 4; i++)
			threads.emplace_back([&ioc] {
					ioc.run();
			});
		ioc.run();
	}
	catch(std::exception const &e) {
		std::cerr << "[FATAL ERROR] Server is broken " << e.what() << std::endl;
	};
	return 0;
};
