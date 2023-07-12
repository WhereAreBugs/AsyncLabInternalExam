# Cpp_internal_exam
> 这个项目将作为实验室内部C++方向的内部考核，考核截止时间为**8月20日**。<br>
## 项目简介
在这个项目中，你们将要完成一个简单的实验室管理系统的后端。<br>
主要愿景为计划接入智能门锁系统和生物身份验证设备(例如: 指纹)，并在实验室门口提供一块屏幕，用于显示一些信息。<br>
我们主要使用网络与门锁系统和身份验证系统进行通信。请确保该系统具有并发处理的能力，可能会同时出现多条请求，这些请求间不会存在竞态条件。 <br>
数据需要作持久化处理，毕竟我们不希望每次重启都需要重新注册。<br>
本项目将作为实验室内部C++方向的内部考核，考核截止时间为**8月20日**。<br>
## 项目要求
### 管理员模式
* 从前端或生物身份验证设备接收登录请求，验证管理员身份，如果正确则返回登录成功，否则返回登录失败。
* 在管理员验证成功之后才能允许前端传入的管理员操作的一些请求
> 管理员模式下特有的操作包括:
> * 添加用户
> * 删除用户
> * 重置用户密码
> * 绑定固定座位
> * 解绑固定座位
> * 分配流动座位

### 针对普通用户
* 从前端或生物身份验证设备接收登录请求，验证用户身份，如果正确则返回登录成功，否则返回登录失败。
* 开门
* 查询自己的座位
* 查询自己的倒垃圾时间
* 查询自己的活动证明

### 特殊说明
为了防止被数据包伪造之类的攻击手段轻易攻破，我们需要对数据包进行一些简单的校验。<br>

## 通讯协议
### 前后端交互
采用RAW socket协议,以下为前后端交互的数据包格式<br>
#### 包头

|    数据类型     | 数据长度 |     数据内容      | 数据名称  |
|:-----------:|:----:|:-------------:|:-----:|
|  uint16_t   | 2字节  |    魔数(114)    | magic |
|  uint16_t   | 2字节  |     数据包长度     | size  |
|   uint8_t   | 1字节  |     数据包类型     | type  |
| uint8_t[64] | 64字节 | 身份校验令牌(token) | token |

#### 包体
##### 注册请求[type=1]
|        数据类型        |     数据长度     |  数据内容  |     数据名称     |
|:------------------:|:------------:|:------:|:------------:|
|      uint16_t      |     2字节      | 用户名长度  | usernameSize |
| char[usernameSize] | usernameSize |  用户名   |   username   |
|      uint16_t      |     2字节      |  密码长度  | passwordSize |
| char[passwordSize] | passwordSize |   密码   |   password   |
 |      uint8_t       |     1字节      |  用户类型  |   userType   |
|      uint16_t      |     2字节      | 真实姓名长度 |   nameSize   |
|   char[nameSize]   |   nameSize   |  真实姓名  |     name     |
##### 登录请求[type=2]
|        数据类型        |     数据长度     | 数据内容  |     数据名称     |
|:------------------:|:------------:|:-----:|:------------:|
|      uint16_t      |     2字节      | 用户名长度 | usernameSize |
| char[usernameSize] | usernameSize |  用户名  |   username   |
|      uint16_t      |     2字节      | 密码长度  | passwordSize |
| char[passwordSize] | passwordSize |  密码   |   password   |
##### 开门请求[type=3]
| 数据类型 | 数据长度 | 数据内容 |  数据名称  |
|:----:|:----:|:----:|:------:|
| bool | 1字节  | 是否开门 | isOpen |

##### 查询座位请求[type=4]
|        数据类型        |     数据长度     | 数据内容  |     数据名称     |
|:------------------:|:------------:|:-----:|:------------:|
|    usernameSize    |     2字节      | 用户名长度 | usernameSize |
| char[usernameSize] | usernameSize |  用户名  |   username   |

##### 查询倒垃圾时间请求[type=5]
|        数据类型        |     数据长度     | 数据内容  |     数据名称     |
|:------------------:|:------------:|:-----:|:------------:|
|    usernameSize    |     2字节      | 用户名长度 | usernameSize |
| char[usernameSize] | usernameSize |  用户名  |   username   |
##### 查询活动证明请求[type=6]
|        数据类型        |     数据长度     | 数据内容  |     数据名称     |
|:------------------:|:------------:|:-----:|:------------:|
|    usernameSize    |     2字节      | 用户名长度 | usernameSize |
| char[usernameSize] | usernameSize |  用户名  |   username   |
##### 重新分配流动座位请求[type=12]
|        数据类型        |     数据长度     | 数据内容  |     数据名称     |
|:------------------:|:------------:|:-----:|:------------:|
|    usernameSize    |     2字节      | 用户名长度 | usernameSize |
| char[usernameSize] | usernameSize |  用户名  |   username   |

