#include "mainframe.h"

MainFrame::MainFrame() : wxFrame(NULL, wxID_ANY, "Websocket Client")
{
    websocket = new WebSocket();
    websocket->startSocket("127.0.0.1", "8080", "");

    // Подключаем сигнал к фукции
    websocket->signalData = [&, this](string dat)
    { this->readData(dat); };
    // Подключаем сигнал к фукции
    websocket->signalClose = [this]()
    { this->eventClose(); };
    // Подключаем сигнал к фукции
    websocket->signalError = [&, this](string dat)
    { this->eventError(dat); };
}

MainFrame::~MainFrame()
{
    websocket->closeSocket();
}
void MainFrame::readData(string data)
{
    cout << data << endl;
}

void MainFrame::eventClose()
{
    cout << "Close" << endl;
}

void MainFrame::eventError(string data)
{
    cout << data << endl;
}
