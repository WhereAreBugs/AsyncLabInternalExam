//
// Created by 神奇bug在哪里 on 7/15/23.
//

#ifndef SIMPLELABMANAGER_WEBDATA_H
#define SIMPLELABMANAGER_WEBDATA_H
#include "databaseCore.h"
#include <map>
#include <mutex>
class WebData : public databaseCore{
public:
    struct userData
    {
        std::string realName;
        std::size_t password; //使用hash值存储密码
        enum userType{Normal, Admin} type;
    };
    inline  bool checkExist(const std::string &username);
private:
    static std::size_t getHash(const std::string &str);
    std::map<std::string , userData> data;
    std::mutex globalMapMutex;
public:
    WebData();
    bool checkLogin(const std::string &username, const std::string &password);
    bool add(const std::string &username, const std::string &realName, const std::string &password, uint8_t type);
    bool remove(const std::string &username);
    bool changePassword(const std::string &username, const std::string &oldPassword, const std::string &newPassword);
    void write2File() override;
    void readFromFile() override;
    bool resetPassword(std::string username);


};


#endif //SIMPLELABMANAGER_WEBDATA_H
