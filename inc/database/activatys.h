//
// Created by 神奇bug在哪里 on 8/3/23.
//

#ifndef SIMPLELABMANAGER_ACTIVATYS_H
#define SIMPLELABMANAGER_ACTIVATYS_H
#include "databaseCore.h"
#include "mutex"
class activatys : public databaseCore{
public:
    struct activaty{
        int id;
        std::string name;
        std::string description;
        time_t startTime;
        time_t endTime;
        std::string username;
        std::string link;
    };
private:
    std::vector<activaty> data;
    std::mutex globalActivatyMutex;
protected:
    std::string filename = "activatys.dat";
public:
    void addActivaty(const std::string& name, const std::string& description, time_t startTime, time_t endTime, const std::string& username, const std::string& link);
    void removeActivaty(int id);
    void write2File() override;
    void readFromFile() override;
    std::vector<activaty> getActivatys();
    std::vector<activaty> getActivatys(const std::string& username);
    int getID(const std::string& name, const std::string& link);
};


#endif //SIMPLELABMANAGER_ACTIVATYS_H
