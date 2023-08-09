//
// Created by 神奇bug在哪里 on 7/14/23.
//

#ifndef SIMPLELABMANAGER_INIT_H
#define SIMPLELABMANAGER_INIT_H

#include <vector>
#include "WebNet.h"
#include "databaseCore.h"
#include "seatData.h"
#include "identifyer.h"
#include "Locker.h"
#define CHECK_NULLPTR(x) if(x==nullptr){ \
    spdlog::error("nullptr error in {} at line {}",__FILE__,__LINE__); \
    exit(1);                             \
    }


class Init {
private:
    std::vector<databaseCore *> databaseCommpents;
    std::vector<std::thread> subThreads;
    WebNet *webnetHandler = nullptr;
    Locker * lockerHandler = nullptr;
    identifyer * identifyerHandler = nullptr;
public:
    Init();

    ~Init();

    [[noreturn]]void start();

    void databaseInit();
};


#endif //SIMPLELABMANAGER_INIT_H
