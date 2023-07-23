//
// Created by 神奇bug在哪里 on 7/22/23.
//

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


