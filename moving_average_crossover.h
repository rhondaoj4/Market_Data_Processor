#pragma once

#include "strategy.h"
#include "simdjson.h"
#include <deque>

using namespace std;

class MovingAverageCrossover : public IStrategy {

public:

    MovingAverageCrossover();

    void on_market_data(const simdjson::dom::element &doc) override;

    void on_tick(const std::string &symbol, double price, double quantity) override;

private:

    // hold the state of this strategy
    deque<double> price_history_;
    double fast_moving_average_ = 0.0;
    double slow_moving_average_ = 0.0;

};