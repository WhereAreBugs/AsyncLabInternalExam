//
// Created by 神奇bug在哪里 on 7/14/23.
//

#include "Init.h"
#include "activatys.h"
#include "WebData.h"
#include "seatData.h"
#include "spdlog/spdlog.h"
#include "thread"
#include "settings.h"
void Init::start() {
    databaseCommpents.emplace_back(new WebData());
    databaseCommpents.emplace_back(new seatData());
    databaseCommpents.emplace_back(new activatys());
    subThreads.emplace_back(&Init::databaseInit, this);
    webnetHandler = new WebNet();
    CHECK_NULLPTR(webnetHandler)
    subThreads.at(0).join();
    subThreads.clear(); //等待数据库完成读取
    subThreads.emplace_back(&WebNet::setup,webnetHandler);
    lockerHandler = new Locker();
    CHECK_NULLPTR(lockerHandler)
    subThreads.emplace_back(&Locker::setup,lockerHandler);
    identifyerHandler = new identifyer();
    CHECK_NULLPTR(identifyerHandler)
    subThreads.emplace_back(&identifyer::setup,identifyerHandler);
    subThreads.at(2).join();
    subThreads.at(1).join();
    subThreads.at(0).join();
    subThreads.clear();
    spdlog::info("System init complete");
    while (true)
    {}
}

Init::Init() = default;

Init::~Init() {
    for (auto i : databaseCommpents) {
        delete i;
    }
    delete webnetHandler;
    delete lockerHandler;
    delete identifyerHandler;
}

void Init::databaseInit() {
    spdlog::trace("Database thread start...");
    for (auto i : databaseCommpents) {
        i->readFromFile();
    }
    spdlog::trace("Database FileLoad complete");
    spdlog::trace("Database thread exit...");
}


