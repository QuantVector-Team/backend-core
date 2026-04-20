#ifndef TRADING_ENGINE_H_
#define TRADING_ENGINE_H_
#pragma once
#include <vector>
struct Candle {
	long long timestamp;
	double close;
};

struct BacktestResult {
	double profit_percent;
	int total_trades;
	std::vector<std::pair<long long, double>> equity_curve;
};

class IStrategy {
	public:
		virtual ~IStrategy() = default;
		virtual int getSignal(const std::vector<Candle> &history, int current_index) = 0;
};



class SmaCrossStrategy : public IStrategy {
	private:
		int fast_period_;
		int slow_period_;
		
		double calculateSMA(const std::vector<Candle> &data, int index, int period);
	public:
		SmaCrossStrategy(int fast, int slow);
		int getSignal(const std::vector<Candle> &history, int current_index) override;
};

class BollingerStrategy : public IStrategy {
	private:
		int window_size_;
		double num_std_dev_;
		void calculateBollinger(const std::vector<Candle> &data, int index, double &sma, double &std_dev);
	public:
		BollingerStrategy(int window, double dev);
		int getSignal(const std::vector<Candle> &history, int current_index) override;
};

class BacktestEngine { 
	public:
		static BacktestResult run(const std::vector<Candle> &history, double start_balance, IStrategy *strategy);
};
#endif
