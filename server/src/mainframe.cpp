#include "mainframe.h"

MainFrame::MainFrame() : wxFrame(NULL, wxID_ANY, "Websocket Server")
{
    webSockServer = new WebSockServer;
    webSockServer->startSocket("0.0.0.0", "8080");
}

MainFrame::~MainFrame()
{
}
