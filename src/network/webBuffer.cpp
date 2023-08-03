//
// Created by 神奇bug在哪里 on 7/22/23.
//

#include <spdlog/spdlog.h>
#include "webBuffer.h"

bool webBuffer::isComplete() {
    //TODO: check if the buffer is complete
    return false;
}

std::string webBuffer::getMessage() {
    removeRubbishFromBuffer();
    return currentBuffer;
}

void webBuffer::removeRubbishFromBuffer() {
    //TODO: 检查当前缓冲区是否存在非预期的数据，例如非法字符，包头size不匹配等

}

void webBuffer::clearBuffer() {
    currentBuffer.clear();
}



std::string webBuffer::getToken() {
    return token;
}

void webBuffer::addBuffer(const std::string message) {
    currentBuffer+=message;
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


