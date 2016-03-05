#ifndef NextDev_h
#define NextDev_h

#include <Arduino.h>
#include <SpiUart.h>
#include <QueueList.h>
#include <Ticker.h>
#include <stdint.h>

#ifdef ESP8266
#include <functional>
#define TOUCHEVENT_CALLBACK_SIGNATURE std::function<void(uint8_t, uint8_t, uint8_t)> touch_callback // pageId, componentId, (0x01 = pressed, 0x00 = released)
#else
#define TOUCHEVENT_CALLBACK_SIGNATURE void (*callback)(uint8_t, uint8_t, uint8_t)
#endif

#define BUF_SIZE 255
#define TIMER_INTERVAL 50
#define DEFAULT_TIMEOUT 100

typedef uint32_t UNINT32;

class Task {
  public:
    Task(String cmd) { 
        _cmd = cmd;
        _sent = false;
        _done = false;
    }

    void setSent(bool flag) {
        _sent = flag;
    }

    bool isSent() {
        return _sent;
    }

    void setDone(bool flag) {
        _done = flag;
    }

    bool isDone() {
        return _done;
    }

    String getStringResult() {
        return _stringResult;
    }

    void setStringResult(String stringResult) {
        _stringResult = stringResult;
    }

    uint32_t getIntResult() {
        return _intResult;
    }

    void setIntResult(uint32_t intResult) {
        _intResult = intResult;
    }

    String getCmd() {
        return _cmd;
    }

  private:
    String _cmd;
    bool _sent;
    bool _done;
    String _stringResult;
    uint32_t _intResult;
};


class Event {
  public:
    Event(byte page, byte component, byte touch) { 
        _page = page;
        _component = component;
        _touch = touch;
    }

    Event(const Event &obj) {
       _page = obj._page;
       _component = obj._component;
       _touch = obj._touch;
    }

    Event(Event* obj) {
       _page = obj->_page;
       _component = obj->_component;
       _touch = obj->_touch;
    }

    byte getPage() { return _page; }
    byte getComponent() { return _component; }
    byte getTouch() { return _touch; }

  private:
    byte _page;
    byte _component;
    byte _touch;
};


class NextDev {

  public:
    //NextDev(int pin);
    NextDev();
    void begin();
    void sendCommand(String cmd);
    int getPage();
    int getIntVariable(String varName);
    String getStringVariable(String varName);
    NextDev& onTouchEvent(TOUCHEVENT_CALLBACK_SIGNATURE);
    bool available();
    Event getEvent();
        
  private:
    int _timeout = DEFAULT_TIMEOUT;
    QueueList<Task*> _cmdQueue;
    TOUCHEVENT_CALLBACK_SIGNATURE;
    static void eventLoop(NextDev*);
    static void doRcv(NextDev*, byte* buf, size_t bufLen );
    void send(String);
    Task* sendCmdAndAwaitResponse(String);
    Ticker _ticker;
    QueueList<Event*> _eventQueue;
};


#endif
