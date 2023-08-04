//
// Created by 神奇bug在哪里 on 7/14/23.
//

#include "network/WebNet.h"
#include <utility>
#include "spdlog/spdlog.h"
#include "Buffer.h"

/**
 * @brief 构造函数，初始化前后端交互的服务端
 * @param 无
 * @return 无
 * @attention 如非必要，请不要修改里面的内容
 */
WebNet::WebNet() {
    server.onNewConnection = [&](TCPSocket<> *newClient) {
        spdlog::info("new connection from" + newClient->remoteAddress());
        try {
            globalBufferMutex.lock();
            buffers.emplace(newClient, new webBuffer());
            globalBufferMutex.unlock();
            char token[64];
            for (char &i: token) {
                i = '0';
            }
            tokens.emplace(newClient->remoteAddress(), token);
        } catch (std::exception &e) {
            spdlog::error("error when creating buffer:" + std::string(e.what()));
            globalBufferMutex.unlock();
        }

        newClient->onMessageReceived = [newClient, this](std::string message) {
            onNewMessage(std::move(message), newClient);

        };
        newClient->deleteAfterClosed = true;
        newClient->onSocketClosed = [newClient, this](int errorCode) {
            spdlog::info("connection closed from" + newClient->remoteAddress() + " Code:" + std::to_string(errorCode));
            try {
                delete buffers[newClient];
                globalBufferMutex.lock();
                buffers.erase(newClient);
                globalBufferMutex.unlock();
            } catch (std::exception &e) {
                spdlog::error("error when deleting buffer:" + std::string(e.what()));
                globalBufferMutex.unlock();
            }
        };
    };
}

/**
 * @brief 处理数据包的头部，根据头部的信息进行处理
 * @param message 收到的原始数据包内容
 * @param newClient 客户端指针(仅用于区分不同的客户端)
 * @attention 该函数在被调用时堆栈上下文可能切换，因此不要使用const reference。
 */
void WebNet::onNewMessage(std::string message, TCPSocket<> *newClient) { // NOLINT(performance-unnecessary-value-param)
    std::lock_guard<std::mutex> lock(globalBufferMutex);
    try {
        buffers.at(newClient)->addBuffer(std::move(message));
    } catch (std::out_of_range &e) {
        spdlog::trace("Out of range: " + std::string(e.what()));
        return;
    }
    if (buffers.at(newClient)->isComplete()) {
        std::string completeMessage = buffers.at(newClient)->getMessage();
        buffers.at(newClient)->clearBuffer();
        if (rejecter(completeMessage)) {
            spdlog::warn("reject message from" + newClient->remoteAddress());
            return;
        }
        auto headerData = processHeader(completeMessage, newClient);
        spdlog::info("message from" + newClient->remoteAddress() + " type:" +
                     std::to_string(headerData.type));
        switch (headerData.type) {
            case 1:
                onRegister(std::move(completeMessage.substr(69, 69 + headerData.size)), newClient);
                break;
            case 2:
                onLogin(std::move(completeMessage.substr(69, 69 + headerData.size)), newClient);
                break;
            case 3:
                onOpenTheDoor(std::move(completeMessage.substr(69, 69 + headerData.size)), newClient);
                break;
            case 4:
                onQuerySeat(std::move(completeMessage.substr(69, 69 + headerData.size)), newClient);
                break;
            case 5:
                onQueryRubbishTime(std::move(completeMessage.substr(69, 69 + headerData.size)), newClient);
                break;
            case 6:
                onQueryActivity(std::move(completeMessage.substr(69, 69 + headerData.size)), newClient);
                break;
            case 7:
                onAdminAddUser(std::move(completeMessage.substr(69, 69 + headerData.size)), newClient);
                break;
            case 8:
                onAdminDeleteUser(std::move(completeMessage.substr(69, 69 + headerData.size)), newClient);
                break;
            case 9:
                onAdminResetUserPassword(std::move(completeMessage.substr(69, 69 + headerData.size)), newClient);
                break;
            case 10:
                onAdminBindUserSeat(std::move(completeMessage.substr(69,69+ headerData.size)),newClient);
                break;
            case 11:
                onAdminUnbindUserSeat(std::move(completeMessage.substr(69,69+headerData.size)),newClient);
                break;
            case 12:
                onQueryResetSeat(std::move(completeMessage.substr(69,69+headerData.size)),newClient);
                break;
            default:
                spdlog::warn("unknown message type from" + newClient->remoteAddress());
                break;
        }
    }
}


/**
 * @param magicNumber 114 uint16_t
 * @param size 数据包body的长度 uint16_t
 * @param type 数据包的类型 uint8_t
 * @param uint8_t[64] body 数据包的内容
 */
