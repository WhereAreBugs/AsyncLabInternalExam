//
// Created by 神奇bug在哪里 on 8/3/23.
//

#include "identifyer.h"
#include <fstream>
#include <utility>
#include "spdlog/spdlog.h"
void identifyer::setup() {
    spdlog::debug("Identifyer setup...");
    server.Bind(1783,[](int errCode, std::string msg){spdlog::error("Error code: {}, Error message: {}",errCode,msg);});
    server.onNewConnection = [this](TCPSocket<> * socket) {
        spdlog::trace("New connection!");
        this->buffers.emplace(socket,IdentifyBuffer());
        socket->onMessageReceived = [this, socket](std::string data) {
            onMessage(std::move(data), socket);
        };
    };
    server.Listen();

}

void identifyer::onMessage(std::string message, TCPSocket<> *socket) {
    std::lock_guard<std::mutex> lock1(globalBufferMutex);
    try {
        buffers.at(socket).addBuffer(std::move(message));
        }
    catch (std::out_of_range &e) {
        spdlog::error("[identifyer] Socket not found in buffers!");
        return;
    }
if (buffers.at(socket).isComplete()) {
        spdlog::trace("[identifyer] Message is complete!");
        std::string token = buffers.at(socket).getToken();
        spdlog::trace("[identifyer] Token: {}",token);
        std::lock_guard<std::mutex> lock2(globalTokensMutex);
        auto buffer = buffers.at(socket).getMessage();
    if (rejecter(buffer,socket))
    {
        spdlog::warn("[identifyer] Rejecting message!");
        buffers.at(socket).clearBuffer();
        std::string replay;
        buildHeader(replay,1,*reinterpret_cast<const uint8_t *>(buffer.substr(4, 5).c_str())+1,socket);
        bool isOk = false;
        replay.append(reinterpret_cast<const char *>(isOk));
        socket->Send(replay);
        spdlog::trace("[identifyer] Rejected Replay sent!");
        return;
    }
        buffers.at(socket).clearBuffer();
    switch (*reinterpret_cast<const uint8_t *>(buffer.substr(4, 5).c_str())) {
        case 1:
        {
            onPassLogin(nullptr);
            break;
        }
        case 2:
        {
            onFailedLogin(nullptr, std::string());
            break;
        }
        default:
        {
            spdlog::warn("[identifyer] Unknown message type!");
            break;
        }
    }
    }
}

void identifyer::onPassLogin(TCPSocket<> *newClinet) {
    spdlog::trace("[identifyer] Pass login!");
    bool isOk = true;
    std::string replay;
    buildHeader(replay,1,2,nullptr);
    replay.append(reinterpret_cast<const char *>(isOk));
    newClinet->Send(replay);
}

bool identifyer::rejecter(const std::string &message, TCPSocket<> *newClinet) {
    if (*reinterpret_cast<const uint8_t *>(message.substr(4,5).c_str())!=message.size()-21) {
        spdlog::warn("[identifyer] Message size not match!");
        return true;
    }
    if (!buffers.at(newClinet).isTokenValid(message.substr(6,21))) {
        spdlog::warn("[identifyer] Token not match!");
        return true;
    }
    return false;
}

void identifyer::buildHeader(std::string &message, uint16_t size, uint8_t type, TCPSocket<> *newClinet) {
    message.clear();
    uint16_t magicNumber = 101;
    message.append(reinterpret_cast<const char *>(&magicNumber), sizeof(magicNumber));
    message.append(reinterpret_cast<const char *>(&size), sizeof(size));
    message.append(reinterpret_cast<const char *>(&type), sizeof(type));
    this->buffers.at(newClinet).generateToken();
    message.append(this->buffers.at(newClinet).getToken());
}

void identifyer::onFailedLogin(TCPSocket<> *newClinet, std::string message) {
    spdlog::trace("[identifyer] Failed login!");
    auto size = *reinterpret_cast<const uint16_t *>(message.substr(22, 24).c_str());
    auto picture = message.substr(24, 24 + size);
    spdlog::trace("[identifyer] Picture size: {}", size);
    bool isOk = false;
    std::fstream file;
    auto time = std::chrono::system_clock::now();
    std::string fileName = "failedLogin_" + std::to_string(
            std::chrono::duration_cast<std::chrono::milliseconds>(time.time_since_epoch()).count()) + ".jpg";
    file.open(fileName, std::ios::out | std::ios::binary);
    if (file.fail()) {
        spdlog::error("[identifyer] Failed to open file!");
        isOk = false;
        std::string replay;
        buildHeader(replay, 1, 3, newClinet);
        replay.append(reinterpret_cast<const char *>(isOk));
        newClinet->Send(replay);
        return;
    }
    file.write(picture.c_str(), size);
    file.close();
    isOk = true;
    std::string replay;
    buildHeader(replay, 1, 3, newClinet);
    replay.append(reinterpret_cast<const char *>(isOk));
    newClinet->Send(replay);
}
