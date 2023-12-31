//
// Created by 神奇bug在哪里 on 7/15/23.
//

#include "WebData.h"
#include <fstream>
#include <spdlog/spdlog.h>

std::size_t WebData::getHash(const std::string &str) {
    //使用STL计算字符串的hash值
    std::hash<std::string> hash_fn;
    std::size_t str_hash = hash_fn(str);
    //将hash值转换为字符串
    return str_hash;
}
WebData::WebData()
{
    ///计划使用逗号分隔(csv)的文件格式来存储数据，可以提供给Excel等软件使用
    setFilename("WebData.csv"); //设置保存时的文件名

}

bool WebData::add(const std::string &username, const std::string &realName, const std::string &password,
                  uint8_t type) {
    struct userData newData{.realName = realName,.password = getHash(password), .type = static_cast<userData::userType>(type)};
    std::lock_guard<std::mutex> lock(globalMapMutex);
    if (checkExist(username))
        return false;
    data.emplace(username,newData);
    return true;
}

bool WebData::checkExist(const std::string &username) {
    try {
        data.at(username);
        return true;
    }
    catch (std::out_of_range &e){
        return false;
    }
}

bool WebData::remove(const std::string &username) {
    std::lock_guard<std::mutex> lock(globalMapMutex);
    if (!checkExist(username))
        return false;
    data.erase(username);
    return true;
}

bool
WebData::changePassword(const std::string &username, const std::string &oldPassword, const std::string &newPassword) {
    std::lock_guard<std::mutex> lock(globalMapMutex);
    //TODO: 使用标准的错误处理，设计错误处理类，抛出异常
    if (!checkExist(username))
        return false;
    if (data.at(username).password != getHash(oldPassword))
        return false;
    data.at(username).password = getHash(newPassword);
    return true;
}

void WebData::write2File() {
    std::ofstream file(getFilename());
    std::lock_guard<std::mutex> lock(globalMapMutex);
    for (auto &i : data) {
        file << i.first << "," << i.second.realName << "," << i.second.password << "," << i.second.type << std::endl;
    }
    file.close();
}

void WebData::readFromFile() {
    std::ifstream file(getFilename());
    std::lock_guard<std::mutex> lock(globalMapMutex);
    std::string username, realName, password, type;
    while (file >> username >> realName >> password >> type)
    {
        data.emplace(username,userData{.realName = realName, .password = std::stoull(password), .type = static_cast<userData::userType>(std::stoi(type))});
    }
    file.close();
}

bool WebData::checkLogin(const std::string &username, const std::string &password) {
    std::lock_guard<std::mutex> lock(globalMapMutex);
    if (!checkExist(username))
        return false;
    return data.at(username).password == getHash(password);
}

bool WebData::resetPassword(std::string username) {
    std::lock_guard<std::mutex> lock(globalMapMutex);
    if (!checkExist(username))
        return false;
    try {
        data.at(username).password = getHash("123456");
    }
    catch (std::out_of_range &e){
        spdlog::error("WebData::resetPassword: {}", e.what());
        return false;
    }
    return true;
}


