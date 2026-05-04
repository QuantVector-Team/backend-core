#ifndef SESSION_H_
#define SESSION_H_
#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/core.hpp>
#include <memory>

class Session : public std::enable_shared_from_this<Session> {
	private:
		boost::asio::ip::tcp::socket socket_;
		boost::beast::flat_buffer buffer_;
		boost::beast::http::request<boost::beast::http::string_body> req_;
		boost::beast::http::response<boost::beast::http::string_body> res_;

	public:
		explicit Session(boost::asio::ip::tcp::socket socket);
		void run();
	private:
		void do_read();
		void do_write();
		void do_close();
		void on_read(const boost::system::error_code &ec, std::size_t bytes_transferred);
		void on_write(const boost::system::error_code &ec, std::size_t bytes_transferred);
};
#endif 
