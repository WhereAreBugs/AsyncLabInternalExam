//
// Created by 神奇bug在哪里 on 7/14/23.
//

#ifndef SIMPLELABMANAGER_WEBNET_H
#define SIMPLELABMANAGER_WEBNET_H
#include "tcpserver.hpp"
#include "webBuffer.h"
#include <map>
class WebNet {
private:
    std::map<std::string,std::string> tokens;
    TCPServer<0x1000> server; //默认为每个链接分配最大0x1000(4096)字节的缓冲区
   typedef struct
    {
        uint16_t size;
        uint8_t type;
    }HeadData;
    std::mutex globalBufferMutex;
    std::map<TCPSocket<> * , webBuffer *> buffers;
private: // 私有的回掉函数及关键函数
    void onNewMessage(std::string message, TCPSocket<> *newClient);
    void buildHeader(std::string &message, uint8_t type, uint16_t size, TCPSocket<> *newClient);
    bool rejecter(const std::string &message);
    HeadData processHeader(const std::string& header, TCPSocket<> *Clinet);
public:
    WebNet();

private: //各种函数的工具函数
    static inline void appendStr(std::string &message, const void * src, std::size_t size);
private: //私有的业务函数
    /**
     * @brief 注册
     * @param message 数据包的内容
     * @param newClient 数据包的来源
     * @arg type==1
     * @attention 目前是计划的是开放注册模式，后续如果需要可以关闭注册入口，仅允许管理员添加用户。
     * @todo 未来可以考虑添加验证码功能
     * @note 后续所有的参数列表都不再重复注释
     */
    void onRegister(std::string message, TCPSocket<> *newClient);
    /**
     * @brief 登录
     * @arg type==2
     * @attention 登录成功后会生成一个token，后续的所有请求都需要携带token，否则会被拒绝
     */
    void onLogin(std::string message, TCPSocket<> *newClient);
    /**
     * @brief 开门
     * @arg type==3
     * @attention 目前考虑到的是开门后会自动锁门，后续如果需要可以添加参数来控制是否自动锁门
     * @todo 未来可以考虑添加管理开门时间段的功能
     * @todo 未来可以考虑记录非网络开门的开门记录
     */
    void onOpenTheDoor(std::string message, TCPSocket<> *newClient);
    /**
     * @brief 查询座位
     * @arg type==4
     * @attention 目前考虑的是查询座位的时候会返回所有座位的状态，并且所有用户都可以查询
     * @todo 未来可以考虑添加预定座位的功能
     */
    void onQuerySeat(std::string message, TCPSocket<> *newClient);
    /**
     * @brief 查询自己需要倒垃圾的时间
     * @arg type==5
     * @attention 目前考虑的是查询自己需要倒垃圾的时间，后续如果需要可以添加管理员可以查询所有人需要倒垃圾的时间的功能
     */
    void onQueryRubbishTime(std::string message, TCPSocket<> *newClient);
    /**
     * @brief 查询自己的活动证明
     * @arg type==6
     * @attention 目前考虑的是查询自己的活动，后续如果需要可以添加管理员可以查询所有人的活动的功能
     */
    void onQueryActivity(std::string message, TCPSocket<> *newClient);
    /**
     * @brief 重置流动座位的分配，重新分配一个座位
     * @arg type==12
     * @attention 仅限类型为流动座位的用户使用，固定座位仅限管理员进行分配
     */
    void onQueryResetSeat(std::string message, TCPSocket<> *newClient);
    /**
     * @brief 用于管理员添加用户
     * @arg type==7
     * @attention 仅限管理员使用
     */
    void onAdminAddUser(std::string message, TCPSocket<> *newClient);
    /**
     * @brief 用于管理员删除用户
     * @arg type==8
     * @attention 后续不再啰嗦
     */
    void onAdminDeleteUser(std::string message, TCPSocket<> *newClient);
    /**
     * @brief 用于管理员重置用户密码
     * @arg type==9
     */
    void onAdminResetUserPassword(std::string message, TCPSocket<> *newClient);
    /**
     * @brief 用于管理员绑定固定座位
     * @arg type==10
     */
    void onAdminBindUserSeat(std::string message, TCPSocket<> *newClient);
    /**
     * @brief 用于管理员解绑固定座位
     * @arg type==11
     */
    void onAdminUnbindUserSeat(std::string message, TCPSocket<> *newClient);
};


#endif //SIMPLELABMANAGER_WEBNET_H
