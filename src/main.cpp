//
// Created by 神奇bug在哪里 on 7/14/23.
//
#include "Init.h"
#include "spdlog/spdlog.h"

inline void setUPHandler()
{

}
Init init;
int main(){
    spdlog::info("System init...");
    spdlog::set_level(spdlog::level::trace);
    init.start();

}