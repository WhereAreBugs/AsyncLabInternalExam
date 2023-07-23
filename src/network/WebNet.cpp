//
// Created by 神奇bug在哪里 on 7/14/23.
//

#include "network/WebNet.h"
#include <utility>
#include "spdlog/spdlog.h"

/**
 * @brief 构造函数，初始化前后端交互的服务端
 * @param 无
 * @return 无
 * @attention 如非必要，请不要修改里面的内容
 */
WebNet::WebNet() {
    server.onNewConnection  = [&](TCPSocket<> *newClient){
        spdlog::info("new connection from"+newClient->remoteAddress());
        try {
        globalBufferMutex.lock();
        buffers.emplace(newClient, new webBuffer());
        globalBufferMutex.unlock();
        } catch (std::exception &e) {
            spdlog::error("error when creating buffer:"+std::string(e.what()));
            globalBufferMutex.unlock();
        }

        newClient->onMessageReceived = [newClient, this](std::string message) {
            onNewMessage(std::move(message), newClient);
        };
        newClient->deleteAfterClosed = true;
        newClient->onSocketClosed = [newClient, this](int errorCode) {
            spdlog::info("connection closed from"+newClient->remoteAddress()+" Code:"+std::to_string(errorCode));
            try {
                delete buffers[newClient];
                globalBufferMutex.lock();
                buffers.erase(newClient);
                globalBufferMutex.unlock();
            } catch (std::exception &e) {
                spdlog::error("error when deleting buffer:"+std::string(e.what()));
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
    buffers.at(newClient)->addBuffer(std::move(message));
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
                ///@todo 注册
                break;
            case 2:
                //登录
                break;
            case 3:
                //开门
                break;
            case 4:
                //查询座位
                break;
            ///@todo 其余的类型
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
    std::string token = message.substr(5,64);
    uint8_t type = *reinterpret_cast<const uint8_t *>(message.c_str()+4);
    if (type==1||type==2||tokens.find(token)!=tokens.end())
    {
        return false;
    }
    else
    {
        return true;
    }
}

WebNet::HeadData WebNet::processHeader(const std::string& header, TCPSocket<> *Clinet) {
    HeadData data;
    data.size = *reinterpret_cast<const uint16_t *>(header.c_str()+2);
    data.type = *reinterpret_cast<const uint8_t *>(header.c_str()+4);
    return data;
}


void WebNet::appendStr(std::string &message,const void *src, std::size_t size) {
    message =  message.append(reinterpret_cast<const char *>(src), size);
}
