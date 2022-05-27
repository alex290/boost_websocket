#ifndef MAINFRAME_H
#define MAINFRAME_H

// #include "websock/websocket.h"
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <vector>
#include <algorithm>
#include "websockserver/websockserver.h"

class MainFrame : public wxFrame
{
public:
    MainFrame();
    ~MainFrame();

private:
    WebSockServer *webSockServer;
};

#endif