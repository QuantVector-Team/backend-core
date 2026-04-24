#ifndef MARKET_FETCHER_H_
#define MARKET_FETCHER_H_
#include "../../core/TradingEngine.h"
#include <string>

class MarketFetcher {

	public:
		static std::vector<Candle>fetchBinanceData(const std::string &symbol, const std::string &interval, int limit);
};

#endif
