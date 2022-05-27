#include "boostwslistener.h"

BoostWSListener::BoostWSListener(net::io_context &ioc, ssl::context &ctx, tcp::endpoint endpoint) : ioc_(ioc), ctx_(ctx), acceptor_(net::make_strand(ioc))
{
    beast::error_code ec;

    // Откройте приемник
    acceptor_.open(endpoint.protocol(), ec);
    if (ec)
    {
        fail(ec, "open");
        return;
    }

    // Разрешить повторное использование адреса
    acceptor_.set_option(net::socket_base::reuse_address(true), ec);
    if (ec)
    {
        fail(ec, "set_option");
        return;
    }

    // Привязка к адресу сервера
    acceptor_.bind(endpoint, ec);
    if (ec)
    {
        fail(ec, "bind");
        return;
    }

    // Начните прослушивать соединения
    acceptor_.listen(net::socket_base::max_listen_connections, ec);
    if (ec)
    {
        fail(ec, "listen");
        return;
    }
}

// Начните принимать входящие соединения
void BoostWSListener::run()
{
    do_accept();
}

void BoostWSListener::do_accept()
{
    // Новое соединение получает свою собственную цепочку
    acceptor_.async_accept(net::make_strand(ioc_), beast::bind_front_handler(&BoostWSListener::on_accept, shared_from_this()));
}

void BoostWSListener::on_accept(beast::error_code ec, tcp::socket socket)
{
    if (ec)
    {
        fail(ec, "accept");
    }
    else
    {
        // Создайте сеанс и запустите его
        cout << "New Seaseon" << endl;
        make_shared<BoostWSSession>(move(socket), ctx_)->run();
    }

    // Принять другое соединение
    do_accept();
}

void BoostWSListener::fail(beast::error_code ec, string what)
{
    cout << what << ": " << ec.message() << "\n";
}
