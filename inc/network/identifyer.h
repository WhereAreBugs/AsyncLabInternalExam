//
// Created by 神奇bug在哪里 on 8/3/23.
//

#ifndef SIMPLELABMANAGER_IDENTIFYER_H
#define SIMPLELABMANAGER_IDENTIFYER_H
#include "tcpserver.hpp"
#include <map>
class identifyer {
    //理想状态下不存在并发的情况，但是以防万一，还是加上。
private:
    TCPServer<4096> server;
    std::mutex globalBufferMutex;
    std::map<TCPSocket<> * , std::string> buffers;
    std::mutex globalTokensMutex;
};


#endif //SIMPLELABMANAGER_IDENTIFYER_H
