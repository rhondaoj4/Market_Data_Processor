#include "moving_average_strategy.h"
#include <iostream>
#include <numeric>

using namespace std;


MovingAverageStrategy::MovingAverageStrategy(){
    cout << "Moving Average Crossover strategy initialized" << endl;    
}

void MovingAverageStrategy::on_market_data(const simdjson::dom::element &doc){
    cout << "Moving Average Crossover received new market data" << endl;

    cout << "Received data: " << doc << endl;
}

void MovingAverageStrategy::on_tick(const std::string &symbol, double price, double quantity){
    cout << "Moving Average Crossover received tick for " << symbol << " at price: " << price <<endl;

    // history isolated by symbol
    SymbolData &data = symbol_data_[symbol];
    // periods for slow and fast moving average
    const size_t SLOW_PERIOD = 10, FAST_PERIOD = 2;

    // update price history;
    data.price_history.push_back(price);
    

    // size of of price_history vector bound by slow period
    if(data.price_history.size() > SLOW_PERIOD){
       data.price_history.pop_front();
    }

    // have enough data to calculate both slow and fast moving averages
    if(data.price_history.size() >= SLOW_PERIOD){
        
        double fast_sum = 0.0;
        for(size_t i = 0; i < FAST_PERIOD; i++){
            // FAST_PERIOD frequent prices
            fast_sum += data.price_history[data.price_history.size()-1-i];
        }
        double new_fast_moving_avg = fast_sum / FAST_PERIOD;

        // std::accumlate for robustness and clean code
        double slow_sum = std::accumulate(data.price_history.begin(), data.price_history.end(), 0.0);
        double new_slow_moving_avg = slow_sum/SLOW_PERIOD;

        // generating a trading signal
        if(new_fast_moving_avg > new_slow_moving_avg && data.fast_moving_average <= data.slow_moving_average){
            // bullish crossover
            cout << "SIGNAL: BUY " << symbol << endl;
        }else if(new_fast_moving_avg < new_slow_moving_avg && data.fast_moving_average >= data.slow_moving_average){
            // bearish crossover
            cout << "SIGNAL: SELL " << symbol <<endl;
        }
            // update state for next tick
        data.fast_moving_average = new_fast_moving_avg;
        data.slow_moving_average = new_slow_moving_avg;
    }
    
}