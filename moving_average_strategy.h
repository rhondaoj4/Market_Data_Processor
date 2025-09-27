#pragma once

#include "strategy.h"
#include "simdjson.h"
#include <deque>
#include <map>
#include <string>

using namespace std;

struct SymbolData{
    deque<double> price_history;
    double fast_moving_average = 0.0;
    double slow_moving_average = 0.0;
};

class MovingAverageStrategy : public IStrategy {

public:

    MovingAverageStrategy();

    void on_market_data(const simdjson::dom::element &doc) override;

    void on_tick(const std::string &symbol, double price, double quantity) override;

private:

    // hold the state of this strategy. Each coin has it own history
    map<string, SymbolData> symbol_data_;

};