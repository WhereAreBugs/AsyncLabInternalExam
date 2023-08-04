//
// Created by 神奇bug在哪里 on 7/22/23.
//

#include <spdlog/spdlog.h>
#include "webBuffer.h"


std::string webBuffer::getToken() {
    return token;
}

void webBuffer::generateToken() {
    //获取当前时间
    auto now = std::chrono::system_clock::now();
    auto now_c = std::chrono::system_clock::to_time_t(now);
    //获取当前时间的hash值
    std::hash<std::string> hash_fn;
    token = std::to_string(hash_fn(std::ctime(&now_c)));
    if (token.size()!=64)
    {
        spdlog::error("token size error");
    }
}

bool webBuffer::isComplete() {
    if (currentBuffer.size()<64)
    {
        return false;
    }

}


