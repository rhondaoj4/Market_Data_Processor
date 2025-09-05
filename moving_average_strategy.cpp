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
    cout << "Moving Average Crossover received tick for " << symbol << " at price: " << price <<endl;

    // periods for slow and fast moving average
    const size_t SLOW_PERIOD = 10, FAST_PERIOD = 2;

    // update price history
    price_history_.push_back(price);

    // size of of price_history vector bound by slow period
    if(price_history_.size() > SLOW_PERIOD){
        price_history_.pop_front();
    }

    // have enough data to calculate both slow and fast moving averages
    if(price_history_.size() >= SLOW_PERIOD){
        
        double fast_sum = 0.0;
        for(size_t i = 0; i < FAST_PERIOD; i++){
            // FAST_PERIOD frequent prices
            fast_sum += price_history_[price_history_.size()-1-i];
        }
        double new_fast_moving_avg = fast_sum / FAST_PERIOD;

        double slow_sum = 0.0;
        for(size_t i = 0; i < SLOW_PERIOD; i++){
            // SLOW_PERIOD frequent prices
            slow_sum += price_history_[price_history_.size()-1-i];
        }
        double new_slow_moving_avg = slow_sum / SLOW_PERIOD;

        // generating a trading signal
        if(new_fast_moving_avg > new_slow_moving_avg && fast_moving_average_ <= slow_moving_average_){
            // bullish crossover
            cout << "SIGNAL: BUY " << symbol << endl;
        }else if(new_fast_moving_avg < new_slow_moving_avg && fast_moving_average_ >= slow_moving_average_){
            // bearish crossover
            cout << "SIGNAL: SELL " << symbol <<endl;
        }
            // update state for next tick
        fast_moving_average_ = new_fast_moving_avg;
        slow_moving_average_ = new_slow_moving_avg;

    }

    
}

