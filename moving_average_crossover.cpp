#include "moving_average_crossover.h"
#include <iostream>

using namespace std;


MovingAverageCrossover::MovingAverageCrossover(){
    cout << "Moving Average Crossover strategy initialized" << endl;    
}

void MovingAverageCrossover::on_market_data(const simdjson::dom::element &doc){
    cout << "Moving Average Crossover received new market data" << endl;
}

void MovingAverageCrossover::on_tick(const std::string &symbol, double price, double quantity){
    cout << "MA  Crossover received tick for " << symbol << " at price: " << price <<endl;
}