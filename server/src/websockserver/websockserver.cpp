#include "websockserver.h"

WebSockServer::WebSockServer()
{
}

WebSockServer::~WebSockServer()
{
}
void WebSockServer::startSocket(string host, string port)
{
    host_ = host;
    port_ = port;

    q = std::async(std::launch::async, [this]
                   { this->asyncStart(); });
}

void WebSockServer::asyncStart()
{
    auto const address = net::ip::make_address(host_.c_str());
    auto const port = static_cast<unsigned short>(std::atoi(port_.c_str()));
    auto const threads = std::max<int>(1, 1);

    // The io_context is required for all I/O
    net::io_context ioc{threads};

    // The SSL context is required, and holds certificates
    ssl::context ctx{ssl::context::sslv23};
    ctx.set_verify_mode(ssl::verify_none);

    // Create and launch a listening port
    std::make_shared<BoostWSListener>(ioc, ctx, tcp::endpoint{address, port})->run();

    // Run the I/O service on the requested number of threads
    std::vector<std::thread> v;
    v.reserve(threads - 1);
    for (auto i = threads - 1; i > 0; --i)
        v.emplace_back([&ioc]
                       { ioc.run(); });
    ioc.run();
}
