//
// Created by 神奇bug在哪里 on 8/3/23.
//

#include "IdentifyBuffer.h"

bool IdentifyBuffer::isComplete() {
    if (!headerIsComplete && currentBuffer.size()>=21)
    {
        headerIsComplete = true;
        bodySize = *reinterpret_cast<const uint16_t *>(currentBuffer.c_str()+2);
    }
    if (headerIsComplete && currentBuffer.size()>=bodySize+21)
    {
        return true;
    }
    return false;
}

void IdentifyBuffer::generateToken() {
    uint8_t token[16];
    uint32_t time = (uint32_t)std::time(nullptr);
    for (int i = 0; i < 16; i++) {
        token[i] = (uint8_t)(time >> (i * 2));
    }
    this->token = std::string(reinterpret_cast<const char *>(token), 16);

}

std::string IdentifyBuffer::getToken() {
    return token;
}

bool IdentifyBuffer::isTokenValid(const std::string token) {
    if (token.size()!=16)
    {
        return false;
    }
    auto time = (uint32_t)std::time(nullptr);
    uint32_t tokenTime = 0;
    for (int i = 0; i < 16; i++) {
        tokenTime |= (uint32_t)token[i] << (i * 2);
    }
    if (time - tokenTime > 60)
    {
        return false;
    }
    return true;
}
/**
 * @brief 判断token是否有效
 * @attention 该函数不会检查token的长度，因此需要保证传入token的长度是为16
 */
bool IdentifyBuffer::isTokenValid(const uint8_t *token) {
    auto time = (uint32_t)std::time(nullptr);
    uint32_t tokenTime = 0;
    for (int i = 0; i < 16; i++) {
        tokenTime |= (uint32_t)token[i] << (i * 2);
    }
    if (time - tokenTime > 60)
    {
        return false;
    }
    return true;
}
