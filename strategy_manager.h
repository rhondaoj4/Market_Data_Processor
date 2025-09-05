#pragma once
#include "strategy.h"
#include "simdjson.h"
#include <memory>
#include <string>
#include <map>

using namespace std;

//Moving Average Crossover, Triangular Arbitrage, Momentum Following, Mean Reversion
class StrategyManager{
    
public:

    // add new trading strategies to manager's collection
    void add_strategy(const string &name, shared_ptr<IStrategy> strategy);
    // iterates through every strategy stored in strategies_ and makes sure
    //it's processed simultaneously 
    void process_data(const simdjson::dom::element &doc);

private:

    // different strategies -> implementation
    map<string, shared_ptr<IStrategy>> strategies_;

};