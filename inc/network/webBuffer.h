//
// Created by 神奇bug在哪里 on 7/22/23.
//

#ifndef SIMPLELABMANAGER_WEBBUFFER_H
#define SIMPLELABMANAGER_WEBBUFFER_H

#include "tcpserver.hpp"
#include "Buffer.h"

class webBuffer : public Buffer {
public:

    bool isComplete() override;
    void generateToken();
    std::string getToken();
};


#endif //SIMPLELABMANAGER_WEBBUFFER_H
