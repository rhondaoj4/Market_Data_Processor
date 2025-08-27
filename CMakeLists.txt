cmake_minimum_required(VERSION 3.18)

project(Market_Data_Processor LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Boost CONFIG REQUIRED COMPONENTS beast)
find_package(simdjson CONFIG REQUIRED)
find_package(OpenSSL REQUIRED)

add_executable(Market_Data_Processor main.cpp)

# Link executable against required libraries
target_link_libraries(Market_Data_Processor PRIVATE
    Boost::beast
    simdjson::simdjson
    OpenSSL::SSL      
    OpenSSL::Crypto    
)