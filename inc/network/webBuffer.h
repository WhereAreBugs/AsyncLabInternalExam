//
// Created by 神奇bug在哪里 on 7/22/23.
//

#ifndef SIMPLELABMANAGER_WEBBUFFER_H
#define SIMPLELABMANAGER_WEBBUFFER_H

#include "tcpserver.hpp"

class webBuffer {
private:
    std::string currentBuffer;
    std::string token;
    inline void removeRubbishFromBuffer();
public:
    void addBuffer(const std::string message);
    void clearBuffer();
    bool isComplete();
    void generateToken();
    std::string getToken();
    std::string getMessage();
};


#endif //SIMPLELABMANAGER_WEBBUFFER_H
