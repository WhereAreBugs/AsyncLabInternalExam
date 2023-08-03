//
// Created by 神奇bug在哪里 on 8/3/23.
//

#include <spdlog/spdlog.h>
#include "activatys.h"
#include "fstream"


void activatys::addActivaty(const std::string &name, const std::string &description, time_t startTime, time_t endTime,
                            const std::string &username, const std::string &link) {
    std::lock_guard<std::mutex> lock(globalActivatyMutex);
    for (auto &i : data) {
        if (i.name == name) {
            if (i.link == link) {
                if (i.username == username) {
                    spdlog::error("[activatys] addActivaty: activaty already exists");
                    return;
                }
            }
        }
    }
        data.emplace_back(
                activaty{static_cast<int>(data.size() + 1), name, description, startTime, endTime, username, link});
}

void activatys::removeActivaty(int id) {
    std::lock_guard<std::mutex> lock(globalActivatyMutex);
    for (auto i = data.begin(); i != data.end(); i++) {
        if (i->id == id) {
            data.erase(i);
            return;
        }
    }
    spdlog::error("[activatys] removeActivaty: id not found");
}

void activatys::write2File() {
    std::fstream file;
    file.open(filename, std::ios::out);
    if (!file.is_open()) {
        spdlog::error("[activatys] write2File: file open failed");
        return;
    }
    for (const auto &i : data) {
        file << i.id << " " << i.name << " " << i.description << " " << i.startTime << " " << i.endTime << " "
             << i.username << " " << i.link << std::endl;
    }
    file.close();
}

void activatys::readFromFile() {
    std::fstream file;
    file.open(filename, std::ios::in);
    if (!file.is_open()) {
        spdlog::error("[activatys] readFromFile: file open failed");
        return;
    }
    activaty temp;
    while (file >> temp.id >> temp.name >> temp.description >> temp.startTime >> temp.endTime >> temp.username >>
           temp.link) {
        data.emplace_back(temp);
    }
    file.close();
}

std::vector<activatys::activaty> activatys::getActivatys() {
    return data;
}

std::vector<activatys::activaty> activatys::getActivatys(const std::string &username) {
    std::vector<activaty> temp;
    for (auto &i : data) {
        if (i.username == username) {
            temp.emplace_back(i);
        }
    }
    return temp;
}

int activatys::getID(const std::string &name, const std::string &link) {
    for (auto &i : data) {
        if (i.name == name) {
            if (i.link == link) {
                return i.id;
            }
        }
    }
    spdlog::error("[activatys] getID: activaty not found");
    return -1;
}

