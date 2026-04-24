#include "MarketFetcher.h"
#include <boost/beast/core.hpp>
#include <boost/beast/version.hpp>
#include <boost/beast/http.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/json.hpp>
#include <iostream>
#include <string>


std::vector<Candle> MarketFetcher::fetchBinanceData(const std::string &symbol, const std::string &interval, int limit)  {
	std::vector<Candle> history;
	std::string host = "api.binance.com";
	std::string port = "443";

	std::string target = "/api/v3/klines?symbol=" + symbol + "&interval=" + interval + "&limit=" + std::to_string(limit);
	try {
		boost::asio::io_context ioc;
		boost::asio::ssl::context ctx(boost::asio::ssl::context::tlsv12_client);
		ctx.set_default_verify_paths();
		boost::asio::ip::tcp::resolver resolver(ioc);
		boost::beast::ssl_stream<boost::beast::tcp_stream> stream(ioc, ctx);

		if(!SSL_set_tlsext_host_name(stream.native_handle(), host.c_str())) {
			boost::system::error_code ec;
			ec.assign(static_cast<int>(::ERR_get_error()), boost::asio::error::get_ssl_category());
			throw boost::system::system_error{ec};
		}
		auto const results = resolver.resolve(host, port);
		boost::beast::get_lowest_layer(stream).connect(results);
		stream.handshake(boost::asio::ssl::stream_base::client);

		boost::beast::http::request<boost::beast::http::string_body> req{boost::beast::http::verb::get, target, 11};
		req.set(boost::beast::http::field::host, host);
		req.set(boost::beast::http::field::user_agent, BOOST_BEAST_VERSION_STRING);

		boost::beast::http::write(stream, req);
		boost::beast::flat_buffer buffer;
		boost::beast::http::response<boost::beast::http::dynamic_body> res;
		boost::beast::http::read(stream, buffer, res);
		boost::system::error_code ec;
		stream.shutdown(ec);



		std::string json_str = boost::beast::buffers_to_string(res.body().data());
		boost::json::value parsed = boost::json::parse(json_str);
		boost::json::array& klines = parsed.as_array();
		for(auto &item : klines) {
			boost::json::array &kline = item.as_array();
			Candle c;
			c.timestamp = kline[0].as_int64();

			c.close = std::stod(kline[4].as_string().c_str());
			history.push_back(c);
		}
		std::cout << "[BINANCE] Successfully download " << history.size() << " candle for " << symbol << "\n";

	}
	catch(std::exception const &e) {
		std::cerr << "[ERROR BINANCE] Error downloaded from Binance: " << e.what() << "\n"; 
	}
	return history;
}
