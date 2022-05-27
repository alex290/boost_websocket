#ifndef BOOSTWSLISTENER_H
#define BOOSTWSLISTENER_H

#include "boostwssession.h"

// Принимает входящие подключения и запускает сеансы
class BoostWSListener : public std::enable_shared_from_this<BoostWSListener>
{
public:
    BoostWSListener(net::io_context &ioc, ssl::context &ctx, tcp::endpoint endpoint);

    void run(); // Начните принимать входящие соединения

private:
    net::io_context &ioc_;
    ssl::context &ctx_;
    tcp::acceptor acceptor_;

    void do_accept();
    void on_accept(beast::error_code ec, tcp::socket socket);

    // Сообщить о сбое
    void fail(beast::error_code ec, string what);
};

#endif