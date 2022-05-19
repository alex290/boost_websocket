#ifndef BOOSTWEBSOCK_H
#define BOOSTWEBSOCK_H

#include <boost/beast/core.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/ssl.hpp>
#include <boost/asio/strand.hpp>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <string>

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
namespace ssl = boost::asio::ssl;       // from <boost/asio/ssl.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>
using namespace std;

class BoostWebsock : public std::enable_shared_from_this<BoostWebsock>
{
    tcp::resolver resolver_;
    websocket::stream<beast::ssl_stream<beast::tcp_stream>> ws_;

public:
    explicit BoostWebsock(net::io_context &ioc, ssl::context &ctx);
    ~BoostWebsock();
    function<void(string)> signalData;
    function<void()> signalClose;
    function<void(string)> signalError;

    void run(char const *host, char const *port, char const *text); // Start the asynchronous operation
    void runTwoo();
    void closeSock(); // Закрытие сокета

private:
    beast::flat_buffer buffer_;
    string host_;
    string text_;

    // Report a failure
    void fail(beast::error_code ec, char const *what);

    void on_resolve(beast::error_code ec, tcp::resolver::results_type results);
    void on_connect(beast::error_code ec, tcp::resolver::results_type::endpoint_type ep);
    void on_ssl_handshake(beast::error_code ec);
    void on_handshake(beast::error_code ec);
    void on_write(beast::error_code ec, size_t bytes_transferred);
    void on_read(beast::error_code ec, size_t bytes_transferred);
    void on_close(beast::error_code ec);
};

#endif