#include "boostwssession.h"

BoostWSSession::BoostWSSession(tcp::socket &&socket, ssl::context &ctx) : ws_(std::move(socket), ctx)
{
}

// Найдите правильного исполнителя
void BoostWSSession::run()
{
    // Мы должны выполняться внутри цепочки, чтобы выполнять асинхронные операции
    // с объектами ввода-вывода в этом сеансе. Хотя это и не является строго необходимым
    // для однопоточных контекстов, этот пример кода по умолчанию написан как потокобезопасный.
    net::dispatch(ws_.get_executor(), beast::bind_front_handler(&BoostWSSession::on_run, shared_from_this()));
}

// Запустите асинхронную операцию
void BoostWSSession::on_run()
{
    // Установить timeout.
    beast::get_lowest_layer(ws_).expires_after(std::chrono::seconds(30));

    // Выполните установление связи SSL
    ws_.next_layer().async_handshake(ssl::stream_base::server, beast::bind_front_handler(&BoostWSSession::on_handshake, shared_from_this()));
}

void BoostWSSession::on_handshake(beast::error_code ec)
{
    if (ec)
        return fail(ec, "handshake");

    // Отключите тайм-аут для tcp_stream, поскольку
    // поток websocket имеет свою собственную систему тайм-аута.
    beast::get_lowest_layer(ws_).expires_never();

    // Установите рекомендуемые параметры тайм-аута для веб-сокета
    ws_.set_option(websocket::stream_base::timeout::suggested(beast::role_type::server));

    // Установите декоратор для изменения сервера установление связи
    ws_.set_option(websocket::stream_base::decorator([](websocket::response_type &res)
                                                     { res.set(http::field::server, std::string(BOOST_BEAST_VERSION_STRING) + " websocket-server-async-ssl"); }));

    // Примите установление связи websocket
    ws_.async_accept(beast::bind_front_handler(&BoostWSSession::on_accept, shared_from_this()));
}

void BoostWSSession::on_accept(beast::error_code ec)
{
    if (ec)
        return fail(ec, "accept");

    // Прочитать сообщение
    do_read();
}

void BoostWSSession::do_read()
{
    // Считываем сообщение в наш буфер
    ws_.async_read(buffer_, beast::bind_front_handler(&BoostWSSession::on_read, shared_from_this()));
}

void BoostWSSession::on_read(beast::error_code ec, std::size_t bytes_transferred)
{
    boost::ignore_unused(bytes_transferred);

    // Это указывает на то, что сессия была закрыта
    if (ec == websocket::error::closed)
        return;

    if (ec)
        fail(ec, "read");

    // Повторите сообщение
    ws_.text(ws_.got_text());
    ws_.async_write(buffer_.data(), beast::bind_front_handler(&BoostWSSession::on_write, shared_from_this()));
}

void BoostWSSession::on_write(beast::error_code ec, std::size_t bytes_transferred)
{
    boost::ignore_unused(bytes_transferred);

    if (ec)
        return fail(ec, "write");

    // Очистите буфер
    buffer_.consume(buffer_.size());

    // Сделайте еще одно чтение
    do_read();
}

void BoostWSSession::fail(beast::error_code ec, string what)
{
    cout << what << ": " << ec.message() << "\n";
}
