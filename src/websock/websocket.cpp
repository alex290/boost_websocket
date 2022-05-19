#include "websocket.h"

WebSocket::WebSocket()
{
    closed = false;
}

WebSocket::~WebSocket()
{
    ioc.stop();
    clearSocket();
}
void WebSocket::startSocket(string host, string port, string text)
{
    host_ = host;
    port_ = port;
    text_ = text;

    q.push(std::async(std::launch::async, [this]
                      { this->asyncStart(); }));
}

void WebSocket::localData(string event)
{
    signalData(event);
}

void WebSocket::localClose()
{
    signalClose();
    clearSocket();
}

void WebSocket::localError(string event)
{
    signalError(event);
}

void WebSocket::asyncStart()
{
    // The SSL context is required, and holds certificates
    ssl::context ctx{ssl::context::sslv23};
    ctx.set_verify_mode(ssl::verify_none);

    // Launch the asynchronous operation
    std::shared_ptr link = make_shared<BoostWebsock>(ioc, ctx);

    // Подключаем сигнал к фукции
    link->signalData = [&, this](string dat)
    { this->localData(dat); };
    // Подключаем сигнал к фукции
    link->signalClose = [this]()
    { this->localClose(); };
    // Подключаем сигнал к фукции
    link->signalError = [&, this](string dat)
    { this->localError(dat); };

    link->run(host_.c_str(), port_.c_str(), text_.c_str());

    // Run the I/O service. The call will return when
    // the socket is closed.
    bool first = true;
    bool exit = true;

    // ioc.run();

    while (exit)
    {
        if (!first)
            ioc.restart();

        if (closed)
        {
            link->closeSock();
            ioc.run();
            exit = false;
        }
        else if (first)
        {
            first = false;
            ioc.run();
        }
        else
        {
            link->runTwoo();
            ioc.run();
        }
    }
}

// Закрытие сокета
void WebSocket::closeSocket()
{
    closed = true;
    work.reset();
    ioc.stop();
}

void WebSocket::clearSocket()
{
    while (!q.empty())
        q.pop();
}