#include "moving_average_strategy.h"
#include <iostream>

using namespace std;


MovingAverageStrategy::MovingAverageStrategy(){
    cout << "Moving Average Crossover strategy initialized" << endl;    
}

void MovingAverageStrategy::on_market_data(const simdjson::dom::element &doc){
    cout << "Moving Average Crossover received new market data" << endl;

    cout << "Received data: " << doc << endl;
}

void MovingAverageStrategy::on_tick(const std::string &symbol, double price, double quantity){
    cout << "MA  Crossover received tick for " << symbol << " at price: " << price <<endl;
}