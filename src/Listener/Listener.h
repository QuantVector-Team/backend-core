#ifndef LISTENER_H_
#define LISTENER_H_
#include <boost/asio.hpp>
#include <memory>
#include <mutex>

class Listener : public std::enable_shared_from_this<Listener> {
	private:
		boost::asio::io_context &ioc_;
		boost::asio::ip::tcp::acceptor acceptor_;
		std::mutex mutex_;
	public:
		Listener(boost::asio::io_context &ioc, boost::asio::ip::tcp::endpoint endpoint);
		void run();
	private:
		void do_accept();
};
#endif
