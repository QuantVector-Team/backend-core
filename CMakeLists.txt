cmake_minimum_required(VERSION 3.17)
project(QuantVector VERSION 1.0)

set(CMAKE_CXX_STANDART 17)
set(CMAKE_CXX_STANDART_REQUIRED True)

find_package(Boost REQUIRED COMPONENTS system)
add_executable(${PROJECT_NAME}
       
	src/main.cpp
	include/Router/Router.cpp
	include/Authenticator/AuthController.cpp
	include/Backtest/BacktestController.cpp
	include/History/HistoryController.cpp

	core/TradingEngine.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE Boost::system)
