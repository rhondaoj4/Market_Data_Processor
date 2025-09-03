#pragma once
#include <string>
#include "simdjson.h"

using namespace std;

//abstract class
class IStrategy{

    //ensures correct destructor from a derived class is called
    virtual ~IStrategy() = default;

    //force other classes to implement on_market_data by itself
    virtual void on_market_data(const simdjson::dom::element &doc) = 0;

    //force other classes to implement on_tick by itself
    virtual void on_tick(const std::string &symbol, double price, double quantity) = 0;
};