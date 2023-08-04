//
// Created by 神奇bug在哪里 on 8/3/23.
//

#ifndef SIMPLELABMANAGER_IDENTIFYER_H
#define SIMPLELABMANAGER_IDENTIFYER_H
#include "tcpserver.hpp"
#include <map>
#include "IdentifyBuffer.h"
class identifyer {
    //理想状态下不存在并发的情况，但是以防万一，还是加上。
private:
    TCPServer<4096> server;
    std::mutex globalBufferMutex;
    std::map<TCPSocket<> * , IdentifyBuffer> buffers;
    std::mutex globalTokensMutex;
    bool rejecter(const std::string &message, TCPSocket<> *newClinet);
    void buildHeader(std::string &message, uint16_t size, uint8_t type, TCPSocket<> *newClinet);
public:
    void setup();
    void onMessage(std::string message, TCPSocket<> * socket);
public://业务函数
    void onPassLogin(TCPSocket<> *newClinet);
    void onFailedLogin(TCPSocket<> *newClinet, std::string message);
};


#endif //SIMPLELABMANAGER_IDENTIFYER_H
