//
// Created by 神奇bug在哪里 on 7/22/23.
//

#ifndef SIMPLELABMANAGER_WEBBUFFER_H
#define SIMPLELABMANAGER_WEBBUFFER_H

#include "tcpserver.hpp"

class webBuffer {
private:
    std::string currentBuffer;
    std::string token = 0;
    inline void removeRubbishFromBuffer();
public:
#pragma clang diagnostic push
#pragma ide diagnostic ignored "readability-avoid-const-params-in-decls"
    void addBuffer(const std::string message);
#pragma clang diagnostic pop
    void clearBuffer();
    bool isComplete();
    void generateToken();
    std::string getToken();
    std::string getMessage();
};


#endif //SIMPLELABMANAGER_WEBBUFFER_H
