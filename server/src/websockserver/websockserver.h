#ifndef WEBSOCKSERVER_H
#define WEBSOCKSERVER_H

#include "boostwslistener.h"
#include <iostream>
#include <chrono>
#include <thread>
#include <future>
#include <queue>

class WebSockServer
{
public:
    WebSockServer();
    ~WebSockServer();

    void startSocket(string host, string port); // Запуск сокета

private:
    string host_;
    string port_;
    future<void> q;

    void asyncStart();
};

#endif