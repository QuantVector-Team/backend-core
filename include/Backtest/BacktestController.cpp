#include "BacktestController.h"
#include "../../core/TradingEngine.h"
#include "../Binance/MarketFetcher.h"
#include <boost/json.hpp>
#include <boost/beast/version.hpp>
#include <pqxx/pqxx>
#include <iostream>

boost::beast::http::response<boost::beast::http::string_body> BacktestController::runTest(const boost::beast::http::request<boost::beast::http::string_body> &req) {
	boost::beast::http::response<boost::beast::http::string_body> res{boost::beast::http::status::ok, req.version()};
	res.set(boost::beast::http::field::content_type, "application/json"); 
	
	try {
		boost::json::value parsed = boost::json::parse(req.body());
		boost::json::object &body = parsed.as_object();
		boost::json::object response_obj;


		// Parsing data
		std::string platform = body.at("platform").as_string().c_str();
		std::string vk_user_id = "";
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
		std::string symbol = sett_obj.at("symbol").as_string().c_str();
		std::string timeframe = sett_obj.at("timeframe").as_string().c_str();
		double start_balance = sett_obj.at("start_balance").as_double();
		double fee_percent = sett_obj.at("fee_percent").as_double();

		boost::json::value strat = body.at("strategy");
		boost::json::object strat_obj = strat.as_object();
		std::string strat_name = strat_obj.at("name").as_string().c_str();
		boost::json::value params = strat.at("params");
		boost::json::object params_obj = params.as_object();

		// Strategy selection
		IStrategy *my_strategy = nullptr;
		if(strat_name == "SMA_Cross") {
			int fast_period = params_obj.contains("fast_period") ?  params_obj.at("fast_period").as_int64() : 10;
			int slow_period = params_obj.contains("slow_period") ?  params_obj.at("slow_period").as_int64() : 50;
			my_strategy = new SmaCrossStrategy(fast_period, slow_period);
		}
		else if(strat_name == "Bollinger Bands") {
			int window = params_obj.contains("window") ? params_obj.at("window").as_int64() : 20;
			double dev = params_obj.contains("deviation") ? params_obj.at("deviation").as_double() : 2.0;
			my_strategy = new BollingerStrategy(window, dev);
		}
		else if(strat_name == "RSI_Oscillator") {
			int period = params_obj.contains("period") ? params_obj.at("period").as_int64() : 14;
			int buy = params_obj.contains("buy_level") ? params_obj.at("buy_level").as_int64() : 30;
			int sell = params_obj.contains("sell_level") ? params_obj.at("sell_level").as_int64() : 70;
			my_strategy = new RsiStrategy(period, buy, sell);
		}
		else if (strat_name == "MACD") {
			int fast_period = params_obj.contains("fast_period") ? params_obj.at("fast_period").as_int64() : 12;
			int slow_period = params_obj.contains("slow_period") ? params_obj.at("slow_period").as_int64() : 26;
			int signal_period = params_obj.contains("signal_period") ? params_obj.at("signal_period").as_int64() : 9;
			my_strategy = new MacdStrategy(fast_period, slow_period, signal_period);
		}
		else {
			throw std::runtime_error("Invalid strategy: " + strat_name);
		}
		std::cout << "[BACKTEST] Coin: " << symbol  << "\nTimeframe: " << timeframe << "\nStrategy name: " << strat_name << std::endl;

	
		// Import data about the coin	
		std::vector<Candle> real_history = MarketFetcher::fetchBinanceData(symbol, timeframe, 1000);

		
		// Calculate results
		BacktestResult result = BacktestEngine::run(real_history, start_balance, my_strategy);
		delete my_strategy;

		std::cout << " >> Result: Profit " << result.profit_percent << "%, Trades: " << result.total_trades << "\n";


		// Adding information to the data base history
		pqxx::connection db_conn("dbname=quant_db user=postgres password=12345 host=127.0.0.1 port=5432");
		pqxx::work txn(db_conn);
		std::string sql = "";
		if(platform == "vk") {
			sql = "INSERT INTO history (vk_user_id, symbol, strategy_name, profit_percent) VALUES (" + txn.quote(vk_user_id) + ", " + txn.quote(symbol) + ", " + 
				txn.quote(strat_name) + ", " + txn.quote(result.profit_percent) + ");";  
		}
		else if(platform == "mobile" || platform == "web") {
			sql = "INSERT INTO history (owner_token, symbol, strategy_name, profit_percent) VALUES (" + txn.quote(token) + ", " + txn.quote(symbol) + ", " + txn.quote(strat_name) + ", " +
				txn.quote(result.profit_percent) + ");";
		}
		txn.exec(sql);
		txn.commit();
		

		// Forming a response
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

