//
// Created by 神奇bug在哪里 on 8/2/23.
//

#include <fstream>
#include "seatData.h"
#include "spdlog/spdlog.h"
void seatData::setSeatStatus(int id, seatData::seatStatus status) {
    std::lock_guard<std::mutex> lock(globalVectorMutex);
    for (auto &i : data) {
        if (i.id == id) {
            i.status = status;
            return;
        }
    }
    spdlog::error("[seatData] setSeatStatus: id not found");
}

seatData::seatStatus seatData::getSeatStatus(const std::string &username) {
    for (auto &i: data) {
        if (i.username == username) {
            return i.status;
        }
    }
    spdlog::error("[seatData] getSeatStatus: username not found");
    return empty;
}

void seatData::setSeatUsername(int id, std::string username) {
    for (auto i : data) {
        if (i.id == id) {
            i.username = std::move(username);
            return;
        }
    }
}

void seatData::addSeat(int id, seatData::seatStatus status) {
    std::lock_guard<std::mutex> lock(globalVectorMutex);
    for (auto &i : data) {
        if (i.id == id) {
            spdlog::error("[seatData] addSeat: id already exists");
            return;
        }
    }
    data.emplace_back(seat{id, status, ""});
}

void seatData::removeSeat(int id) {
    std::lock_guard<std::mutex> lock(globalVectorMutex);
    for (auto i = data.begin(); i != data.end(); i++) {
        if (i->id == id) {
            data.erase(i);
            return;
        }
    }
    spdlog::error("[seatData] removeSeat: id not found");

}

void seatData::write2File() {
    std::lock_guard<std::mutex> lock(globalVectorMutex);
    std::ofstream file;
    file.open(filename, std::ios::out | std::ios::binary);
    if (!file.is_open()) {
        spdlog::error("[seatData] write2File: file open failed");
        return;
    }
    for (auto &i : data) {
        file.write(reinterpret_cast<char *>(&i), sizeof(i));
    }
    file.close();
}

void seatData::readFromFile() {
    std::lock_guard<std::mutex> lock(globalVectorMutex);
    std::ifstream file;
    file.open(filename, std::ios::in | std::ios::binary);
    if (!file.is_open()) {
        spdlog::error("[seatData] readFromFile: file open failed");
        return;
    }
    seat temp;
    while (file.read(reinterpret_cast<char *>(&temp), sizeof(temp))) {
        data.emplace_back(temp);
    }
    file.close();
}

time_t seatData::getRubbishTime(std::string name) {
    for (auto &i : data) {
        if (i.username == name) {
            return i.rubbishTime;
        }
    }
    spdlog::warn("[seatData] getRubbishTime: username not found");
    return 0;
}

bool seatData::ResetSeatStatus(const std::string& name) {
    for (auto &i : data) {
        if (i.username == name) {
            if (i.status == flexible)
            {
                data.erase(data.begin() + (&i - &data[0]));
                return true;
            }
        }
    }
    spdlog::warn("[seatData] ResetSeatStatus: username not found or status is not flexible");
    return false;
}

bool seatData::bindSeat(const std::string &name, int id) {
    for (auto &i : data) {
        if (i.username == name) {
            if (i.status!=fixed&&i.id!=id)
            {
                i.status = flexible;
                i.id = id;
                return true;
            }
            else if (i.id == id)
            {
                spdlog::warn("[seatData] bindSeat: id already exists");
                return true;
            } else
            {
                spdlog::warn("[seatData] bindSeat: status is fixed, please remove it first!");
                return false;
            }
        }
    }
    for (auto &i : data) {
        if (i.id == id) {
            i.username = name;
            return true;
        }
    }
    spdlog::warn("[seatData] bindSeat: id not found");
    return false;
}

bool seatData::unbindSeat(const std::string &name) {
    for (auto &i : data) {
        if (i.username == name) {
            i.username = "";
            i.status = empty;
            return true;
        }
    }
    spdlog::warn("[seatData] unbindSeat: username not found");
    return false;
}
