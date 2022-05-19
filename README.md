# wxWidgets Boost Websocket

Создаем WebSocket

	websocket = new WebSocket();

Подключаем события

    // Подключаем сигнал к фукции - Чтение данных
    websocket->signalData = [&, this](string dat)
    { this->readData(dat); };
	
    // Подключаем сигнал к фукции - Закрыте сокета
    websocket->signalClose = [this]()
    { this->eventClose(); };
	
    // Подключаем сигнал к фукции - Получение ошибки
    websocket->signalError = [&, this](string dat)
    { this->eventError(dat); };

Запускаем сокет (Адрес, Порт, Данные запроса)

    websocket->startSocket("stream.binance.com", "9443", "/ws/btcusdt@kline_5m");

Создаем фукции события

    // Получаем данные
    void MainFrame::readData(string data)
	{
    	cout << data << endl;
	}
    
    // Закрытие
    void MainFrame::eventClose()
    {
        std::cout << "CLOSE EVENT" << std::endl;
    }
    
    // получаем ошибки
    void MainFrame::eventError(string data)
    {
        std::cout << "Error EVENT " << data << std::endl;
    }

Вебсокет работает в асинхронном режиме. На каждый сокет отдельный объект.. Если использовать несколько сокетов.. Запускать через вектор объектов