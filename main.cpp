#include <boost/beast/core.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/strand.hpp>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <simdjson.h>
#include "strategy_manager.h"
#include "moving_average_strategy.h"
// Namespace aliases
namespace beast = boost::beast;
namespace http = beast::http;
namespace websocket = beast::websocket;
namespace net = boost::asio;
namespace ssl = net::ssl;
using tcp = net::ip::tcp;

//------------------------------------------------------------------------------

// print out an error message if something fails
void fail(beast::error_code ec, const char *what) {
    std::cerr << "!!! " << what << " Error: " << ec.message() << "\n";
}

class session : public std::enable_shared_from_this<session>
{

private:
    // get ip address
    tcp::resolver resolver_;
    // creating a websocket connection
    websocket::stream<beast::ssl_stream<beast::tcp_stream>> ws_;
    // store read data
    beast::flat_buffer buffer_;
    std::string host_;
    std::string target_;
    StrategyManager strategy_manager_;

public:

    explicit session(net::io_context& ioc, ssl::context& ctx)
        // prevent race conditions
        : resolver_(net::make_strand(ioc))
        , ws_(net::make_strand(ioc), ctx)
    {}

    // start the whole process
    void run(char const* host, char const* port, char const* target) {
        host_ = host;
        target_ = target;
        resolver_.async_resolve(host, port, beast::bind_front_handler(&session::on_resolve, shared_from_this()));
    }

    StrategyManager &get_strategy_manager(){
        return strategy_manager_;
    }

private:
    // This is called after the hostname is resolved to an IP address
    void on_resolve(beast::error_code ec, tcp::resolver::results_type results) {
        if(ec) return fail(ec, "resolve");
        // get the underlying tcp layer and connect
        beast::get_lowest_layer(ws_).async_connect(results, beast::bind_front_handler(&session::on_connect, shared_from_this()));
    }

    // This is called after the TCP connection is established
    void on_connect(beast::error_code ec, tcp::resolver::results_type::endpoint_type) {
        if(ec) return fail(ec, "connect");
        
        // set the SNI hostname, this is required for SSL
        if(! SSL_set_tlsext_host_name(ws_.next_layer().native_handle(), host_.c_str()))
        {
            beast::error_code ec2{static_cast<int>(::ERR_get_error()), net::error::get_ssl_category()};
            std::cerr << ec2.message() << "\n";
            return;
        }

        // start the SSL handshake
        ws_.next_layer().async_handshake(ssl::stream_base::client, beast::bind_front_handler(&session::on_ssl_handshake, shared_from_this()));
    }

    // This is called after the SSL handshake is done
    void on_ssl_handshake(beast::error_code ec) {
        if(ec) return fail(ec, "ssl_handshake");
        // now start the websocket handshake
        ws_.async_handshake(host_, target_, beast::bind_front_handler(&session::on_handshake, shared_from_this()));
    }

    // This is called after the websocket handshake is done
    void on_handshake(beast::error_code ec) {
        if(ec) return fail(ec, "handshake");
        // sets a callback to handle ping/pong frames to keep connection alive
        ws_.control_callback(beast::bind_front_handler(&session::on_control_callback, shared_from_this()));
        // start reading from the websocket
        do_read();
    }

    // This is called to handle control frames like pings
    void on_control_callback(websocket::frame_type kind, beast::string_view payload) {
        boost::ignore_unused(payload);
        if(kind == websocket::frame_type::ping) {
            // we got a ping, so send a pong back
            ws_.async_pong({}, [self = shared_from_this()](beast::error_code ec){
                if(ec) return fail(ec, "pong");
            });
        }
    }

    // start an asynchronous read
    void do_read() {
        ws_.async_read(buffer_, beast::bind_front_handler(&session::on_read, shared_from_this()));
    }

    // This is called after a read operation is complete
    void on_read(beast::error_code ec, std::size_t bytes_transferred) {
        boost::ignore_unused(bytes_transferred);
        // check if the connection was closed by the server
        if(ec == websocket::error::closed) return;
        if(ec) return fail(ec, "read");

        string json_data = beast::buffers_to_string(buffer_.data());

        simdjson::dom::parser parser;
        try {
            simdjson::dom::element doc = parser.parse(json_data);

            // check for a trade stream by examining the "stream" field
            auto stream_result = doc["stream"];
            if (stream_result.is_string() && stream_result.get_string().value().ends_with("@trade")) {
                
                // safely access the "data" field
                auto data_field_result = doc["data"];
                
                if (data_field_result.is_object()) {
                    
                    auto event_type_result = data_field_result["e"].get_string();
                    
                    // check if the get_string() operation was successful, which also means the element is a string
                    if (event_type_result.error() == simdjson::SUCCESS && event_type_result.value() == "trade") {
                        
                        // cnversion to handle string values for price and quantity
                        string_view symbol_view = (string) data_field_result["s"];
                        string symbol_str(symbol_view);
                        string_view price_str_view = data_field_result["p"];
                        double price = std::stod(string(price_str_view));
                        string_view quantity_str_view = data_field_result["q"];
                        double quantity = std::stod(string(quantity_str_view));

                        // create a new JSON object to pass to strategy manager with converted data
                        simdjson::dom::object new_data_object;
                        
                        strategy_manager_.process_data(doc);
                    }
                }
            } else {
                // This handles non-trade messages gracefully
                cout << "Ignoring non-trade event: " << json_data << endl;
            }

        } catch (const simdjson::simdjson_error &e) {
            cerr << "simdjson Parse Error: " << e.what() << endl;
        }

        // consume the buffer so it's ready for the next read
        buffer_.consume(buffer_.size());
        // keep reading
        do_read();
    }
};

//------------------------------------------------------------------------------

int main() {
    // spent more time than I should've to figure i need to use .us not .com
    auto const host = "stream.binance.us";
    auto const port = "9443";
    auto const target = "/stream?streams=ethusdt@trade/solusdt@trade/dogeusdt@trade/avaxusdt@trade/ltcusdt@trade/arbusdt@trade";    

    // this is what manages all the network stuff
    net::io_context ioc;
    // this handles all the ssl stuff
    ssl::context ctx{ssl::context::tlsv12_client};
    ctx.set_default_verify_paths();
    ctx.set_verify_mode(ssl::verify_peer);

    // create sesion object using a shared_ptr
    auto my_session = std::make_shared<session>(ioc, ctx);
    // instance of a strategy (first to implement)
    auto moving_average_strategy = std::make_shared<MovingAverageStrategy>();
    //add strategy to strategy manager (TEST)
    my_session->get_strategy_manager().add_strategy("moving_average",moving_average_strategy);
    // start the session
    my_session->run(host,port, target);

    // run the io_context, this will block until all work is done
    ioc.run();

    return EXIT_SUCCESS;
}