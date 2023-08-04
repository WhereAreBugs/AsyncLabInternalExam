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
class Init {
private:
    std::vector<WebNet *> networkCommpents;
    std::vector<databaseCore *> databaseCommpents;
public:
    Init();
    ~Init();
    void start();
};


#endif //SIMPLELABMANAGER_INIT_H
