//
// Created by 神奇bug在哪里 on 8/3/23.
//

#ifndef SIMPLELABMANAGER_IDENTIFYBUFFER_H
#define SIMPLELABMANAGER_IDENTIFYBUFFER_H

#include "Buffer.h"
class IdentifyBuffer : public Buffer{
private:
    bool headerIsComplete = false;
    uint16_t bodySize = 0;
public:
    bool isComplete() override;
    void generateToken();
    static bool isTokenValid(const std::string token);
    bool isTokenValid(const uint8_t * token);
    std::string getToken();

};


#endif //SIMPLELABMANAGER_IDENTIFYBUFFER_H
