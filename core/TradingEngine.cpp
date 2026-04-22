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




RsiStrategy::RsiStrategy(int period, int buy, int sell) : rsi_period_(period), buy_level_(buy), sell_level_(sell) {};


double RsiStrategy::calculateRsi(const std::vector<Candle> &data, int index) {
	if(index < rsi_period_)
		return 50.0;
	double gain_sum = 0.0;
	double loss_sum = 0.0;
	for(int i = 0; i < rsi_period_; i++) {
		double diff = data[index - i].close - data[index - i - 1].close;
		if(diff > 0) 
			gain_sum += diff;
		else 
			loss_sum -= diff;
	}
	double avg_gain = gain_sum / rsi_period_;
	double avg_loss = loss_sum / rsi_period_;
	if(avg_loss == 0.0) 
		return 100.0;
	double rs = avg_gain / avg_loss;
	return 100.0 - (100.0 / (1.0 + rs));
};


int RsiStrategy::getSignal(const std::vector<Candle> &history, int current_index) {
	if(current_index <= rsi_period_)
		return 0;
	double rsi_now = calculateRsi(history, current_index);
	double rsi_prev = calculateRsi(history, current_index - 1);

	if(rsi_prev <= buy_level_ && rsi_now > buy_level_)
		return 1;
	if(rsi_prev >= sell_level_ && rsi_now < sell_level_)
		return -1;
	return 0;
};


MacdStrategy::MacdStrategy(int fast, int slow, int signal) : fast_period_(fast), slow_period_(slow), signal_period_(signal) {}; 


double MacdStrategy::calculateEMA(const std::vector<Candle> &data, int index, int period) {
	if(index < period)
		return data[index].close;
	double multiplier = 2.0 / (period + 1.0);

	int start_idx = std::max(0, index - period * 3);
	double ema = data[start_idx].close;
	for(int i = start_idx + 1; i <= index; i++) {
		ema = (data[i].close - ema) * multiplier + ema;
	}
	return ema;
};

double MacdStrategy::getMacdLine(const std::vector<Candle> &data, int index) {
	return calculateEMA(data, index, fast_period_) - calculateEMA(data, index, slow_period_);
};


int MacdStrategy::getSignal(const std::vector<Candle> &history, int current_index) {
	if(current_index < slow_period_ * 3 + signal_period_)
		return 0;
	double macd_now = getMacdLine(history, current_index);
	double macd_prev = getMacdLine(history, current_index - 1);
	
	double signal_sum_now = 0.0;
	double signal_sum_prev = 0.0;
	for(int i = 0; i < signal_period_; i++) {
		signal_sum_now += getMacdLine(history, current_index - i);
		signal_sum_prev += getMacdLine(history, current_index - 1 - i);
	}
	double signal_line_now = signal_sum_now / signal_period_;
	double signal_line_prev = signal_sum_prev / signal_period_;

	double hist_now = macd_now - signal_line_now;
	double hist_prev = macd_prev - signal_line_prev;	

	if(hist_prev <= 0 && hist_now > 0) return 1;
	if(hist_prev >= 0 && hist_now < 0) return -1;
	return 0;
}






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

};


























