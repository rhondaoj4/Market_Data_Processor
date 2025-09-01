# Market Data Processor

This project is a high-performance C++ application designed for real-time processing of market data.
It uses Boost.Beast for WebSocket communication to consume live trade data from the Binance US API

## Current Features

    * Secure WebSocket Connection: Establishes a secure connection using SSL to consume real-time market data.

    * Multi-Stream Subscription: Connects to and reads multiple data streams simultaneously. The current streams include trade data for ETH/USDT, SOL/USDT, DOGE/USDT, AVAX/USDT, LTC/USDT, and ARB/USDT.

    * Asynchronous I/O: Leverages Boost.Asio to handle network operations without blocking, ensuring efficient and responsive data handling.

    * Ping-Pong Management: Automatically responds to server ping frames with pong frames to maintain a stable, persistent connection.

    * Error Handling: Includes robust error handling and reporting for network and SSL-related failures.

## Planned Features

    * High-Speed Data Parsing: Use simdjson to parse incoming JSON data with optimized, SIMD-accelerated methods. Allows for low-latency data processing.

    * Back-Testing Engine: Implement a back-testing framework that can replay historical data. This will allow for the validation and evaluation of strategies in a simulated environment using real-time data.

    * Modular Strategy Integration: Design a flexible architecture that allows for the easy integration of different trading strategies (Moving Average Crossover, Triangular Arbitrage, Momentum Following, Mean Reversion). 

    * Order Management System (OMS) Simulation: Develop a lightweight, internal OMS to simulate order placement, execution, and position management. Will help test the logic of trading strategies.

    * Performance Optimization: Profile the application to identify bottlenecks and apply low-level optimizations, such as minimizing dynamic memory allocations and leveraging lock-free data structures for concurrent processing.

    * Graphical User Interface (GUI): Create a real-time GUI using a library like ImGui. The GUI will provide a live view of incoming data, active positions, and strategy performance metrics.

## Technologies

    C++20: Modern C++ standards -> clean and efficient code.

    Boost.Beast: asynchronous and low-latency WebSocket communication.

    OpenSSL: Provides the necessary functions for secure, encrypted connections.

    simdjson: (Planned) A high-performance JSON parsing library that uses SIMD instructions to parse gigabytes of JSON per second. 

    Boost.Asio: The foundational library for asynchronous network operations.

    ImGui: (Planned) For a lightweight and real-time graphical user interface.