void WebNet::buildHeader(std::string &message, uint8_t type, uint16_t size, TCPSocket<> *newClient) {
    short magicNumber = 114;
    appendStr(message, &magicNumber, sizeof(magicNumber));
    appendStr(message, &size, sizeof(size));
    appendStr(message, &type, sizeof(type));
    std::lock_guard<std::mutex> lock(globalBufferMutex);
    if (type == 13) {}
    appendStr(message, buffers.at(newClient)->getToken().c_str(), 64);
}

/**
 * @brief 检查数据包的token是否有效，类型是否正确等，若有问题则直接拒绝
 * @param message 数据包的内容
 * @return 是否拒绝
 */

bool WebNet::rejecter(const std::string &message) {
    //token生成规则：md5(时间戳+随机数+密码)
    //token有效期：60分钟
    //token长度：64位
    //token生成时间：每次登录时生成，其余情况下不变
    std::string token = message.substr(5, 64);
    uint8_t type = *reinterpret_cast<const uint8_t *>(message.c_str() + 4);
    if (type == 1 || type == 2 || tokens.find(token) != tokens.end()) {
        return false;
    } else {
        return true;
    }
}

WebNet::HeadData WebNet::processHeader(const std::string &header, TCPSocket<> *Clinet) {
    HeadData data;
    data.size = *reinterpret_cast<const uint16_t *>(header.c_str() + 2);
    data.type = *reinterpret_cast<const uint8_t *>(header.c_str() + 4);
    return data;
}


void WebNet::appendStr(std::string &message, const void *src, std::size_t size) {
    message = message.append(reinterpret_cast<const char *>(src), size);
}

void WebNet::onRegister(std::string message, TCPSocket<> *newClient) {
    auto usernameSize = *reinterpret_cast<const uint16_t *>(message.c_str());
    auto userName = message.substr(2, usernameSize);
    auto passwordSize = *reinterpret_cast<const uint16_t *>(message.c_str() + 2 + usernameSize);
    auto password = message.substr(2 + usernameSize + 2, passwordSize);
    uint8_t userType = *reinterpret_cast<const uint8_t *>(message.c_str() + 2 + usernameSize + 2 + passwordSize);
    uint16_t nameSize = *reinterpret_cast<const uint16_t *>(message.c_str() + 2 + usernameSize + 2 + passwordSize + 1);
    auto name = message.substr(2 + usernameSize + 2 + passwordSize + 1 + 2, nameSize);
    ///数据读取完成
    auto result = dataHandler->add(userName, name, password, userType);
    std::string response;
    buildHeader(response, 13, 1, newClient);
    response += reinterpret_cast<const char *>(&result);
    newClient->Send(response);
}

void WebNet::onLogin(std::string message, TCPSocket<> *newClient) {
    auto usernameSize = *reinterpret_cast<const uint16_t *>(message.c_str());
    auto userName = message.substr(2, usernameSize);
    auto passwordSize = *reinterpret_cast<const uint16_t *>(message.c_str() + 2 + usernameSize);
    auto password = message.substr(2 + usernameSize + 2, passwordSize);
    ///数据读取完成
    auto result = dataHandler->checkLogin(userName, password);
    std::string response;
    buildHeader(response, 14, 65, newClient);
    std::lock_guard<std::mutex> lock(globalBufferMutex);
    std::lock_guard<std::mutex> lock1(globalTokensMutex);
    if (result) {
        buffers.at(newClient)->generateToken();
        std::string token = buffers.at(newClient)->getToken();
        tokens.at(userName) = token;
        response += token;
    } else {
        response += std::string(64, 0);
    }
}

void WebNet::onOpenTheDoor(std::string message, TCPSocket<> *newClient) {
    //TODO: 开门

}

void WebNet::onQuerySeat(std::string message, TCPSocket<> *newClient) {
    auto usernameSize = *reinterpret_cast<const uint16_t *>(message.c_str());
    auto userName = message.substr(2, usernameSize);
    auto result = seatHandler->getSeatStatus(userName);
    spdlog::trace("query seat status:" + std::to_string(result));
    std::string response;
    buildHeader(response, 16, 1, newClient);
    response += reinterpret_cast<const char *>(&result);
    newClient->Send(response);
}

void WebNet::onQueryRubbishTime(std::string message, TCPSocket<> *newClient) {
    auto usernameSize = *reinterpret_cast<const uint16_t *>(message.c_str());
    auto userName = message.substr(2, usernameSize);
    auto result = seatHandler->getRubbishTime(userName);
    spdlog::trace("query rubbish time:" + std::to_string(result));
    std::string response;
    buildHeader(response, 17, 1, newClient);
    response += reinterpret_cast<const char *>(&result);
    newClient->Send(response);
}

void WebNet::onQueryActivity(std::string message, TCPSocket<> *newClient) {

}

