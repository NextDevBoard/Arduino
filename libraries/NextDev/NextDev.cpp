#include <NextDev.h>

SpiUartDevice SpiSerial;

NextDev::NextDev() {
}


void NextDev::begin() {
    SpiSerial.begin();
    _ticker.attach_ms(TIMER_INTERVAL, eventLoop, this);
    send("bkcmd=3");   // Configure NextDev to allways return a command response
    sendCommand("");   // send a NOOP
}

Task* NextDev::sendCmdAndAwaitResponse(String cmd) {
    Task* task = new Task(cmd);
    _cmdQueue.push(task);
    int countDown = _timeout;
    while(!task->isDone() && countDown-- >= 0) {
        delay(1);  // yield back to ESP8266 stack while waiting
    }
    //if (countDown < 0)
    //    Serial.println("Task timed out.");
    return task;
}


int NextDev::getPage() {
    Task* task = sendCmdAndAwaitResponse("sendme");
    int ret = -1;
    if (task->isDone())
        ret = (int) task->getIntResult();
    delete task;
    return ret;
}


int NextDev::getIntVariable(String varName) {
    Task* task = sendCmdAndAwaitResponse("get " + varName);
    int ret = -1;
    if (task->isDone())
        ret = (int) task->getIntResult();
    delete task;
    return ret;
}

String NextDev::getStringVariable(String varName) {
    Task* task = sendCmdAndAwaitResponse("get " + varName);
    String ret = "";
    if (task->isDone())
        ret = task->getStringResult();
    delete task;
    return ret;
}



void NextDev::sendCommand(String cmd) {
    Task* task = sendCmdAndAwaitResponse(cmd);
    delete task;
}
 

NextDev& NextDev::onTouchEvent(TOUCHEVENT_CALLBACK_SIGNATURE) {
    this->touch_callback = touch_callback; 
    return *this;
}


void NextDev::send(String cmd) {
    SpiSerial.print(cmd);
    SpiSerial.write(0xFF);
    SpiSerial.write(0xFF);
    SpiSerial.write(0xFF);
}


bool NextDev::available() {
    return !_eventQueue.isEmpty();
}


Event NextDev::getEvent() {
   if (available()) {
       Event* p = _eventQueue.pop();
       Event e(p);
       delete p;
       return e;
   }
}


void NextDev::eventLoop(NextDev* n) {

    byte rxBuf[BUF_SIZE]; 
    int rxBufLen = 0;
    int escCnt;

    escCnt = 0;
    while (SpiSerial.available()) {
        byte b = SpiSerial.read();
        if (b == 0xFF) {
            escCnt++;
            if (escCnt == 3) {
                escCnt = 0;
                doRcv(n, rxBuf, rxBufLen);
                rxBufLen = 0;
            }
        }
        else {
            *(rxBuf + rxBufLen) = b;
            rxBufLen++;
        }
      
    }

    if (!n->_cmdQueue.isEmpty()) {
        Task* currTask = n->_cmdQueue.peek();
        if (!currTask->isSent()) {
            currTask->setSent(true);
            n->send(currTask->getCmd());
        }
    }
}


void NextDev::doRcv(NextDev* n, byte* buf, size_t bufLen) {

    byte op = buf[0];
    Task* c;
    
    if (bufLen == 1 && op == 0x01) {  // OK response
        if (!n->_cmdQueue.isEmpty()) {
            c = n->_cmdQueue.pop();
            c->setDone(true);
        }
        //Serial.println("GOT OK. ");
    }
    else if (bufLen == 1 && op >= 0x00 && op <= 0x1B) { // ERROR response
        if (!n->_cmdQueue.isEmpty()) {
            c = n->_cmdQueue.pop();
            c->setDone(true);
        }
        Serial.print("GOT error code ");
        Serial.println(op, HEX);
    } 
    else if (bufLen == 4 && op == 0x65) {
        if (n->touch_callback != NULL)
            n->touch_callback(buf[1], buf[2], buf[3]);
        else 
            n->_eventQueue.push(new Event(buf[1], buf[2], buf[3]));
    }
    else if (op == 0x70) {    // return string value
        if (!n->_cmdQueue.isEmpty()) {
            c = n->_cmdQueue.pop();
            buf[bufLen] = 0x00;  // terminate string
            c->setStringResult((const char*) (buf + 1));
            c->setDone(true);
        }
    }
    else if (bufLen == 2 && op == 0x66) {    // return page id
        if (!n->_cmdQueue.isEmpty()) {
            c = n->_cmdQueue.pop();
            c->setIntResult((uint32_t) buf[1]);
            c->setDone(true);
        }
    }
    else if (bufLen == 5 && op == 0x71) {    // return int value
        if (!n->_cmdQueue.isEmpty()) {
            c = n->_cmdQueue.pop();
            uint32_t ret = (uint32_t) buf[4] << 24;
            ret += (uint32_t) buf[3] << 16;
            ret += (uint32_t) buf[2] << 8;
            ret += (uint32_t) buf[1];
            c->setIntResult(ret);
            c->setDone(true);
        }
    }
    else {
        Serial.print("Command not handled -> ");
        Serial.println(op, HEX);
    }
            
}


