#include "strategy_manager.h"
#include <iostream>
#include <stdexcept>

using namespace std;

void StrategyManager::add_strategy(const string &name, shared_ptr<IStrategy> strategy){
    strategies_[name] = strategy;
    cout << "Strategy '"<< name << "' added." << endl;
}

void StrategyManager::process_data(const simdjson::dom::element &doc){
    // parse to extra data
    for(auto const &[name, strategy] : strategies_){
        strategy->on_market_data(doc);

        // check for data object
        if(doc.type() == simdjson::dom::element_type::OBJECT){
            simdjson::dom::element data_object = doc["data"];
            if (data_object.type() == simdjson::dom::element_type::OBJECT){

                // get symbol price and quantity
                string_view symbol_view = data_object["s"];
                string symbol_str(symbol_view);

                // get price and quantity as strings first, then convert to double
                string_view price_str_view = data_object["p"];
                double price = std::stod(string(price_str_view));
                string_view quantity_str_view = data_object["q"];
                double quantity = std::stod(string(quantity_str_view));
                
                strategy->on_tick(symbol_str, price, quantity);
            }
        }
    }
}