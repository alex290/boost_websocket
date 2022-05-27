#include "boostwebsock.h"

// Resolver and socket require an io_context
BoostWebsock::BoostWebsock(net::io_context &ioc, ssl::context &ctx) : resolver_(net::make_strand(ioc)), ws_(net::make_strand(ioc), ctx)
{
}

BoostWebsock::~BoostWebsock()
{
}

// Start the asynchronous operation
void BoostWebsock::run(char const *host, char const *port, char const *text)
{
    // Save these for later
    host_ = host;
    text_ = text;
    closed = false;

    // Look up the domain name
    resolver_.async_resolve(host, port, beast::bind_front_handler(&BoostWebsock::on_resolve, shared_from_this()));
}

void BoostWebsock::runTwoo()
{
    ws_.async_read(buffer_, beast::bind_front_handler(&BoostWebsock::on_read, shared_from_this()));
}

// Закрытие сокета
void BoostWebsock::closeSock()
{
    closed = true;
    // Close the WebSocket connection
    ws_.async_close(websocket::close_code::normal, beast::bind_front_handler(&BoostWebsock::on_close, shared_from_this()));
}

// Report a failure
void BoostWebsock::fail(beast::error_code ec, char const *what)
{
    signalError(string(what) + string(":") + string(ec.message()));
}

void BoostWebsock::on_resolve(beast::error_code ec, tcp::resolver::results_type results)
{
    if (ec)
        return fail(ec, "resolve");

    // Set a timeout on the operation
    beast::get_lowest_layer(ws_).expires_after(chrono::seconds(30));

    // Make the connection on the IP address we get from a lookup
    beast::get_lowest_layer(ws_).async_connect(results, beast::bind_front_handler(&BoostWebsock::on_connect, shared_from_this()));
}

void BoostWebsock::on_connect(beast::error_code ec, tcp::resolver::results_type::endpoint_type ep)
{
    if (ec)
        return fail(ec, "connect");

    // Set a timeout on the operation
    beast::get_lowest_layer(ws_).expires_after(chrono::seconds(30));

    // Set SNI Hostname (many hosts need this to handshake successfully)
    if (!SSL_set_tlsext_host_name(ws_.next_layer().native_handle(), host_.c_str()))
    {
        ec = beast::error_code(static_cast<int>(::ERR_get_error()), net::error::get_ssl_category());
        return fail(ec, "connect");
    }

    // Update the host_ string. This will provide the value of the
    // Host HTTP header during the WebSocket handshake.
    // See https://tools.ietf.org/html/rfc7230#section-5.4
    host_ += ':' + to_string(ep.port());

    // Perform the SSL handshake
    ws_.next_layer().async_handshake(ssl::stream_base::client, beast::bind_front_handler(&BoostWebsock::on_ssl_handshake, shared_from_this()));
}
void BoostWebsock::on_ssl_handshake(beast::error_code ec)
{
    if (ec)
        return fail(ec, "ssl_handshake");

    // Turn off the timeout on the tcp_stream, because
    // the websocket stream has its own timeout system.
    beast::get_lowest_layer(ws_).expires_never();

    // Set suggested timeout settings for the websocket
    ws_.set_option(websocket::stream_base::timeout::suggested(beast::role_type::client));

    // Set a decorator to change the User-Agent of the handshake
    ws_.set_option(websocket::stream_base::decorator(
        [](websocket::request_type &req)
        {
            req.set(http::field::user_agent, string(BOOST_BEAST_VERSION_STRING) + " websocket-client-async-ssl");
        }));

    // Perform the websocket handshake
    ws_.async_handshake(host_, text_, beast::bind_front_handler(&BoostWebsock::on_handshake, shared_from_this()));
}

void BoostWebsock::on_handshake(beast::error_code ec)
{
    if (ec)
        return fail(ec, "handshake");

    // Send the message
    // ws_.async_write(net::buffer(text_), beast::bind_front_handler(&BoostWebsock::on_write, shared_from_this()));

    // Read a message into our buffer
    ws_.async_read(buffer_, beast::bind_front_handler(&BoostWebsock::on_read, shared_from_this()));
}

void BoostWebsock::on_write(beast::error_code ec, size_t bytes_transferred)
{
    boost::ignore_unused(bytes_transferred);

    if (ec)
        return fail(ec, "write");

    // Read a message into our buffer
    ws_.async_read(buffer_, beast::bind_front_handler(&BoostWebsock::on_read, shared_from_this()));
}

void BoostWebsock::on_read(beast::error_code ec, size_t bytes_transferred)
{
    boost::ignore_unused(bytes_transferred);
    if (!closed)
    {

        if (ec)
            return fail(ec, "read");

        string d = beast::buffers_to_string(buffer_.data());
        signalData(d);
        buffer_.clear();

        // Close the WebSocket connection
        // ws_.async_close(websocket::close_code::normal, beast::bind_front_handler(&BoostWebsock::on_close, shared_from_this()));

        // Read a message into our buffer
        ws_.async_read(buffer_, beast::bind_front_handler(&BoostWebsock::on_read, shared_from_this()));
    }
}

void BoostWebsock::on_close(beast::error_code ec)
{
    signalClose();
    if (ec)
        return fail(ec, "close");
}
