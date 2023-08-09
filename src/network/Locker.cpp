//
// Created by 神奇bug在哪里 on 8/3/23.
//

#include "Locker.h"
#include "settings.h"
#include <utility>
#include <spdlog/spdlog.h>

void Locker::setup() {
    socket.onMessageReceived = [this](std::string data) {
        this->onRecvice(std::move(data));
    };
    socket.deleteAfterClosed = false;
    socket.onSocketClosed = [this](int a) {
        spdlog::error("[Locker] Socket closed! error code: {}", a);
        status = 2; // 2 means socket closed
        this->setup();
    };
    socket.Connect(LOCKER_ADDR, PORT_LOCKER);

}

void Locker::lock(std::string userName) {
    std::lock_guard<std::mutex> lock(globalLockerMutex);
    auto size = userName.size()+2;
    std::string data;
    buildHeader(data, size, 1);
    appendStr(data, &size, 2);
    appendStr(data, userName.c_str(), userName.size());
    socket.Send(data);
    while (status!=0);

}

void Locker::onRecvice(std::string data) {
    std::lock_guard<std::mutex> lock(globalLockerMutex);
    buffer+=data;
    while (buffer.size()>=21) {
        auto header = buffer.substr(0, 21);
        uint16_t size;
        uint8_t type;
        auto headerData = processHeader(header, size, type);
        if (headerData.size>buffer.size()) {
            return;
        }
        auto message = buffer.substr(21, headerData.size);
        buffer = buffer.substr(21+headerData.size);
        if (rejecter(message)) {
            spdlog::error("[Locker] Rejected!");
            status = 3;
            return;
        }
        if (headerData.type==4 || headerData.type==5 ) {
            recvData = *reinterpret_cast<const bool *>(message.c_str()+21);
            status = 1;
        } else if (headerData.type==6) {
            auto isSuccess = *reinterpret_cast<const bool * >(message.c_str()+21);
            if (isSuccess) {
                recvData = *reinterpret_cast<const bool *>(message.c_str()+22);
            } else {
                spdlog::info("[Locker] Query failed!");
            }
            status = 1;
        }
        else {
            spdlog::error("[Locker] Unknown type!");
            status = 4;
            return;
        }
    }
}

void Locker::buildHeader(std::string &data, uint16_t size, uint8_t type) {
    uint16_t magicNumber = 514;
    auto token = genToken();
    data = std::string((char*)&magicNumber, 2) + std::string((char*)&size, 2) + std::string((char*)&type, 1) + token;
}

std::string Locker::genToken() {
    uint8_t token[16];
    auto time = (uint32_t)std::time(nullptr);
    for (int i = 0; i < 16; i++) {
        token[i] = (uint8_t)(time >> (i * 2));
    }
    return {(char*)token, 16};
}

void Locker::appendStr(std::string &message, const void *src, std::size_t size) {
    message = message.append(reinterpret_cast<const char *>(src), size);
}

time_t Locker::restoreToken(const std::string &token) {
    uint32_t time = (uint32_t)std::time(nullptr);
    uint32_t tokenTime = 0;
    for (int i = 0; i < 16; i++) {
        tokenTime |= (uint32_t)token[i] << (i * 2);
    }
    return time;
}

bool Locker::unlock(std::string userName) {
    auto size = userName.size()+2;
    std::string data;
    buildHeader(data, size, 2);
    appendStr(data, &size, 2);
    appendStr(data, userName.c_str(), userName.size());
    socket.Send(data);
    while (status==0);
    if (status==1) {
        status = 0;
        return recvData;
    }
    else
        spdlog::error("[Locker] Unknown response! error code: {}", status);
    status = 0;
    return false;
}

bool Locker::isLocked(std::string userName) {
    auto size = userName.size()+2;
    std::string data;
    buildHeader(data, size, 3);
    appendStr(data, &size, 2);
    appendStr(data, userName.c_str(), userName.size());
    socket.Send(data);
    while (status==0);
    if (status==1) {
        status = 0;
        return recvData;
    }
    else
        spdlog::error("[Locker] Unknown response! error code: {}", status);
    status = 0;
    return false;
}

Locker::headerData Locker::processHeader(const std::string &header, uint16_t &size, uint8_t &type) {
    headerData data;
    data.size = *(uint16_t*)header.c_str();
    data.type = *(uint8_t*)(header.c_str()+2);
    size = data.size;
    type = data.type;
    return data;
}

Locker::headerData Locker::processHeader(const std::string &header) {
    headerData data;
    data.size = *(uint16_t*)header.c_str();
    data.type = *(uint8_t*)(header.c_str()+2);
    return data;
}

bool Locker::rejecter(const std::string &message) {
    if(restoreToken(message.substr(5, 21))- time(nullptr)>60) {
        spdlog::error("[Locker] Token out of date!");
        return true;
    }
    return false;
}