void WebNet::setDataHandler(WebData *dataHandler) {
    WebNet::dataHandler = dataHandler;

}

void WebNet::setSeatHandler(seatData *seatHandler) {
    WebNet::seatHandler = seatHandler;

}

void WebNet::setActivatysHandler(activatys *activatysHandler) {
    WebNet::activatysHandler = activatysHandler;

}

void WebNet::setToken(std::string token, std::string username) {
    std::lock_guard<std::mutex> lock(globalTokensMutex);
    try {
        tokens.at(username) = std::move(token);
    }
    catch (std::out_of_range &e) {
        spdlog::error("set token error:" + std::string(e.what()));
    }
}

void WebNet::onQueryResetSeat(std::string message, TCPSocket<> *newClient) {
    auto usernameSize = *reinterpret_cast<const uint16_t *>(message.c_str());
    auto userName = message.substr(2, usernameSize);
    auto result = seatHandler->ResetSeatStatus(userName);
    spdlog::trace("query reset seat status:" + std::to_string(result));
    std::string response;
    buildHeader(response, 18, 1, newClient);
    response += reinterpret_cast<const char *>(&result);
    newClient->Send(response);
}

void WebNet::onAdminAddUser(std::string message, TCPSocket<> *newClient) {
    auto usernameSize = *reinterpret_cast<const uint16_t *>(message.c_str());
    auto userName = message.substr(2, usernameSize);
    auto passwordSize = *reinterpret_cast<const uint16_t *>(message.c_str() + 2 + usernameSize);
    auto password = message.substr(2 + usernameSize + 2, passwordSize);
    uint8_t userType = *reinterpret_cast<const uint8_t *>(message.c_str() + 2 + usernameSize + 2 + passwordSize);
    uint16_t nameSize = *reinterpret_cast<const uint16_t *>(message.c_str() + 2 + usernameSize + 2 + passwordSize + 1);
    auto name = message.substr(2 + usernameSize + 2 + passwordSize + 1 + 2, nameSize);
    ///数据读取完成
    auto result = dataHandler->add(userName, name, password, userType);
    std::string response;
    buildHeader(response, 19, 1, newClient);
    response += reinterpret_cast<const char *>(&result);
    newClient->Send(response);
}

void WebNet::onAdminDeleteUser(std::string message, TCPSocket<> *newClient) {
    auto usernameSize = *reinterpret_cast<const uint16_t *>(message.c_str());
    auto userName = message.substr(2, usernameSize);
    ///数据读取完成
    auto result = dataHandler->remove(userName);
    std::string response;
    buildHeader(response, 20, 1, newClient);
    response += reinterpret_cast<const char *>(&result);
    newClient->Send(response);
}

void WebNet::onAdminResetUserPassword(std::string message, TCPSocket<> *newClient) {
    auto usernameSize = *reinterpret_cast<const uint16_t *>(message.c_str());
    auto userName = message.substr(2, usernameSize);
    ///数据读取完成
    auto result = dataHandler->resetPassword(userName);
    std::string response;
    buildHeader(response, 21, 1, newClient);
    response += reinterpret_cast<const char *>(&result);
    newClient->Send(response);
}

void WebNet::onAdminBindUserSeat(std::string message, TCPSocket<> *newClient) {
    auto usernameSize = *reinterpret_cast<const uint16_t *>(message.c_str());
    auto userName = message.substr(2, usernameSize);
    uint8_t seatId = *reinterpret_cast<const uint8_t *>(message.c_str() + 2 + usernameSize);
    ///数据读取完成
    auto result = seatHandler->bindSeat(userName, seatId);
    std::string response;
    buildHeader(response, 22, 1, newClient);
    response += reinterpret_cast<const char *>(&result);
    newClient->Send(response);
}

void WebNet::onAdminUnbindUserSeat(std::string message, TCPSocket<> *newClient) {
    auto usernameSize = *reinterpret_cast<const uint16_t *>(message.c_str());
    auto userName = message.substr(2, usernameSize);
    ///数据读取完成
    auto result = seatHandler->unbindSeat(userName);
    std::string response;
    buildHeader(response, 23, 1, newClient);
    response += reinterpret_cast<const char *>(&result);
    newClient->Send(response);
}

std::string WebNet::getToken(std::string username) {
    std::lock_guard<std::mutex> lock(globalTokensMutex);
    try {
        return tokens.at(username);
    }
    catch (std::out_of_range &e) {
        spdlog::error("get token error:" + std::string(e.what()));
        return "";
    }
}

void WebNet::removeToken(std::string username) {
    std::lock_guard<std::mutex> lock(globalTokensMutex);
    try {
        tokens.erase(username);
    }
    catch (std::out_of_range &e) {
        spdlog::error("remove token error:" + std::string(e.what()));
    }
}

WebNet::~WebNet() {


}
