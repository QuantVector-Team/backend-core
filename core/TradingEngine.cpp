#include "TradingEngine.h"
#include <cmath>

SmaCrossStrategy::SmaCrossStrategy(int fast, int slow) : fast_period_(fast), slow_period_(slow) {};


double SmaCrossStrategy::calculateSMA(const std::vector<Candle> &data, int index, int period) {
	if(index < period - 1)
		return 0.0;
	double sum = 0.0;
	for(int i = 0; i < period; i++) 
		sum += data[index - i].close;
	return sum / period;
};



int SmaCrossStrategy::getSignal(const std::vector<Candle> &history, int current_index) {
	if(current_index < slow_period_)
		return 0;

	int fast_now = calculateSMA(history, current_index, fast_period_);
	int slow_now = calculateSMA(history, current_index, slow_period_);

	int fast_prev = calculateSMA(history, current_index - 1, fast_period_);
	int slow_prev = calculateSMA(history, current_index - 1, slow_period_);

	if(fast_prev <= slow_prev && fast_now > slow_now)
		return 1;
	if(fast_prev >= slow_prev && fast_now < slow_now)
		return -1;
	return 0;
};



BollingerStrategy::BollingerStrategy(int window, double dev) : window_size_(window), num_std_dev_(dev) {};


void BollingerStrategy::calculateBollinger(const std::vector<Candle> &data, int index, double &sma, double &std_dev) {
	if(index < window_size_ - 1) {
		sma = 0;
		std_dev = 0;
		return;
	}

	double sum = 0;
	for(int i = 0; i < window_size_; i++)
		sum += data[index - i].close;
	sma = sum / window_size_;

	double variance_sum = 0;
	for(int i = 0; i < window_size_; i++) {
		double diff = data[index - i].close - sma;
		variance_sum += (diff * diff);
	}
	std_dev = std::sqrt(variance_sum / window_size_);
};

int BollingerStrategy::getSignal(const std::vector<Candle> &history, int current_index) {
	if(current_index < window_size_ - 1)
		return 0;

	double sma = 0, std_dev = 0;
	calculateBollinger(history, current_index, sma, std_dev);

	double upper_band = sma + (num_std_dev_ * std_dev);
	double lower_band = sma - (num_std_dev_ * std_dev);

	double current_price = history[current_index].close;
	double prev_price = history[current_index - 1].close;

	if(prev_price < lower_band && current_price >= lower_band)
		return 1;
	if(prev_price > upper_band && current_price <= upper_band)
		return -1;
	return 0;
};



BacktestResult BacktestEngine::run(const std::vector<Candle> &history, double start_balance, IStrategy *strategy) {
	BacktestResult result;
	result.total_trades = 0;

	double balance_usd = start_balance;
	double crypto_amount = 0.0;
	bool is_in_position = false;

	if(!history.empty()) 
		result.equity_curve.push_back({history[0].timestamp, balance_usd});
	for(int i = 0; i < history.size(); i++) {
		double price = history[i].close;
		int signal = strategy->getSignal(history, i);

		if(signal == 1 && !is_in_position) {
			crypto_amount = balance_usd / price;
			balance_usd = 0.0;
			is_in_position = true;
			result.total_trades++;
		}
		else if(signal == -1 && is_in_position) {
			balance_usd = crypto_amount * price;
			crypto_amount = 0.0;
			is_in_position = false;
			result.total_trades++;
		}
		double current_equity = is_in_position ? (crypto_amount * price) : balance_usd;
		result.equity_curve.push_back({history[i].timestamp, current_equity});
	}

	if(is_in_position)
		balance_usd = crypto_amount * history.back().close;
	result.profit_percent = ((balance_usd - start_balance) / start_balance) * 100.0;
	return result;

}






