##### 管理员添加用户请求[type=7]
|        数据类型        |     数据长度     |  数据内容  |     数据名称     |
|:------------------:|:------------:|:------:|:------------:|
|      uint16_t      |     2字节      | 用户名长度  | usernameSize |
| char[usernameSize] | usernameSize |  用户名   |   username   |
|      uint16_t      |     2字节      |  密码长度  | passwordSize |
| char[passwordSize] | passwordSize |   密码   |   password   |
 |      uint8_t       |     1字节      |  用户类型  |   userType   |
|      uint16_t      |     2字节      | 真实姓名长度 |   nameSize   |
|   char[nameSize]   |   nameSize   |  真实姓名  |     name     |

##### 管理员删除用户请求[type=8]
|        数据类型        |     数据长度     |  数据内容  |     数据名称     |
|:------------------:|:------------:|:------:|:------------:|
|      uint16_t      |     2字节      | 用户名长度  | usernameSize |
| char[usernameSize] | usernameSize |  用户名   |   username   |

##### 管理员重置用户密码请求[type=9]
|        数据类型        |     数据长度     |  数据内容  |     数据名称     |
|:------------------:|:------------:|:------:|:------------:|
|      uint16_t      |     2字节      | 用户名长度  | usernameSize |
| char[usernameSize] | usernameSize |  用户名   |   username   |
##### 管理员绑定固定座位请求[type=10]
|        数据类型        |     数据长度     | 数据内容  |     数据名称     |
|:------------------:|:------------:|:-----:|:------------:|
|      uint16_t      |     2字节      | 用户名长度 | usernameSize |
| char[usernameSize] | usernameSize |  用户名  |   username   |
|      uint64_t      |     8字节      |  座位号  |  seatNumber  |

##### 管理员解绑固定座位请求[type=11]
|        数据类型        |     数据长度     | 数据内容  |     数据名称     |
|:------------------:|:------------:|:-----:|:------------:|
|      uint16_t      |     2字节      | 用户名长度 | usernameSize |
| char[usernameSize] | usernameSize |  用户名  |   username   |

##### 注册响应[type=13]
| 数据类型 | 数据长度 |  数据内容  |    数据名称    |
|:----:|:----:|:------:|:----------:|
| bool | 1字节  | 是否注册成功 | isRegister |

##### 登录响应[type=14]
|    数据类型     | 数据长度 |  数据内容  |  数据名称   |
|:-----------:|:----:|:------:|:-------:|
|    bool     | 1字节  | 是否登录成功 | isLogin |
 | uint8_t[64] | 64字节 | token  |  token  |

##### 开门响应[type=15]
| 数据类型 | 数据长度 |  数据内容  |  数据名称  |
|:----:|:----:|:------:|:------:|
| bool | 1字节  | 是否开门成功 | isOpen |

##### 查询座位响应[type=16]
|   数据类型   | 数据长度 |  数据内容  |    数据名称    |
|:--------:|:----:|:------:|:----------:|
|   bool   | 1字节  | 是否查询成功 |  isQuery   |
| uint8_t  | 1字节  |  座位状态  | seatStatus |
| uint64_t | 8字节  |  座位号   | seatNumber |
注意：若为流动座位，则自动分配座位号
##### 查询倒垃圾时间响应[type=17]
|   数据类型   | 数据长度 |  数据内容  |    数据名称     |
|:--------:|:----:|:------:|:-----------:|
|   bool   | 1字节  | 是否查询成功 |   isQuery   |
| uint64_t | 8字节  | 倒垃圾时间  | garbageTime |

##### 查询活动证明响应[type=18]
|           数据类型           |        数据长度        |   数据内容   |        数据名称         |
|:------------------------:|:------------------:|:--------:|:-------------------:|
|           bool           |        1字节         |  是否查询成功  |       isQuery       |
|         uint8_t          |        1字节         |  活动证明数量  |   activityNumber    |
| uint16_t[activityNumber] | 2*activityNumber字节 | 活动证明地址长度 | activityAddressSize |


##### 重新分配流动座位响应[type=24]
|           数据类型           |        数据长度        |  数据内容  |   数据名称    |
|:------------------------:|:------------------:|:------:|:---------:|
|           bool           |        1字节         | 是否分配成功 | isSuccess |


##### 管理员添加用户响应[type=19]
| 数据类型 | 数据长度 |  数据内容  |   数据名称    |
|:----:|:----:|:------:|:---------:|
| bool | 1字节  | 是否添加成功 | isAddUser |

##### 管理员删除用户响应[type=20]
| 数据类型 | 数据长度 |  数据内容  |     数据名称     |
|:----:|:----:|:------:|:------------:|
| bool | 1字节  | 是否删除成功 | isDeleteUser |

