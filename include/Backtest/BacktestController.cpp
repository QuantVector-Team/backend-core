#include "BacktestController.h"
#include "../../core/TradingEngine.h"
#include <boost/json.hpp>
#include <boost/beast/version.hpp>
#include <iostream>

boost::beast::http::response<boost::beast::http::string_body> BacktestController::runTest(const boost::beast::http::request<boost::beast::http::string_body> &req) {
	boost::beast::http::response<boost::beast::http::string_body> res{boost::beast::http::status::ok, req.version()};
	res.set(boost::beast::http::field::content_type, "application/json"); try {
		boost::json::value parsed = boost::json::parse(req.body());
		boost::json::object &body = parsed.as_object();
		boost::json::object response_obj;

		std::string platform = body.at("platform").as_string().c_str();
		std::string vk_user_id = 0;
		std::string token = "";
		bool need_chart = false;
		if(platform == "vk") 
			vk_user_id = body.at("vk_user_id").as_string().c_str();
		if(platform == "web" || platform == "mobile") {
			token = body.at("token").as_string().c_str();	
			need_chart = true;
		}

		boost::json::value settings = body.at("settings");
		boost::json::object &sett_obj = settings.as_object();
		std::string coin = sett_obj.at("symbol").as_string().c_str();
		std::string timeframe = sett_obj.at("timeframe").as_string().c_str();
		int start_balance = sett_obj.at("start_balance").as_int64();
		int fee_percent = sett_obj.at("fee_percent").as_int64();

		boost::json::value strat = body.at("strategy");
		boost::json::object strat_obj = strat.as_object();
		std::string strat_name = strat_obj.at("name").as_string().c_str();
		boost::json::value params = strat.at("params").as_string().c_str();
		boost::json::object params_obj = params.as_object();
		IStrategy *my_strategy = nullptr;
		if(strat_name == "SMA_Cross") {
			int fast_period = params_obj.contains("fast_period") ?  params_obj.at("fast_period").as_int64() : 10;
			int slow_period = params_obj.contains("slow_period") ?  params_obj.at("slow_period").as_int64() : 50;
			my_strategy = new SmaCrossStrategy(fast_period, slow_period);
		}
		else if(strat_name == "Bollinger") {
			int window = params_obj.contains("window") ? params_obj.at("window").as_int64() : 20;
			double dev = params_obj.contains("deviation") ? params_obj.at("deviation").as_int64() : 2.0;
			my_strategy = new BollingerStrategy(window, dev);
		}
		else {
			throw std::runtime_error("Invalid strategy: " + strat_name);
		}
		std::cout << "[BACKTEST] Coin: " << coin << "\nTimeframe: " << timeframe << "\nStrategy name: " << strat_name << std::endl;

		int rsi_period = params_obj.at("rsi_period").as_int64();
		int buy_level = params_obj.at("buy_level").as_int64();
		int sell_level = params_obj.at("sell_level").as_int64();



		// fake history..
		std::vector<Candle> fake_history;
		long long current_time = 1672531200;
		for(int i = 0; i < 500; ++i) {
			Candle c;
			c.timestamp = current_time + (i * 3600);
			c.close = 50000.0 + std::sin(i * 0.1) * 10000.0;
			fake_history.push_back(c);
		}
		
		BacktestResult result = BacktestEngine::run(fake_history, start_balance, my_strategy);
		delete my_strategy;

		std::cout << " >> Result: Profit " << result.profit_percent << "%, Trades: " << result.total_trades << "\n";
		
		
		
		response_obj["status"] = "success";
		boost::json::object summary;
		summary["profit_percent"] = result.profit_percent;
		summary["total_trades"] = result.total_trades;
		response_obj["summary"] = summary;
		if(need_chart) {
			boost::json::array chart_data;
			for(const auto &point : result.equity_curve) {
				boost::json::object dot;
				dot["time"] = point.first;
				dot["balance"] = point.second;
				chart_data.push_back(dot);
			}
			response_obj["chart_data"] = chart_data;
		}
		res.body() = boost::json::serialize(response_obj);
	}
	catch(std::exception const &e) {
		std::cerr << "[ERROR] Backtest parsing error: " << e.what() << std::endl;
		boost::json::object err_obj;
		err_obj["status"] = "error";
		err_obj["message"] = "invalid format JSON or absent field (coin, timeframe, strategy)";
		res.body() = boost::json::serialize(err_obj);
	}
	res.prepare_payload();
	return res;
}

