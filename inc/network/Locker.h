//
// Created by 神奇bug在哪里 on 8/3/23.
//

#ifndef SIMPLELABMANAGER_LOCKER_H
#define SIMPLELABMANAGER_LOCKER_H
#include "tcpsocket.hpp"

class Locker {
private:
    typedef struct  {
        uint16_t size;
        uint8_t type;
    }headerData;
    TCPSocket<4096> socket;
    std::mutex globalLockerMutex;
    std::string buffer;
    std::mutex globalBufferMutex;
    bool recvData = false;
    static void buildHeader(std::string &data, uint16_t size, uint8_t type);
    static std::string genToken();
    static time_t restoreToken(const std::string &token);
    static void appendStr(std::string &message, const void * src, std::size_t size);
    static headerData processHeader(const std::string& header, uint16_t &size, uint8_t &type);
    static headerData processHeader(const std::string& header);
    static bool rejecter(const std::string &message);
    uint8_t status = 0;
public:
    void setup();
    void lock(std::string userName);
    bool unlock(std::string userName);
    bool isLocked(std::string userName);
    void onRecvice(std::string data);
};


#endif //SIMPLELABMANAGER_LOCKER_H