##### 管理员重置用户密码响应[type=21]
| 数据类型 | 数据长度 |  数据内容  |      数据名称       |
|:----:|:----:|:------:|:---------------:|
| bool | 1字节  | 是否重置成功 | isResetPassword |

##### 管理员绑定固定座位响应[type=22]
| 数据类型 | 数据长度 |  数据内容  |    数据名称    |
|:----:|:----:|:------:|:----------:|
| bool | 1字节  | 是否绑定成功 | isBindSeat |

##### 管理员解绑固定座位响应[type=23]
| 数据类型 | 数据长度 |  数据内容  |     数据名称     |
|:----:|:----:|:------:|:------------:|
| bool | 1字节  | 是否解绑成功 | isUnbindSeat |

### 门锁
#### 包头
|    数据类型     | 数据长度 |      数据内容      | 数据名称  |
|:-----------:|:----:|:--------------:|:-----:|
|  uint16_t   | 2字节  |    魔数(514)     | magic |
|  uint16_t   | 2字节  |     数据包长度      | size  |
|   uint8_t   | 1字节  |     数据包类型      | type  |
| uint8_t[16] | 16字节 | 动态身份校验令牌(依据时间) | token |

#### 包体
注意：在与门锁进行通信时，门锁为服务端，本项目为客户端
##### 开门请求[type=1]
|         数据类型          |      数据长度      |   数据内容   |     数据名称     |
|:---------------------:|:--------------:|:--------:|:------------:|
|       uint16_t        |      2字节       | 请求者用户名长度 | usernameSize |
| uint8_t[usernameSize] | usernameSize字节 |  请求者用户名  |   username   |
##### 锁门请求[type=2]
|         数据类型          |      数据长度      |   数据内容   |     数据名称     |
|:---------------------:|:--------------:|:--------:|:------------:|
|       uint16_t        |      2字节       | 请求者用户名长度 | usernameSize |
| uint8_t[usernameSize] | usernameSize字节 |  请求者用户名  |   username   |

##### 查询门锁状态请求[type=3]
|         数据类型          |      数据长度      |   数据内容   |     数据名称     |
|:---------------------:|:--------------:|:--------:|:------------:|
|       uint16_t        |      2字节       | 请求者用户名长度 | usernameSize |
| uint8_t[usernameSize] | usernameSize字节 |  请求者用户名  |   username   |

##### 开门响应[type=4]
| 数据类型 | 数据长度 |  数据内容  |   数据名称    |
|:----:|:----:|:------:|:---------:|
| bool | 1字节  | 是否开门成功 | isSuccess |
##### 锁门响应[type=5]
| 数据类型 | 数据长度 |  数据内容  |   数据名称    |
|:----:|:----:|:------:|:---------:|
| bool | 1字节  | 是否锁门成功 | isSuccess |
##### 查询门锁状态响应[type=6]
| 数据类型 | 数据长度 |  数据内容  |   数据名称    |
|:----:|:----:|:------:|:---------:|
| bool | 1字节  | 是否查询成功 | isSuccess |
| bool | 1字节  |  门锁状态  |  status   |
### 生物身份验证设备
注意：在与生物身份验证设备进行通信时，门锁为服务端，本项目为客户端
#### 包头
|    数据类型     | 数据长度 |   数据内容   | 数据名称  |
|:-----------:|:----:|:--------:|:-----:|
|  uint16_t   | 2字节  | 魔数(101)  | magic |
|  uint16_t   | 2字节  |  数据包长度   | size  |
|   uint8_t   | 1字节  |  数据包类型   | type  |
| uint8_t[16] | 16字节 | 令牌(依据时间) | token |

#### 包体
##### 身份验证通过请求[type=1]
|         数据类型          |      数据长度      |   数据内容   |     数据名称     |
|:---------------------:|:--------------:|:--------:|:------------:|
|       uint16_t        |      2字节       | 通过者用户名长度 | usernameSize |
| uint8_t[usernameSize] | usernameSize字节 |  通过者用户名  |   username   |

##### 身份验证通过响应[type=2]
| 数据类型 | 数据长度 |  数据内容  |   数据名称    |
|:----:|:----:|:------:|:---------:|
| bool | 1字节  | 是否验证成功 | isSuccess |
##### 身份验证失败请求[type=3]
|        数据类型        |    数据长度     |  数据内容   |   数据名称    |
|:------------------:|:-----------:|:-------:|:---------:|
|      uint16_t      |     2字节     | 失败者用户照片 | photoSize |
| uint8_t[photoSize] | photoSize字节 | 失败者用户照片 |   photo   |

##### 身份验证失败响应[type=4]
| 数据类型 | 数据长度 |  数据内容  |   数据名称    |
|:----:|:----:|:------:|:---------:|
| bool | 1字节  | 是否记录成功 | isSuccess |



