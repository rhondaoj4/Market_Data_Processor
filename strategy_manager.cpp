#include "strategy_manager.h"
#include <iostream>

using namespace std;

void StrategyManager::add_strategy(const string &name, shared_ptr<IStrategy> strategy){
    strategies_[name] = strategy;
    cout << "Strategy '"<< name << "' added." << endl;
}

void StrategyManager::process_data(const simdjson::dom::element &doc){
    // parse to extra data
    for(auto const &[name, strategy] : strategies_){
        strategy->on_market_data(doc);
    }
}