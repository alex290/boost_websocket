#ifndef WEBSOCKET_H
#define WEBSOCKET_H

#include "boostwebsock.h"
#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include <future>
#include <queue>

class WebSocket
{
public:
    WebSocket();
    ~WebSocket();
    function<void(string)> signalData;
    function<void()> signalClose;
    function<void(string)> signalError;

    void startSocket(string host, string port, string text); // Запуск сокета

    void closeSocket(); // Закрытие сокета

private:
    string host_;
    string port_;
    string text_;
    std::queue<std::future<void>> q;
    bool closed;

    net::io_context ioc;
    net::executor_work_guard<decltype(ioc.get_executor())> work{ioc.get_executor()};

    void asyncStart();
    void clearSocket();

    void localData(string event);  // Отправка сигнала
    void localClose();             // Закрытие сокета
    void localError(string event); // Ошибка сокета
};

#endif