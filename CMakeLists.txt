cmake_minimum_required(VERSION 3.17)
project(QuantVector VERSION 1.0)

set(CMAKE_CXX_STANDART 17)
set(CMAKE_CXX_STANDART_REQUIRED True)

find_package(Boost REQUIRED COMPONENTS system)
find_package(PkgConfig REQUIRED)
find_package(OpenSSL REQUIRED)
pkg_check_modules(PQXX REQUIRED libpqxx)
add_executable(${PROJECT_NAME}
       
	src/main.cpp
	include/Router/Router.cpp
	include/Authenticator/AuthController.cpp
	include/Backtest/BacktestController.cpp
	include/History/HistoryController.cpp
	include/Binance/MarketFetcher.cpp

	core/TradingEngine.cpp

)

target_include_directories(${PROJECT_NAME} PRIVATE ${Boost_INCLUDE_DIRS} ${PQXX_INCLUDE_DIRS} ${OPENSSL_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} PRIVATE Boost::system ${PQXX_LIBRARIES} OpenSSL::SSL OpenSSL::Crypto)
