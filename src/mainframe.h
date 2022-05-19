#ifndef MAINFRAME_H
#define MAINFRAME_H

// #include "websock/websocket.h"
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <vector>
#include <algorithm>
#include "websock/websocket.h"

class MainFrame : public wxFrame
{
public:
    MainFrame();
    ~MainFrame();

private:
    WebSocket *websocket;

    void readData(string data);   // Получаем данные
    void eventClose();            // Закрытие
    void eventError(string data); // получаем ошибки
};

#endif